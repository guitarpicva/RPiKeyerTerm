#ifndef LOGDIALOG_H
#define LOGDIALOG_H

#include <QDialog>
#include <QFile>

namespace Ui {
class LogDialog;
}

class LogDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LogDialog(QWidget *parent = nullptr);
    ~LogDialog();
    void setCurrentValues();

public slots:
    void setOtherCall(const QString call);
    void setOpName(const QString op);
    void setMode(const QString mode);
    void setFrequency(const double freq);
    void setPower(const double power);
    void setRemoteLoc(const QString remote);
    void setSentReport(const QString sent);
    void setRecdReport(const QString recd);
private slots:
    void on_logSaveButton_clicked();
    void on_logUpdateButton_clicked();
    void on_logClearButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::LogDialog *ui;
    const QString DELIM = ",";
    QFile logfile;
    bool b_keepPower = true;
    bool b_keepFreq = true;
};

#endif // LOGDIALOG_H
