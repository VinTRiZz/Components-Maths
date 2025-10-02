#pragma once

#include <type_traits>
#include <math.h>

namespace Maths
{

template <unsigned iterationCount = 1000000, typename FunctionT>
std::enable_if_t<iterationCount % 2 == 0, double>
simpsonIntegral(double valueFrom, double valueTo, FunctionT&& integralFunction) {
    double x;
    double h = (valueTo - valueFrom) / static_cast<double>(iterationCount);

    double result = integralFunction(valueFrom) + integralFunction(valueTo);
    for (unsigned i = 1; i < iterationCount; i+=2) {
        x = integralFunction(valueFrom + i * h);
        result += 2.0 * x + 4.0 * x;
    }

    return result * h / 3.0;
}

}
