#ifndef RBNDIALOG_H
#define RBNDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDebug>

namespace Ui {
class RBNDialog;
}

class RBNDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RBNDialog(QWidget *parent = nullptr);
    ~RBNDialog();

public slots:
    void setBand(const QString band);

private slots:
    void replyFinished(QNetworkReply *reply);
    void on_rbnRefreshButton_clicked();

    void on_label_linkHovered(const QString &link);

private:
    Ui::RBNDialog *ui;
    QJsonDocument *rbnDoc = nullptr;
    QNetworkAccessManager *manager = nullptr;
    QStringList headerList = QStringList()<<"DX Call"<<"Frequency"<<"WPM";
    QString s_band = "30";
    QHash<QString, QString> band2num = {
        {"160","3"},
        {"80", "7"},
        {"60", "84"},
        {"40", "12"},
        {"30", "17"},
        {"20", "22"},
        {"17", "27"},
        {"15", "32"},
        {"12", "37"},
        {"10", "42"},
        {"6", "50"},
        {"4", "55"},
        {"2", "62"}
                                       };
};

#endif // RBNDIALOG_H
