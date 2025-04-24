//
// Created by mhmoud on 1/16/25.
//

#ifndef GPS_LOGGER_H
#define GPS_LOGGER_H

#include <iostream>

namespace gps {
    enum LogLevel {
        DEBUG,
        INFO,
        WARNING,
        ERROR
    };

    class Logger {
    public:
        static void log(LogLevel level, const std::string &serviceName, const std::string &message);

    private:
        static std::string getLevelString(LogLevel level);

#ifndef MCU

        static std::string getColorForLevel(LogLevel level);

        static std::string resetColor();

#endif

        static std::string getCurrentTime();

        static bool isWindows();

#ifdef _WIN32
        static void enableAnsiColors();
#endif
    };
}
#endif //GPS_LOGGER_H
