#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDate>
#include <QMessageBox>

#include "config.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _timer = new Timer();

    LoadSessionState();

    connect(ui->pbStartStop, SIGNAL(toggled(bool)), this, SLOT(StartTimer(bool)));
    connect(ui->pbConfig, SIGNAL(clicked()), this, SLOT(EditParameters()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartTimer(bool isStarted)
{
    _timer->StartTimer(isStarted);
    SaveSessionState();
    UpdateWindow();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SaveSessionState();
    QMainWindow::closeEvent(event);
}

void MainWindow::LoadSessionState()
{
    bool newDay = true;
    bool errorReading = false;

    QFile configFile(".thirdtime");
    if (configFile.exists())
    {
        configFile.open(QIODevice::ReadOnly);

        QXmlStreamReader stream(&configFile);

        if (stream.readNextStartElement()) // ROOT
        {
            while (stream.readNextStartElement())
            {
                if (stream.name().toString() == "lastlaunch")
                {
                    while (stream.readNextStartElement())
                    {
                        if (stream.name().toString() == "date")
                        {
                            QString dateText = stream.readElementText();
                            QDate lastDate = QDate::fromString(dateText);
                            if (lastDate == QDate::currentDate())
                                newDay = false;
                        }
                    }
                }
                else if (!newDay && stream.name().toString() == "lastsession")
                {
                    _timer->Load(stream);
                    UpdateWindow();
                }
            }
        }

        if (stream.hasError())
        {
            QMessageBox::critical(this, "Reading Error", stream.errorString());
            errorReading = true;
        }
    }

    if (newDay || errorReading)
    {
        ResetWindow();
    }
}

void MainWindow::SaveSessionState()
{
    QFile configFile(".thirdtime");
    configFile.open(QIODevice::WriteOnly);

    QXmlStreamWriter stream(&configFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("ROOT");

    stream.writeStartElement("lastlaunch");
    stream.writeTextElement("date", QDate::currentDate().toString());
    stream.writeEndElement(); // lastlaunch

    _timer->Save(stream);

    stream.writeEndElement(); // ROOT
    stream.writeEndDocument();
    configFile.close();
}

void MainWindow::EditParameters()
{
    ConfigDialog
}

void MainWindow::ResetWindow()
{
    ui->lbreakLabel->setText("New day !");
    ui->lBreakBalance->clear();
    ui->lSessionTime->clear();
}

void MainWindow::UpdateWindow()
{
    bool inFocusMode = _timer->InFocusMode();
    ui->pbStartStop->setChecked(inFocusMode);

    if (inFocusMode)
    {
        ui->lbreakLabel->setText("Focus Mode");
        ui->lSessionTime->setText(_timer->SessionStartTime().toString());
    }
    else
    {
        ui->lSessionTime->setText(QString("%1 - %2").arg(_timer->SessionStartTime().toString()).arg(_timer->SessionStopTime().toString()));

        int breakDuration = _timer->BreakDuration();
        if (breakDuration > 0)
        {

            ui->lbreakLabel->setText(QString("You have earned a break of %1 minutes. See you at %2. Enjoy !")
                                         .arg(breakDuration)
                                         .arg(_timer->EndOfBreak().toString()));
        }
        else
        {
            ui->lbreakLabel->setText("Focus Mode");
        }
    }

    ui->lBreakBalance->setText(QString::number(_timer->BreakBalance()));
}
