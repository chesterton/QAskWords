#include "mainwindow.h"
#include "../compile/ui_mainwindow.h"
#include <QActionGroup>
#include <QCloseEvent>
#include <QDir>
#include <QTextStream>
#include <QTime>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  createObjects();
  createConnections();

  readSettings();

  ui->labelQuestion->hide();
  populateComboCsvFile();
  activateButtons();

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
  connect(ui->actionSelectCsvFolder, SIGNAL(triggered()), this, SLOT(selectCsvFolder()));
  connect(ui->actionResetCsvFolder, SIGNAL(triggered()), this, SLOT(resetCsvFolder()));
  connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(showAbout()));
  connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(showAboutQt()));
  connect(ui->buttonStart, SIGNAL(clicked()), this, SLOT(start()));
  connect(ui->buttonStop, SIGNAL(clicked()), this, SLOT(stop()));
  connect(ui->buttonAcceptAnswer, SIGNAL(clicked()), this, SLOT(acceptAnswerClicked()));
}

void MainWindow::activateButtons(bool activate)
{
  bool comboItemSelected = ui->comboCsvFile->currentIndex() != -1;
  ui->buttonStart->setEnabled(activate && comboItemSelected);
  ui->buttonStop->setEnabled(!activate);
  ui->edAnswer->setEnabled(!activate);
  ui->buttonAcceptAnswer->setEnabled(!activate);
  ui->spinBoxQuestions->setEnabled(activate);
  ui->comboCsvFile->setEnabled(activate);
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

//add filenames found in csv folder to the combobox
void MainWindow::populateComboCsvFile()
{
  ui->comboCsvFile->clear();
  ui->comboCsvFile->insertItems(0, QDir(csvFolder).entryList(QStringList("*.csv"), QDir::Files, QDir::Name).replaceInStrings(".csv", ""));
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
  int wrongAnswers = questionNumber - correctAnswers;
  int successRate = questionNumber > 0 ? correctAnswers * 100 / questionNumber : 0;
  QString results = tr("Questions: ") + QString::number(questionNumber) + "\n"
    + tr("Correct: ") + QString::number(correctAnswers) + "\n"
    + tr("Wrong: ") + QString::number(wrongAnswers) + "\n"
    + tr("Success rate: ") + QString::number(successRate) + " %" + "\n"
    + textResult(successRate);
  QMessageBox::information(this, tr("Final results"), results);
  ui->labelQuestion->hide();
  activateButtons();
}

bool MainWindow::readQuestionsAndAnswers()
{
  questionsAndAnswers.clear();
  questionsAlreadyAsked.clear();

  QString fileName = csvFolder + ui->comboCsvFile->currentText() + ".csv";
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
  int translationsCount = questionsAndAnswers[questionId].count() - 1;
  QStringList answers = ui->edAnswer->text().split(",");
  int answersCount = answers.count();

  if(translationsCount != answersCount)
    return false;

  int correct = 0;
  for(int i = 1; i <= translationsCount; i++) {
    for(int j = 0; j < answersCount; j++) {
      if(answers[j].trimmed().compare(questionsAndAnswers[questionId][i], Qt::CaseInsensitive) == 0)
        correct++;
      if(correct == translationsCount)
        return true;
    }
  }
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

QString MainWindow::textResult(int n)
{
  if(n == 100)
    return tr("Perfect. You're the best!");
  else if(n >= 90)
    return tr("Almost perfect. You're great!");
  else if(n >= 75)
    return tr("Pretty good. You're going in the right direction.");
  else if(n >= 50)
    return tr("Acceptable. You need to practice a little more.");
  else if(n >= 35)
    return tr("Pretty bad. You need to practice a lot more.");
  else if(n >= 20)
    return tr("You're very bad, but don't despair. Some day you'll make it if you keep trying.");
  else
    return tr("Are you kidding me? I can't believe you're really so bad.");
}

void MainWindow::showAbout()
{
  QMessageBox::about(this, tr("About QAskWords"),
    tr("QAskWords is a simple application that makes words translation tests by reading csv files (sepearated by semicolons) containing rows with a word and one or more translations for that word.")
    + tr("Enjoy it and send comments, bug reports, or whatever you want to miguelescudero@users.sourceforge.net.") + "\n\n"
    + tr("Credits:") + "\n"
    + tr("Developer:") + " Miguel Escudero" + "\n"
    + tr("Spanish translation:") + " Miguel Escudero" + "\n"
    + tr("Galician translation:") + " Miguel Escudero" + "\n");
}

void MainWindow::showAboutQt()
{
  QMessageBox::aboutQt(this);
}

void MainWindow::selectCsvFolder()
{
  QString folder = QFileDialog::getExistingDirectory(this, tr("Select a folder containing CSV files."), csvFolder);
  if(!folder.isEmpty() && QDir(folder).isReadable())
    csvFolder = folder;
  populateComboCsvFile();
  activateButtons();
}

void MainWindow::resetCsvFolder()
{
  csvFolder = "csv/";
  populateComboCsvFile();
  activateButtons();
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
  csvFolder = settings.value("csvFolder", "csv/").toString();
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
  settings.setValue("csvFolder", csvFolder);
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
