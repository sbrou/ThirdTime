#ifndef CONFIG_H
#define CONFIG_H

#include <QDialog>

namespace Ui {
class ConfigDialog;
}

class ConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigDialog(QWidget *parent = nullptr);
    ~ConfigDialog();

private:
    Ui::ConfigDialog *ui;
};

#endif // CONFIG_H
