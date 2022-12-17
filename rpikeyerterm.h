#ifndef RPIKEYERTERM_H
#define RPIKEYERTERM_H

#define GPIO21 21u

#include "gpiod.hpp"

#include "macrodialog.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QSettings>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
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
    void on_updateButton_clicked();
    void on_updateClearButton_clicked();
    void on_sendClearButton_clicked();
    void on_actionConfig_triggered();
    void on_receiveTextClearButton_clicked();
    void loadMacros();
    void on_sendButton_clicked();
    void on_sendingSpeedSpinBox_valueChanged(int arg1);
    void on_actionLOG_triggered(bool checked);
    void on_actionKILL_TX_triggered();
    void on_actionTUNE_triggered(bool checked);
    void on_delCallButton_clicked();
    void on_sendCallButton_clicked();
    void sendText();
    void on_actionUpdate_triggered();
    void on_actionView_Log_triggered();
    void on_action_Server_Settings_triggered();
    // starting the server starts the TCP server socket for outside data tx/control
    // and puts the UI into "client" mode, so it does not try to key the radio directly
    // rather tells the server side to do it (as well as config changes)
    void on_actionS_tart_Server_triggered(bool checked);
    void on_newConnection();
    void on_socketReadyRead();
    void on_socketTimerTimeout();
private:
    Ui::RPiKeyerTerm *ui;
    QTcpServer *server = nullptr;
    QString s_serverAddress = "127.0.0.1";
    int i_serverPort = 9888;
    QTcpSocket *socket = nullptr; // only one!
    QTimer *socketTimer = nullptr;
    QByteArray socketBytes;
    MacroDialog *md = nullptr;
    QString mycall = "N0CALL";
    QString mhGrid = "FM16";
    QString tokey, keystr;
    QChar keychar, kc;
    bool b_keyit = true;
    bool b_killTx = false;
    bool b_doneSending = true;
    const QString SPACE = " ";
    struct gpiod_chip *chip;
    struct gpiod_line *line;    // key line
    QSettings * settings = nullptr;
    uint dit = 80u; // dit length ms which is 80 ms dir = 15 PARIS WPM

    void loadSettings();
    void saveSettings();

};
#endif // RPIKEYERTERM_H
