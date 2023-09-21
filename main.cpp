/******************************************************************************
** Copyright (C) 2023 Efektivus project
** SPDX-License-Identifier: GPL-3.0-or-later
** https://spdx.org/licenses/GPL-3.0-or-later.html
*******************************************************************************
** Copyright (C) 2017 The Qt Company Ltd.
*******************************************************************************
** For more information, see the AUTHORS and CONTRIBUTORS files
******************************************************************************/

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
///
#include <QTranslator>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    //Q_INIT_RESOURCE(application);
#ifdef Q_OS_ANDROID
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    /// TODO: improve it
    QString locale = QLocale::system().name();
    QTranslator translator;
    //
    if (translator.load("plain-text-editor-efektivus_" + locale)) {
        app.installTranslator(&translator);
    }else if (translator.load("plain-text-editor-efektivus_" + locale, "/usr/share/plain-text-editor-efektivus/translations")) {
       app.installTranslator(&translator);
    }
    ///

    QCoreApplication::setOrganizationName("FolhiumProject");
    QCoreApplication::setApplicationName("plain-text-editor-efektivus");
    //QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    QCoreApplication::setApplicationVersion("0.0");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", "The file to open.");
    parser.process(app);

    TextEdit mainWin;
    if (!parser.positionalArguments().isEmpty())
        mainWin.loadFile(parser.positionalArguments().first());
    mainWin.show();
    return app.exec();
}
//! [0]
