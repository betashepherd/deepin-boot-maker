/*
main.cpp from UNetbootin <http://unetbootin.sourceforge.net>
Copyright (C) 2007-2008 Geza Kovacs <geza0kovacs@gmail.com>

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License at <http://www.gnu.org/licenses/> for more details.
*/
#include "usbcreator.h"
#include "unetbootin.h"
#include "qml/Deepin/Widgets/plugins/dwindow.h"
#include "qml/Deepin/Widgets/plugins/dicon.h"

#include <QQuickView>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlApplicationEngine>


#include <QtGui>
#include <QtWidgets/QApplication>

QString checkforgraphicalsu(QString graphicalsu)
{
	QProcess whereiscommand;
	whereiscommand.start(QString("which %1").arg(graphicalsu));
	whereiscommand.waitForFinished(-1);
	QString commandbinpath = QString(whereiscommand.readAll()).trimmed();
	if (!commandbinpath.isEmpty() && QFile::exists(commandbinpath))
		return commandbinpath;
	else
		return "REQCNOTFOUND";
}
void loadTranslate(QApplication& app) {
    QTranslator custranldr;
    QTranslator translator;
    QString tnapplang;
    QString tnappcoun;
    QString clangcode = "";
    QStringList allappargs = app.arguments();
    QList<QPair<QString, QString> > oppairs;
    for (QList<QString>::const_iterator i = allappargs.constBegin(); i < allappargs.constEnd(); ++i) {
        if (i->count('=') == 1) {
            oppairs.append(QPair<QString, QString>(i->section('=', 0, 0).simplified(), i->section('=',1, 1).simplified()));
        }
    }
    for (QList<QPair<QString, QString> >::const_iterator i = oppairs.constBegin(); i < oppairs.constEnd(); ++i) {
        if (i->first.contains("lang", Qt::CaseInsensitive))
        {
            clangcode = i->second;
            tnapplang = clangcode.left(2);
            if (clangcode.contains('_') && clangcode.size() == 5)
            {
                tnappcoun = clangcode.section('_', -1, -1);
            }
            break;
        }
    }
    if (clangcode.isEmpty())
    {
        clangcode = QLocale::system().name();
        tnapplang = clangcode.left(2);
        if (clangcode.contains('_') && clangcode.size() == 5)
        {
            tnappcoun = clangcode.section('_', -1, -1);
        }
    }

    QDir applocdir(app.applicationDirPath());
    QStringList applocfiles = applocdir.entryList(QStringList() << "*.qm", QDir::Files);
    if (!applocfiles.isEmpty())	{
        QString custqmfilepath = applocfiles.at(0);
        if (!applocfiles.filter("unetbootin").isEmpty()) {
            custqmfilepath = applocfiles.filter("unetbootin").at(0);
            if (!applocfiles.filter("unetbootin").filter(tnapplang).isEmpty())			{
                custqmfilepath = applocfiles.filter("unetbootin").filter(tnapplang).at(0);
                if (!tnappcoun.isEmpty() && !applocfiles.filter("unetbootin").filter(tnapplang).filter(tnappcoun).isEmpty())
                    custqmfilepath = applocfiles.filter("unetbootin").filter(tnapplang).filter(tnappcoun).at(0);
            }
        }
        if (custranldr.load(custqmfilepath, app.applicationDirPath()))
            app.installTranslator(&custranldr);
    }

    if (!tnappcoun.isEmpty() && QFile::exists(QString("%1/unetbootin_%2_%3.qm").arg(app.applicationDirPath()).arg(tnapplang).arg(tnappcoun)) && translator.load(QString("%1/unetbootin_%2_%3.qm").arg(app.applicationDirPath()).arg(tnapplang).arg(tnappcoun)))
    {
        app.installTranslator(&translator);
    }
    else if (!tnappcoun.isEmpty() && QFile::exists(QString(":/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)) && translator.load(QString(":/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)))
    {
        app.installTranslator(&translator);
    }
    else if (!tnappcoun.isEmpty() && QFile::exists(QString("/usr/share/unetbootin/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)) && translator.load(QString("/usr/share/unetbootin/unetbootin_%1_%2.qm").arg(tnapplang).arg(tnappcoun)))
    {
        app.installTranslator(&translator);
    }
    else if (QFile::exists(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)) && translator.load(QString("%1/unetbootin_%2.qm").arg(app.applicationDirPath(), tnapplang)))
    {
        app.installTranslator(&translator);
    }
    else if (QFile::exists(QString(":/unetbootin_%1.qm").arg(tnapplang)) && translator.load(QString(":/unetbootin_%1.qm").arg(tnapplang)))
    {
        app.installTranslator(&translator);
    }
    else if (QFile::exists(QString("/usr/share/unetbootin/unetbootin_%1.qm").arg(tnapplang)) && translator.load(QString("/usr/share/unetbootin/unetbootin_%1.qm").arg(tnapplang)))
    {
        app.installTranslator(&translator);
    }
    else
    {
        tnapplang = "en";
        tnappcoun = "US";
        clangcode = "en_US";
    }
    app.installTranslator(&translator);
}

int main(int argc, char **argv)
{
    QApplication app(argc, argv, true);

    loadTranslate(app);

    if (QObject::tr("LeftToRight") == "RightToLeft") {
        app.setLayoutDirection(Qt::RightToLeft);
    }

#ifdef Q_OS_UNIX
    bool disabledrootcheck = false;
    disabledrootcheck = true;
    QStringList allappargs = app.arguments();
	if (!disabledrootcheck)
	{
		QProcess whoamip;
		whoamip.start("whoami");
		whoamip.waitForFinished();
		if (QString(whoamip.readAll()).remove("\r").remove("\n") != "root")
		{
			QString argsconc = "";
            QString argsconcSingleQuote = "";
			for (int i = 1; i < allappargs.size(); ++i)
			{
				argsconc += QString("\"%1\" ").arg(allappargs.at(i));
                argsconcSingleQuote += QString("'%1' ").arg(allappargs.at(i));
			}
            argsconc += "\"rootcheck=no\"";
            argsconcSingleQuote += "'rootcheck=no'";
#ifdef Q_OS_LINUX
			QString gksulocation = checkforgraphicalsu("gksu");
			if (gksulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(gksulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString kdesulocation = checkforgraphicalsu("kdesu");
			if (kdesulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(kdesulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString gnomesulocation = checkforgraphicalsu("gnomesu");
			if (gnomesulocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(gnomesulocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QString kdesudolocation = checkforgraphicalsu("kdesudo");
			if (kdesudolocation != "REQCNOTFOUND")
			{
				QProcess::startDetached(QString("%1 %2 %3").arg(kdesudolocation).arg(app.applicationFilePath()).arg(argsconc));
				return 0;
			}
			QMessageBox rootmsgb;
			rootmsgb.setIcon(QMessageBox::Warning);
			rootmsgb.setWindowTitle(uninstaller::tr("Must run as root"));
			rootmsgb.setTextFormat(Qt::RichText);
			rootmsgb.setText(uninstaller::tr("%2 must be run as root. Close it, and re-run using either:<br/><b>sudo %1</b><br/>or:<br/><b>su - -c '%1'</b>").arg(app.applicationFilePath()).arg(UNETBOOTINB));
			rootmsgb.setStandardButtons(QMessageBox::Ok);
			switch (rootmsgb.exec())
			{
				case QMessageBox::Ok:
					break;
				default:
					break;
			}
#endif
#ifdef Q_OS_MAC
            /*
			QProcess osascriptProc;
			osascriptProc.start("osascript");
			osascriptProc.write(QString("do shell script \""+app.applicationFilePath()+"\" with administrator privileges\n").toAscii().data());
			osascriptProc.closeWriteChannel();
			osascriptProc.waitForFinished(-1);
            */
            //qDebug() << QString("osascript -e 'do shell script \"%1 %2\" with administrator privileges'").arg(app.applicationFilePath()).arg(argsconc);
            //QProcess::startDetached(QString("osascript -e 'do shell script \"%1 %2\" with administrator privileges'").arg(app.applicationFilePath()).arg(argsconc));
            QProcess::startDetached("osascript", QStringList() << "-e" << QString("do shell script \"'%1' %2\" with administrator privileges").arg(app.applicationFilePath()).arg(argsconcSingleQuote));
            return 0;
#endif
		}
	}
	#endif

//    unetbootinPtr->appNlang = tnapplang;
//    unetbootinPtr->appDir = QDir::toNativeSeparators(QString("%1/").arg(app.applicationDirPath()));
//    unetbootinPtr->appLoc = app.applicationFilePath();
    app.addLibraryPath("qrc:/lib/");
    app.addLibraryPath("/home/iceyer/dev/linuxdeepin/deepin-usb-creator/");

    qmlRegisterType<UsbCreator>("com.deepin.usbcreator", 1, 0, "UsbCreator");
    qmlRegisterType<DOverrideWindow>("com.deepin.usbcreator", 1, 0, "DOverrideWindow");
    qmlRegisterType<DWindow>("com.deepin.usbcreator", 1, 0, "DWindow");
    qmlRegisterType<DIcon>("com.deepin.usbcreator", 1, 0, "DIcon");

    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/qml/");
    engine.load(QUrl("qrc:/qml/main.qml"));

    QList<QObject *> roots = engine.rootObjects();
    QObject *topLevel = roots.value(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    if (!window) {
        QMessageBox unotenoughinputmsgb;
        unotenoughinputmsgb.setIcon(QMessageBox::Information);
        unotenoughinputmsgb.setWindowTitle(("Insert a USB flash drive"));
        QString info = QString("%1").arg(qulonglong(window));
        unotenoughinputmsgb.setText(info);
        unotenoughinputmsgb.setStandardButtons(QMessageBox::Ok);
        switch (unotenoughinputmsgb.exec())
        {
            case QMessageBox::Ok:
                break;
            default:
                break;
        }
    }

    QIcon icon;
    icon.addFile("qrc:/unetbootin_16.png", QSize(16,16));
    icon.addFile("qrc:/unetbootin_22.png", QSize(22,22));
    icon.addFile("qrc:/unetbootin_24.png", QSize(24,24));
    icon.addFile("qrc:/unetbootin_32.png", QSize(32,32));
    icon.addFile("qrc:/unetbootin_48.png", QSize(48,48));
    icon.addFile("qrc:/deepin-usb-creator.png");
#ifdef Q_OS_LINUX
	icon.addFile("/usr/share/pixmaps/unetbootin.png");
	icon.addFile("/usr/share/pixmaps/unetbootin.xpm");
#endif
    window->setIcon(icon);
    window->show();

    return app.exec();
}