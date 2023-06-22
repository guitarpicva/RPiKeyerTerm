#include "rbndialog.h"
#include "rbndialog.h"
#include "ui_rbndialog.h"

#include <QTimer>

RBNDialog::RBNDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RBNDialog)
{
    ui->setupUi(this);
    setModal(false);
    this->setWindowTitle("RBN Spots");
    this->setAttribute(Qt::WA_DeleteOnClose, true);
    manager = new QNetworkAccessManager(this);
    connect(manager, &QNetworkAccessManager::finished, this, &RBNDialog::replyFinished);
    //manager->get(QNetworkRequest(QUrl("https://www.hamqth.com/rbn_data.php?data=1&mode=CW&waz=*&itu=*&age=30&order=3&band=" + s_band)));
    manager->get(QNetworkRequest(QUrl("https://www.reversebeacon.net/spots.php?h=fb94a8&b=" + s_band  + "&m=1&s=1614482236&r=10")));
}

RBNDialog::~RBNDialog()
{
    delete ui;
}

void RBNDialog::setBand(const QString band)
{
    // need to resolve the band number from the band name here
    s_band = band2num.value(band);
    qDebug()<<"set band:"<<s_band<<band;
    on_rbnRefreshButton_clicked(); // get the new data
}

void RBNDialog::replyFinished(QNetworkReply *reply)
{
    //qDebug().noquote()<<reply->readAll();
    ui->rbnTableWidget->clear();
    ui->rbnTableWidget->setHorizontalHeaderLabels(headerList);

    //qDebug()<<"row count:"<<ui->rbnTableWidget->rowCount();
//    for(int i = 0; i < 10; i++)
//        ui->rbnTableWidget->removeRow(0);
//qDebug()<<"new row count:"<<ui->rbnTableWidget->rowCount();

    QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
    //qDebug().noquote()<<"JSON doc:"<<doc.toJson();
    QJsonObject obj = doc.object();
    QVariantMap map = obj.toVariantMap();

    //qDebug()<<"JSON Map size:"<<map.size();


    foreach(const QString key, map.keys()) {
        //qDebug()<<"map key:"<<key;
        if(key == "spots") {
            QVariantMap imap = map.value(key).toJsonObject().toVariantMap();
            QString age, dxcall, freq, mode;
            ui->rbnTableWidget->setRowCount(10);
//            foreach(const QString ikey, imap.keys())
//            {
//                qDebug()<<"key:"<<ikey;
//                if(ikey == "lsn" || ikey == "mode")continue;
//                if(ikey == "age") age = imap.value(ikey).toString();
//                else if(ikey == "dxcall") dxcall = imap.value(ikey).toString();
            //                else if(ikey == "freq") freq = imap.value(ikey).toString();
            //                //else if(ikey == "age") mode = imap.value(ikey).toString();

            //            }
            int i = 0; // row index
            foreach(const QString ikey, imap.keys()) {
                //qDebug()<<"ikey value:"<<ikey<<(imap.value(ikey))<<"\n";
                const QVariantList line = imap.value(ikey).toList();
                freq = line.at(1).toString();
                dxcall = line.at(2).toString();
                age = line.at(4).toString();
                //ui->rbnTableWidget->insertRow(i);
                ui->rbnTableWidget->setItem(i, 0, new QTableWidgetItem(dxcall));
                ui->rbnTableWidget->setItem(i, 1, new QTableWidgetItem(freq));
                //while(age.length() < 2) age.prepend('0');
                ui->rbnTableWidget->setItem(i, 2, new QTableWidgetItem(age));
                ui->rbnTableWidget->sortByColumn(2, Qt::AscendingOrder);
                //qDebug()<<"end record";
                ++i;
            }
            break;
        }
    }
}

void RBNDialog::on_rbnRefreshButton_clicked()
{
    manager->get(QNetworkRequest(QUrl("https://www.reversebeacon.net/spots.php?h=fb94a8&b=" + s_band  + "&m=1&s=1614482236&r=10")));
}

void RBNDialog::on_label_linkHovered(const QString &link)
{
    setToolTip(link);
}
