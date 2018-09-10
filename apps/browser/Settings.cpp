//
// Created by koncord on 31.03.18.
//

#include "Settings.hpp"
#include <QDebug>

#include <components/files/configurationmanager.hpp>
#include "SettingsWindow.hpp"
#include "MainWindow.hpp"

std::string loadSettings (Settings::Manager & settings, const std::string &cfgName)
{
    Files::ConfigurationManager mCfgMgr;
    // Create the settings manager and load default settings file
    const std::string localdefault = (mCfgMgr.getLocalPath() / (cfgName + "-default.cfg")).string();
    const std::string globaldefault = (mCfgMgr.getGlobalPath() / (cfgName + "-default.cfg")).string();

    // prefer local
    if (boost::filesystem::exists(localdefault))
        settings.loadDefault(localdefault);
    else if (boost::filesystem::exists(globaldefault))
        settings.loadDefault(globaldefault);
    else
        throw std::runtime_error ("No default settings file found! Make sure the file \"" + cfgName + "-default.cfg\" was properly installed.");

    // load user settings if they exist
    const std::string settingspath = (mCfgMgr.getUserConfigPath() / (cfgName + ".cfg")).string();
    if (boost::filesystem::exists(settingspath))
        settings.loadUser(settingspath);

    return settingspath;
}

SettingsMgr &SettingsMgr::get()
{
    static SettingsMgr mgr;
    return mgr;
}

SettingsMgr::SettingsMgr()
{
    clientCfg = loadSettings(clientMgr, "tes3mp-client");
    serverCfg = loadSettings(serverMgr, "tes3mp-server");
    browserCfg = loadSettings(browserMgr, "tes3mp-browser");
}

void SettingsMgr::loadBrowserSettings(MainWindow &mw)
{
    mw.masterAddress = QString::fromStdString(browserMgr.getString("address", "Master"));
    mw.comboLatency->setCurrentIndex(browserMgr.getInt("maxLatency", "Browser"));
    mw.leGamemode->setText(QString::fromStdString(browserMgr.getString("gameMode", "Browser")));
    mw.cBoxNotFull->setCheckState(browserMgr.getBool("notFull", "Browser") ? Qt::Checked : Qt::Unchecked);
    mw.cBoxWithPlayers->setCheckState(browserMgr.getBool("withPlayers", "Browser") ? Qt::Checked : Qt::Unchecked);
    mw.cBBoxWOPass->setCheckState(browserMgr.getBool("noPassword", "Browser") ? Qt::Checked : Qt::Unchecked);

    mw.tblServerBrowser->sortByColumn(browserMgr.getInt("sortByCol", "Browser"),
                                      browserMgr.getBool("sortByColAscending", "Browser") ? Qt::AscendingOrder : Qt::DescendingOrder);

}

void SettingsMgr::saveBrowserSettings(MainWindow &mw)
{
    browserMgr.setInt("maxLatency", "Browser", mw.comboLatency->currentIndex());
    browserMgr.setString("gameMode", "Browser", mw.leGamemode->text().toStdString());
    browserMgr.setBool("notFull", "Browser", mw.cBoxNotFull->checkState() == Qt::Checked);
    browserMgr.setBool("withPlayers", "Browser", mw.cBoxWithPlayers->checkState() == Qt::Checked);
    browserMgr.setBool("noPassword", "Browser", mw.cBBoxWOPass->checkState() == Qt::Checked);

    browserMgr.setInt("sortByCol", "Browser", mw.tblServerBrowser->horizontalHeader()->sortIndicatorSection());
    browserMgr.setBool("sortByColAscending", "Browser", mw.tblServerBrowser->horizontalHeader()->sortIndicatorOrder() == Qt::AscendingOrder);

    browserMgr.saveUser(browserCfg);
}

void SettingsMgr::loadClientSettings(Ui::DialogSettings &mw)
{
    mw.leClientAddress->setText(QString::fromStdString(clientMgr.getString("destinationAddress", "General")));
    mw.leClientPort->setText(QString::fromStdString(clientMgr.getString("port", "General")));
    mw.leClientPassword->setText(QString::fromStdString(clientMgr.getString("password", "General")));
    mw.combLoglevel->setCurrentIndex(clientMgr.getInt("logLevel", "General"));

    mw.pbChatKey->setText(QString::fromStdString(clientMgr.getString("keySay", "Chat")));
    mw.pbModeKey->setText(QString::fromStdString(clientMgr.getString("keyChatMode", "Chat")));

    mw.sbPosX->setValue(clientMgr.getInt("x", "Chat"));
    mw.sbPosY->setValue(clientMgr.getInt("y", "Chat"));
    mw.sbPosW->setValue(clientMgr.getInt("w", "Chat"));
    mw.sbPosH->setValue(clientMgr.getInt("h", "Chat"));

    mw.sbDelay->setValue(clientMgr.getFloat("delay", "Chat"));
}

void SettingsMgr::saveClientSettings(Ui::DialogSettings &mw)
{
    clientMgr.setString("destinationAddress", "General", mw.leClientAddress->text().toStdString());
    clientMgr.setString("port", "General", mw.leClientPort->text().toStdString());
    clientMgr.setString("password", "General", mw.leClientPassword->text().toStdString());
    clientMgr.setInt("logLevel", "General", mw.combLoglevel->currentIndex());

    clientMgr.setString("keySay", "Chat", mw.pbChatKey->text().toStdString());
    clientMgr.setString("keyChatMode", "Chat", mw.pbModeKey->text().toStdString());

    clientMgr.setInt("x", "Chat", mw.sbPosX->value());
    clientMgr.setInt("y", "Chat", mw.sbPosY->value());
    clientMgr.setInt("w", "Chat", mw.sbPosW->value());
    clientMgr.setInt("h", "Chat", mw.sbPosH->value());

    clientMgr.setFloat("delay", "Chat", mw.sbDelay->value());

    clientMgr.saveUser(clientCfg);

}

bool SettingsMgr::isPreRewrite()
{
    static int preRewrite = -1;

    if (preRewrite == -1)
    {
        preRewrite = 0;

        try
        {
            serverMgr.getString("home", "Modules");
        }
        catch(...)
        {
            preRewrite = 1;
        }
    }

    return preRewrite == 1;
}

void SettingsMgr::loadServerSettings(Ui::DialogSettings &mw)
{
    mw.leServerAddress->setText(QString::fromStdString(serverMgr.getString("localAddress", "General")));
    mw.leServerPort->setText(QString::fromStdString(serverMgr.getString("port", "General")));
    mw.sbMaxPlayers->setValue(serverMgr.getInt("maximumPlayers", "General"));
    mw.leHostname->setText(QString::fromStdString(serverMgr.getString("hostname", "General")));
    mw.combServerLoglevel->setCurrentIndex(serverMgr.getInt("logLevel", "General"));
    mw.leServerPassword->setText(QString::fromStdString(serverMgr.getString("password", "General")));


    if (!isPreRewrite())
    {
        mw.leModulePath->setText(QString::fromStdString(serverMgr.getString("home", "Modules")));
        mw.chbAutosort->setCheckState(serverMgr.getBool("autoSort", "Modules") ? Qt::Checked : Qt::Unchecked);
    }
    else
    {
        mw.chbAutosort->setVisible(false);
        mw.gbModules->setTitle("Plugins");

        QString pluginsStr = QString::fromStdString(serverMgr.getString("plugins", "Plugins"));
        mw.listModules->addItems(pluginsStr.split(",", QString::SkipEmptyParts));

        mw.leModulePath->setText(QString::fromStdString(serverMgr.getString("home", "Plugins")));
    }

}

void SettingsMgr::saveServerSettings(Ui::DialogSettings &mw)
{
    serverMgr.setString("localAddress", "General", mw.leServerAddress->text().toStdString());
    serverMgr.setString("port", "General", mw.leServerPort->text().toStdString());
    serverMgr.setInt("maximumPlayers", "General", mw.sbMaxPlayers->value());
    serverMgr.setString("hostname", "General", mw.leHostname->text().toStdString());
    serverMgr.setInt("logLevel", "General", mw.combServerLoglevel->currentIndex());
    serverMgr.setString("password", "General", mw.leServerPassword->text().toStdString());

    if(!isPreRewrite())
    {
        serverMgr.setString("home", "Modules", mw.leModulePath->text().toStdString());
        serverMgr.setBool("autoSort", "Modules", mw.chbAutosort->checkState() == Qt::Checked);
    }
    else
    {
        QString plugins;
        for(int i = 0; i <  mw.listModules->count(); ++i)
        {
            plugins += mw.listModules->item(i)->text();
            if (i < mw.listModules->count() - 1)
                plugins += ",";
        }

        serverMgr.setString("plugins", "Plugins", plugins.toStdString());
        serverMgr.setString("home", "Plugins", mw.leModulePath->text().toStdString());
    }

    serverMgr.saveUser(serverCfg);
}
