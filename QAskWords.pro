#-------------------------------------------------
#
# Project created by QtCreator 2012-07-22T11:17:22
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QAskWords
TEMPLATE = app
MOC_DIR = compile
OBJECTS_DIR = compile
RCC_DIR = compile
UI_DIR = compile

SOURCES += src/main.cpp\
  src/mainwindow.cpp \
    src/itemdelegate.cpp

HEADERS += src/mainwindow.h \
    src/itemdelegate.h

FORMS += src/mainwindow.ui

TRANSLATIONS += translations/qaskwords_es.ts\
  translations/qaskwords_gl.ts

OTHER_FILES += README.md

# INSTALL
isEmpty(PREFIX):PREFIX = /usr
target.path = $$PREFIX/bin/