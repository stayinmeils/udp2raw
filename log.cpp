#include "log.h"
#include "misc.h"

int log_level = log_info;
int enable_log_position = 0;
int enable_log_color = 1;

Logger::Logger(const std::string& baseFilename):baseFilename(baseFilename) {
    currentLogDate=getCurrentDate();
    openLogFile();
}

Logger::~Logger() {
    closeLogFile();
}

std::string Logger::getCurrentDate() {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", localTime);
    return buffer;
}

Logger& Logger::GetInstance(){
    static Logger logger(log_path);
    return logger; 
}
FILE* Logger::getlogfile() {
    std::string currentDate = getCurrentDate();
    if (currentDate != currentLogDate) {
        currentLogDate = currentDate;
        openLogFile();
    }
    return file;
}

void Logger::openLogFile() {
    closeLogFile();
    std::string logFilename = baseFilename +"/udp2raw_"+ currentLogDate + ".log";
    file = fopen(logFilename.c_str(), "a");
    if (file == nullptr) {
        throw std::runtime_error("无法打开日志文件: " + logFilename);
    }
}

void Logger::closeLogFile() {
    if (file != nullptr) {
        fclose(file);
        file = nullptr;
    }
}

void log0(const char* file, const char* function, int line, int level, const char* str, ...) {
    if (level > log_level) return;
    if (level > log_trace || level < 0) return;

    if(use_log_path==1){
        Logger& logger=Logger::GetInstance();
        FILE* logFile=logger.getlogfile();
        time_t timer;
        char buffer[100];
        struct tm* tm_info;
        time(&timer);
        tm_info = localtime(&timer);
        strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", tm_info);
        fprintf(logFile,"[%s][%s]", buffer, log_text[level]);
        if (enable_log_position) fprintf(logFile,"[%s,func:%s,line:%d]", file, function, line);
        va_list vlist;
        va_start(vlist, str);
        vfprintf(logFile, str, vlist);
        va_end(vlist);
        // printf("\n");
        // if(enable_log_color)
        // printf(log_color[level]);
        fflush(logFile);
        if (log_level == log_fatal) {
            about_to_exit = 1;
        }
    }else{
        time_t timer;
        char buffer[100];
        struct tm* tm_info;

        time(&timer);
        tm_info = localtime(&timer);

        if (enable_log_color)
            printf("%s", log_color[level]);

        strftime(buffer, 100, "%Y-%m-%d %H:%M:%S", tm_info);
        printf("[%s][%s]", buffer, log_text[level]);

        if (enable_log_position) printf("[%s,func:%s,line:%d]", file, function, line);

        va_list vlist;
        va_start(vlist, str);
        vfprintf(stdout, str, vlist);
        va_end(vlist);
        if (enable_log_color)
            printf("%s", RESET);

        // printf("\n");
        // if(enable_log_color)
        // printf(log_color[level]);
        fflush(stdout);
        if (log_level == log_fatal) {
            about_to_exit = 1;
        }
    }   
}

void log_bare(int level, const char* str, ...) {
    if (level > log_level) return;
    if (level > log_trace || level < 0) return;

    if(use_log_path==1){
        Logger& logger=Logger::GetInstance();
        FILE* logFile=logger.getlogfile();
        va_list vlist;
        va_start(vlist, str);
        vfprintf(logFile, str, vlist);
        va_end(vlist);
        fflush(logFile);
    }else{
        if (enable_log_color)
            printf("%s", log_color[level]);
        va_list vlist;
        va_start(vlist, str);
        vfprintf(stdout, str, vlist);
        va_end(vlist);
        if (enable_log_color)
            printf("%s", RESET);
        fflush(stdout);
    }
}
