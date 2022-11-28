#ifndef RPIKEYERTERM_H
#define RPIKEYERTERM_H

#define GPIO21 21u

#include "gpiod.hpp"

#include "macrodialog.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>

#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class RPiKeyerTerm; }
QT_END_NAMESPACE

class RPiKeyerTerm : public QMainWindow
{
    Q_OBJECT

public:
    RPiKeyerTerm(QWidget *parent = nullptr);
    ~RPiKeyerTerm();
    void closeEvent(QCloseEvent *event);

private slots:
    QString resolveTextSubstitutions(QString toSend);
    void macroTriggered(const int index);
    void on_actionMAC0_triggered();
    void on_actionMAC1_triggered();
    void on_actionMAC2_triggered();
    void on_actionMAC3_triggered();
    void on_actionMAC4_triggered();
    void on_actionMAC5_triggered();
    void on_actionMAC6_triggered();
    void on_actionMAC7_triggered();
    void on_actionMAC8_triggered();
    void on_actionMAC9_triggered();
    void on_actionMAC10_triggered();
    void on_actionMAC11_triggered();
    void on_actionMAC12_triggered();
    void on_pushButton_2_clicked();
    void on_updateClearButton_clicked();
    void on_sendClearButton_clicked();
    void on_actionConfig_triggered();
    void on_receiveTextClearButton_clicked();
    void loadMacros();
    void on_sendButton_clicked();

    void on_sendingSpeedSpinBox_valueChanged(int arg1);

private:
    Ui::RPiKeyerTerm *ui;
    MacroDialog *md = nullptr;
    QString mycall = "N0CALL";
    QString mhGrid = "FM16";
    QString tokey, keystr;
    QChar keychar, kc;
    bool b_keyit = true;
    const QString SPACE = " ";
    struct gpiod_chip *chip;
    struct gpiod_line *line;    // key line
    QSettings * settings = nullptr;
    uint dit = 80u; // dit length ms which is 80 ms dir = 15 PARIS WPM

    void loadSettings();
    void saveSettings();

};
#endif // RPIKEYERTERM_H
