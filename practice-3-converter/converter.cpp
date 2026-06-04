#include "converter.h"

namespace {
double lengthToMeters(double value, const QString &unit)
{
    if (unit.contains("kilometers")) {
        return value * 1000.0;
    }
    if (unit.contains("inches")) {
        return value * 0.0254;
    }
    if (unit.contains("feet")) {
        return value * 0.3048;
    }
    if (unit.contains("miles")) {
        return value * 1609.344;
    }
    return value;
}

double metersToLength(double value, const QString &unit)
{
    if (unit.contains("kilometers")) {
        return value / 1000.0;
    }
    if (unit.contains("inches")) {
        return value / 0.0254;
    }
    if (unit.contains("feet")) {
        return value / 0.3048;
    }
    if (unit.contains("miles")) {
        return value / 1609.344;
    }
    return value;
}

double massToKilograms(double value, const QString &unit)
{
    if (unit.contains("pounds")) {
        return value * 0.45359237;
    }
    if (unit.contains("ounces")) {
        return value * 0.028349523125;
    }
    return value;
}

double kilogramsToMass(double value, const QString &unit)
{
    if (unit.contains("pounds")) {
        return value / 0.45359237;
    }
    if (unit.contains("ounces")) {
        return value / 0.028349523125;
    }
    return value;
}

double temperatureToKelvin(double value, const QString &unit)
{
    if (unit.contains("Celsius")) {
        return value + 273.15;
    }
    if (unit.contains("Fahrenheit")) {
        return (value - 32.0) * 5.0 / 9.0 + 273.15;
    }
    return value;
}

double kelvinToTemperature(double value, const QString &unit)
{
    if (unit.contains("Celsius")) {
        return value - 273.15;
    }
    if (unit.contains("Fahrenheit")) {
        return (value - 273.15) * 9.0 / 5.0 + 32.0;
    }
    return value;
}
}

double Converter::convert(Mode mode, double value, const QString &fromUnit, const QString &toUnit)
{
    if (mode == Mode::Length) {
        return metersToLength(lengthToMeters(value, fromUnit), toUnit);
    }
    if (mode == Mode::Mass) {
        return kilogramsToMass(massToKilograms(value, fromUnit), toUnit);
    }

    return kelvinToTemperature(temperatureToKelvin(value, fromUnit), toUnit);
}
