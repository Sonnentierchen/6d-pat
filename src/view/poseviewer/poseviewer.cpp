#include "poseviewer.hpp"
#include "ui_poseviewer.h"
#include "view/misc/displayhelper.hpp"

#include <QRect>

PoseViewer::PoseViewer(QWidget *parent, ModelManager* modelManager) :
    QWidget(parent),
    ui(new Ui::PoseViewer),
    poseViewer3DWidget(new PoseViewer3DWidget),
    awesome(new QtAwesome( qApp )),
    modelManager(modelManager)
{
    ui->setupUi(this);

    poseViewer3DWidget->setParent(ui->graphicsContainer);

    connect(poseViewer3DWidget, &PoseViewer3DWidget::positionClicked,
            this, &PoseViewer::onImageClicked);

    awesome->initFontAwesome();

    ui->buttonSwitchView->setFont(awesome->font(18));
    ui->buttonSwitchView->setIcon(awesome->icon(fa::toggleoff));
    ui->buttonSwitchView->setToolTip("Click to switch views between segmentation \n"
                                     "image (if available) and normal image.");
    ui->buttonSwitchView->setEnabled(false);

    ui->buttonResetPosition->setFont(awesome->font(18));
    ui->buttonResetPosition->setIcon(awesome->icon(fa::arrows));
    ui->buttonResetPosition->setToolTip("Click to reset the position of the image.");
    ui->buttonResetPosition->setEnabled(false);

    if (modelManager) {
        connectModelManagerSlots();
    }

    //connect(ui->openGLWidget, SIGNAL(positionClicked(QPoint)), this, SLOT(onImageClicked(QPoint)));
}

PoseViewer::~PoseViewer() {
    delete ui;
}

void PoseViewer::setModelManager(ModelManager* modelManager) {
    Q_ASSERT(modelManager != Q_NULLPTR);
    if (this->modelManager) {
        disconnect(modelManager, SIGNAL(posesChanged()), this, SLOT(onPosesChanged()));
        disconnect(modelManager, SIGNAL(poseAdded(QString)),
                   this, SLOT(onPoseAdded(QString)));
        disconnect(modelManager, SIGNAL(poseDeleted(QString)),
                   this, SLOT(onPoseDeleted(QString)));
        disconnect(this->modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
        disconnect(this->modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    }
    this->modelManager = modelManager;
    connectModelManagerSlots();
}

Image *PoseViewer::currentlyViewedImage() {
    return currentlyDisplayedImage.data();
}

void PoseViewer::setImage(Image *image) {
    currentlyDisplayedImage.reset(new Image(*image));

    poseViewer3DWidget->removePoses();
    ui->buttonResetPosition->setEnabled(true);
    ui->sliderTransparency->setEnabled(false);
    ui->sliderTransparency->setValue(100);
    ui->sliderZoom->setEnabled(true);

    qDebug() << "Displaying image (" + currentlyDisplayedImage->getImagePath() + ").";

    // Enable/disable functionality to show only segmentation image instead of normal image
    if (currentlyDisplayedImage->getSegmentationImagePath().isEmpty()) {
        ui->buttonSwitchView->setEnabled(false);

        // If we don't find a segmentation image, set that we will now display the normal image
        // because the formerly set image could have had a segmentation image and set this value
        // to false
        showingNormalImage = true;
    } else {
        ui->buttonSwitchView->setEnabled(true);
    }
    QString toDisplay = showingNormalImage ?  currentlyDisplayedImage->getAbsoluteImagePath() :
                                    currentlyDisplayedImage->getAbsoluteSegmentationImagePath();
    QList<Pose> posesForImage = modelManager->getPosesForImage(*image);
    poseViewer3DWidget->setBackgroundImageAndPoses(toDisplay, image->getCameraMatrix(), posesForImage);
    ui->sliderTransparency->setEnabled(posesForImage.size() > 0);
}

void PoseViewer::connectModelManagerSlots() {
    connect(modelManager, SIGNAL(poseAdded(QString)),
               this, SLOT(onPoseAdded(QString)));
    connect(modelManager, SIGNAL(poseDeleted(QString)),
               this, SLOT(onPoseDeleted(QString)));
    connect(modelManager, SIGNAL(imagesChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(objectModelsChanged()), this, SLOT(reset()));
    connect(modelManager, SIGNAL(posesChanged()), this, SLOT(onPosesChanged()));
}

void PoseViewer::reset() {
    qDebug() << "Resetting pose viewer.";
    poseViewer3DWidget->reset();
    ui->buttonResetPosition->setEnabled(false);
    ui->buttonSwitchView->setEnabled(false);
    ui->sliderTransparency->setEnabled(false);
    ui->sliderZoom->setEnabled(false);
    currentlyDisplayedImage.reset();
}

void PoseViewer::reloadPoses() {
    if (!currentlyDisplayedImage.isNull()) {
        Image image = *(currentlyDisplayedImage.data());
        setImage(&image);
    } else {
        reset();
    }
}

void PoseViewer::visualizeLastClickedPosition(int posePointIndex) {
    Q_ASSERT(posePointIndex >= 0);
    poseViewer3DWidget->addClick(lastClickedPosition);
}

void PoseViewer::onPoseCreationAborted() {
    removePositionVisualizations();
}

void PoseViewer::removePositionVisualizations() {
    poseViewer3DWidget->removeClicks();
}

void PoseViewer::onPosePointStarted(QPoint /* point2D */,
                                    int currentNumberOfPoints,
                                    int /* minimumNumberOfPoints */) {
    // We can use the number of points as index directly, because the number of points only increases
    // after the user successfully clicked a 2D location and the corresponding 3D point
    visualizeLastClickedPosition(currentNumberOfPoints);
}

void PoseViewer::onPoseUpdated(Pose *pose){
    poseViewer3DWidget->updatePose(*pose);
}

void PoseViewer::switchImage() {
    ui->buttonSwitchView->setIcon(awesome->icon(showingNormalImage ? fa::toggleon : fa::toggleoff));
    showingNormalImage = !showingNormalImage;

    if (showingNormalImage)
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteImagePath(),
                                               currentlyDisplayedImage->getCameraMatrix());
    else
        poseViewer3DWidget->setBackgroundImage(currentlyDisplayedImage->getAbsoluteSegmentationImagePath(),
                                               currentlyDisplayedImage->getCameraMatrix());

    if (showingNormalImage)
        qDebug() << "Setting viewer to display normal image.";
    else
        qDebug() << "Setting viewer to display segmentation image.";

}

void PoseViewer::onOpacityChanged(int opacity) {
    poseViewer3DWidget->setObjectsOpacity(opacity / 100.0);
}

void PoseViewer::onZoomChanged(int zoom) {
    int direction = zoom < this->zoom ? -1 : 1;
    this->zoom = zoom;
    qDebug() << zoom;
    if (zoom == 1) {
        this->zoomMultiplier = 0.5f;
    } else if (zoom == 2) {
        this->zoomMultiplier = 1.f;
    } else if (zoom == 3) {
        this->zoomMultiplier = 2.f;
    }
    if (!resizeAnimation) {
        resizeAnimation = new QPropertyAnimation(poseViewer3DWidget, "geometry");
    } else {
        resizeAnimation->stop();
    }
    int oldWidth = poseViewer3DWidget->width();
    int oldHeight = poseViewer3DWidget->height();
    int newWidth = 0;
    int newHeight = 0;
    if (zoom == 2) {
        newWidth = poseViewer3DWidget->imageSize().width();
        newHeight = poseViewer3DWidget->imageSize().height();
    } else {
        newWidth = oldWidth * this->zoomMultiplier;
        newHeight = oldHeight * this->zoomMultiplier;
    }
    resizeAnimation->setDuration(250);
    QPoint position = poseViewer3DWidget->pos();
    resizeAnimation->setStartValue(QRect(position.x(),
                                         position.y(),
                                         oldWidth,
                                         oldHeight));
    resizeAnimation->setEndValue(QRect(position.x() - (newWidth - oldWidth) / 2,
                                       position.y() - (newHeight - oldHeight) / 2,
                                       poseViewer3DWidget->imageSize().width() * this->zoomMultiplier,
                                       poseViewer3DWidget->imageSize().height() * this->zoomMultiplier));
    resizeAnimation->start();
}

void PoseViewer::resetPositionOfGraphicsView() {
    poseViewer3DWidget->setGeometry(0, 0, poseViewer3DWidget->width(), poseViewer3DWidget->height());
}

void PoseViewer::onImageClicked(QPoint point) {
    qDebug() << "Image (" + currentlyDisplayedImage->getImagePath() + ") clicked at: (" +
                QString::number(point.x()) + ", " + QString::number(point.y()) + ").";
    lastClickedPosition = point;
    Q_EMIT imageClicked(currentlyDisplayedImage.data(), point / zoomMultiplier);
}

void PoseViewer::onPoseDeleted(const QString &id) {
    poseViewer3DWidget->removePose(id);
}

void PoseViewer::onPoseAdded(const QString &id) {
    QSharedPointer<Pose> pose = modelManager->getPoseById(id);
    if (!pose.isNull()) {
        poseViewer3DWidget->addPose(*pose.data());
        ui->sliderTransparency->setEnabled(true);
    }
    poseViewer3DWidget->removeClicks();
}

void PoseViewer::onPosesChanged() {
    reloadPoses();
}

void PoseViewer::onImagesChanged() {
    reset();
}

void PoseViewer::onObjectModelsChanged() {
    reset();
}
