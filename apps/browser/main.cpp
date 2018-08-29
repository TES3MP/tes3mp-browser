#include <QApplication>
#include <QResource>
#include <components/settings/settings.hpp>
#include <components/files/configurationmanager.hpp>
#include "MainWindow.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
    qRegisterMetaType<Server>("Server");
    qRegisterMetaType<ServerExtra>("ServerExtra");

    QResource::registerResource("/files/tes3mp/browser.qrc");


    QApplication app(argc, argv);
    MainWindow d;

    d.show();
    return QApplication::exec();
}
