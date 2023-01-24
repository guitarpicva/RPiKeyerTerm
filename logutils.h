#ifndef LOGUTILS_H
#define LOGUTILS_H

#include <QObject>

class logutils : public QObject
{
    Q_OBJECT
public:
    explicit logutils(QObject *parent = nullptr);

public slots:
    QString logLineToADIF(QString line);
signals:

};

#endif // LOGUTILS_H
