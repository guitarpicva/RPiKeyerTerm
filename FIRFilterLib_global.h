#ifndef FIRFILTERLIB_GLOBAL_H
#define FIRFILTERLIB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(FIRFILTERLIB_LIBRARY)
#  define FIRFILTERLIB_EXPORT Q_DECL_EXPORT
#else
#  define FIRFILTERLIB_EXPORT Q_DECL_IMPORT
#endif

#endif // FIRFILTERLIB_GLOBAL_H
