#include "maidenhead.h"
#include "qdebug.h"

Maidenhead::Maidenhead(const QString mhin, QObject *parent) : QObject(parent)
{
    //qDebug()<<"MHtoLL: FM16"<<mh2ll("FM16")<<mh2ll("FM16dw");
    if(!mhin.trimmed().isEmpty())
        mh2ll(mhin.trimmed());
}

QPair<double, double> Maidenhead::mh2ll(const QString mh)
{
    bool ok;
    double lon = 0.0;
    double lat = 0.0;
    QByteArray ba2 = mh.toLatin1();
    // latitude calcs  sample: FM16dw
    int cc2 = ba2.mid(1, 1).toUpper().toHex().toInt(&ok, 16);
    cc2 -= 65; // hex of capitol letter of alphabet - 'A' = deg above South pole * 1/10
    cc2 *= 10; // = degrees above zero at South Pole
    // example: M = 120 deg. North of the South pole

    int cc4 = ba2.mid(3, 1).toInt(&ok, 10);

    if(ba2.length()>4) {
        double cc6 = 0;
        cc6 = ba2.mid(5, 1).toLower().toHex().toInt(&ok, 16);
        //qDebug()<<"cc6:"<<cc6;
        cc6 -= 97.0;
        //qDebug()<<"cc6-1:"<<cc6;
        cc6 /= 24.0;
        //qDebug()<<"cc6-2:"<<cc6;
        cc6 += (1.0/48.0);
        // qDebug()<<"cc6-3:"<<cc6;
        cc6 -= 90;
        //qDebug()<<"cc6-4:"<<cc6;

        lat = (double)cc2 + (double)cc4 + cc6;
    }
    else {
        //qDebug()<<"short version";
        lat = ((double)cc2 - 90) + (double)cc4;
    }

    // end latitude calcs

    // longitude calcs
    int cc1 = ba2.mid(0, 1).toUpper().toHex().toInt(&ok, 16);
    cc1 -= 65;
    cc1 *= 20;

    int cc3 = ba2.mid(2, 1).toInt(&ok, 10) * 2;

    if(ba2.length() > 4) {
        double cc5 = 0;
        cc5 = ba2.mid(4, 1).toLower().toHex().toInt(&ok, 16);
        cc5 -= 97.0;
        cc5 /= 12.0;
        cc5 += (1.0/24.0);
        lon = ((double)cc1 + (double) cc3 + cc5) - 180.0;
    }
    else {
        lon = (double)cc1 + (double)cc3 - 180.0;
    }
    // end longitude calcs

    qDebug()<<"MH:"<<ba2; //<<cc2<<cc4;
    qDebug()<<"lat:"<<lat;
    //qDebug()<<cc1<<cc3;
    qDebug()<<"lon:"<<lon;
    return QPair<double, double>(lat, lon);
}
