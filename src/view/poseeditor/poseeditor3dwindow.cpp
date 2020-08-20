#include "view/poseeditor/poseeditor3dwindow.hpp"
#include "misc/global.hpp"

#include <QVector3D>
#include <QUrl>

#include <Qt3DCore/QNode>
#include <Qt3DCore/QNodeVector>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QPointLight>
#include <Qt3DCore/QTransform>
#include <Qt3DRender/QPickEvent>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/qpickingsettings.h>

PoseEditor3DWindow::PoseEditor3DWindow()
    : Qt3DExtras::Qt3DWindow(),
      rootEntity(new Qt3DCore::QEntity) {
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity();
    setRootEntity(rootEntity);

    Qt3DRender::QCamera *cameraEntity = this->camera();

    cameraEntity->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    cameraEntity->setPosition(QVector3D(0, 0, 20.0f));
    cameraEntity->setUpVector(QVector3D(0, 1, 0));
    cameraEntity->setViewCenter(QVector3D(0, 0, 0));

    cameraController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    cameraController->setCamera(camera());

    Qt3DCore::QEntity *lightEntity = new Qt3DCore::QEntity(rootEntity);
    Qt3DRender::QPointLight *light = new Qt3DRender::QPointLight(lightEntity);
    light->setColor("white");
    light->setIntensity(1);
    lightEntity->addComponent(light);
    Qt3DCore::QTransform *lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(cameraEntity->position());
    lightEntity->addComponent(lightTransform);
    connect(camera(), &Qt3DRender::QCamera::positionChanged, [lightTransform, this](){lightTransform->setTranslation(this->camera()->position());});

    this->renderSettings()->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);

    picker = new Qt3DRender::QObjectPicker(rootEntity);
    rootEntity->addComponent(picker);
    connect(picker, &Qt3DRender::QObjectPicker::clicked,
           [this](Qt3DRender::QPickEvent *pickEvent){Q_EMIT positionClicked(pickEvent->localIntersection());});
    // Needs to be placed after setRootEntity on the window because it doesn't work otherwise -> leave it here
    objectRenderable = new ObjectRenderable(rootEntity);
    connect(objectRenderable, &ObjectRenderable::statusChanged, this, &PoseEditor3DWindow::onObjectRenderableStatusChanged);

}

PoseEditor3DWindow::~PoseEditor3DWindow() {
}

void PoseEditor3DWindow::onObjectRenderableStatusChanged(Qt3DRender::QSceneLoader::Status) {
    camera()->viewAll();
}

void PoseEditor3DWindow::setObjectModel(const ObjectModel &objectModel) {
    objectRenderable->setObjectModel(&objectModel);
    objectRenderable->setEnabled(true);
}

void PoseEditor3DWindow::addClick(QVector3D position, QColor color) {
    objectRenderable->addClick(position, color);
}

void PoseEditor3DWindow::removeClicks() {
    objectRenderable->removeClicks();
}

void PoseEditor3DWindow::reset() {
    removeClicks();
    objectRenderable->setEnabled(false);
}
