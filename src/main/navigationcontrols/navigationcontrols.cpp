#include "navigationcontrols.h"
#include "ui_navigationcontrols.h"
#include <QtAwesome/QtAwesome.h>
#include <QFileDialog>

NavigationControls::NavigationControls(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationControls)
{
    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();
    ui->setupUi(this);
    ui->navigateLeftButton->setFont(awesome->font(16));
    ui->navigateLeftButton->setIcon(awesome->icon(fa::chevronleft));
    ui->navigateRightButton->setFont(awesome->font(26));
    ui->navigateRightButton->setIcon(awesome->icon(fa::chevronright));
    ui->openFolderButton->setFont(awesome->font(16));
    ui->openFolderButton->setIcon(awesome->icon(fa::folderopen));

}

NavigationControls::~NavigationControls()
{
    delete ui;
}

void NavigationControls::addListener(NavigationControlsListener listener) {
    listeners.push_back(listener);
}

void NavigationControls::folderButtonClicked() {
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                "/home",
                                                QFileDialog::ShowDirsOnly
                                                | QFileDialog::DontResolveSymlinks);
    if (dir == "") {
        return;
    }
    boost::filesystem::path newPath = boost::filesystem::path(dir.toStdString().c_str());
    emit pathChanged(newPath);
    for (NavigationControlsListener listener : listeners) {
        listener(newPath);
    }
}
