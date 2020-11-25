#include "cachingmodelmanager.hpp"
#include "misc/generalhelper.hpp"

#include <QtConcurrent/QtConcurrent>
#include <QThread>
#include <QApplication>

CachingModelManager::CachingModelManager(LoadAndStoreStrategy& loadAndStoreStrategy) : ModelManager(loadAndStoreStrategy) {
    connect(&loadAndStoreStrategy, &LoadAndStoreStrategy::dataChanged,
            this, &CachingModelManager::dataChanged);
    //connect(&reloadFutureWatcher, &QFutureWatcher<void>::finished, this, &CachingModelManager::dataReady);
    connect(&loadAndStoreStrategy, &LoadAndStoreStrategy::error,
            [this](LoadAndStoreStrategy::Error error){
        this->m_error = error;
        Q_EMIT stateChanged(ModelManager::Error);
    });
}

CachingModelManager::~CachingModelManager() {
}

void CachingModelManager::createConditionalCache() {
    m_posesForImages.clear();
    m_posesForObjectModels.clear();
    for (int i = 0; i < m_poses.size(); i++) {
        PosePtr pose = m_poses[i];

        //! Setup cache of poses that can be retrieved via an image
        QList<PosePtr> &posesForImage =
                m_posesForImages[pose->image()->imagePath()];
        posesForImage.append(pose);

        //! Setup cache of poses that can be retrieved via an object model
        QList<PosePtr> &posesForObjectModel =
                m_posesForObjectModels[pose->objectModel()->path()];
        posesForObjectModel.append(pose);
    }
}

void CachingModelManager::onDataChanged(int data) {
    Q_EMIT stateChanged(State::Loading);
    if (data == Images) {
        m_images = loadAndStoreStrategy.loadImages();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    if (data == ObjectModels) {
        m_objectModels = loadAndStoreStrategy.loadObjectModels();
        // Add to flag that poses have been changed too
        data |= Data::Poses;
    }
    // We need to load poses no matter what
    m_poses = loadAndStoreStrategy.loadPoses(m_images, m_objectModels);
    createConditionalCache();
    Q_EMIT stateChanged(State::Ready);
    Q_EMIT dataChanged(data);
}

QList<ImagePtr> CachingModelManager::images() const {
    return m_images;
}

QList<PosePtr> CachingModelManager::posesForImage(const Image &image) const  {
    if (m_posesForImages.find(image.imagePath()) != m_posesForImages.end()) {
        return m_posesForImages[image.imagePath()];
    }

    return QList<PosePtr>();
}

QList<ObjectModelPtr> CachingModelManager::objectModels() const {
    return m_objectModels;
}

QList<PosePtr> CachingModelManager::posesForObjectModel(const ObjectModel &objectModel) const {
    if (m_posesForObjectModels.find(objectModel.path()) != m_posesForObjectModels.end()) {
        return m_posesForObjectModels[objectModel.path()];
    }

    return QList<PosePtr>();
}

QList<PosePtr> CachingModelManager::poses() const {
    return m_poses;
}

PosePtr CachingModelManager::poseById(const QString &id) const {
    PosePtr result;
    auto itObj = std::find_if(
        m_poses.begin(), m_poses.end(),
        [id](PosePtr o) { return o->id() == id; }
    );
    if (itObj != m_poses.end()) {
        result = *itObj;
    }
    return result;
}

QList<PosePtr> CachingModelManager::posesForImageAndObjectModel(const Image &image, const ObjectModel &objectModel) {
    QList<PosePtr> posesForImageAndObjectModel;
    for (PosePtr &pose : m_posesForImages[image.imagePath()]) {
        if (pose->objectModel()->path().compare(objectModel.path()) == 0) {
           posesForImageAndObjectModel.append(pose);
        }
    }
    return posesForImageAndObjectModel;
}

PosePtr CachingModelManager::addPose(ImagePtr image,
                                     ObjectModelPtr objectModel,
                                     const QVector3D &position,
                                     const QMatrix3x3 &rotation) {
    Q_ASSERT(image);
    Q_ASSERT(objectModel);
    return this->addPose(Pose(GeneralHelper::createPoseId(*image, *objectModel),
                              position,
                              rotation,
                              image,
                              objectModel));
}

PosePtr CachingModelManager::addPose(const Pose &pose) {
    // Persist the pose
    if (!loadAndStoreStrategy.persistPose(pose, false)) {
        //! if there is an error persisting the pose for any reason we should not add the pose to this manager
        return PosePtr();
    }

    //! pose has not yet been added
    PosePtr newPose(new Pose(pose));
    m_poses.push_back(newPose);

    createConditionalCache();

    Q_EMIT poseAdded(newPose);

    return newPose;
}

bool CachingModelManager::updatePose(const QString &id,
                                     const QVector3D &position,
                                     const QMatrix3x3 &rotation) {
    PosePtr pose;
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i]->id() == id) {
            pose = m_poses[i];
            break;
        }
    }

    if (pose.isNull()) {
        //! this manager does not manage the given pose
        return false;
    }

    QVector3D previousPosition = pose->position();
    QMatrix3x3 previousRotation = pose->rotation().toRotationMatrix() ;

    pose->setPosition(position);
    pose->setRotation(rotation);

    if (!loadAndStoreStrategy.persistPose(*pose, false)) {
        // if there is an error persisting the pose for any reason we should not keep the new values
        pose->setPosition(previousPosition);
        pose->setRotation(previousRotation);
        return false;
    }

    createConditionalCache();

    Q_EMIT poseUpdated(pose);

    return true;
}

bool CachingModelManager::removePose(const QString &id) {
    PosePtr pose;
    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses[i]->id() == id)
            pose = m_poses[i];
    }

    if (!pose) {
        //! this manager does not manager the given pose
        return false;
    }

    if (!loadAndStoreStrategy.persistPose(*pose, true)) {
        //! there was an error persistently removing the corresopndence, maybe wrong folder, maybe the pose didn't exist
        //! thus it doesn't make sense to remove the pose from this manager
        return false;
    }

    for (int i = 0; i < m_poses.size(); i++) {
        if (m_poses.at(i)->id() == id)
            m_poses.removeAt(i);
    }

    createConditionalCache();

    Q_EMIT poseDeleted(pose);

    return true;
}

void CachingModelManager::reload() {
    Q_EMIT stateChanged(State::Loading);
    QThread::sleep(10);
    m_images = loadAndStoreStrategy.loadImages();
    m_objectModels = loadAndStoreStrategy.loadObjectModels();
    m_poses = loadAndStoreStrategy.loadPoses(m_images, m_objectModels);
    createConditionalCache();
    Q_EMIT dataReady();
}

LoadAndStoreStrategy::Error CachingModelManager::error() {
    return m_error;
}

void CachingModelManager::onLoadAndStoreStrategyError(LoadAndStoreStrategy::Error error) {
    m_error = error;
    Q_EMIT stateChanged(State::Error);
}

void CachingModelManager::dataReady() {
    Q_EMIT stateChanged(State::Ready);
    Q_EMIT dataChanged(Data::Images | Data::ObjectModels | Data::Poses);
}
