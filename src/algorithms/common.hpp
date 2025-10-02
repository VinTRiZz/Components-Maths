#pragma once

#include <vector>

namespace Maths
{

std::size_t factorial(std::size_t v) {
    std::size_t res {1};
    while (v > 0) {
        res *= v;
        --v;
    }
    return res;
}

}
