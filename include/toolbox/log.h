#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#define INFO(...) (printf("INFO: [%40s: %40s]: ", __FILE__, __func__), printf(__VA_ARGS__))
#define ERROR(...) (printf("ERROR [%40s]: ", __func__), printf(__VA_ARGS__))
#endif // LOG_H
