#include "logger.h"
#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>


#ifdef _WIN32
#include <windows.h>
#endif

using namespace gps;

void Logger::log(LogLevel level, const std::string &serviceName, const std::string &message) {
    std::string levelStr = getLevelString(level);
    std::string timestamp = getCurrentTime();
#ifdef _WIN32
    // Windows-specific color handling
    if (isWindows()) {
        enableAnsiColors();
    }
#endif
#ifdef MCU
    //TODO: LCD_print(serviceName << " (" << timestamp << "): " << message)
#else
    std::string color = getColorForLevel(level);
    std::cout << color << serviceName << " (" << timestamp << "): " << message << resetColor() << std::endl;
#endif
}

void Logger::logServer(LogLevel level, const std::string &key, const std::string &message) {
    std::string levelStr = getLevelString(level);
    std::string timestamp = getCurrentTime();
#ifdef _WIN32
    // Windows-specific color handling
    if (isWindows()) {
        enableAnsiColors();
    }
#endif
#ifdef MCU
    //TODO: LCD_print(serviceName << " (" << timestamp << "): " << message)
#else
    std::string color = getColorForLevel(level);
    std::cout << color << "Received" << " (" << timestamp << "): " << key << " : " << message << resetColor()
              << std::endl;
#endif
}

std::string Logger::getLevelString(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "DEBUG";
        case INFO:
            return "INFO";
        case WARNING:
            return "WARNING";
        case ERROR:
            return "ERROR";
        case ALARM:
            return "ALARM";
        default:
            return "UNKNOWN";
    }
}

#ifndef MCU

std::string Logger::getColorForLevel(LogLevel level) {
    switch (level) {
        case DEBUG:
            return "\033[37m"; // White
        case INFO:
            return "\033[32m"; // Green
        case WARNING:
            return "\033[33m"; // Yellow
        case ERROR:
            return "\033[31m"; // Red
        case ALARM:
            return "\033[35m"; // Magenta
        default:
            return "\033[0m";  // Reset
    }
}

std::string Logger::resetColor() {
    return "\033[0m"; // Reset color
}

#endif

std::string Logger::getCurrentTime() {
#ifdef MCU
    // get time here from MCU
#else
    std::time_t now = std::time(nullptr);
    std::tm *timeInfo = std::localtime(&now);
    std::ostringstream timeStream;
    timeStream << std::put_time(timeInfo, "%Y-%m-%d %H:%M:%S");
    return timeStream.str();
#endif
}

bool Logger::isWindows() {
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

#ifdef _WIN32
void Logger::enableAnsiColors() {
    // Windows 10+ allows enabling ANSI escape codes in the terminal
    DWORD dwMode = 0;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        GetConsoleMode(hConsole, &dwMode);
        dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hConsole, dwMode);
    }
}
#endif