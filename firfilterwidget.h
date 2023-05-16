#ifndef FIRFILTERWIDGET_H
#define FIRFILTERWIDGET_H

#include "firbpfilter.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QAudioInput>
#include <QAudioOutput>
#include <QBuffer>
#include <QDataStream>
#include <QQueue>
#include <QSettings>
#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class FIRFilterWidget; }
QT_END_NAMESPACE

class FIRFilterWidget : public QWidget
{
    Q_OBJECT

public:
    FIRFilterWidget(QWidget *parent = nullptr);
    ~FIRFilterWidget();

private slots:
    void on_rxInReadyRead();
    void handle_ainStateChanged();
    void on_aoutReadyRead();
    void handle_aoutStateChanged();
    void on_ainNotify();
    void on_aOutNotify();
    void on_cwBpNarrowRadioButton_clicked();
    void on_cwBpMediumRadioButton_clicked();
    void on_cwBpWideRadioButton_clicked();
    void on_cwBpAudioInputComboBox_activated(int index);
    void on_cwBpAudioOutputComboBox_activated(int index);
    void on_cwBpFilterCenterSpinBox_valueChanged(double arg1);
    void on_cwBpFilterBWSpinBox_valueChanged(double arg1);
    void on_swFilterGroupBox_toggled(bool arg1);
    void cleanupBuffers();
    void on_inputLevelPercentageSlider_valueChanged(int arg1);
    void on_outputLevelPercentageSlider_valueChanged(int value);

private:
    Ui::FIRFilterWidget *ui;
    FirBpFilter *ff = nullptr;
    double d_flo, d_fhi, d_fcf, d_fbw;
    QSettings *settings = nullptr;
    QDataStream ds; // for writing floats to a QByteArray
    QAudioFormat fmt; // used for both
    QAudioInput *ain = nullptr;
    QAudioOutput *aout = nullptr;
    QByteArray ba; // the transform byte array
    QByteArray in; // the inbuf byte array
    QByteArray out; // the outbuf byte array
    QBuffer *inbuf = nullptr;
    QBuffer *outbuf = nullptr;
    QQueue<float> transform;
    void connectToRxAudioIn();
    void connectToRxAudioOut();
};
#endif // FIRFILTERWIDGET_H
