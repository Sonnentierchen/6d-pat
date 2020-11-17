#include "settingsgeneralpage.hpp"
#include "ui_settingsgeneralpage.h"
#include "view/misc/displayhelper.hpp"
#include <QFileDialog>

SettingsGeneralPage::SettingsGeneralPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsGeneralPage)
{
    ui->setupUi(this);
    DisplayHelper::setIcon(ui->buttonPosesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonImagesPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonObjectModelsPath, fa::folderopen, 20);
    DisplayHelper::setIcon(ui->buttonSegmentationImages, fa::folderopen, 20);
}

SettingsGeneralPage::~SettingsGeneralPage()
{
    delete ui;
}

void SettingsGeneralPage::setPreferences(Settings *preferences) {
    this->preferences = preferences;
    ui->editImagesPath->setText(preferences->imagesPath());
    ui->editObjectModelsPath->setText(preferences->objectModelsPath());
    ui->editPosesPath->setText(preferences->posesFilePath());
    ui->editSegmentationImagesPath->setText(preferences->segmentationImagesPath());
}

QString SettingsGeneralPage::openFolderDialogForPath(QString path) {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    path,
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks
                                                    | QFileDialog::DontUseNativeDialog);
    return dir;
}

QString SettingsGeneralPage::openFileDialogForPath(QString path) {
    QString dir = QFileDialog::getOpenFileName(this,
                                               tr("Open JSON File"),
                                               path,
                                               tr("JSON Files (*.json)"),
                                               Q_NULLPTR,
                                               QFileDialog::DontUseNativeDialog);
    return dir;
}

//! Public slots
void SettingsGeneralPage::buttonImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editImagesPath->setText(newPath);
        preferences->setImagesPath(newPath);
    }
}

void SettingsGeneralPage::buttonSegmentationImagesPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editSegmentationImagesPath->text());
    if (newPath.compare("") != 0) {
        ui->editSegmentationImagesPath->setText(newPath);
        preferences->setSegmentationImagePath(newPath);
    }
}

void SettingsGeneralPage::buttonObjectModelsPathClicked() {
    QString newPath = openFolderDialogForPath(ui->editObjectModelsPath->text());
    if (newPath.compare("") != 0) {
        ui->editObjectModelsPath->setText(newPath);
        preferences->setObjectModelsPath(newPath);
    }
}

void SettingsGeneralPage::buttonPosesPathClicked() {
    QString newPath = openFileDialogForPath(ui->editPosesPath->text());
    if (newPath.compare("") != 0) {
        ui->editPosesPath->setText(newPath);
        preferences->setPosesFilePath(newPath);
    }
}

