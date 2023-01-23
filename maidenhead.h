#ifndef MAIDENHEAD_H
#define MAIDENHEAD_H

//#include <QtMath>

#include <QByteArray>
#include <QPair>
//#include <QString>

#include <QDebug>

#include "qobjectdefs.h"
class Maidenhead : public QObject
{
    Q_OBJECT
public:
    Maidenhead(const QString mhin = "", QObject *parent = nullptr);
    QPair<double, double> mh2ll(const QString mh);
    QPair<double, double> ll;
};

#endif // MAIDENHEAD_H
