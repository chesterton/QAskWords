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
