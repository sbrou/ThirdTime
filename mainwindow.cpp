#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QFile>
#include <QDate>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _timer = new Timer();

    LoadConfiguration();

    connect(ui->pbStartStop, SIGNAL(toggled(bool)), this, SLOT(StartTimer(bool)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::StartTimer(bool isStarted)
{
    QTime time = _timer->StartTimer(isStarted);
    if (isStarted)
    {
        ui->lbreakLabel->setText("Focus Mode");
        ui->lSessionTime->setText(time.toString());
    }
    else
    {
        QString startTime = ui->lSessionTime->text();
        ui->lSessionTime->setText(QString("%1 - %2").arg(startTime).arg(time.toString()));

        int breakDuration = _timer->breakDurationMin();
        if (breakDuration > 0)
        {

            ui->lbreakLabel->setText(QString("You have earned a break of %1 minutes. See you at %2. Enjoy !")
                                     .arg(breakDuration)
                                     .arg(_timer->endOfBreak().toString()));
        }
        else
        {
            ui->lbreakLabel->setText("Focus Mode");
        }
    }
    ui->lBreakBalance->setText(QString::number(_timer->breakBalance()));
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    SaveConfiguration();
    QMainWindow::closeEvent(event);
}

void MainWindow::LoadConfiguration()
{
    bool newDay = true;


    QFile configFile(".thirdtime");
    if (configFile.exists())
    {
        configFile.open(QIODevice::ReadOnly);

        QXmlStreamReader stream(&configFile);

        while (!stream.atEnd())
        {
            if (stream.readNextStartElement())
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

                            break;
                        }
                    }
                    stream.skipCurrentElement();
                    break;
                }
            }
        }


        if (!newDay)
        {
            _timer->Load(stream);
            if (stream.hasError())
            {
                QMessageBox::critical(this, "Reading Error", stream.errorString());
                return;
            }
            return;
        }
    }

    ui->lbreakLabel->setText("New day !");
    ui->lBreakBalance->clear();
    ui->lSessionTime->clear();
}

void MainWindow::SaveConfiguration()
{
    QFile configFile(".thirdtime");
    configFile.open(QIODevice::WriteOnly);

    QXmlStreamWriter stream(&configFile);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();

    stream.writeStartElement("lastlaunch");
    stream.writeTextElement("date", QDate::currentDate().toString());
    stream.writeEndElement(); // lastlaunch

    _timer->Save(stream);

    stream.writeEndDocument();
    configFile.close();
}

