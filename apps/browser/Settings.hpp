//
// Created by koncord on 31.03.18.
//

#pragma once

#include <components/settings/settings.hpp>

namespace Ui
{
    class DialogSettings;
}
class MainWindow;

class SettingsMgr
{
public:
    static SettingsMgr &get();
    void loadBrowserSettings(MainWindow &mw);
    void saveBrowserSettings(MainWindow &mw);

    void loadClientSettings(Ui::DialogSettings &mw);
    void saveClientSettings(Ui::DialogSettings &mw);

    void loadServerSettings(Ui::DialogSettings &mw);
    void saveServerSettings(Ui::DialogSettings &mw);

    bool isPreRewrite();

private:
    SettingsMgr();
    Settings::Manager serverMgr, clientMgr, browserMgr;
    std::string serverCfg, clientCfg, browserCfg;
};
