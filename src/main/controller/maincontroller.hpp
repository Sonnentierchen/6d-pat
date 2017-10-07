#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "model/cachingmodelmanager.hpp"
#include "model/textfileloadandstorestrategy.hpp"
#include "view/mainwindow.hpp"
#include "view/settings/settingsitem.h"
#include "view/settings/settingsdialogdelegate.h"
#include "view/gallery/galleryobjectmodelmodel.h"
#include "view/gallery/galleryimagemodel.h"
#include <QApplication>
#include <QMap>

//! This class is responsible for the overall program to work. It maintains references to all the important parts and
//! ensures them to work properly and updates or makes update-requests when necessary.
class MainController : public QApplication, public SettingsDialogDelegate
{
private:
    TextFileLoadAndStoreStrategy strategy;
    CachingModelManager modelManager;
    MainWindow mainWindow;
    QMap<QString, ObjectModel*> segmentationCodes;
    SettingsItem* currentSettingsItem;
    GalleryImageModel *galleryImageModel;
    GalleryObjectModelModel *galleryObjectModelModel;

    void initializeSettingsItem();
    void initializeMainWindow();
    void setSegmentationCodesOnGalleryObjectModelModel();

public:
    MainController(int &argc, char *argv[]);
    ~MainController();

    /*!
     * \brief initialize initializes this controller, i.e. sets up the necessary models and further initializes the view.
     */
    void initialize();

    /*!
     * \brief showView shows the view of this controller.
     */
    void showView();

    void applySettings(const SettingsItem* currentSettingsItem);
};

#endif // MAINCONTROLLER_H