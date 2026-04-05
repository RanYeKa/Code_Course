// logger.c
#include "logger.h"

#include <pthread.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h> // For mkdir()
#include <errno.h>


static FILE* g_log_file = NULL;
static pthread_mutex_t logger_lock = PTHREAD_MUTEX_INITIALIZER;
static char g_filepath[256] = {0};


// Helper function to extract just the folder path and create it
static void ensure_directory_exists(const char* filepath) {
    char temp_path[256];
    strncpy(temp_path, filepath, sizeof(temp_path) - 1);

    // Find the last slash in the path (e.g., "/home/user/logs/app.log" -> points to "/app.log")
    char* last_slash = strrchr(temp_path, '/');
    if (last_slash != NULL) {
        *last_slash = '\0'; // Chop off the filename, leaving just "/home/user/logs"

        // Try to create the directory (0777 means standard read/write/execute permissions)
        // If it already exists, mkdir returns -1 with errno == EEXIST, which is perfectly fine!
        mkdir(temp_path, 0777);
    }
}

int logger_init(const char* filename) {
    // 1. Ensure the folder actually exists!
    ensure_directory_exists(filename);

    // 2. Save the filepath for future reloads (Trap 3 fix!)
    strncpy(g_filepath, filename, sizeof(g_filepath) - 1);

    // 3. Open the file
    g_log_file = fopen(g_filepath, "a");
    if (g_log_file == NULL) {
        return -1;
    }
    return 0;
}

void logger_close(void) {
    pthread_mutex_lock(&logger_lock);
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = NULL;
    }
    pthread_mutex_unlock(&logger_lock);
}

void logger_reload(void) {
    pthread_mutex_lock(&logger_lock);
    if (g_log_file) {
        fclose(g_log_file);
        g_log_file = fopen(g_filepath, "a");
    }
    pthread_mutex_unlock(&logger_lock);
}

void logging_backend(const char* prefix, const char* file, int line, const char* format, ...)
{
    pthread_mutex_lock(&logger_lock);
    char time_buff[32];
    time_t now = time(NULL);
    struct tm t_info;
    localtime_r(&now, &t_info);
    strftime(time_buff, sizeof(time_buff), "%Y-%m-%d %H:%M:%S", &t_info);
    FILE* dest = (g_log_file) ? g_log_file : stdout;
    fprintf(dest, "[%s] [%s] %s:%d - ", time_buff, prefix, file, line);

    va_list args; // 1. Create the list pointer
    va_start(args, format); // 2. Tell it to start capturing immediately AFTER 'format'

    vfprintf(dest, format, args); // 3. Let vprintf handle the magic mapping of %d, %s, etc.

    va_end(args); // 4. Clean up
    fprintf(dest, "\n");
    fflush(dest); // Ensure the message is written out immediately
    pthread_mutex_unlock(&logger_lock);
}

void logger_raw(const char* format, ...) {
    pthread_mutex_lock(&logger_lock);

    // Pick the log file if open, otherwise default to stdout
    FILE* dest = (g_log_file) ? g_log_file : stdout;

    va_list args;
    va_start(args, format);
    vfprintf(dest, format, args);
    va_end(args);

    // Optional: flush so it shows up immediately even without a \n
    fflush(dest);

    pthread_mutex_unlock(&logger_lock);
}
