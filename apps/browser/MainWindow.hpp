//
// Created by koncord on 06.01.17.
//

#pragma once

#include "ui_Main.h"
#include "ServerModel.hpp"
#include "MySortFilterProxyModel.hpp"
#include <components/process/processinvoker.hpp>

class QueryHelper;
class SettingsWindow;
class SettingsMgr;

class MainWindow : public QMainWindow,  private Ui::MainWindow
{
    Q_OBJECT
    friend class SettingsMgr;
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;
protected:
    void closeEvent(QCloseEvent * event) Q_DECL_OVERRIDE;
    void addServerAndUpdate(const QString &addr);
protected slots:
    void tabSwitched(int index);
    void addServer();
    void deleteServer();
    void play();
    void serverSelected();
    void notFullSwitch(bool state);
    void havePlayersSwitch(bool state);
    void noPasswordSwitch(bool state);
    void maxLatencyChanged(int index);
    void gamemodeChanged(const QString &text);

    void setMotdHTML(const QString &data);
    void setMotdImage(QPixmap *image, const QString &link);

private:
    void setMotdWidget(QWidget *widget);
private:
    SettingsWindow *settingsWindow;
    QueryHelper *queryHelper;
    Process::ProcessInvoker *mGameInvoker;
    ServerModel *browser, *favorites;
    MySortFilterProxyModel *proxyModel;
    void loadFavorites();
    QLabel *lblSignStatus;
    QLabel *lblBrowserVersion;
    QString masterAddress;
};
