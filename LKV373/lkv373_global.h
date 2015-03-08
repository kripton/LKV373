#ifndef LKV373_GLOBAL_H
#define LKV373_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LKV373_LIBRARY)
#  define LKV373SHARED_EXPORT Q_DECL_EXPORT
#else
#  define LKV373SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // LKV373_GLOBAL_H
