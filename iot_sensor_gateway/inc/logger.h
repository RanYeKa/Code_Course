// logger.h
#pragma once

#include <stdarg.h>
#include <stdio.h>

#define log_dbg(format, ...)  logging_backend("DBG", __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_err(format, ...)  logging_backend("ERR", __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_info(format, ...) logging_backend("INFO", __FILE__, __LINE__, format, ##__VA_ARGS__)
#define log_warn(format, ...) logging_backend("WARN", __FILE__, __LINE__, format, ##__VA_ARGS__)


// static void ensure_directory_exists(const char* filepath);
int logger_init(const char* filename);

void logger_close(void);
void logger_reload(void);

void logging_backend(const char* prefix, const char* file, int line, const char* format, ...);