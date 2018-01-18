#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "view/navigationcontrols/navigationcontrols.hpp"
#include "misc/preferences/preferencesstore.hpp"
#include "view/gallery/galleryimagemodel.hpp"
#include "view/gallery/galleryobjectmodelmodel.hpp"
#include <QGuiApplication>
#include <QMainWindow>
#include <QMouseEvent>
#include <QFrame>
#include <QLabel>
#include <QString>

namespace Ui {
class MainWindow;
}

/*!
 * \brief The ClickOverlay class is there to intercept clicks anywhere (except for the 3D models)
 *  on the UI after the user clicked on a position in the image. This is to change back the cursor
 * to normal appearance.
 */
class ClickOverlay : public QFrame {

    Q_OBJECT

public:
    ClickOverlay(QWidget* parent) : QFrame(parent) {
    }

    void mousePressEvent(QMouseEvent *event)  {
        event->accept();
        emit clickedAnywhere();
    }

signals:
    void clickedAnywhere();
};

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);

    //! Overriden from QWidget.
    /*!
      \param event the closing event
    */
    virtual void closeEvent(QCloseEvent *event);
    ~MainWindow();

    //! Various functions to initialize or alter the state of the window and its components.
    //! Most should be self-explanatory.
    void setPathOnLeftBreadcrumbView(const QString &pathToShow);
    void setPathOnRightBreadcrumbView(const QString &pathToShow);
    void setPathOnLeftNavigationControls(const QString &path);
    void setPathOnRightNavigationControls(const QString &path);
    void resetCorrespondenceEditor();

    void setStatusBarText(const QString& text);

    /*!
     * \brief setGalleryImageModel Sets the model for the gallery view of images on the left side.
     * \param model the model that holds the images for the gallery
     */
    void setGalleryImageModel(GalleryImageModel* model);

    /*!
     * \brief setGalleryObjectModelModel Sets the model for the gallery that displays the 3D models
     * on the right side.
     * \param model the model that holds the 3D objects for the gallery
     */
    void setGalleryObjectModelModel(GalleryObjectModelModel* model);

    /*!
     * \brief setModelManagerForCorrespondenceEditor sets the model manager for the correspondence editor.
     * This method passes the model manager directly because th editor has to access its functions at various
     * places making a proxy inbetween to complex.
     * \param modelManager the model manager that the editor uses to load correspondeces, update them, etc...
     */
    void setModelManager(ModelManager* modelManager);

    void setPreferencesStore(PreferencesStore *preferencesStore);

public slots:

    //! The slot that catches the emitted signal when the 3D model in the lower right correspondence controls
    //! is clicked (see CorrespondenceEditorControls)
    void onObjectModelClicked(ObjectModel* objectModel, QVector3D position);

    /*!
     * \brief onSelectedObjectModelChanged will be called from the right gallery that displays
     * objects models so that the main view can retrieve the actual object model an pass it on
     * to the correspondence editor controls.
     */
    void onSelectedObjectModelChanged(int index);

    void onImagesPathChangedByNavigation(const QString &path);
    void onObjectModelsPathChangedByNavigation(const QString &path);
    void displayWarning(const QString &title, const QString& text);

signals:
    /*!
     * \brief imageClicked emitted when the image in the correspondence viewer is clicked.
     * \param image the image that was clicked
     * \param position the position where it was clicked
     */
    void imageClicked(Image* image, QPoint position);

    /*!
     * \brief objectModelClicked emitted when the object model displayed in the correspondence editor
     * is clicked.
     * \param objectModel the object model that was clicked
     * \param position the position where it was clicked
     */
    void objectModelClicked(ObjectModel* objectModel, QVector3D position);
    /*!
     * \brief selectedObjectModelChanged is triggered after the index of the object model is received.
     * \param objectModel the actual object model retrieved from the index
     */
    void selectedObjectModelChanged(ObjectModel *objectModel);
    /*!
     * \brief selectedImageChanged is emitted when the image selected in the gallery changes.
     * \param index the index of the new image
     */
    void selectedImageChanged(int index);

    /*!
     * \brief correspondenceCreationInterrupted this signal is emitted when the user clicks the
     * overlay that is being added to the view as soon as the image is clicked anywhere. Clicking
     * the overlay can be an accident or because the image was clicked at the wrong position.
     * Thus we assume that the user only interrupted the creation, not aborted it. The user
     * can abort the creation from the menu.
     */
    void correspondenceCreationInterrupted();

    /*!
     * \brief correspondenceCreationAborted is emitted when the user clicks the abort creation
     * button
     */
    void correspondenceCreationAborted();

    /*!
     * \brief imagesPathChanged emitted when the images path changes because the user used the
     * navigation controls to change it
     * \param newPath the new images path that was set
     */
    void imagesPathChanged(const QString& newPath);

    /*!
     * \brief objectModelsPathChanged emitted when object models path changes because the user
     * used the navigation controls to change it
     * \param newPath the new object models path that was set
     */
    void objectModelsPathChanged(const QString &newPath);

private slots:
    void onActionAboutTriggered();
    void onActionExitTriggered();
    void onActionSettingsTriggered();
    void onActionAbortCreationTriggered();

    //! Mouse event receivers of the bottom left widget to draw a line behind the mouse when the user
    //! right clicks in the image to start creating a correspondence
    void onImageClicked(Image* image, QPoint position);

    /*!
     * \brief onOverlayClickedAnywhere is called when the user clicks the overlay. This means
     * he did not proceed to click the object model after clicking the image, which would have
     * added a correspondence point to the correspondence creator.
     */
    void onOverlayClickedAnywhere();
    void onPreferencesChanged(const QString &identifier);

private:
    // The overlay that is shown when the user clicks on a position in the displayed image to start
    // to create a correspondence. When they then click anywhere except for the 3D model on the right
    // the formerly changed curser (crosshair) will change back to normal, i.e. the correspondence
    // will not be created.
    ClickOverlay *clickOverlay = Q_NULLPTR;

    Ui::MainWindow *ui;

    // The label that displays the status of the program, like how many correspondence points have
    // been added, etc.
    QLabel *statusBarLabel = new QLabel();

    PreferencesStore *preferencesStore = Q_NULLPTR;
    ModelManager* modelManager;

    void writeSettings();
    void readSettings();
    //! The name of the settings - QT requests this to store settings "offline"
    static QString SETTINGS_NAME;
    //! Same as above
    static QString SETTINGS_PROGRAM_NAME;
    //! To group settings
    static QString SETTINGS_GROUP_NAME;
    //! The following are the keys to store settings persistently by using Qt's own settings system
    static QString WINDOW_IS_FULLSCREEN_KEY;
    static QString WINDOW_SIZE_KEY;
    static QString WINDOW_POSITION_KEY;
    static QString SPLITTER_MAIN_SIZE_LEFT_KEY;
    static QString SPLITTER_MAIN_SIZE_RIGHT_KEY;
    static QString SPLITTER_LEFT_SIZE_TOP_KEY;
    static QString SPLITTER_LEFT_SIZE_BOTTOM_KEY;
    static QString SPLITTER_RIGHT_SIZE_TOP_KEY;
    static QString SPLITTER_RIGHT_SIZE_BOTTOM_KEY;
};

#endif // MAINWINDOW_H
