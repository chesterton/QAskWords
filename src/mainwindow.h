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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QActionGroup;
class QCloseEvent;
//class QResizeEvent;
//class QShowEvent;

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  enum Encoding {
    UTF_8 = 0,
    ISO_8859_1 = 1
  };

  Ui::MainWindow *ui;
  QActionGroup *actionGroupEncoding;
  QString csvFolder;
  QList<QStringList> questionsAndAnswers;
  QList<int> questionsAlreadyAsked;
  int encoding;
  QString language;
  int questionNumber;
  int questionId;
  int correctAnswers;

  void createObjects();
  void createConnections();

private slots:
  void activateButtons(bool activate = true);
  void updateStatusBar(bool finished = false);
  void populateComboCsvFile();

  void start();
  void stop();
  bool readQuestionsAndAnswers();
  void showNextQuestion();
  void selectQuestion();
  void acceptAnswerClicked();
  bool checkAnswer();
  void addRow();
  QString textResult(int n);

  void showAbout();
  void showAboutQt();
  void selectCsvFolder();
  void openCsvFolder();
  void resetCsvFolder();
  void setEncoding(QAction *action);
  void selectLanguage();
  void readSettings();
  void writeSettings();

protected:
  void closeEvent(QCloseEvent *event);
  //void resizeEvent(QResizeEvent *event);
  //void showEvent(QShowEvent *event);
};

#endif // MAINWINDOW_H
