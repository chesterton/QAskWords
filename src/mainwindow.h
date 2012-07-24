#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QActionGroup;
class QCloseEvent;

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
  int questionNumber;
  int questionId;
  int correctAnswers;

  void createObjects();
  void createConnections();

private slots:
  void activateButtons(bool activate = true);
  void updateStatusBar();
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
  void resetCsvFolder();
  void setEncoding(QAction *action);
  void readSettings();
  void writeSettings();

protected:
  void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
