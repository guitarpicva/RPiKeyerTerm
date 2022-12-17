// ----------------------------------------------------------------------------
// Copyright (C) 2019 GrizzWorks, LLC
// ALL RIGHTS RESERVED
// ----------------------------------------------------------------------------
#include "scrollingdigitlabel.h"
#include <QDebug>

void ScrollingDigitLabel::mouseDoubleClickEvent(QMouseEvent *event) {
    qDebug()<<"double-click!";
    emit goToZero(); // tell others listening to set to zero, usually to the right
    emit valueChanged(0);
    event->accept();
}

void ScrollingDigitLabel::wheelEvent(QWheelEvent *event)
{    // test the transform of a digit label by mouse wheel
    QPoint numPixels = event->pixelDelta();
    QPoint numDegrees = event->angleDelta() / 8;
    bool b_dir = 0;
    if (!numPixels.isNull()) {
        b_dir = (numPixels.ry() > 0);
    } else if (!numDegrees.isNull()) {
        QPoint numSteps = numDegrees / 15;
        b_dir = (numSteps.ry() > 0);
    }
    int next;
    if(b_dir)
    {
        next = this->text().toInt() + 1;
        if(!b_roll && (next >= i_max))
        {
            //qDebug()<<"goToZero"<<next;
            emit goToZero();
            next = i_max;
        }
        else if(next > i_max)
        {
            //qDebug()<<"b_roll:"<<b_roll;
            if(b_roll)
            {
                next = i_min;
                emit rolledUp();
            }
        }
        this->setText(QString::number(next));
        emit valueChanged(next);
    }
    else
    {
        b_hitMax = false;
        next = this->text().toInt() - 1;
        if(next < i_min)
        {
            if(b_roll)
            {
                next = i_max;
                emit rolledDown();
            }
        }
        this->setText(QString::number(next));
        emit valueChanged(next);
    }
    event->accept();
}

void ScrollingDigitLabel::on_rolledUp()
{
    int next = this->text().toInt() + 1;
    //qDebug()<<"on_rolledUp()"<<next;
    if(!b_roll && (next >= i_max))
    {
        //qDebug()<<"goToZero";
        if(b_markMax)
            emit hitMax();
        emit goToZero();
        this->setText(QString::number(i_max));
        return;
    }
    if(next > i_max)
    {
        if(b_roll)
        {
            next = i_min;
            emit rolledUp();  // this digit needs to talk to the one on it's left also
            this->setText(QString::number(next));
        }
    }
    else {
        this->setText(QString::number(next));
    }
    //emit valueChanged(next);
}

void ScrollingDigitLabel::on_rolledDown()
{
    int prev = this->text().toInt() - 1;
    if(!b_roll && (prev == i_min))
    {
        // qDebug()<<"goToZero";
        //
        emit goToZero();
        this->setText(QString::number(prev));
        return;
    }
    if(prev < i_min)
    {
        if(b_roll)
        {
            prev = i_max;
            emit rolledDown();  // this digit needs to talk to the one on it's left also
            this->setText(QString::number(prev));
        }
    }
    else {
        this->setText(QString::number(prev));
    }
    //emit valueChanged(prev);
}

void ScrollingDigitLabel::on_goToZero()
{
    this->setText("0");
    //emit valueChanged(0);
}

