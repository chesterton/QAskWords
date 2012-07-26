/***************************************************************************
 *   Copyright (C) 2012 by Miguel Escudero Gestal
 *   miguelescudero@users.sourceforge.net
 *
 *   This file is part of QAskWords
 *
 *   QAskWords is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   QAskWords is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/

#include <QApplication>
#include <QLocale>
#include <QSettings>
#include <QTextCodec>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QCoreApplication::setOrganizationName("QAskWords");
  QCoreApplication::setOrganizationDomain("qaskwords.sourceforge.net");
  QCoreApplication::setApplicationName("QAskWords");

  //read settings to establish language
  QSettings settings;
  settings.beginGroup("General");
  QString language = settings.value("language", QLocale::system().name()).toString();
  settings.endGroup();

  QTranslator translator;
  if(!translator.load("qaskwords_" + language)) {
    if(!translator.load(QCoreApplication::applicationDirPath() + "/translations/qaskwords_" + language)) {
      translator.load("/usr/share/qaskwords/qaskwords_" + language);
    }
  }
  a.installTranslator(&translator);

  //configure the codec used by tr()
  QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));

  MainWindow w;
  w.show();
  
  return a.exec();
}
