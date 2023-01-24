#include "logutils.h"

#include <QStringBuilder>

#include <QDebug>
logutils::logutils(QObject *parent) : QObject(parent)
{

}

QString logutils::logLineToADIF(QString line)
{
    QString out, tmp;
    int len = 0;
    QStringList parts = line.split(",", Qt::KeepEmptyParts);
    // format dateon/timeon/dateoff/timeoff/other/loc/freqband/mode/rptout/rptin/pwr/comment/opname
    //parts: ("2022-12-21", "21:24", "2022-12-21", "21:24", "KN4YTA", "NC", "10111000", "CW", "55N", "55N", "5.0", "POTA", "")
    tmp = parts.at(1).trimmed().remove(":");
    len = tmp.length();
    out = "<QSO_DATE:8>" % QString(parts.at(0)).replace("-", "") % "<TIME_ON:" % QString::number(len) % ">" % tmp;
    tmp = parts.at(6).trimmed() + "M";
    len = tmp.length();
    out.append("<BAND:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(7).trimmed();
    len = tmp.length();
    out.append("<MODE:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(8).trimmed();// RST_Sent
    len = tmp.length();
    out.append("<RST_Sent:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(9).trimmed();
    len = tmp.length();
    out.append("<RST_Rcvd:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(10).trimmed();
    len = tmp.length();
    out.append("<TX_PWR:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(4).trimmed(); // power out
    len = tmp.length();
    out.append("<CALL:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(4).trimmed(); // op name
    len = tmp.length();
    out.append("<NAME:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(12).trimmed(); // loc/grid
    len = tmp.length();
    out.append("<GRIDSQUARE:" % QString::number(len) % ">" % tmp);
    tmp = parts.at(11).trimmed(); // comment
    len = tmp.length();
    out.append("<COMMENT:" % QString::number(len) % ">" % tmp);
    out.append("<EOR>");
    //qDebug()<<out;
    return out;
}
