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
std::vector<TimeSeries> series;
int cellSize;
int gridDimension;
public:
    using knnSearcher::knnSearcher;
    ~ParSketchSearcher() = default;

    
    
    std::vector<TimeSeries> randomSample(vector<TimeSeries>& data, int L);
    std::vector<TimeSeries> calculateBreakpoints(const std::vector<TimeSeries>& sampledData,
                                                         const std::vector<std::vector<float>>& randomMatrix,
                                                         int gridDimension);
    void insert(TimeSeries t) override {
        // Empty implementation
    }

    void createIndex() {
    for (auto&ts: dataset){
        series.push_back(ts);
    }
}  
    void initialize(){
        createIndex();
    } 

    std::vector<TimeSeries> search(TimeSeries q, int k) override{

        
    }

    std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k) override {

    }

    std::vector<TimeSeries> getSeries(){
        return this->series;
    }

TimeSeries mult(TimeSeries& a, vector<TimeSeries>& b) {
    TimeSeries result;
    vector<float> v;
    for (auto& col : b) {
        float sum = 0;
        
        for (std::size_t i = 0; i < a.getLength(); ++i) {
            sum += a.getValues()[i] * col.getValues()[i];
        }
        v.push_back(sum);
    }
    result.setValues(v);

    return result;
}

std::vector<TimeSeries> ranD(int a, int b) {
    std::vector<TimeSeries> result;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(-1.0, 1.0);

    for (int j = 0; j < a; ++j) {
        TimeSeries row;
        vector<float> v;
        for (int i = 0; i < b; ++i) {
            float randomValue = dis(gen);
            v.push_back(randomValue);
        }
        row.setValues(v);
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

std::pair<float, float> stats(TimeSeries ts) {
    float mean = 0.0f;
    for (float x : ts.getValues()) {
        mean += x;
    }
    mean /= ts.getLength();

    float sumSquareDiff = 0.0f;
    for (float x : ts.getValues()) {
        float diff = x - mean;
        sumSquareDiff += diff * diff;
    }

    float stdev = std::sqrt(sumSquareDiff / ts.getLength());

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

/*std::vector<std::vector<int>> tsToSketch(const TSWithStats& tsWithStats, const std::vector<std::vector<float>>& RandMxBroad) {
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

bool isSimilar(const std::vector<std::vector<int>>& sketch1, 
               const std::vector<std::vector<int>>& sketch2, 
               int threshold) {
    // Verificar si los sketches tienen el mismo número de subvectores
    if (sketch1.size() != sketch2.size()) {
        return false;
    }

    int similarCount = 0;

    // Iterar a través de cada subvector (celda de la cuadrícula) en los sketches
    for (size_t i = 0; i < sketch1.size(); ++i) {
        // Comparar los subvectores de ambos sketches
        if (sketch1[i] == sketch2[i]) {
            ++similarCount;
        }
    }

    // Verificar si el número de celdas similares alcanza el umbral
    return similarCount >= threshold;
}


std::vector<std::tuple<long, std::vector<float>, float>> search(const TSWithStats& queryTS, const std::vector<std::vector<float>>& RandMxBroad, 
                                                                const std::vector<TSWithStats>& dataset, int threshold, 
                                                                int k) {
    // Convertir la serie temporal de consulta en un sketch
    auto querySketch = tsToSketch(queryTS, RandMxBroad);

    // Contenedor para los candidatos kNN
    std::vector<std::tuple<long, std::vector<float>, float>> candidates;

    // Iterar sobre el conjunto de datos
    for (const auto& ts : dataset) {
        // Convertir cada serie temporal del conjunto de datos en un sketch
        auto dataSketch = tsToSketch(ts, RandMxBroad);

        // Comparar el sketch de la consulta con el sketch de la serie temporal actual
        if (isSimilar(querySketch, dataSketch, threshold)) {
            // Calcular la distancia entre la consulta y la serie temporal actual
            float dist = distance(queryTS, ts);

            // Agregar a los candidatos con su ID, serie temporal y distancia
            candidates.push_back(std::make_tuple(std::get<0>(ts), std::get<1>(ts), dist));
        }
    }

    // Ordenar los candidatos por distancia
    std::sort(candidates.begin(), candidates.end(), 
              [](const auto& a, const auto& b) { return std::get<2>(a) < std::get<2>(b); });

    // Conservar solo los k primeros candidatos
    if (candidates.size() > k) {
        candidates.resize(k);
    }

    return candidates;
}

*/
};

#endif
