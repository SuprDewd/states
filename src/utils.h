#ifndef UTILS_H
#define UTILS_H

#include <cstdio>

#define DEBUG 0

#if DEBUG
#define DEBUGMSG(fmt, ...) \
    fprintf(stderr, fmt, ##__VA_ARGS__)
#else
#define DEBUGMSG(fmt, ...)
#endif

template <typename T> int size(const T &x) { return x.size(); }

#endif
