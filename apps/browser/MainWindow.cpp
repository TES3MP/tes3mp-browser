//
// Created by koncord on 06.01.17.
//

#include "MainWindow.hpp"
#include "QueryHelper.hpp"
#include "PingHelper.hpp"
#include "ServerInfoDialog.hpp"
#include "SettingsWindow.hpp"
#include "Settings.hpp"

#include "components/files/configurationmanager.hpp"
#include <qdebug.h>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QJsonDocument>
#include <QtWidgets/QFileDialog>
#include <QStatusBar>
#include <QDesktopServices>
#include <QPainter>

#include "netutils/Utils.hpp"
#include "netutils/MasterClient.hpp"

using namespace Process;
using namespace std;

class ImageButton final: public QAbstractButton
{
public:
    ImageButton(QWidget *parent) : QAbstractButton(parent) { setCursor(Qt::PointingHandCursor); }

    void setPixmap(const QPixmap &pm)
    {
        pixmap = pm;
        update();
    }

    QSize sizeHint() const override { return pixmap.size(); }

protected:
    void paintEvent(QPaintEvent *e) override
    {
        QPainter painter(this);
        painter.drawPixmap(0, 0, pixmap);
    }
private:
    QPixmap pixmap;
};

MainWindow::MainWindow(QWidget *parent)
{
    setupUi(this);

    MasterClient::create(this);

    mGameInvoker = new ProcessInvoker();

    browser = new ServerModel;
    favorites = new ServerModel;
    proxyModel = new MySortFilterProxyModel(this);
    proxyModel->setSourceModel(browser);
    tblServerBrowser->setModel(proxyModel);
    tblFavorites->setModel(proxyModel);

    tblServerBrowser->hideColumn(Server::ADDR);
    tblFavorites->hideColumn(Server::ADDR);
    tblServerBrowser->hideColumn(Server::PORT);
    tblFavorites->hideColumn(Server::PORT);
    tblServerBrowser->hideColumn(Server::ID);
    tblFavorites->hideColumn(Server::ID);

    PingHelper::Get().SetModel((ServerModel *) proxyModel->sourceModel());
    queryHelper = new QueryHelper(proxyModel->sourceModel());
    connect(queryHelper, &QueryHelper::started, [this]() { actionRefresh->setEnabled(false); });
    connect(queryHelper, &QueryHelper::finished, [this]() { actionRefresh->setEnabled(true); });

    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSwitched(int)));
    connect(actionAdd, SIGNAL(triggered(bool)), this, SLOT(addServer()));
    connect(actionDelete, SIGNAL(triggered(bool)), this, SLOT(deleteServer()));
    connect(actionRefresh, SIGNAL(triggered(bool)), queryHelper, SLOT(refresh()));
    connect(actionPlay, SIGNAL(triggered(bool)), this, SLOT(play()));
    connect(tblServerBrowser, SIGNAL(clicked(QModelIndex)), this, SLOT(serverSelected()));
    connect(tblFavorites, SIGNAL(clicked(QModelIndex)), this, SLOT(serverSelected()));
    connect(tblFavorites, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(play()));
    connect(tblServerBrowser, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(play()));
    connect(cBoxNotFull, SIGNAL(toggled(bool)), this, SLOT(notFullSwitch(bool)));
    connect(cBoxWithPlayers, SIGNAL(toggled(bool)), this, SLOT(havePlayersSwitch(bool)));
    connect(cBBoxWOPass, SIGNAL(toggled(bool)), this, SLOT(noPasswordSwitch(bool)));
    connect(comboLatency, SIGNAL(currentIndexChanged(int)), this, SLOT(maxLatencyChanged(int)));
    connect(leGamemode, SIGNAL(textChanged(const QString &)), this, SLOT(gamemodeChanged(const QString &)));

    settingsWindow = new SettingsWindow(this);

    connect(actionSettings, &QAction::triggered, [&](bool v) {
        settingsWindow->exec();
    });

    SettingsMgr::get().loadBrowserSettings(*this);
    MasterClient::get()->address(masterAddress);

    FIX_UNTIL(make_version(2, 0), "Favorites list should be implemented");
    {
        tabWidget->removeTab(1);
        //loadFavorites();
    }
    queryHelper->refresh();

    FIX_UNTIL(make_version(2, 0), "Account system should be implemented");
    {
        //lblSignStatus = new QLabel("Not signed in");
        //statusBar()->addPermanentWidget(lblSignStatus);
        actionAccount->setVisible(false);
        for (auto action : toolBar->actions()) // hacky way to delete first separator
        {
            if (action->isSeparator())
            {
                toolBar->removeAction(action);
                break;
            }
        }
    }

    lblBrowserVersion = new QLabel(QString::fromStdString(strVersion()));


    statusBar()->addPermanentWidget(lblBrowserVersion);

    /*QPixmap img("/home/koncord/GH/banner.png");
    setMotdImage(&img, "https://master.tes3mp.com");*/

    connect(MasterClient::get(), &MasterClient::latestVersion, [this](const QString &version){
        if (version.isEmpty())
            return;
        QStringList ver = version.split('.');
        int major = ver[0].toInt();
        int minor = ver[1].toInt();
        if (major > MAJOR_VERSION || minor > MINOR_VERSION)
            setMotdHTML(R"(<p>Update for browser <a href="https://)" + masterAddress +
                        R"(/browser/latest">available</a></p>)");
    });
    MasterClient::get()->requestLatestVersionStr();

}

MainWindow::~MainWindow()
{
    SettingsMgr::get().saveBrowserSettings(*this);
    delete mGameInvoker;
}

void MainWindow::addServerAndUpdate(const QString &addr)
{
    favorites->insertRow(0);
    QModelIndex mi = favorites->index(0, Server::ADDR);
    favorites->setData(mi, addr, Qt::EditRole);
    /*auto address = addr.split(":");
    auto data = getExtendedData(address[0].toLatin1(), address[1].toUShort());*/

    //NetController::get()->updateInfo(favorites, mi);
    //QueryClient::Update(RakNet::SystemAddress())
    /*auto data = QueryClient::Get().Query();
    if (data.empty())
       return;
    transform(data.begin(), data.end(), back_inserter());*/
}

void MainWindow::addServer()
{
    int id = tblServerBrowser->selectionModel()->currentIndex().row();

    if (id >= 0)
    {
        int sourceId = proxyModel->mapToSource(proxyModel->index(id, Server::ADDR)).row();
        favorites->myData.push_back(browser->myData[sourceId]);
    }
}

void MainWindow::deleteServer()
{
    if (tabWidget->currentIndex() != 1)
        return;
    int id = tblFavorites->selectionModel()->currentIndex().row();
    if (id >= 0)
    {
        int sourceId = proxyModel->mapToSource(proxyModel->index(id, Server::ADDR)).row();
        favorites->removeRow(sourceId);
        if (favorites->myData.isEmpty())
        {
            actionPlay->setEnabled(false);
            actionDelete->setEnabled(false);
        }
    }
}

void MainWindow::play()
{
    QTableView *curTable = tabWidget->currentIndex() ? tblFavorites : tblServerBrowser;
    int id = curTable->selectionModel()->currentIndex().row();
    if (id < 0)
        return;


    ServerModel *sm = ((ServerModel*)proxyModel->sourceModel());

    int sourceId = proxyModel->mapToSource(proxyModel->index(id, Server::ADDR)).row();
    ServerInfoDialog infoDialog(sm->myData[sourceId], this);

    if (!infoDialog.exec())
        return;

    if (!infoDialog.isUpdated())
        return;

    QStringList arguments;
    arguments.append(QString::fromStdString("--connect=" + sm->myData[sourceId].address).toLatin1());

    if (sm->myData[sourceId].password)
    {
        bool ok;
        QString passw = QInputDialog::getText(this, tr("Connecting to: ") + QString::fromStdString(sm->myData[sourceId].address), tr("Password: "),
                                              QLineEdit::Password, "", &ok);
        if (!ok)
            return;
        arguments.append(QLatin1String("--password=") + passw.toLatin1());
    }

    if (mGameInvoker->startProcess(QLatin1String("tes3mp"), arguments, true))
        return qApp->quit();
}

void MainWindow::tabSwitched(int index)
{
    if (index == 0)
    {
        proxyModel->setSourceModel(browser);
        actionDelete->setEnabled(false);
    }
    else
    {
        proxyModel->setSourceModel(favorites);
    }
    actionPlay->setEnabled(false);
    actionAdd->setEnabled(false);
}

void MainWindow::serverSelected()
{
    actionPlay->setEnabled(true);
    if (tabWidget->currentIndex() == 0)
        actionAdd->setEnabled(true);
    if (tabWidget->currentIndex() == 1)
        actionDelete->setEnabled(true);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Files::ConfigurationManager cfgMgr;
    QString cfgPath = QString::fromStdString((cfgMgr.getUserConfigPath() / "favorites.dat").string());

    QJsonArray saveData;
    for (auto server : favorites->myData)
        saveData.push_back(QString::fromStdString(server.address));

    QFile file(cfgPath);

    if (!file.open(QIODevice::WriteOnly))
    {
        qDebug() << "Cannot save " << cfgPath;
        return;
    }

    file.write(QJsonDocument(saveData).toJson());
    file.close();
}


void MainWindow::loadFavorites()
{
    Files::ConfigurationManager cfgMgr;
    QString cfgPath = QString::fromStdString((cfgMgr.getUserConfigPath() / "favorites.dat").string());

    QFile file(cfgPath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open " << cfgPath;
        return;
    }

    QJsonDocument jsonDoc(QJsonDocument::fromJson(file.readAll()));

    for (auto server : jsonDoc.array())
        addServerAndUpdate(server.toString());

    file.close();
}

void MainWindow::notFullSwitch(bool state)
{
    proxyModel->filterFullServer(state);
}

void MainWindow::havePlayersSwitch(bool state)
{
    proxyModel->filterEmptyServers(state);
}

void MainWindow::noPasswordSwitch(bool state)
{
    proxyModel->filterPassworded(state);
}

void MainWindow::maxLatencyChanged(int index)
{
    int maxLatency = index * 50;
    proxyModel->pingLessThan(maxLatency);

}

void MainWindow::gamemodeChanged(const QString &text)
{
    proxyModel->setFilterFixedString(text);
    proxyModel->setFilterKeyColumn(Server::MODNAME);
}


void MainWindow::setMotdHTML(const QString &data)
{
    auto lbl = new QLabel(this);

    lbl->setText(data);
    lbl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lbl->setOpenExternalLinks(true);
    lbl->setTextFormat(Qt::RichText);
    lbl->setAlignment(Qt::AlignCenter);

    setMotdWidget(lbl);
}

void MainWindow::setMotdImage(QPixmap *image, const QString &link)
{
    auto btn = new ImageButton(this);

    if (image == nullptr)
    {
        btn->deleteLater();
        if (!link.isEmpty())
            setMotdHTML("<p><a href=\"" + link + "\">" + link + "</a></p>");
        return;
    }

    btn->setPixmap(image->scaled(728, 60));

    connect(btn, &QAbstractButton::released, this, [link]() {
        QDesktopServices::openUrl(link);
    });

    setMotdWidget(btn);
}

void MainWindow::setMotdWidget(QWidget *widget)
{
    motdWidget->deleteLater();
    motdWidget = widget;
    motdWidget->setFixedSize(QSize(728, 60));
    motdLayout->addWidget(motdWidget, 0, Qt::AlignHCenter);
}
