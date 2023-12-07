#include <time.h>
#include <random>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <bits/stdc++.h>

using namespace std;

class ParSketch
{
private:
    vector<vector<float>> dataset;
    int cellSize;
    int gridDimension;

public:
    ~ParSketch() = default;
    ParSketch(std::string filename, int gridDim, int cellSz) : gridDimension(gridDim), cellSize(cellSz)
    {
        std::ifstream file(filename);
        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::vector<float> values;
            float num;

            while (iss >> num)
            {
                values.push_back(num);
            }

            this->dataset.emplace_back(values);
        }
    }

    int getGridDimension()
    {
        return this->gridDimension;
    }
    int getCellSize()
    {
        return this->cellSize;
    }

    std::vector<std::tuple<long, std::vector<float>, float>> knnSearch(vector<float> &queryTS,
                                                                       vector<vector<float>> &RandMxBroad,
                                                                       vector<vector<float>> &breakpoints,
                                                                       double threshold, int k, float fraction)
    {
        // Genera el "sketch progresivo" de la consulta
        vector<vector<int>> progrSketch = tsProgrSketch(queryTS, RandMxBroad, breakpoints);
        /*
        cout << "Query sketch: " << endl;
        for (auto &i : progrSketch)
        {
            for (auto &j : i)
            {
                cout << j << " ";
            }
            cout << endl;
        }*/

        // Almacena los candidatos potenciales
        vector<tuple<long, vector<float>, float>> candidates;

        cout << "Dataset size: " << this->dataset.size() << endl;
        // Itera sobre todas las series temporales en el conjunto de datos
        for (size_t i = 0; i < this->dataset.size(); ++i)
        {
            auto ts = this->dataset[i];

            // Genera el "sketch" de la serie temporal actual
            // auto dataSketch = tsToSketch(ts, RandMxBroad);
            auto dataSketch = tsProgrSketch(ts, RandMxBroad, breakpoints);

            // Verifica si los "sketches" son lo suficientemente similares
            if (isSimilar(progrSketch, dataSketch, threshold))
            {
                // Calcula la distancia entre la consulta y la serie temporal actual
                float dist = distancia(queryTS, ts);

                // Almacena la serie temporal como candidato
                candidates.emplace_back(i, ts, dist);
            }
        }

        cout << "Candidates size: " << candidates.size() << endl;
        // Calcula el número requerido de coincidencias según la fracción
        int requiredMatches = static_cast<int>(fraction * progrSketch.size());

        // Filtra los candidatos que cumplen con el número requerido de coincidencias
        std::vector<std::tuple<long, std::vector<float>, float>> filteredCandidates;
        cout << "Required matches: " << requiredMatches << endl;

        for (const auto &candidate : candidates)
        {
            auto ts = std::get<1>(candidate);
            // auto dataSketch = tsToSketch(ts, RandMxBroad);
            auto dataSketch = tsProgrSketch(ts, RandMxBroad, breakpoints);

            int matches = countMatches(progrSketch, dataSketch);

            if (matches >= requiredMatches)
            {
                filteredCandidates.push_back(candidate);
            }
        }
        cout << "Filtered candidates size: " << filteredCandidates.size() << endl;

        // Ordenamos los candidatos por distancia
        std::sort(filteredCandidates.begin(), filteredCandidates.end(),
                  [](const auto &a, const auto &b)
                  { return std::get<2>(a) < std::get<2>(b); });

        // Limita la cantidad de resultados a los primeros k
        cout << "K: " << k << endl;
        cout << "Filtered candidates size: " << filteredCandidates.size() << endl;
        if (filteredCandidates.size() > k)
        {
            filteredCandidates.resize(k);
        }

        return filteredCandidates;
    }

    int countMatches(const vector<vector<int>> &sketch1, const vector<vector<int>> &sketch2)
    {
        int matches = 0;

        for (size_t i = 0; i < sketch1.size(); ++i)
        {
            if (sketch1[i] == sketch2[i])
            {
                ++matches;
            }
        }

        return matches;
    }

    vector<std::vector<float>> transpose(vector<vector<float>> &matrix)
    {
        size_t rows = matrix.size();
        size_t cols = matrix[0].size();

        vector<std::vector<float>> result(cols, std::vector<float>(rows, 0.0));

        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result[j][i] = matrix[i][j];
            }
        }

        return result;
    }

    vector<float> normalize(vector<float> &inputVector)
    {
        float sum = 0.0f;
        for (float value : inputVector)
        {
            sum += value;
        }
        float mean = sum / inputVector.size();

        sum = 0.0f;
        for (float value : inputVector)
        {
            float diff = value - mean;
            sum += diff * diff;
        }
        float stdev = std::sqrt(sum / inputVector.size());

        std::vector<float> normalizedVector;
        normalizedVector.reserve(inputVector.size());

        for (float value : inputVector)
        {
            float normalizedValue = (stdev > 0.000001) ? (value - mean) / stdev : 0.0f;
            normalizedVector.push_back(normalizedValue);
        }

        return normalizedVector;
    }
    vector<vector<int>> tsToSketch(vector<float> &ts, vector<vector<float>> &RandMxBroad)
    {
        auto normalizedTs = normalize(ts);

        auto multiplied = mult(normalizedTs, RandMxBroad);

        vector<int> result1;

        for (float value : multiplied)
        {
            int newValue = static_cast<int>(value / getCellSize()) - (value < 0 ? 1 : 0);
            result1.push_back(newValue);
        }

        vector<vector<int>> result;

        for (size_t i = 0; i <= result1.size() - getGridDimension(); i += getGridDimension())
        {
            result.emplace_back(result1.begin() + i, result1.begin() + i + getGridDimension());
        }

        return result;
    }
    vector<vector<int>> tsProgrSketch(vector<float> &ts, vector<vector<float>> &RandMxBroad, vector<vector<float>> &breakpoints)
    {
        auto normalizedTs = normalize(ts);
        auto multiplied = mult(normalizedTs, RandMxBroad);

        vector<pair<double, size_t>> zipped;
        zipped.reserve(multiplied.size());
        for (size_t i = 0; i < multiplied.size(); ++i)
        {
            zipped.emplace_back(multiplied[i], i);
        }

        vector<int> result0;

        transform(zipped.begin(), zipped.end(), back_inserter(result0),
                  [&breakpoints](const pair<double, size_t> &p)
                  {
                      const auto &breakpoint = breakpoints[p.second];
                      auto it = find_if(breakpoint.begin(), breakpoint.end(),
                                        [&p](double value)
                                        { return p.first <= value; });
                      return (it != breakpoint.end()) ? distance(breakpoint.begin(), it) : -1;
                  });

        vector<int> result1;

        for (int value : result0)
        {
            if (value == -1)
            {
                // Si v == -1 usamos la longitud del primer vector de breakpoints
                result1.push_back(breakpoints[0].size() - 1);
            }
            else
            {
                result1.push_back(value);
            }
        }

        vector<vector<int>> result;

        for (size_t i = 0; i <= result1.size() - getGridDimension(); i += getGridDimension())
        {
            result.emplace_back(result1.begin() + i, result1.begin() + i + getGridDimension());
        }

        return result;
    }

    std::vector<vector<float>> createBreakpoints(vector<vector<float>> &sampleProject, int gridSize)
    {
        vector<vector<float>> sampleProjectTransposed = transpose(sampleProject);

        int segmentSize = sampleProjectTransposed[0].size() / gridSize;

        std::vector<vector<float>> breakpoints;
        for (auto &dimension : sampleProjectTransposed)
        {
            std::vector<float> sortedDimension(dimension.begin(), dimension.end());
            std::sort(sortedDimension.begin(), sortedDimension.end());

            std::vector<float> dimensionBreakpoints;
            for (int i = 0; i < gridSize; ++i)
            {
                dimensionBreakpoints.push_back(sortedDimension[(i + 1) * segmentSize - 1]);
            }

            breakpoints.push_back(dimensionBreakpoints);
        }

        return breakpoints;
    }
    vector<vector<float>> getData()
    {
        return this->dataset;
    }

    std::vector<vector<float>> randomSample(vector<vector<float>> &data, int L)
    {
        std::random_device rd;
        std::mt19937 gen(42);
        std::vector<vector<float>> sampledData;
        std::sample(data.begin(), data.end(), std::back_inserter(sampledData), L, gen);

        return sampledData;
    }
    vector<float> mult(vector<float> &a, vector<vector<float>> &b)
    {
        vector<float> result;
        for (auto &col : b)
        {
            float sum = 0;

            for (size_t i = 0; i < a.size(); ++i)
            {
                sum += a[i] * col[i];
            }
            result.push_back(sum);
        }
        return result;
    }

    vector<vector<float>> ranD(int a, int b)
    {
        vector<vector<float>> result;
        random_device rd;
        mt19937 gen(42);
        uniform_real_distribution<float> dis(-1.0, 1.0);

        for (int j = 0; j < a; ++j)
        {
            vector<float> row;
            for (int i = 0; i < b; ++i)
            {
                float randomValue = dis(gen);
                row.push_back(randomValue);
            }
            result.push_back(row);
        }

        return result;
    }
    bool isSimilar(const vector<vector<int>> &sketch1,
                   const vector<vector<int>> &sketch2,
                   double threshold)
    {
        if (sketch1.size() != sketch2.size())
        {
            return false; // Los sketches deben tener la misma cantidad de celdas
        }

        int similarCount = 0;

        for (size_t i = 0; i < sketch1.size(); ++i)
        {
            const vector<int> &cell1 = sketch1[i];
            const vector<int> &cell2 = sketch2[i];
            // Compara las celdas elemento por elemento
            bool cellSimilar = std::equal(cell1.begin(), cell1.end(), cell2.begin());

            if (cellSimilar)
            {
                ++similarCount;
            }
        }

        //  Calcula la proporción de celdas similares
        double similarityRatio = static_cast<double>(similarCount) / sketch1.size();
        /*
        if (similarCount > 0 && similarityRatio > 0)
        {
            cout << "Similar count: " << similarCount << endl;
            cout << "Similarity ratio: " << similarityRatio << endl;
            cout << "Threshold: " << threshold << endl;
        }
        */
        // Compara la proporción con el umbral
        return similarityRatio >= threshold;
    }

    float distancia(vector<float> &vector1, vector<float> &vector2)
    {
        if (vector1.size() != vector2.size())
        {
            throw std::invalid_argument("Los vectores deben tener la misma longitud");
        }

        float distanceSquared = 0.0f;

        for (size_t i = 0; i < vector1.size(); ++i)
        {
            float diff = vector1[i] - vector2[i];
            distanceSquared += std::pow(diff, 2);
        }

        return sqrt(distanceSquared);
    }
};
