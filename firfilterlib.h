#ifndef FIRFILTERLIB_H
#define FIRFILTERLIB_H

#include "firfilterwidget.h"
#include "FIRFilterLib_global.h"

class FIRFILTERLIB_EXPORT FIRFilterLib
{
public:
    FIRFilterLib();
    FIRFilterWidget * createFIRFilterWidget();
};

#endif // FIRFILTERLIB_H
