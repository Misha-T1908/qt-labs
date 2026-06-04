#ifndef CONVERTER_H
#define CONVERTER_H

#include <QString>

class Converter
{
public:
    enum class Mode {
        Length,
        Mass,
        Temperature
    };

    static double convert(Mode mode, double value, const QString &fromUnit, const QString &toUnit);
};

#endif
