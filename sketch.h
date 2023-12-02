#include <time.h>
#include <random>
#include <iomanip>
#include <chrono>
#include <sstream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <bits/stdc++.h>

// using TSWithStats = tuple<vector<float>, pair<float, float>>;
using namespace std;

class ParSketch
{
private:
    vector<vector<float>> dataset;
    int cellSize;
    int gridDimension;

public:
    ~ParSketch() = default;
    int getGridDimension()
    {
        return this->gridDimension;
    }
    int getCellSize()
    {
        return this->cellSize;
    }
    vector<tuple<long, vector<float>, float>> search(vector<float> &queryTS, vector<vector<float>> &RandMxBroad,
                                                     vector<vector<float>> &breakpoints, int threshold, int k, float fraction)
    {

        // Normalizamos la serie temporal de consulta
        // auto normalizedQueryTS = normalize(queryTS);

        // Calculamos el sketch para la serie temporal de consulta normalizada
        auto querySketch = tsToSketch(queryTS, RandMxBroad);
        cout << "Query sketch: " << endl;
        for (auto &i : querySketch)
        {
            for (auto &j : i)
            {
                cout << j << " ";
            }
            cout << endl;
        }

        // Asignamos el sketch a las celdas de la cuadrícula utilizando los breakpoints
        auto progrSketch = tsProgrSketch(queryTS, RandMxBroad, breakpoints);
        vector<tuple<long, vector<float>, float>> candidates;

        // Iteramos sobre el conjunto de datos
        cout << "Dataset size: " << this->dataset.size() << endl;
        for (size_t i = 0; i < this->dataset.size(); ++i)
        {
            auto ts = this->dataset[i];

            // Calculamos el sketch para la serie temporal actual en el conjunto de datos
            auto dataSketch = tsToSketch(ts, RandMxBroad);

            // Verificamos la similitud entre el sketch de la consulta y el sketch de la serie temporal actual
            if (isSimilar(progrSketch, dataSketch, threshold))
            {
                // Calculamos la distancia entre la serie temporal de consulta y la serie temporal actual
                float dist = distancia(queryTS, ts);

                // Agregamos la serie temporal actual a la lista de candidatos
                candidates.emplace_back(i, ts, dist);
            }
        }
        cout << "Candidates size: " << candidates.size() << endl;
        // Filtramos los candidatos usando la fracción y la consulta SELECT (simulado)
        int requiredMatches = static_cast<int>(fraction * progrSketch.size());
        vector<tuple<long, vector<float>, float>> filteredCandidates;
        cout << "Required matches: " << requiredMatches << endl;

        // Filtramos los candidatos que cumplen con la fracción requerida
        for (const auto &candidate : candidates)
        {
            auto ts = get<1>(candidate);
            auto dataSketch = tsToSketch(ts, RandMxBroad);

            int matches = countMatches(progrSketch, dataSketch);
            // cout << "Matches: " << matches << endl;
            if (matches >= requiredMatches)
            {
                filteredCandidates.push_back(candidate);
            }
        }
        cout << "Filtered candidates size: " << filteredCandidates.size() << endl;

        // Ordenamos los candidatos por distancia
        std::sort(filteredCandidates.begin(), filteredCandidates.end(),
                  [](const auto &a, const auto &b)
                  { return get<2>(a) < get<2>(b); });

        // Conservamos solo los k primeros candidatos
        if (filteredCandidates.size() > k)
        {
            filteredCandidates.resize(k);
        }
        return filteredCandidates;
    }

    // Funcion para contar coincidencias entre dos sketches
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
        // Obtenemos las dimensiones de la matriz original
        size_t rows = matrix.size();
        size_t cols = matrix[0].size();

        // Inicializamos la matriz transpuesta con las dimensiones intercambiadas
        vector<std::vector<float>> result(cols, std::vector<float>(rows, 0.0));

        // Llenamos la matriz transpuesta
        for (size_t i = 0; i < rows; ++i)
        {
            for (size_t j = 0; j < cols; ++j)
            {
                result[j][i] = matrix[i][j];
            }
        }

        return result;
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

    vector<float> normalize(vector<float> &inputVector)
    {
        // Calculamos la media del vector
        float sum = 0.0f;
        for (float value : inputVector)
        {
            sum += value;
        }
        float mean = sum / inputVector.size();

        // Calculamos la desviación estándar del vector
        sum = 0.0f;
        for (float value : inputVector)
        {
            float diff = value - mean;
            sum += diff * diff;
        }
        float stdev = std::sqrt(sum / inputVector.size());

        // Normalizamos el vector
        std::vector<float> normalizedVector;
        normalizedVector.reserve(inputVector.size());

        for (float value : inputVector)
        {
            // Normalizamos utilizando z-score (media = 0, desviación estándar = 1)
            float normalizedValue = (stdev > 0.000001) ? (value - mean) / stdev : 0.0f;
            normalizedVector.push_back(normalizedValue);
        }

        return normalizedVector;
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
        // Transponemos la matriz para trabajar con las dimensiones como columnas
        vector<vector<float>> sampleProjectTransposed = transpose(sampleProject);

        // Calculamos el tamaño de cada segmento en la proyección
        int segmentSize = sampleProjectTransposed[0].size() / gridSize;

        // Calculamos los breakpoints para cada dimensión en la proyección
        std::vector<vector<float>> breakpoints;
        for (auto &dimension : sampleProjectTransposed)
        {
            // Tomamos una porción del array y ordenar
            std::vector<float> sortedDimension(dimension.begin(), dimension.end());
            std::sort(sortedDimension.begin(), sortedDimension.end());

            // Creamos un array de breakpoints tomando el último elemento de cada segmento
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
        std::mt19937 gen(rd());
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
        mt19937 gen(rd());
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

    string getMinSec(long long millis)
    {
        using namespace std;

        auto minutes = chrono::duration_cast<chrono::minutes>(chrono::milliseconds(millis));
        auto seconds = chrono::duration_cast<chrono::seconds>(chrono::milliseconds(millis)) - minutes;

        ostringstream oss;
        oss << minutes.count() << " min " << seconds.count() << " sec";

        return oss.str();
    }

    pair<float, float> stats(vector<float> ts)
    {
        float mean = 0.0f;
        for (float x : ts)
        {
            mean += x;
        }
        mean /= ts.size();

        float sumSquareDiff = 0.0f;
        for (float x : ts)
        {
            float diff = x - mean;
            sumSquareDiff += diff * diff;
        }

        float stdev = sqrt(sumSquareDiff / ts.size());

        return make_pair(mean, stdev);
    }

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

    bool isSimilar(vector<vector<int>> &sketch1,
                   vector<vector<int>> &sketch2,
                   int threshold)
    {
        // Verificamos si los sketches tienen el mismo número de subvectores
        if (sketch1.size() != sketch2.size())
        {
            return false;
        }

        int similarCount = 0;

        // Iteramos a través de cada subvector (celda de la cuadrícula) en los sketches
        for (size_t i = 0; i < sketch1.size(); ++i)
        {
            // Comparamos los subvectores de ambos sketches
            if (sketch1[i] == sketch2[i])
            {
                ++similarCount;
            }
        }

        // Verificamos si el número de celdas similares alcanza el umbral
        return similarCount >= threshold;
    }

    float distancia(vector<float> &vector1, vector<float> &vector2)
    {
        // Verificamos si los vectores tienen la misma longitud
        if (vector1.size() != vector2.size())
        {
            throw std::invalid_argument("Los vectores deben tener la misma longitud");
        }

        float distanceSquared = 0.0f;

        // Calcular la suma de los cuadrados de las diferencias
        for (size_t i = 0; i < vector1.size(); ++i)
        {
            float diff = vector1[i] - vector2[i];
            distanceSquared += std::pow(diff, 2);
        }

        // Calculamos la raíz cuadrada de la suma
        return sqrt(distanceSquared);
    }
    vector<tuple<long, vector<float>, float>> mergeDistances(
        const vector<tuple<long, vector<float>, float>> &xs,
        const vector<tuple<long, vector<float>, float>> &ys,
        int topCand)
    {

        vector<tuple<long, vector<float>, float>> rs;
        size_t i = 0;

        for (const auto &x : xs)
        {
            while (i < ys.size() && get<2>(x) > get<2>(ys[i]))
            {
                rs.push_back(ys[i]);
                ++i;
            }

            rs.push_back(x);
        }

        if (i < ys.size())
        {
            rs.insert(rs.end(), ys.begin() + i, ys.end());
        }

        if (rs.size() > static_cast<size_t>(topCand))
        {
            rs.resize(topCand);
        }

        return rs;
    };
};
