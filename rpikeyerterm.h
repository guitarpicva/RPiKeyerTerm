#ifndef RPIKEYERTERM_H
#define RPIKEYERTERM_H

#define GPIO21 21u

#include "gpiod.hpp"
#include "logdialog.h"
#include "logutils.h"
#include "macrodialog.h"

#include <QCloseEvent>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
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
    void loadMacros();
    void on_sendButton_clicked();
    void on_sendingSpeedSpinBox_valueChanged(int arg1);
    void on_actionLOG_triggered(bool checked);
    void on_actionKILL_TX_triggered();
    void on_actionTUNE_triggered(bool checked);
    void on_delCallButton_clicked();
    void on_sendCallButton_clicked();
    void sendText();
    void sendTextRemote(QString tokey);
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
    void on_actionStart_Client_triggered(bool checked);
    void on_clientReadyRead();
    void on_clientTimerTimeout();
    void on_ditDitButton_clicked();
    void on_actionClient_Settings_triggered();
    void on_mapButton_clicked();

    void on_actionCLEAR_triggered();

private:
    Ui::RPiKeyerTerm *ui;
    LogDialog *ld = nullptr;
    logutils lu;
    QTcpServer *server = nullptr;
    QString s_serverAddress = "127.0.0.1";
    int i_serverPort = 9888;
    QString s_clientHost = "127.0.0.1";
    int i_clientPort = 9888;
    QTcpSocket *socket = nullptr; // only one!
    QTcpSocket *clientSocket = nullptr; // only one!
    QTcpSocket *n1mm = nullptr; // for the N1MM logger on 52001
    QNetworkAccessManager * nam = nullptr;
    QTimer *socketTimer = nullptr;
    QByteArray socketBytes;
    QByteArray clientBytes;
    MacroDialog *md = nullptr;
    QString mycall = "N0CALL";
    QString mhGrid = "FM16";
    QString tokey, keystr;
    QChar keychar, kc;
    bool b_clientMode = false;
    bool b_n1mmConnected = false;
    bool b_eQSLEnabled = true; // craetes nam, the QNetworkAccessManager object above
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
