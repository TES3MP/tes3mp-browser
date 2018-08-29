//
// Created by koncord on 18.07.18.
//

#pragma once

#include <QtWidgets/QDialog>
#include "ui_Settings.h"

class SettingsWindow : public QDialog, private Ui::DialogSettings
{
    Q_OBJECT
public:
    SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
public slots:

private:

};
