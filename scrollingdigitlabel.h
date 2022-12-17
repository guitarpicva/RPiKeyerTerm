// ----------------------------------------------------------------------------
// Copyright (C) 2019 GrizzWorks, LLC
// ALL RIGHTS RESERVED
// ----------------------------------------------------------------------------
#ifndef SCROLLINGDIGITLABEL_H
#define SCROLLINGDIGITLABEL_H

#include <QMouseEvent>
#include <QWheelEvent>
#include <QLabel>
/**
 * @brief The ScrollingDigitLabel class
 * The scrolling digit is a bit of a mis-nomer, because the "digit" can simply
 * be any value which is integer incrementable, and it not limited to one digit.
 *
 * The idea was to use a set of these digits to facilitate mouse wheel control
 * over the single digits in a frequency display.
 *
 * The scrolling "digit" could however be simply a numeral representing a
 * channel number perhaps...say channel numbers 1-1000.
 *
 * Usage: create the digit object, set the min and max values and whether the digit
 * will roll when it is pushed past the min or max values.  min rolls down to max
 * and max rolls down to min
 */
class ScrollingDigitLabel : public QLabel
{
Q_OBJECT
signals:
    void rolledUp(); // tell a neighbor I have rolled up
    void rolledDown(); // tell a neighbor I have rolled down
    void goToZero(); // tell a neighbor that they should set themself to zero
    void hitMax(); // this object has set b_markMax and we are there
    void valueChanged(const int newval); // this object's value has changed.
public:
    explicit ScrollingDigitLabel(QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
        : QLabel( parent, f )
    {
        this->setMargin(1);
        this->setAlignment(Qt::AlignHCenter);
    }

    explicit ScrollingDigitLabel(const QString &text, QWidget *parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags())
    : QLabel( text, parent, f )
    {
        this->setMargin(1);
        this->setAlignment(Qt::AlignHCenter);
    }
    // lowest digit displayed
    int i_min = 0;
    // highest digit displayed
    int i_max = 9;
    // the digit should roll from max to min and min to max
    // and provide signals to other listening digits
    bool b_roll = true;
    // when a b_roll false digit hits it's max, signal
    // and set b_hitMax
    bool b_markMax = false;
    // if set, then this digit is at it's maximum value
    // implies that b_markMax = true;
    bool b_hitMax = false;
    // setters
    void setMin(const int newMin) {i_min = newMin;}
    void setMax(const int newMax) {i_max = newMax;}
    void setMarkMax(bool set) {b_markMax = set;}
    void setHitMax(bool set) {b_hitMax = set;}
    // overridden double click event to set all to the right to zero
    void mousePressEvent(QMouseEvent *event) {event->accept();}
    void mouseReleaseEvent(QMouseEvent *event) {event->accept();}
    void mouseMovedEvent(QMouseEvent *event) {event->accept();}
    void mouseDoubleClickEvent(QMouseEvent *event);
    // overridden wheel event which does all the magic
    void wheelEvent(QWheelEvent *event);
public slots:
    // when a neighbor says "I rolled up"
    void on_rolledUp();
    // when a neighbor says "I rolled down"
    void on_rolledDown();
    // when a neighbor says so, set to zero.  This implies initial usage of the
    // digits from left to right (higest to lowest).
    void on_goToZero();
};

#endif // SCROLLINGDIGITLABEL_H

