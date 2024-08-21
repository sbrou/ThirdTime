#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <timer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void StartTimer(bool isStarted);

protected:
   virtual void closeEvent(QCloseEvent *event) override;

private:
    Ui::MainWindow *ui;
    Timer *_timer;

    void LoadConfiguration();
    void SaveConfiguration();
};
#endif // MAINWINDOW_H
