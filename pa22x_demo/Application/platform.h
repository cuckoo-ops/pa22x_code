#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef Q_OS_DARWIN
#define _countof(array) sizeof(array) / sizeof(array[0])
#endif

#endif // PLATFORM_H
