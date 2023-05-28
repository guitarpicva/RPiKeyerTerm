#include "rpikeyerterm.h"
#include "ui_rpikeyerterm.h"
#include "alphabet.h"
#include "maidenhead.h"
#include "rbndialog.h"

#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkInterface>
#include <QStringBuilder>
#include <QThread>
#include <QDebug>
#include <QUrlQuery>
#include <QMessageBox>

RPiKeyerTerm::RPiKeyerTerm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RPiKeyerTerm)
{
    QDir d(".");
    d.mkdir("logs"); // create if not exists
    ui->setupUi(this);
    // set up the GPIO pin from settings and initialize it and test it
    settings = new QSettings("RPiKeyerTerm.ini", QSettings::IniFormat);
    loadSettings();
    loadMacros();
    chip = gpiod_chip_open_by_name("gpiochip0");
    line = gpiod_chip_get_line(chip, GPIO21);
    gpiod_line_request_output(line, "keyline", 0); // set to output direction
    gpiod_line_set_value(line, 0); // ensure low to start, not really needed
    // Set up the audio BP filter widget
    dw = new QDockWidget(this);
    dw->setObjectName("FIR_BP_Filter");
    dw->setWindowTitle("FIR BP Filter");
    dw->setAllowedAreas(Qt::BottomDockWidgetArea | Qt::TopDockWidgetArea);
    dw->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    FIRFilterLib ff;
    ffw = ff.createFIRFilterWidget();
    dw->setWidget(ffw);
    this->addDockWidget(Qt::BottomDockWidgetArea, dw);
    dw->setVisible(b_firFilterEnabled);
    // TEST n1mm connection
//    if(b_n1mmEnabled) {
//        n1mm = new QTcpSocket(this);
//        n1mm->connectToHost("127.0.0.1", 52001);
//        if(n1mm->waitForConnected(5000))
//            b_n1mmConnected = true; // def. false
//        qDebug()<<b_n1mmConnected<<n1mm->state()<<n1mm->errorString();
//    }
    // END TETS n1mm connection
    // TEST eQSL
//    if(b_eQSLEnabled) {
//        if(!nam) {
//            nam = new QNetworkAccessManager(this);
//            //nam->setAutoDeleteReplies(true);
//            connect(nam, &QNetworkAccessManager::finished, this, [=](QNetworkReply *answer){
//                qDebug()<<"eQSL/QRZ finished\n"<<answer->readAll();
//                answer->deleteLater();
//            }, Qt::UniqueConnection);
//        }
////        QString payload;
////        if(nam) {
////            payload = "https://www.eQSL.cc/qslcard/ImportADIF.cfm?ADIFData=Upload";
////            payload.append(QUrl::toPercentEncoding("<EOH><QSO_DATE:8>20221221<TIME_ON:4>2124<BAND:3>30M<MODE:2>CW<RST_Sent:3>55N<RST_Rcvd:3>55N<TX_PWR:3>5.0<CALL:7>AB4MW-3<NAME:6>GLARRY<GRIDSQUARE:0><COMMENT:4>POTA<EOR>"));
////            payload.append("&eQSL_User=ab4mw&eQSL_Pswd=Mag96!dc");
////            //qDebug()<<"url:"<<payload;
////            //nam->get(QNetworkRequest(QUrl(QUrl::toPercentEncoding(payload))));
////            nam->get(QNetworkRequest(payload));
////        }
//    }

    // END TEST eQSL
    /*
    // TEST QRZ.com
    const QString adif = "<QSO_DATE:8>20221221<TIME_ON:4>2124<BAND:3>30M<MODE:2>CW<RST_Sent:3>55N<RST_Rcvd:3>55N<TX_PWR:3>5.0<CALL:7>AB4MW-3<NAME:6>GLARRY<GRIDSQUARE:0><COMMENT:4>POTA<EOR>";
    const QString APIKey = "KEY=DC9F-CDB0-E424-14FD"; //DC9F-CDB0-E424-14FD
    const QString action = "&ACTION=INSERT&ADIF=";
    QString payload =  APIKey % action % adif;
    if(nam) {
        //qDebug()<<payload;
        QNetworkRequest nr;
        nr.setUrl(QUrl("https://logbook.qrz.com/api"));
        nr.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
        nam->post(nr, QUrl::toPercentEncoding(payload));
    }
    // END TEST QRZ.com */
    // TEST RBN dialog
//    on_action_RBN_Dialog_triggered();
    // END TEST RBN dialog
}

RPiKeyerTerm::~RPiKeyerTerm()
{
    delete ui;
}

void RPiKeyerTerm::closeEvent(QCloseEvent *event)
{
    gpiod_line_release(line);
    gpiod_chip_close(chip);
    saveSettings();
    event->accept();
}

QString RPiKeyerTerm::resolveTextSubstitutions(QString toSend)
{
    //qDebug()<<"resolve:"<<toSend;
    // We'll build a list of text substitutions for
    // users to use here.
    QString out, tmp;
    out = toSend.trimmed();
    if(toSend.contains("{")) {
        if(toSend.contains("{CS}")) {
            out = toSend.replace("{CS}", ui->mycallLineEdit->text().trimmed().toUpper());
        }
        if(toSend.contains("{CN}")) {
            out = toSend.replace("{CN}", ui->myNameLineEdit->text().trimmed().toUpper());
        }
        if(toSend.contains("{DS}")) {
            tmp = ui->heardListComboBox->currentText();
            //qDebug()<<"DS:"<<tmp;
            out = toSend.replace("{DS}", tmp);
        }
        if(toSend.contains("{MH}")) {
            tmp = ui->mhGridLineEdit->text().trimmed();
            out = toSend.replace("{MH}", tmp);
        }
        if(toSend.contains("{OP}")) {
            tmp = ui->opNameLineEdit->text().trimmed();
            out = toSend.replace("{OP}", tmp);
        }
        if(toSend.contains("{RO}")) {
            tmp = ui->rstOutLineEdit->text().trimmed();
            out = toSend.replace("{RO}", tmp);
        }
        if(toSend.contains("{RI}")) {
            tmp = ui->rstInLineEdit->text().trimmed();
            out = toSend.replace("{RI}", tmp);
        }
        if(toSend.contains("{LO}")) {
            tmp = ui->destGridLineEdit->text().trimmed();
            out = toSend.replace("{LO}", tmp);
        }
        if(toSend.contains("{PO}")) {
            tmp = ui->pwrSpinBox->text();
            out = toSend.replace("{PO}", tmp);
        }
        if(toSend.contains("{FR}")) {
            tmp = ui->bandComboBox->currentText();
            out = toSend.replace("{FR}", tmp.mid(0, tmp.indexOf(".")));
        }
    }
    return out;
}

void RPiKeyerTerm::macroTriggered(const int index)
{
    QString toSend = settings->value("Macros/macro" + QString::number(index) + "Text", "").toString();
    toSend = resolveTextSubstitutions(toSend);
    //qDebug()<<"Macro:"<<index<<toSend;
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    ui->sendTextArea->setPlainText(toSend);
    ui->sendTextArea->setFocus();
}

void RPiKeyerTerm::on_actionMAC12_triggered()
{
    macroTriggered(12);
}

void RPiKeyerTerm::on_actionMAC1_triggered()
{
    macroTriggered(1);
}

void RPiKeyerTerm::on_actionMAC2_triggered()
{
    macroTriggered(2);
}

void RPiKeyerTerm::on_actionMAC3_triggered()
{
    macroTriggered(3);
}

void RPiKeyerTerm::on_actionMAC4_triggered()
{
    macroTriggered(4);
}

void RPiKeyerTerm::on_actionMAC5_triggered()
{
    macroTriggered(5);
}

void RPiKeyerTerm::on_actionMAC6_triggered()
{
    macroTriggered(6);
}

void RPiKeyerTerm::on_actionMAC7_triggered()
{
    macroTriggered(7);
}

void RPiKeyerTerm::on_actionMAC8_triggered()
{
    macroTriggered(8);
}

void RPiKeyerTerm::on_actionMAC9_triggered()
{
    macroTriggered(9);
}

void RPiKeyerTerm::on_actionMAC10_triggered()
{
    macroTriggered(10);
}

void RPiKeyerTerm::on_actionMAC11_triggered()
{
    macroTriggered(11);
}

void RPiKeyerTerm::on_updateButton_clicked()
{
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    ui->receiveTextArea->insertPlainText("\nRECEIVED: " + ui->conversationTextEdit->toPlainText().trimmed());
    ui->sendTextArea->setFocus();
}

void RPiKeyerTerm::loadSettings()
{
    b_firFilterEnabled = settings->value("firFilterEnabled", false).toBool();
    ui->actionShow_FIR_BP_Filter->setChecked(b_firFilterEnabled);
    if(dw) {
        dw->setVisible(b_firFilterEnabled);
    }
    b_n1mmEnabled = settings->value("n1mmEnabled", false).toBool();
    ui->actionEnable_N1MM->setChecked(b_n1mmEnabled);
    if(b_n1mmEnabled) {on_actionEnable_N1MM_triggered(true);qDebug()<<"start n1mm connection...";}
    b_eQSLEnabled = settings->value("eQSLEnabled", false).toBool();
    ui->actionEnable_eQSL->setChecked(b_eQSLEnabled);
    if(b_eQSLEnabled) on_actionEnable_eQSL_triggered(true);
    b_QRZEnabled = settings->value("QRZEnabled", false).toBool();
    ui->actionEnable_QRZ_com->setChecked(b_QRZEnabled);
    if(b_QRZEnabled) on_actionEnable_QRZ_com_triggered(true);
    ui->myNameLineEdit->setText(settings->value("myname", "NONAME").toString());
    mycall = settings->value("mycall", "N0CALL").toString();
    ui->mycallLineEdit->setText(mycall);
    ui->mhGridLineEdit->setText(settings->value("mygrid", "FM00").toString());
    ui->sendingSpeedSpinBox->setValue(settings->value("sendingSpeed", 15).toInt());
    QStringList items = settings->value("heardList", "").toStringList();
    ui->heardListComboBox->addItems(items);
    ui->heardListComboBox->model()->sort(0);
    ui->updateGroupBox->setVisible(settings->value("showUpdate", false).toBool());
    s_serverAddress = settings->value("serverAddress", "127.0.0.1").toString();
    i_serverPort = settings->value("serverPort", 9888).toInt();
    s_clientHost = settings->value("clientHost", "127.0.0.1").toString();
    i_clientPort = settings->value("clientPort", 9888).toInt();
    restoreGeometry(settings->value("geometry", "").toByteArray());
    ui->splitter->restoreState(settings->value("splitter").toByteArray());
    restoreState(settings->value("windowState", "").toByteArray());
}

void RPiKeyerTerm::saveSettings()
{    
    settings->setValue("splitter", ui->splitter->saveState());
    settings->setValue("n1mmEnabled", ui->actionEnable_N1MM->isChecked());
    if(dw) {
        settings->setValue("firFilterEnabled", dw->isVisible());
    }
    settings->setValue("eQSLEnabled", ui->actionEnable_eQSL->isChecked());
    settings->setValue("myname", ui->myNameLineEdit->text().trimmed().toUpper());
    settings->setValue("mycall", ui->mycallLineEdit->text().trimmed().toUpper());
    settings->setValue("mygrid", ui->mhGridLineEdit->text().trimmed().toUpper());
    settings->setValue("sendingSpeed", ui->sendingSpeedSpinBox->value());
    QStringList items;
    for(int i = 0; i < ui->heardListComboBox->count(); i++) {
        items<<ui->heardListComboBox->itemText(i);
    }
    items.removeDuplicates();
    settings->setValue("showUpdate", ui->updateGroupBox->isVisible());
    settings->setValue("heardList", items);
    settings->setValue("geometry", saveGeometry());
    settings->setValue("windowState", saveState());
}

void RPiKeyerTerm::on_updateClearButton_clicked()
{
    ui->conversationTextEdit->clear();
    // this is just a test of what happens when a "used" line is accessed, essentially returns -1 fail
//    gpiod_chip *chip2 = gpiod_chip_open_by_name("gpiochip2");
//    qDebug()<<"chip2:"<<chip2;
//    gpiod_line *line2 = gpiod_chip_get_line(chip2, 4);
//    qDebug()<<"line2:"<<line2;
//    int ret = gpiod_line_request_output(line2, "led0", 1);
//    qDebug()<<"ret:"<<ret;
//    if(ret)
//    {
//        QThread::msleep(3000);
//        gpiod_line_request_output(line2, "led0", 0);
//    }
}

void RPiKeyerTerm::on_sendClearButton_clicked()
{
    ui->sendTextArea->clear();
}

void RPiKeyerTerm::on_actionConfig_triggered()
{
    // open macro config dialog
    if(!md) {
        md = new MacroDialog(this);
        connect(md, &MacroDialog::loadMacros, this, &RPiKeyerTerm::loadMacros);
    }
    md->show();
}

void RPiKeyerTerm::loadMacros()
{
    QSettings s("RPiKeyerTerm.ini", QSettings::IniFormat);
    QString text, label, content;
    for(int i = 1; i < 13; i++)
    {
        label = s.value("Macros/macro" + QString::number(i) + "Label", "").toString();
        content = s.value("Macros/macro" + QString::number(i) + "Text", "").toString();
        if(label.isEmpty())
            label = "MAC" + QString::number(i);
        switch(i){
        case 1: ui->actionMAC1->setText(label);ui->actionMAC1->setToolTip(content);break;
        case 2: ui->actionMAC2->setText(label);ui->actionMAC2->setToolTip(content);break;
        case 3: ui->actionMAC3->setText(label);ui->actionMAC3->setToolTip(content);break;
        case 4: ui->actionMAC4->setText(label);ui->actionMAC4->setToolTip(content);break;
        case 5: ui->actionMAC5->setText(label);ui->actionMAC5->setToolTip(content);break;
        case 6: ui->actionMAC6->setText(label);ui->actionMAC6->setToolTip(content);break;
        case 7: ui->actionMAC7->setText(label);ui->actionMAC7->setToolTip(content);break;
        case 8: ui->actionMAC8->setText(label);ui->actionMAC8->setToolTip(content);break;
        case 9: ui->actionMAC9->setText(label);ui->actionMAC9->setToolTip(content);break;
        case 0: ui->actionMAC10->setText(label);ui->actionMAC10->setToolTip(content);break;
        case 11: ui->actionMAC11->setText(label);ui->actionMAC11->setToolTip(content);break;
        case 12: ui->actionMAC12->setText(label);ui->actionMAC12->setToolTip(content);break;
        }
    }
}

void RPiKeyerTerm::sendText() // send whatever is in the QString tokey
{
    //qDebug()<<"Send Text: socket"<<socket<<"server:"<<server;
    const int end = tokey.size(); // current size so when we send this doesn't change
    const QString tosend = tokey;
    tokey = tokey.mid(end); // so more can be added
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    ui->receiveTextArea->insertPlainText("\nSENDING: ");
    b_doneSending = false; //
    for(int i = 0; i < end; i++) {
        if(b_killTx) {
            tokey.clear();// may as well wipe the input buffer too
            b_killTx = false;
            gpiod_line_set_value(line, 0); // key off
            break;
        }
        keystr.clear();
        keychar = tosend.at(i);
        ui->receiveTextArea->insertPlainText(keychar); // echo to rx area
        if(socket) { // tell the client that this text has been sent
            socket->write(QString(tosend[i]).toLatin1());
            socket->flush();
        }
        //qApp->processEvents(QEventLoop::ExcludeUserInputEvents); // only update the text area
        qApp->processEvents();
        keystr = alphabet[keychar.cell()]; // look up the keying pattern by ASCII char value
        //qDebug()<<"keystr:"<<keystr;
        if(keystr == "") {
            QThread::msleep(dit * 3);
            continue; // skip the unknown char
        }
        if(keystr == SPACE) {
            ui->receiveTextArea->insertPlainText(SPACE); // echo to rx area
            QThread::msleep(dit * 6); // inter-word space = 7, so 1 dit time from end of word + 6 * dit times
            continue;
        }
        // one string of dits and dahs makes ONE character
        int j = 0; // index in keystr
        kc = keystr.at(j); // DIT or DAH
        while (kc != 0x00) {
//            qDebug()<<"key char:"<<kc;
            if(b_keyit) gpiod_line_set_value(line, 1); // key on
            //gpio_put(LED, 1);
            if(kc == '.') {
                QThread::msleep(dit); // a DIT length
            }
            else {
                QThread::msleep(dit * 3); // DAH is 3 dit lengths
            }
            if(b_keyit) gpiod_line_set_value(line, 0); // key off
            //gpio_put(LED, 0);
            QThread::msleep(dit); // intra-key delay
            j++;
            kc = keystr.at(j);
        }
        QThread::msleep(dit * 3); // wait 3 dit lengths between characters
    }
    b_doneSending = true;
    ui->sendButton->setEnabled(true);
}

void RPiKeyerTerm::sendTextRemote(QString tokey)
{
    qDebug()<<"sendTextRemote:"<<tokey;
    if(clientSocket && clientSocket->state() == QTcpSocket::ConnectedState) {
        ui->receiveTextArea->moveCursor(QTextCursor::End);
        ui->receiveTextArea->insertPlainText("\nSENDING: ");
        clientSocket->write(tokey.toLatin1());
        clientSocket->flush();
    }
    b_doneSending = true;
    ui->sendButton->setEnabled(true);
}

void RPiKeyerTerm::on_sendButton_clicked()
{
    tokey.append(ui->sendTextArea->toPlainText().trimmed().toUpper().remove(QRegularExpression("[\r\n]"))); // no line ends in Morse
    //tokey = resolveTextSubstitutions(tokey);
    ui->sendButton->setEnabled(false);
    if(b_clientMode)
        sendTextRemote(tokey);
    else
        sendText();
}

void RPiKeyerTerm::on_sendingSpeedSpinBox_valueChanged(int arg1)
{
    //qDebug()<<"speed value:"<<arg1;
    if(arg1 < 5) arg1 = 5; // min speed is 5 WPM
    dit = (int) 1200/arg1;
    if(clientSocket) { // tell connected client
        clientSocket->write("@" + QString::number(arg1).toLatin1() + "\n");
    }
    else if(socket) {
        socket->write("@" + QString::number(arg1).toLatin1() + "\n");
    }
}

void RPiKeyerTerm::on_actionLOG_triggered()
{
    // do log dialog
    if(!ld) {
        ld = new LogDialog(this);
        connect(ld, &LogDialog::logSaved, this, [=](QString logline) {
            if(logline.isEmpty()) return;
            ui->actionLOG->setChecked(false);
            QString adif = lu.logLineToADIF(logline);
            QFile logout("logs/RPiKeyerTerm.adi");
            if(logout.open(QFile::ReadWrite | QFile::Append)) {
                logout.write(adif.toLatin1());
                logout.close();
            }
            if(b_n1mmEnabled && b_n1mmConnected) {
                adif.prepend("<command:3>Log<parameters:" % QString::number(adif.length()) % ">");
                // if we want to send to N1MM, put this text on the socket
                //qDebug()<<"adif:"<<adif;
                n1mm->write(adif.toLatin1());
                n1mm->flush();
            }
            if(b_eQSLEnabled) {
                QString payload;
                if(nam) {
                    payload = "https://www.eQSL.cc/qslcard/ImportADIF.cfm?ADIFData=Upload";
                    payload.append(QUrl::toPercentEncoding("<EOH>" + adif));
                    payload.append("&eQSL_User=" % settings->value("eQSLUser", "").toString() % "&eQSL_Pswd=" %  settings->value("eQSLPswd", "").toString());
                    //qDebug()<<"url:"<<payload;
                    nam->get(QNetworkRequest(payload));
                }
                else qDebug()<<"nam not created...";
            }
            if(b_QRZEnabled) {
                QUrlQuery urlquery;
                const QString APIKey = settings->value("QRZKey", "").toString();
                urlquery.addQueryItem("KEY", APIKey);
                urlquery.addQueryItem("ACTION", "INSERT");
                urlquery.addQueryItem("ADIF", adif);
                if(nam) {
                    //qDebug()<<"QRZ:" << payload;
                    QNetworkRequest nr;
                    nr.setUrl(QUrl("https://logbook.qrz.com/api"));
                    nr.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
                    //nam->post(nr, QUrl::toPercentEncoding(payload));
                    nam->post(nr, urlquery.toString(QUrl::FullyEncoded).toUtf8());
                }
            }
        });
    }
    ld->setCurrentValues();
    ld->setFrequency(ui->bandComboBox->currentText());
    ld->setOtherCall(ui->heardListComboBox->currentText().trimmed());
    ld->setOpName(ui->opNameLineEdit->text().trimmed());
    ld->setPower(ui->pwrSpinBox->value());
    ld->setRecdReport(ui->rstInLineEdit->text().trimmed());
    ld->setRemoteLoc(ui->destGridLineEdit->text().trimmed());
    ld->setSentReport(ui->rstOutLineEdit->text().trimmed());
    ld->show();
    ld->raise();

}

void RPiKeyerTerm::on_actionKILL_TX_triggered()
{
    b_killTx = true;
    tokey.clear(); // clear anything in the tx buffer
    gpiod_line_set_value(line, 0); // key off
    ui->sendButton->setEnabled(true);
    b_doneSending = true;

}

void RPiKeyerTerm::on_actionTUNE_triggered(bool checked)
{
    if(checked)
        gpiod_line_set_value(line, 1); // key on
    else
        gpiod_line_set_value(line, 0); // key off
}

void RPiKeyerTerm::on_delCallButton_clicked()
{
    ui->heardListComboBox->removeItem(ui->heardListComboBox->currentIndex());
}

void RPiKeyerTerm::on_sendCallButton_clicked()
{
    tokey = ui->mycallLineEdit->text().trimmed().toUpper();
    if(b_clientMode) {
        qDebug()<<"Sending to remote server...";
        sendTextRemote(tokey);
    }
    else
        sendText();
}

void RPiKeyerTerm::on_actionUpdate_triggered()
{
    bool isVisible = ui->updateGroupBox->isVisible();
    ui->updateGroupBox->setVisible(!isVisible);
    settings->setValue("showUpdate", !isVisible);
}

void RPiKeyerTerm::on_actionView_Log_triggered()
{
    QFile f("logs/RPiKeyerTerm.log");
    if(f.open(QFile::ReadOnly)) {
        ui->receiveTextArea->appendPlainText(f.readAll());
        ui->receiveTextArea->moveCursor(QTextCursor::End);
    }
}

void RPiKeyerTerm::on_action_Server_Settings_triggered()
{
    // first, choose IP address to listen on
    QStringList items;
    const QList<QHostAddress> nal = QNetworkInterface::allAddresses();
    foreach(QHostAddress h, nal) {
        if(h.toString().startsWith("169.254")) // not link local
            continue;
        if(h.toString().contains(":")) // not IPv6
            continue;
        items << h.toString();
    }
    items.sort();
    QString chosen = QInputDialog::getItem(this, "Choose IP Address", "Select the IP Address to use for the server.", items);
    if(chosen.isEmpty()) chosen = "127.0.0.1"; // default to localhost
    settings->setValue("serverAddress", chosen);
    s_serverAddress = chosen;

    // second, choose the TCP port to bind
    int port = QInputDialog::getInt(this, "Set TCP Port", "Set the TCP Port for the Server to bind to.", 9888, 1024, 65535);
    // any escape should return default of 9888
    settings->setValue("serverPort", port);
    i_serverPort = port;
    // starting the server is done by the action below
}

void RPiKeyerTerm::on_actionS_tart_Server_triggered(bool checked)
{
    qDebug()<<"Start Server..."<<checked;
    // cannot be a server and a client, so stop any client socket
    if(clientSocket) {
        clientSocket->disconnectFromHost();
        disconnect(clientSocket, 0, 0, 0);
        ui->actionStart_Client->setChecked(false);
    }
    if(socketTimer)
        socketTimer->stop();
    else {
        socketTimer = new QTimer(this);
        socketTimer->setInterval(20);
        connect(socketTimer, &QTimer::timeout, this, &RPiKeyerTerm::on_socketTimerTimeout, Qt::UniqueConnection);
    }
    // if true, start the server, if false, stop the server
    if(checked) {
        if (server)
        {
            server->close(); // stop listening
            disconnect(server, 0, 0, 0);
        }
        else
        {
            server = new QTcpServer(this);
        }
        server->listen(QHostAddress(s_serverAddress), i_serverPort);
        connect(server, &QTcpServer::newConnection, this, &RPiKeyerTerm::on_newConnection);
    }
    else {
        if(socket) socket->disconnectFromHost();
        disconnect(socket, 0, 0, 0);
        if(server) server->close(); // stop listening
    }
}

void RPiKeyerTerm::on_newConnection()
{
    qDebug()<< "On New Connection...";
    if (socket) {
        if(socket->state() == QTcpSocket::ConnectedState) {
            socket->disconnectFromHost();
            socket->waitForDisconnected(2000);
        }
        disconnect(socket, 0, 0, 0);
    }
    socket = server->nextPendingConnection();
    connect(socket, &QTcpSocket::readyRead, this, &RPiKeyerTerm::on_socketReadyRead);
}

void RPiKeyerTerm::on_socketReadyRead()
{
    socketTimer->stop();
    //qDebug()<<"Socket Ready Read:"<<socket->readAll();
    socketBytes.append(socket->readAll());
    socketTimer->start(20);
}

void RPiKeyerTerm::on_socketTimerTimeout()
{
    socketTimer->stop(); // readyRead turns it back on
    // process socketBytes delimited on \n, then trimmed
    QString cmdlist = socketBytes;
    cmdlist.remove("\r");
    const QStringList cmds = cmdlist.split("\n", Qt::SkipEmptyParts);
    qDebug()<<"Socket timer timeout..."<<cmdlist<<cmds;
    socketBytes.clear();
    foreach(QString cmd, cmds) {
        if(cmd.startsWith('#')) {
            // this is a command string so make changes accordingly
            qDebug()<<"Cmd String:"<<cmd;
            if(cmd.contains("#CFG")) {
                // send the full config as JSON
            }
        }
        else if(cmd.startsWith('@')) {
            ui->sendingSpeedSpinBox->setValue(cmd.mid(1).toInt());
        }
        else {
            qDebug()<<"Send to transmit:"<<cmd;
            // send to the keyer
            tokey.append(cmd); // no line ends in Morse
            //tokey = resolveTextSubstitutions(tokey);
            ui->sendButton->setEnabled(false);
            sendText();
        }
    }
}

void RPiKeyerTerm::on_actionStart_Client_triggered(bool checked)
{
    if(checked) {
        b_clientMode = true;
        if(clientSocket) {
            if(clientSocket->state() == QTcpSocket::ConnectedState) {
                clientSocket->disconnectFromHost();
            }
            disconnect(clientSocket, 0, 0, 0);

        }
        else {
            clientSocket = new QTcpSocket(this);
            clientSocket->connectToHost(s_clientHost, i_clientPort);
            qDebug()<<"Connecting to:"<<s_clientHost<<i_clientPort;
        }
        connect(clientSocket, &QTcpSocket::readyRead, this, &RPiKeyerTerm::on_clientReadyRead);
        if(!socketTimer)
            socketTimer = new QTimer(this);
        socketTimer->setInterval(20);
        connect(socketTimer, &QTimer::timeout, this, &RPiKeyerTerm::on_clientTimerTimeout);
    }
    else {
        b_clientMode = false;
        clientSocket->disconnectFromHost();
        disconnect(clientSocket, 0, 0, 0);
    }
}

void RPiKeyerTerm::on_clientReadyRead()
{
    socketTimer->stop();
    //qDebug()<<"Socket Ready Read:"<<socket->readAll();
    clientBytes.append(clientSocket->readAll());
    socketTimer->start(20);
}

void RPiKeyerTerm::on_clientTimerTimeout()
{
    socketTimer->stop(); // clientReadyRead turns it back on
    // process socketBytes delimited on \n, then trimmed
    QString cmdlist = clientBytes;
    cmdlist.remove("\r");
    const QStringList cmds = cmdlist.split("\n", Qt::SkipEmptyParts);
    qDebug()<<"Client Socket timer timeout..."<<cmdlist<<cmds;
    clientBytes.clear();
    foreach(QString cmd, cmds) {
        if(cmd.startsWith('#')) {
            // this is a command string so make changes accordingly
            qDebug()<<"Client Cmd String:"<<cmd;
            if(cmd.contains("#CFG")) {
                // send the full config as JSON
            }
        }
        else if(cmd.startsWith('@')) {
            // speed change
            ui->sendingSpeedSpinBox->blockSignals(true);
            ui->sendingSpeedSpinBox->setValue(cmd.mid(1).toInt());
            ui->sendingSpeedSpinBox->blockSignals(false);
        }
        else {
            // since CW is reported sent one char at a time
            // all other updates to the rx text must include appropriate
            // line ends to format text otherwise.
            qDebug()<<"Transmitted:"<<cmd;
            ui->receiveTextArea->moveCursor(QTextCursor::End);
            ui->receiveTextArea->insertPlainText(cmd);
            // send to the keyer
        }
    }
}

void RPiKeyerTerm::on_ditDitButton_clicked()
{
    tokey = "EE";
    if(b_clientMode)
        sendTextRemote(tokey);
    else
        sendText();
}

void RPiKeyerTerm::on_actionClient_Settings_triggered()
{

    // first, choose IP address to connect to
    QStringList items;
    const QList<QHostAddress> nal = QNetworkInterface::allAddresses();
    foreach(QHostAddress h, nal) {
        if(h.toString().startsWith("169.254")) // not link local
            continue;
        if(h.toString().contains(":")) // not IPv6
            continue;
        items << h.toString();
    }
    items.sort();
    QString chosen = QInputDialog::getItem(this, "Choose IP Address", "Select the IP Address to use for the server.", items);
    if(chosen.isEmpty()) chosen = "127.0.0.1"; // default to localhost
    settings->setValue("clientHost", chosen);
    s_clientHost = chosen;

    // second, choose the TCP port to bind
    int port = QInputDialog::getInt(this, "Set TCP Port", "Set the TCP Port for the Server to bind to.", 9888, 1024, 65535);
    // any escape should return default of 9888
    settings->setValue("clientPort", port);
    i_clientPort = port;
    // connectint to the server is done by the action below
}

void RPiKeyerTerm::on_mapButton_clicked()
{
    QString val = ui->destGridLineEdit->text().trimmed();
    if(val.length() > 3 && val.contains(QRegularExpression("^[A-Ra-r]{2,2}[0-9]{2,2}"))) {
        // use the MH grid to send coordinates to Google Maps
        Maidenhead mh;
        QPair<double, double> spot = mh.mh2ll(ui->destGridLineEdit->text().trimmed());
        if(spot.first)
            QDesktopServices::openUrl(QUrl("https://maps.google.com/maps/@" +
                                           QString::number(spot.first)
                                           + ","
                                               + QString::number(spot.second)
                                           + ",11z"));
    }
}

void RPiKeyerTerm::on_actionCLEAR_triggered()
{
    ui->receiveTextArea->clear();
    ui->sendTextArea->clear();
    ui->conversationTextEdit->clear();
}

void RPiKeyerTerm::on_actionEnable_N1MM_triggered(bool checked)
{
    if(checked)
    {
        b_n1mmEnabled = true;
        if(!n1mm) n1mm = new QTcpSocket(this);

        const QString ip = settings->value("n1mmIP", "127.0.0.1").toString();
        const int port = settings->value("n1mmPort", 52001).toInt();
        n1mm->connectToHost(ip, port);
        if(n1mm->waitForConnected(5000)) {
            b_n1mmConnected = true; // def. false
        }
        else b_n1mmConnected = false; // turn it off
    }
    else {
        //qDebug()<<"b_n1mmConnected:"<<b_n1mmConnected<<"b_n1mmEnabled:"<<b_n1mmEnabled;
        b_n1mmConnected = false; // turn it off
        b_n1mmEnabled = false;
        if(n1mm) n1mm->disconnectFromHost();
    }
}

void RPiKeyerTerm::on_actionEnable_eQSL_triggered(bool checked)
{
    b_eQSLEnabled = checked;
    if(!nam) {
        nam = new QNetworkAccessManager(this);
        //nam->setAutoDeleteReplies(true);
        connect(nam, &QNetworkAccessManager::finished, this, [=](QNetworkReply *answer){
            //qDebug()<<"eQSL/QRZ finished\n"<<answer->readAll();
            ui->receiveTextArea->appendHtml(answer->readAll());
            ui->receiveTextArea->moveCursor(QTextCursor::End);
            answer->deleteLater();
        }, Qt::UniqueConnection);
    }
}

void RPiKeyerTerm::on_actionConfigure_N1MM_triggered()
{
    QString ip = settings->value("n1mmIP", "127.0.0.1").toString();
    ip = QInputDialog::getText(this, "Set N1MM IP address", "Enter IP Address of N1MM Connection", QLineEdit::Normal, ip);
    int port = settings->value("n1mmPort", 52001).toInt();
    if(!ip.isEmpty()) {
        port = QInputDialog::getInt(this, "TCP Port Number", "Enter TCP Port Number for N1MM", port, 1025, 65535);
    }
    //qDebug()<<"n1mm:"<<ip<<port;
    settings->setValue("n1mmIP", ip);
    settings->setValue("n1mmPort", port);
    if(ui->actionEnable_N1MM->isChecked()) on_actionEnable_N1MM_triggered(true);
}

void RPiKeyerTerm::on_actionConfigure_eQSL_triggered()
{
    QString user = settings->value("eQSLUser", "").toString();
    QString pswd = settings->value("eQSLPswd", "").toString();
    user = QInputDialog::getText(this, "eQSL Login (callsign)", "Enter the eQSL login id.", QLineEdit::Normal, user).trimmed();
    if(user.isEmpty()) return;
    pswd = QInputDialog::getText(this, "eQSL Password", "Enter the eQSL password id.", QLineEdit::Normal, pswd).trimmed();
    if(pswd.isEmpty()) return;
    settings->setValue("eQSLUser", user);
    settings->setValue("eQSLPswd", pswd);
}

void RPiKeyerTerm::on_actionConfigure_QRZ_com_triggered()
{
    QString key = settings->value("QRZKey", "").toString();
    key = QInputDialog::getText(this, "QRZ.com Access Key", "Enter the QRZ.com Access Key.\n\nA subscription is required.", QLineEdit::Normal, key).trimmed();
    if(key.isEmpty()) return;
    settings->setValue("QRZKey", key);
}

void RPiKeyerTerm::on_actionEnable_QRZ_com_triggered(bool checked)
{
    b_QRZEnabled = checked;
    settings->setValue("QRZEnabled", checked);
    if(!nam) {
        nam = new QNetworkAccessManager(this);
        //nam->setAutoDeleteReplies(true);
        connect(nam, &QNetworkAccessManager::finished, this, [=](QNetworkReply *answer){
            //qDebug()<<"eQSL/QRZ finished\n"<<answer->readAll();
            ui->receiveTextArea->appendHtml(answer->readAll());
            ui->receiveTextArea->moveCursor(QTextCursor::End);
            answer->deleteLater();
        }, Qt::UniqueConnection);
    }
}

void RPiKeyerTerm::on_actionOpen_Logs_Folder_triggered()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile(qApp->applicationDirPath() % "/logs"));
}

void RPiKeyerTerm::on_destGridLineEdit_returnPressed()
{
    //qDebug()<<"return pressed...";
    on_mapButton_clicked();
}

void RPiKeyerTerm::on_action_Getting_Started_triggered()
{
    QMessageBox::information(this, "Getting Started", "Set 'My Loc', 'My Call' and 'My Name' first. Then set the desired WPM for keying speed.\n\n"
"Next choose the 'Band' you are working on and record your current 'PWR Out' level.\n\nAs you make or take calls, enter the other station's call sign into the"
" field next to the 'X' button at the left end of the logging line. As your exchange takes place enter the 'Op Name' and other values as required for logging.\n\n"
"When you are ready to log the QSO, click the 'LOG' button on the toolbar to view the information and add or change the values.\n\nIf you get the Maidenhead"
" location for the other station, enter it into the 'Loc' field and press <Enter> or click the 'MAP' button to show an approximate location for the other station"
" (requires internet connection).\n\n"
"To send text, type it into the box provided and click the 'SEND' button or use <Ctrl>+<Enter> to send it.  The 'KILL TX' toolbar button may be used to stop the"
" current transmission.");
}

void RPiKeyerTerm::on_action_Networking_triggered()
{
    QMessageBox::information(this, "Server and Client Networking", "(OPTIONAL!) Networking features allow multiple operator environments"
" to switch between dedicated station installations for things such as Field Day or other group activations on multiple bands.  Operators"
" need not move to the other station, they would simply put all stations into 'Server' mode and use 'Client' mode at their own operating position"
" in order to work the chosen station.\n\n"
"Server mode allows the user to use a second copy of this program on another Raspberry Pi via a connected network to control keying.\n\n"
"Choose either 'Server Settings' or 'Client Settings' and once configured, click the corresponding checkbox menu item to enable"
" the mode selected.\n\nUse Server mode on the"
" Raspberry Pi which is connected to the radio.\n\nUse Client mode"
" on the remote Raspberry Pi computer.  Match up the IP address and port numbers on each.");
}

void RPiKeyerTerm::on_action_Macros_triggered()
{
    QMessageBox::information(this, "Creating and Using Macros", "Click the small 'gear' button at the end of the Macro toolbar to open the Macro dialog.\n\nEach macro may contain both static text"
" and the displayed text substitution values, such as '{CS}', which means the configured station call sign.  This allows the user to build powerful"
" macros for use in exchanges and contesting.  The text substitution values are taken from the currently displayed values of the station line and"
" the log line in the main UI.\n\nYou may create a short name for each macro which is displayed on it's button for easy visual reference.  Clicking a macro"
" button builds the text and replaces the text in the send text area.  Then click the 'SEND' button or <Ctrl>+<Enter> to transmit the displayed text.\n\n"
"Using the macros makes working sprint, party, *OTA and other contests very efficient and the act of entering the other station's data makes logging a"
" two button click process.");
}

void RPiKeyerTerm::on_actionE_xit_triggered()
{
    close();
}

void RPiKeyerTerm::on_action_RBN_Dialog_triggered()
{
    // open a dialog with timer that polls the HamQTH RBN data in JSON periodically
    RBNDialog *rbn = new RBNDialog(this);
    connect(this, &RPiKeyerTerm::bandChanged, rbn, &RBNDialog::setBand);
    rbn->setBand(ui->bandComboBox->currentText());
    rbn->show();
}

void RPiKeyerTerm::on_bandComboBox_activated(const QString &arg1)
{
    emit bandChanged(arg1);
}

void RPiKeyerTerm::on_actionShow_FIR_BP_Filter_triggered(bool checked)
{
    if(dw) {
        dw->setVisible(checked);
    }
}

void RPiKeyerTerm::on_actionConvert_RPKT_Log_to_ADIF_triggered()
{
    const QString fname = QFileDialog::getOpenFileName(this, "Choose RPI Keyer Term Log File", "Select the RPiKeyerTerm .log file to convert to ADIF", "*.log" );
    //qDebug()<<"log file:"<<fname;
    QFile f(fname);
    f.open(QFile::ReadOnly);
    QString lineses = f.readAll();
    f.close();
    const QStringList lines = lineses.split("\n");
    foreach(QString logline, lines) {
        if(logline.trimmed().isEmpty())
            continue;
        const QString adif = lu.logLineToADIF(logline.trimmed());
        QFile logout("logs/RPiKeyerTerm.adi");
        if(logout.open(QFile::ReadWrite | QFile::Append)) {
            logout.write(adif.toLatin1());
            logout.close();
        }
    }
}
