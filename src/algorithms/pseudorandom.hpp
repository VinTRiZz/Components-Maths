#pragma once

#include <vector>
#include <algorithm>
#include <random>

namespace Maths
{

template <typename ValueT>
std::vector<ValueT> pseudoRandomShuffle(unsigned deltaN, unsigned valueCount, const std::vector<ValueT>& availableValues) {
    if (deltaN >= availableValues.size() ||
        valueCount == 0) {
        return {};
    }

    // Result
    std::vector<ValueT> res;
    res.reserve(valueCount);

    // Random generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<std::size_t> distrAvailable(0, availableValues.size() - 1);

    // Generate first values
    auto genCount = std::min(std::size_t(valueCount), availableValues.size());
    std::copy_n(availableValues.begin(), genCount, std::back_inserter(res));
    std::shuffle(res.begin(), res.end(), gen);
    valueCount -= genCount;

    // Generate other values
    while (valueCount > 0) {
        auto nextV = availableValues[distrAvailable(gen)];

        // Check if result exist
        bool valueExist {false};
        auto curResSize = res.size();
        auto minPos = (curResSize > deltaN ? curResSize - deltaN - 1 : 0);
        for (auto curpos = curResSize - 1; !valueExist && curpos > minPos; --curpos) {
            valueExist = res[curpos] == nextV;
        }

        // Add value only if result not exist in prev positions
        if (!valueExist) {
            res.push_back(nextV);
            --valueCount;
        }
    }

    return res;
}

}
