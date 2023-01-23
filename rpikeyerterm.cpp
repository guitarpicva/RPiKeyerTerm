#include "rpikeyerterm.h"
#include "ui_rpikeyerterm.h"
#include "alphabet.h"
#include "maidenhead.h"

#include <QDesktopServices>
#include <QDir>
//#include <QMessageBox>
#include <QInputDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkInterface>
#include <QThread>
#include <QDebug>

RPiKeyerTerm::RPiKeyerTerm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RPiKeyerTerm)
{
    QDir d(".");
    d.mkdir("logs");
    ui->setupUi(this);
    // set up the GPIO pin from settings and initialize it and test it
    settings = new QSettings("RPiKeyerTerm.ini", QSettings::IniFormat);
    loadSettings();
    loadMacros();
    chip = gpiod_chip_open_by_name("gpiochip0");
    line = gpiod_chip_get_line(chip, GPIO21);
    gpiod_line_request_output(line, "keyline", 0); // set to output direction
    gpiod_line_set_value(line, 0); // ensure low to start, not really needed

//   gpiod_line_set_value(line, 1);
//    qDebug()<<gpiod_line_get_value(line);
//    QThread::msleep(1000);
//    gpiod_line_set_value(line, 0);
//    qDebug()<<gpiod_line_get_value(line);
//    QThread::msleep(1000);
//    gpiod_line_set_value(line, 1);
//    qDebug()<<gpiod_line_get_value(line);
//    QThread::msleep(1000);
//    gpiod_line_set_value(line, 0);
//    qDebug()<<gpiod_line_get_value(line);
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
            qDebug()<<"Macro:"<<index<<toSend;
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
    restoreState(settings->value("windowState", "").toByteArray());
}

void RPiKeyerTerm::saveSettings()
{    
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

void RPiKeyerTerm::on_receiveTextClearButton_clicked()
{
    ui->receiveTextArea->clear();
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
    qDebug()<<"Send Text:"<<socket<<server;
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

void RPiKeyerTerm::on_actionLOG_triggered(bool checked)
{
    // do log dialog
    if(checked) {
        if(!ld) {
            ld = new LogDialog(this);
            connect(ld, &LogDialog::logSaved, this, [=]{ui->actionLOG->setChecked(false);});
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
