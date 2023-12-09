#include "sketch.h"
#include <bits/stdc++.h>
#include <chrono>
#include <omp.h>
using namespace std;

void printCandidates(vector<tuple<long, vector<float>, float>> &candidates)
{
    // cout << "Candidates:" << endl;
    for (auto &candidate : candidates)
    {
        // cout << "Index: " << get<0>(candidate) << ", Distance: " << get<2>(candidate) << endl;
        vector<float> &ts = get<1>(candidate);
        // cout << "Time Series: [";
        for (size_t i = 0; i < ts.size(); ++i)
        {
            cout << ts[i] << " ";
            /*
            if (i < ts.size() - 1)
            {
                cout << ", ";
            }
            */
        }
        cout << endl;
        // cout << "]" << endl;
        // cout << "--------------------------------------" << endl;
    }
}

vector<vector<float>> dividirDataset(const vector<vector<float>> &dataset, int rank, int size)
{
    int totalSize = dataset.size();
    int localSize = totalSize / size;

    int start = rank * localSize;
    int end = (rank == size - 1) ? totalSize : start + localSize;

    return vector<vector<float>>(dataset.begin() + start, dataset.begin() + end);
}

int main(int argc, char *argv[])
{
    auto startSketches = chrono::high_resolution_clock::now();

    int N = 20;
    int gridDimension = 5;
    int cellSize = 10;
    string datapath = argv[2];

    ParSketch searcher(datapath, gridDimension, cellSize);
    vector<vector<float>> R = searcher.ranD(N, searcher.getData().size());

    int L = 100;
    std::vector<vector<float>> data = searcher.getData();

    std::vector<vector<float>> sampleInput = searcher.randomSample(data, L);
    vector<vector<float>> sampleProject;
    for (auto &t : sampleInput)
    {
        sampleProject.push_back(searcher.mult(t, R));
    }

    vector<vector<float>> breakpoints = searcher.createBreakpoints(sampleProject, gridDimension);

    // Ahora, calculamos los sketches y asignamos a las celdas de la cuadrícula
    vector<vector<vector<int>>> sketches; // Almacena los sketches
#pragma omp parallel for shared(sketches) schedule(dynamic)
    for (int i = 0; i < data.size(); ++i)
    {
        // Asigna el sketch a las celdas de la cuadrícula usando breakpoints
        vector<vector<int>> progrSketch = searcher.tsProgrSketch(data[i], R, breakpoints);

// Agregamos el sketch asignado a la lista de sketches
#pragma omp critical
        sketches.push_back(progrSketch);
    }

    auto stopSketches = chrono::high_resolution_clock::now();
    auto durationSketches = chrono::duration_cast<chrono::duration<double>>(stopSketches - startSketches);
    // cout << "Tiempo de construcción de sketches: " << durationSketches.count() << " segundos" << endl;

    auto startQuery = chrono::high_resolution_clock::now();

    double threshold = 0.2;
    float fraction = 0.45;
    int k = atoi(argv[1]);
    // Consultamos una serie temporal del dataset
    vector<float> queryTS = data[argv[3]];
    for (auto &t : queryTS)
    {
        cout << t << " ";
    }
    cout << endl;
    auto ans = searcher.knnSearch(queryTS, R, breakpoints, threshold, k, fraction);
    printCandidates(ans);
    auto stopQuery = chrono::high_resolution_clock::now();
    auto durationQuery = chrono::duration_cast<chrono::duration<double>>(stopQuery - startQuery);
    // cout << "Tiempo de ejecución de la función de consulta: " << durationQuery.count() << " segundos" << endl;

    return 0;
}