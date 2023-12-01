#ifndef PARSKETCH_H
#define PARSKETCH_H

#include <time.h>
#include <random>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <vector>
#include <algorithm>

#include "knnSearcher.h"

using TSWithStats = std::tuple<std::vector<float>, std::pair<float, float>>;

using namespace std;
class ParSketchSearcher: public knnSearcher {
private:
// attributes

public:
    ParSketchSearcher() = default;
    ~ParSketchSearcher() = default;
    int cellSize;
    int gridDimension;
template <typename A>
std::vector<A> mult(const std::vector<A>& a, const std::vector<std::vector<A>>& b) {
    std::vector<A> result;

    for (const auto& col : b) {
        A sum = 0;
        for (std::size_t i = 0; i < a.size(); ++i) {
            sum += a[i] * col[i];
        }
        result.push_back(sum);
    }

    return result;
}

std::vector<std::vector<float>> ranD(int a, int b) {
    std::vector<std::vector<float>> result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0, 1.0);

    for (int j = 0; j < a; ++j) {
        std::vector<float> row;
        for (int i = 0; i < b; ++i) {
            float randomValue = dis(gen);
            row.push_back(randomValue);
        }
        result.push_back(row);
    }

    return result;
}

std::string getMinSec(long long millis) {
    using namespace std;

    auto minutes = chrono::duration_cast<chrono::minutes>(chrono::milliseconds(millis));
    auto seconds = chrono::duration_cast<chrono::seconds>(chrono::milliseconds(millis)) - minutes;

    ostringstream oss;
    oss << minutes.count() << " min " << seconds.count() << " sec";

    return oss.str();
}

std::pair<float, float> stats(const std::vector<float>& ts) {
    float mean = 0.0f;
    for (float x : ts) {
        mean += x;
    }
    mean /= ts.size();

    float sumSquareDiff = 0.0f;
    for (float x : ts) {
        float diff = x - mean;
        sumSquareDiff += diff * diff;
    }

    float stdev = std::sqrt(sumSquareDiff / ts.size());

    return std::make_pair(mean, stdev);
}

std::vector<float> normalize(const TSWithStats& tsWithStats) {
    const auto& ts = std::get<0>(tsWithStats);
    float mean = std::get<0>(std::get<1>(tsWithStats));
    float stdev = std::get<1>(std::get<1>(tsWithStats));

    std::vector<float> normalizedTs;
    normalizedTs.reserve(ts.size());

    for (float x : ts) {
        float normalizedValue = (stdev > 0.000001) ? (x - mean) / stdev : 0.0f;
        normalizedTs.push_back(normalizedValue);
    }

    return normalizedTs;
}
float distance(const TSWithStats& xs, const TSWithStats& ys) {
    const std::vector<float>& normalizedXs = normalize(xs);
    const std::vector<float>& normalizedYs = normalize(ys);

    float distanceSquared = 0.0f;
    for (size_t i = 0; i < normalizedXs.size(); ++i) {
        float diff = normalizedYs[i] - normalizedXs[i];
        distanceSquared += std::pow(diff, 2);
    }

    return std::sqrt(distanceSquared);
}

std::vector<std::tuple<long, std::vector<float>, float>> mergeDistances(
    const std::vector<std::tuple<long, std::vector<float>, float>>& xs,
    const std::vector<std::tuple<long, std::vector<float>, float>>& ys,
    int topCand) {

    std::vector<std::tuple<long, std::vector<float>, float>> rs;
    size_t i = 0;

    for (const auto& x : xs) {
        while (i < ys.size() && std::get<2>(x) > std::get<2>(ys[i])) {
            rs.push_back(ys[i]);
            ++i;
        }

        rs.push_back(x);
    }

    if (i < ys.size()) {
        rs.insert(rs.end(), ys.begin() + i, ys.end());
    }

    if (rs.size() > static_cast<size_t>(topCand)) {
        rs.resize(topCand);
    }

    return rs;
}

std::vector<std::vector<int>> tsToSketch(const TSWithStats& tsWithStats, const std::vector<std::vector<float>>& RandMxBroad) {
    const auto& ts = normalize(tsWithStats);
    auto multiplied = mult(ts, RandMxBroad);
 
    std::vector<int> result1;
    
    for (float value : multiplied) {
        int newValue = static_cast<int>(value / cellSize) - (value < 0 ? 1 : 0);
        result1.push_back(newValue);
    }
    
    std::vector<std::vector<int>> result;

    for (size_t i = 0; i <= result1.size() - gridDimension; i += gridDimension) {
        result.push_back(std::vector<int>(result1.begin() + i, result1.begin() + i + gridDimension));
    }

    return result;
}

std::vector<std::vector<int>> tsProgrSketch(const TSWithStats& tsWithStats,
                                            const std::vector<std::vector<float>>& RandMxBroad,
                                            const std::vector<std::vector<float>>& breakpoints) {

    const auto& ts = normalize(tsWithStats);
    auto multiplied = mult(ts, RandMxBroad);

    std::vector<std::pair<double, size_t>> zipped;
    zipped.reserve(multiplied.size());
    for (size_t i = 0; i < multiplied.size(); ++i) {
        zipped.emplace_back(multiplied[i], i);
    }

    std::vector<int> result0;

    // Map
    // Map
    std::transform(zipped.begin(), zipped.end(), std::back_inserter(result0),
                   [&breakpoints](const std::pair<double, size_t>& p) {
                       const auto& breakpoint = breakpoints[p.second];
                       auto it = std::find_if(breakpoint.begin(), breakpoint.end(),
                                              [&p](double value) { return p.first <= value; });
                       return (it != breakpoint.end()) ? std::distance(breakpoint.begin(), it) : -1;
                   });
    
    std::vector<int> result1;

    for (int value :result0) {
        if (value == -1) {
            // If v == -1, use the length of the first breakpoint vector - 1
            result1.push_back(breakpoints[0].size() - 1);
        } else {
            result1.push_back(value);
        }
    }

    std::vector<std::vector<int>> result;

    for (size_t i = 0; i <= result1.size() - gridDimension; i += gridDimension) {
        result.push_back(std::vector<int>(result1.begin() + i, result1.begin() + i + gridDimension));
    }


    return result;
}

};

#endif
