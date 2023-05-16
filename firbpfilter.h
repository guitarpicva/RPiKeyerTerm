#ifndef FIRBPFILTER_H
#define FIRBPFILTER_H

// FLDIGI includes, may dwindle
#include <complex>

#ifndef M_PI
#define M_PI (3.1415926535897932385)
#endif
#define FIRBufferLen 4096
// END FLGIDI includes

#include <QObject>
using namespace std;

class FirBpFilter : public QObject
{
    Q_OBJECT
public:
    explicit FirBpFilter(QObject *parent = nullptr);

    void init (int len, int dec, double *ifil, double *qfil);
    void init_lowpass (int len, int dec, double freq );
    void init_bandpass (int len, int dec, double freq1, double freq2);
    void init_hilbert (int len, int dec);
    double *bp_FIR(int len, int hilbert, double f1, double f2);

    void clear();
    int run (const complex<double> &in, complex<double> &out);
    int Irun (const double &in, double &out);
    int Qrun (const double &in, double &out);
signals:
    // ----------------------------------------------------------------------------
    //
    // FirBpFilter.h  --  Digital Bandpass Filter class used in fldigi
    //
    // From the work of Copyright (C) 2006-2008	Dave Freese, W1HKJ
    //
    //    This file is part of fldigi.  These filters are based on the
    //    gmfsk design and the design notes given in
    //    "Digital Signal Processing", A Practical Guid for Engineers and Scientists
    //	  by Steven W. Smith.
    //
    // Fldigi is free software: you can redistribute it and/or modify
    // it under the terms of the GNU General Public License as published by
    // the Free Software Foundation, either version 3 of the License, or
    // (at your option) any later version.
    //
    // Fldigi is distributed in the hope that it will be useful,
    // but WITHOUT ANY WARRANTY; without even the implied warranty of
    // MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    // GNU General Public License for more details.
    //
    // You should have received a copy of the GNU General Public License
    // along with fldigi.  If not, see <http://www.gnu.org/licenses/>.
    // ----------------------------------------------------------------------------

    //=====================================================================
    // FIR filters
    //=====================================================================
private:
    int length;
    int decimateratio;

    double *ifilter;
    double *qfilter;

    double ffreq;

    double ibuffer[FIRBufferLen];
    double qbuffer[FIRBufferLen];

    int pointer;
    int counter;

    //_complex fu;

    inline double sinc(double x) {
        if (fabs(x) < 1e-10)
            return 1.0;
        else
            return sin(M_PI * x) / (M_PI * x);
    }
    inline double cosc(double x) {
        if (fabs(x) < 1e-10)
            return 0.0;
        else
            return (1.0 - cos(M_PI * x)) / (M_PI * x);
    }
    inline double hamming(double x) {
        return 0.54 - 0.46 * cos(2 * M_PI * x);
    }
    inline double mac(const double *a, const double *b, unsigned int size) {
        // MW bp params (iptr - length), ifilter, length (heap values)
        double sum = 0.0;
        double sum2 = 0.0;
        double sum3 = 0.0;
        double sum4 = 0.0;
        // Reduces read-after-write dependencies : Each subsum does not wait for the others.
        // The CPU can therefore schedule each line independently.
        // MW +=  4 and -= 4 because we are using 32 bit float samples?
        for (; size > 3; size -= 4, a += 4, b+=4)
        {
            sum  += a[0] * b[0];
            sum2 += a[1] * b[1];
            sum3 += a[2] * b[2];
            sum4 += a[3] * b[3];
        }
        for (; size; --size)
            sum += (*a++) * (*b++);
        return sum + sum2 + sum3 + sum4 ;
    }

protected:


};
#endif // FIRBPFILTER_H
