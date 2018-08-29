//
// Created by koncord on 18.07.18.
//

#include <QtWidgets/QFileDialog>
#include "SettingsWindow.hpp"
#include "Settings.hpp"

SettingsWindow::SettingsWindow(QWidget *parent):
        QDialog(parent)
{
    setupUi(this);

    connect(pbModulePath, &QPushButton::clicked, [this](bool) {
        QString str = QFileDialog::getExistingDirectory(this, tr("Module path"),
                                                        leModulePath->text(), QFileDialog::ShowDirsOnly);
        if(!str.isEmpty())
            leModulePath->setText(str);
    });

    SettingsMgr::get().loadClientSettings(*this);
    SettingsMgr::get().loadServerSettings(*this);
}

SettingsWindow::~SettingsWindow()
{
    SettingsMgr::get().saveClientSettings(*this);
    SettingsMgr::get().saveServerSettings(*this);
}
