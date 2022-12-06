#include "logdialog.h"
#include "ui_logdialog.h"

#include <QDateTime>
#include <QDir>
#include <QMessageBox>
#include <QSettings>

LogDialog::LogDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LogDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QDir d;
    d.mkdir("logs"); // just in case
    logfile.setFileName("logs/QDXPro.log");
    ui->logCommentLineEdit->setFocus();
}

LogDialog::~LogDialog()
{
    delete ui;
}

void LogDialog::setCurrentValues()
{
    const QDateTime now = QDateTime::currentDateTimeUtc();
    ui->dtOffDateTimeEdit->setDateTime(now);
    ui->dtOnDateTimeEdit->setDateTime(now);    
}

void LogDialog::setFrequency(const double freq)
{
    if(freq == 0.0) {
        return;
    }
    ui->logFrequencySpinBox->setValue((int) freq * 1000);
}

void LogDialog::setPower(const double power)
{
    ui->logPowerSpinBox->setValue(power);
}

void LogDialog::setRemoteLoc(const QString remote)
{
    ui->logMHLineEdit->setText(remote);
}

void LogDialog::setSentReport(const QString sent)
{
    ui->logSentReportLineEdit->setText(sent);
}

void LogDialog::setRecdReport(const QString recd)
{
    ui->logReceivedReportLineEdit->setText(recd);
}

void LogDialog::setMode(const QString mode)
{
    ui->logModeComboBox->setCurrentText(mode.trimmed().toUpper());
}

void LogDialog::setOtherCall(const QString call)
{
    ui->logCallLineEdit->setText(call.trimmed().toUpper());
}

void LogDialog::setOpName(const QString op)
{
    ui->logOpNameLineEdit->setText(op.trimmed());
}

void LogDialog::on_logSaveButton_clicked()
{

    // wsjt style CSV log line
    // date on, time on, date off, time off, CALL, MHG, freq, mode, Sent rpt, Recd rpt, pwr,comments,opname
    //2022-11-07,16:23:30,2022-11-07,16:29:44,WF1T,FN43,14.074404,FT8,+18,-16,5,comment, Larry,

    QString out = ui->dtOnDateTimeEdit->dateTime().toString("yyyy-MM-dd").append(DELIM).append(ui->dtOnDateTimeEdit->dateTime().toString("hh:mm:ss")).append(DELIM);
    out.append(ui->dtOffDateTimeEdit->dateTime().toString("yyyy-MM-dd")).append(DELIM).append(ui->dtOffDateTimeEdit->dateTime().toString("hh:mm:ss")).append(DELIM);
    out.append(ui->logCallLineEdit->text().trimmed().toUpper().replace(","," ")).append(DELIM).append(ui->logMHLineEdit->text().trimmed().replace(","," ")).append(DELIM);
    out.append(ui->logFrequencySpinBox->text()).append(DELIM).append(ui->logModeComboBox->currentText().trimmed().toUpper().replace(","," ")).append(DELIM);
    out.append(ui->logSentReportLineEdit->text().trimmed().replace(","," ")).append(DELIM).append(ui->logReceivedReportLineEdit->text().trimmed().replace(","," ")).append(DELIM);
    out.append(ui->logPowerSpinBox->text()).append(DELIM).append(ui->logCommentLineEdit->text().trimmed().replace(","," ")).append(DELIM);
    out.append(ui->logOpNameLineEdit->text().trimmed().replace(","," "));
    if(logfile.open(QFile::WriteOnly | QFile::Append)) {
        logfile.write(out.toLatin1());
        logfile.write("\r\n");
        logfile.close();
    }
    close();
}

void LogDialog::on_logClearButton_clicked()
{
    ui->logCallLineEdit->clear();
    ui->logCommentLineEdit->clear();
    ui->logMHLineEdit->clear();
    ui->logReceivedReportLineEdit->clear();
    ui->logSentReportLineEdit->clear();
    ui->logOpNameLineEdit->clear();
    setCurrentValues();
}

void LogDialog::on_closeButton_clicked()
{
    close();
}
