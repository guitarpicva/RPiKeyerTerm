#include "rpikeyerterm.h"
#include "ui_rpikeyerterm.h"
#include "alphabet.h"

#include <QThread>

#include <QDebug>

RPiKeyerTerm::RPiKeyerTerm(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RPiKeyerTerm)
{
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
    if(toSend.contains("{")) {
        if(toSend.contains("{CS}")) {
            out = toSend.replace("{CS}", ui->mycallLineEdit->text().trimmed().toUpper());
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
    }
    return out;
}

void RPiKeyerTerm::macroTriggered(const int index)
{
    QString toSend = settings->value("Macros/macro" + QString::number(index) + "Text", "").toString();
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    ui->sendTextArea->setPlainText(resolveTextSubstitutions(toSend));
    ui->sendTextArea->moveCursor(QTextCursor::Down);
    ui->sendTextArea->setFocus();
}

void RPiKeyerTerm::on_actionMAC0_triggered()
{
    macroTriggered(0);
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

void RPiKeyerTerm::on_actionMAC12_triggered()
{
    macroTriggered(12);
}

void RPiKeyerTerm::on_pushButton_2_clicked()
{
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    ui->receiveTextArea->insertPlainText("\nRECEIVED: " + ui->conversationTextEdit->toPlainText().trimmed());
    ui->sendTextArea->setFocus();
}

void RPiKeyerTerm::loadSettings()
{
    ui->mycallLineEdit->setText(settings->value("mycall", "N0CALL").toString());
    ui->mhGridLineEdit->setText(settings->value("mygrid", "FM00").toString());
    ui->sendingSpeedSpinBox->setValue(settings->value("sendingSpeed", 15).toInt());
    QStringList items = settings->value("heardList", "").toStringList();
    ui->heardListComboBox->addItems(items);            
}

void RPiKeyerTerm::saveSettings()
{    
    settings->setValue("mycall", ui->mycallLineEdit->text().trimmed().toUpper());
    settings->setValue("mygrid", ui->mhGridLineEdit->text().trimmed().toUpper());
    settings->setValue("sendingSpeed", ui->sendingSpeedSpinBox->value());
    QStringList items;
    for(int i = 0; i < ui->heardListComboBox->count(); i++) {
        items<<ui->heardListComboBox->itemText(i);
    }
    items.removeDuplicates();
    settings->setValue("heardList", items);
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
    for(int i = 0; i < 7; i++)
    {
        label = s.value("Macros/macro" + QString::number(i) + "Label", "").toString();
        content = s.value("Macros/macro" + QString::number(i) + "Text", "").toString();
        if(label.isEmpty())
            label = "MAC" + QString::number(i);
        switch(i){
        case 0: ui->actionMAC0->setText(label);ui->actionMAC0->setToolTip(content);break;
        case 1: ui->actionMAC1->setText(label);ui->actionMAC1->setToolTip(content);break;
        case 2: ui->actionMAC2->setText(label);ui->actionMAC2->setToolTip(content);break;
        case 3: ui->actionMAC3->setText(label);ui->actionMAC3->setToolTip(content);break;
        case 4: ui->actionMAC4->setText(label);ui->actionMAC4->setToolTip(content);break;
        case 5: ui->actionMAC5->setText(label);ui->actionMAC5->setToolTip(content);break;
        case 6: ui->actionMAC6->setText(label);ui->actionMAC6->setToolTip(content);break;
        case 7: ui->actionMAC7->setText(label);ui->actionMAC7->setToolTip(content);break;
        case 8: ui->actionMAC8->setText(label);ui->actionMAC8->setToolTip(content);break;
        case 9: ui->actionMAC9->setText(label);ui->actionMAC9->setToolTip(content);break;
        case 10: ui->actionMAC10->setText(label);ui->actionMAC10->setToolTip(content);break;
        case 11: ui->actionMAC11->setText(label);ui->actionMAC11->setToolTip(content);break;
        }

    }
}

void RPiKeyerTerm::on_sendButton_clicked()
{
    tokey = ui->sendTextArea->toPlainText().trimmed().toUpper().remove(QRegularExpression("[\r\n]")); // no line ends in Morse
    //tokey = resolveTextSubstitutions(tokey);
    const int end = tokey.size(); // so when we pop this doesn't change
    ui->receiveTextArea->moveCursor(QTextCursor::End);
    for(int i = 0; i < end; i++) {
        keystr.clear();
        keychar = tokey.at(i);
        ui->receiveTextArea->insertPlainText(keychar); // echo to rx area
        qApp->processEvents(QEventLoop::ExcludeUserInputEvents); // only update the text area
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
}

void RPiKeyerTerm::on_sendingSpeedSpinBox_valueChanged(int arg1)
{
    if(arg1 < 5) arg1 = 5; // min speed is 5 WPM
    dit = (int) 1200/arg1;
}


void RPiKeyerTerm::on_tuneButton_clicked(bool checked)
{
    if(checked)
        gpiod_line_set_value(line, 1); // key on
    else
        gpiod_line_set_value(line, 0); // key off
}
