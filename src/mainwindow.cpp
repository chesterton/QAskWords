#include "mainwindow.h"
#include "../compile/ui_mainwindow.h"
#include <QActionGroup>
#include <QCloseEvent>
#include <QDir>
#include <QTextStream>
#include <QTime>
#include <QSettings>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  createObjects();
  createConnections();

  readSettings();

  ui->labelQuestion->hide();
  activateButtons();

  //TODO: game working
  //TODO: more options
  //TODO: about, about qt
  //TODO: compile for windows
  //TODO: editor
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::createObjects()
{
  actionGroupEncoding = new QActionGroup(this);
  actionGroupEncoding->addAction(ui->actionUTF_8);
  actionGroupEncoding->addAction(ui->actionISO_8859_1);

  //add filenames found in csv folder to the combobox
  ui->comboCategory->insertItems(0, QDir("csv").entryList(QStringList("*.csv"), QDir::Files, QDir::Name).replaceInStrings(".csv", ""));

  ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui->tableWidget->verticalHeader()->hide();
  QStringList headerLabels(QString(tr("Answer,Translations,Original")).split(","));
  int headerLabelsCount = headerLabels.count();
  ui->tableWidget->setColumnCount(headerLabelsCount);
  ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
  for(int i = 0; i < headerLabelsCount; i++)
    ui->tableWidget->setColumnWidth(i, 192);
}

void MainWindow::createConnections()
{
  connect(actionGroupEncoding, SIGNAL(triggered(QAction*)), this, SLOT(setEncoding(QAction*)));

  connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->buttonStart, SIGNAL(clicked()), this, SLOT(start()));
  connect(ui->buttonStop, SIGNAL(clicked()), this, SLOT(stop()));
  connect(ui->buttonAcceptAnswer, SIGNAL(clicked()), this, SLOT(acceptAnswerClicked()));
}

void MainWindow::activateButtons(bool activate)
{
  ui->buttonStart->setEnabled(activate);
  ui->buttonStop->setEnabled(!activate);
  ui->edAnswer->setEnabled(!activate);
  ui->buttonAcceptAnswer->setEnabled(!activate);
  ui->spinBoxQuestions->setEnabled(activate);
  ui->comboCategory->setEnabled(activate);
}

void MainWindow::updateStatusBar()
{
  int questions = ui->spinBoxQuestions->value();
  int wrongAnswers = (questionNumber - 1) - correctAnswers;
  int successRate = (questionNumber - 1) > 0 ? correctAnswers * 100 / (questionNumber - 1) : 0;
  QString results = tr("Question ") + QString::number(questionNumber) + "/" + QString::number(questions)
    + tr(" (Correct: ") + QString::number(correctAnswers) + tr(", Wrong: ") + QString::number(wrongAnswers)
    + tr(", Success rate: ") + QString::number(successRate) + " %)";
  ui->statusBar->showMessage(results);
  //TODO: add progress bar
}

void MainWindow::start()
{
  if(!readQuestionsAndAnswers())
    return;

  if(questionsAndAnswers.count() < ui->spinBoxQuestions->value()) {
    QMessageBox::warning(this, tr("Error"),
      tr("Error: the number of questions should be at most the number of questions found in the selected file (")
      + QString::number(questionsAndAnswers.count()) + tr(" questions)"));
    return;
  }

  questionNumber = 0;
  correctAnswers = 0;
  qsrand(QTime::currentTime().msec());
  ui->tableWidget->setRowCount(0);

  ui->labelQuestion->show();
  activateButtons(false);
  ui->edAnswer->setFocus();

  showNextQuestion();
}

void MainWindow::stop()
{
  int questions = ui->spinBoxQuestions->value();
  int wrongAnswers = questionNumber - correctAnswers;
  int successRate = questionNumber > 0 ? correctAnswers * 100 / questionNumber : 0;
  QString results = tr("Question ") + QString::number(questionNumber) + "/" + QString::number(questions)
    + tr(" (Correct: ") + QString::number(correctAnswers) + tr(", Wrong: ") + QString::number(wrongAnswers)
    + tr(", Success rate: ") + QString::number(successRate) + " %)";
  QMessageBox::information(this, tr("Final results"), results);
  ui->labelQuestion->hide();
  activateButtons();
}

bool MainWindow::readQuestionsAndAnswers()
{
  questionsAndAnswers.clear();
  questionsAlreadyAsked.clear();

  QString fileName = "csv/" + ui->comboCategory->currentText() + ".csv";
  QFile file(fileName);
  if(!file.open(QIODevice::ReadOnly  | QIODevice::Text)) {
    QMessageBox::warning(this, tr("Error"), tr("Error opening file ") + fileName + ": " + file.errorString());
    return false;
  }

  QTextStream in(&file);
  in.setCodec(encoding == UTF_8 ? "UTF-8" : "ISO 8859-1");

  while(!in.atEnd())
    questionsAndAnswers.append(in.readLine().split(";").replaceInStrings("\"", "").filter(QRegExp(".")));

  //trim all strings in the list
  int questionsAndAnswersCount = questionsAndAnswers.count();
  for(int i = 0; i < questionsAndAnswersCount; i++) {
    int questionsAndAnswersICount = questionsAndAnswers[i].count();
    for(int j = 0; j < questionsAndAnswersICount; j++) {
      questionsAndAnswers[i][j] = questionsAndAnswers[i][j].trimmed();
    }
  }

  file.close();

  return true;
}

void MainWindow::showNextQuestion()
{
  ui->buttonAcceptAnswer->setEnabled(true);
  ui->edAnswer->clear();

  if(questionNumber < ui->spinBoxQuestions->value()) {
    questionNumber++;
    selectQuestion();
    int posibleAnswers = questionsAndAnswers[questionId].count() - 1;
    ui->labelQuestion->setText(questionsAndAnswers[questionId][0] + (posibleAnswers > 1 ? " (" + QString::number(posibleAnswers) + ")" : ""));
    updateStatusBar();
  }
  else {
    stop();
  }
}

void MainWindow::selectQuestion()
{
  int i = 0;
  do {
    questionId = qrand() % questionsAndAnswers.length();
    i++;
  } while(questionsAlreadyAsked.indexOf(questionId) != -1 && i < 100000);
  questionsAlreadyAsked.append(questionId);
}

void MainWindow::acceptAnswerClicked()
{
  ui->buttonAcceptAnswer->setEnabled(false);

  if(checkAnswer()) {
    correctAnswers++;
    if(!ui->actionShowOnlyWrongAnswers->isChecked())
      addRow();
  }
  else {
    addRow();
    if(!ui->actionShowOnlyWrongAnswers->isChecked()) {
      int count = ui->tableWidget->columnCount();
      for(int i = 0; i < count; i++)
        ui->tableWidget->item(0, i)->setTextColor(QColor("#dd0000"));
    }
  }

  showNextQuestion();
}

bool MainWindow::checkAnswer()
{
  //TODO: return answer correct or not
  if(ui->edAnswer->text().trimmed() == questionsAndAnswers[questionId][0])
    return true;
  else
    return false;
}

void MainWindow::addRow()
{
  ui->tableWidget->insertRow(0);
  ui->tableWidget->setItem(0, 0, new QTableWidgetItem(ui->edAnswer->text().trimmed()));
  int count = questionsAndAnswers[questionId].count();
  QString translations;
  for(int i = 1; i < count; i++)
    translations += questionsAndAnswers[questionId][i];
  ui->tableWidget->setItem(0, 1, new QTableWidgetItem(translations));
  ui->tableWidget->setItem(0, 2, new QTableWidgetItem(questionsAndAnswers[questionId][0]));
}

void MainWindow::setEncoding(QAction *action)
{
  if(action->objectName() == "actionUTF_8")
    encoding = UTF_8;
  else if(action->objectName() == "actionISO_8859_1")
    encoding = ISO_8859_1;
}

void MainWindow::readSettings()
{
  QSettings settings;
  settings.beginGroup("General");
  encoding = settings.value("encoding", UTF_8).toInt();
  if(encoding < UTF_8 || encoding > ISO_8859_1)
    encoding = UTF_8;
  actionGroupEncoding->actions()[encoding]->setChecked(true);
  ui->spinBoxQuestions->setValue(settings.value("questions", 50).toInt());
  ui->actionShowOnlyWrongAnswers->setChecked(settings.value("showOnlyWrongAnswers", false).toBool());
  setGeometry(settings.value("geometry", QRect(50, 50, 640, 480)).toRect());
  settings.endGroup();
}

void MainWindow::writeSettings()
{
  QSettings settings;
  settings.beginGroup("General");
  settings.setValue("encoding", encoding);
  settings.setValue("questions", ui->spinBoxQuestions->value());
  settings.setValue("showOnlyWrongAnswers", ui->actionShowOnlyWrongAnswers->isChecked());
  settings.setValue("geometry", geometry());
  settings.endGroup();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  event->accept();
}
