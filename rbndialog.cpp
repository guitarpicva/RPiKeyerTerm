#include "rbndialog.h"
#include "rbndialog.h"
#include "ui_rbndialog.h"

#include <QTimer>

RBNDialog::RBNDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RBNDialog)
{
    ui->setupUi(this);
    this->setAttribute(Qt::WA_DeleteOnClose, true);

    // set up the table widget

    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RBNDialog::replyFinished);

    manager->get(QNetworkRequest(QUrl("https://www.hamqth.com/rbn_data.php?data=1&mode=CW&waz=*&itu=*&age=30&order=3&band=" + s_band)));
}

RBNDialog::~RBNDialog()
{
    delete ui;
}

void RBNDialog::setBand(const QString band)
{
    s_band = band;
    on_rbnRefreshButton_clicked(); // get the new data
}

void RBNDialog::replyFinished(QNetworkReply *reply)
{
//    qDebug()<<reply->readAll();
    ui->rbnTableWidget->clear();
    ui->rbnTableWidget->setHorizontalHeaderLabels(headerList);

    for(int i = 0; i < ui->rbnTableWidget->rowCount(); i++) ui->rbnTableWidget->removeRow(0);
    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    //qDebug()<<"JSON:"<<doc.toJson();
    QJsonObject obj = doc.object();
    QVariantMap map = obj.toVariantMap();

    //qDebug()<<"JSON Map size:"<<map.size();
    int i = 0;
    foreach(const QString key, map.keys()) {
        QVariantMap imap = map.value(key).toJsonObject().toVariantMap();
        QString age, dxcall, freq, mode;
        ui->rbnTableWidget->setRowCount(imap.size());
        foreach(const QString ikey, imap.keys())
        {
            if(ikey == "lsn" || ikey == "mode")continue;
            if(ikey == "age") age = imap.value(ikey).toString();
            else if(ikey == "dxcall") dxcall = imap.value(ikey).toString();
            else if(ikey == "freq") freq = imap.value(ikey).toString();
            //else if(ikey == "age") mode = imap.value(ikey).toString();

        }

        QString out = "<p><pre>" + dxcall;
        out.append(" " + freq);
        out.append(" " + age + "</pre></p>");
        //qDebug()<<"list keys:"<<dxcall<<freq<<age;
        //ui->rbnTextEdit->appendHtml(out);
        ui->rbnTableWidget->insertRow(i);
        ui->rbnTableWidget->setItem(i, 0, new QTableWidgetItem(dxcall));
        ui->rbnTableWidget->setItem(i, 1, new QTableWidgetItem(freq));
        while(age.length() < 2) age.prepend('0');
        ui->rbnTableWidget->setItem(i, 2, new QTableWidgetItem(age));
        ui->rbnTableWidget->sortByColumn(2, Qt::AscendingOrder);
        //qDebug()<<"end record";
        ++i;
    }
}

void RBNDialog::on_rbnRefreshButton_clicked()
{
    manager->get(QNetworkRequest(QUrl("https://www.hamqth.com/rbn_data.php?data=1&mode=CW&waz=*&itu=*&age=30&order=3&band=" + s_band)));
}
