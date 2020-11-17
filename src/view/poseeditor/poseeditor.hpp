#ifndef CORRESPONDENCEEDITORCONTROLS_H
#define CORRESPONDENCEEDITORCONTROLS_H

#include "model/modelmanager.hpp"
#include "controller/poserecoveringcontroller.hpp"
#include "misc/global.hpp"
#include "view/poseeditor/poseeditor3dwidget.hpp"

#include <QWidget>
#include <QFrame>
#include <QItemSelection>
#include <QStringListModel>
#include <QListView>
#include <QMap>
#include <QPointer>

// Need to pre-load this class
class PoseEditor3DWindow;

namespace Ui {
class PoseEditor;
}

class PoseEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PoseEditor(QWidget *parent = Q_NULLPTR);
    ~PoseEditor();
    // Need setters because the forms file can't create the widget
    // by passing the two
    void setModelManager(ModelManager *modelManager);
    void setPoseRecoverer(PoseRecoveringController* poseRecoverer);
    bool isDisplayingObjectModel();
    void setPoses(const QList<PosePtr> poses);

public Q_SLOTS:
    void setObjectModel(ObjectModelPtr objectModel);
    void onSelectedObjectModelChanged(int index);
    void onSelectedImageChanged(int index);
    // For poses selected in the PoseViewer
    void selectPose(PosePtr selected, PosePtr deselected);
    void onSelectedPoseValuesChanged(PosePtr pose);
    void onPosesSaved();
    void onPoseCreationAborted();
    void reset();

Q_SIGNALS:
    void buttonPredictClicked();
    void loadingObjectModel();
    void objectModelLoaded();
    void poseSelected(PosePtr pose);
    void buttonSaveClicked();

private Q_SLOTS:
    /*!
     * \brief onObjectModelClickedAt handles clicking the 3D model
     */
    void onObjectModelClickedAt(const QVector3D &position);
    /*!
     * \brief updateCurrentlyEditedPose gets called whenever the user clicks on one of
     * the position or rotation controls or modifies the articulation angle.
     */
    void updateCurrentlyEditedPose();
    void onPoseAdded(PosePtr pose);
    void onPoseDeleted(PosePtr pose);

    void onButtonCreateClicked();
    void onButtonSaveClicked();
    void onButtonDuplicateClicked();
    /*!
     * \brief removeCurrentlyEditedPose gets called when the user wants to remove the
     * currenlty edited pose from the currenlty displayed image.
     */
    void onButtonRemoveClicked();
    void onButtonCopyClicked();

    //! React to signal from the model manager
    void onDataChanged(int data);

    // For PoseRecoverer
    void onCorrespondencesChanged();
    void onPoseRecovererStateChanged(PoseRecoveringController::State state);

    /*!
     * \brief onListViewPosesSelectionChanged Reacts to the user selecting a different pose from
     * the poses list view.
     */
    void onListViewPosesSelectionChanged(const QItemSelection &selected,
                                         const QItemSelection &deselected);
    void onSpinBoxValueChanged();
    void onObjectModelLoaded();

private:
    Ui::PoseEditor *ui;
    // QPointer is set to nullptr when object doesn't exist but is a weak pointer
    QPointer<ModelManager> modelManager;
    QPointer<PoseRecoveringController> poseRecoverer;

    PoseEditor3DWindow *poseEditor3DWindow;

    ObjectModelPtr currentlySelectedObjectModel;
    PosePtr currentlySelectedPose;
    PosePtr previouslySelectedPose;
    // We need to store what image the user currently views that in case that they select an object
    // model we can restore the list of all poses available for the currently viewed image
    ImagePtr currentlySelectedImage;

    // When the pose is selected by the pose viewer we still emit the pose selected signal
    // which causes the program to crash
    bool doNotEmitPoseSelected = false;

    // Indices of the list view for simplicity
    QMap<QString, int> posesIndices;

    QStringListModel *listViewPosesModel;
    QStringListModel *listViewImagesModel;

    // To know whether to re-enable to save button after loading a different pose
    bool posesDitry = false;

    // To prevent the spin boxes from emitting their changed signal when we set the pose
    // values e.g. because the user selected a pose in the PoseViewer
    bool ignoreSpinBoxValueChanges = false;
    // To prevent from reacting to the poses list view selection changed signal again
    // when we receive the selected pose changed signal from the PoseEditingController
    bool ignorePoseSelectionChanges = false;

    void setEnabledPoseEditorControls(bool enabled);
    void setEnabledAllControls(bool enabled);
    void resetControlsValues();
    // For the poses list view, the images list view and the copy button
    // they can stay enabled as long as there is an image that is being viewed
    void setEnabledPoseInvariantControls(bool enabled);
    void addPosesToListViewPoses(const Image &image, const QString &poseToSelect = "");
    void setPoseValuesOnControls(const Pose &pose);

};

#endif // CORRESPONDENCEEDITORCONTROLS_H
