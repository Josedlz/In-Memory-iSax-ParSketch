#include "sketch.h"
#include <bits/stdc++.h>
using namespace std;

void printCandidates(vector<tuple<long, vector<float>, float>> &candidates)
{
    cout << "Candidates:" << endl;
    for (auto &candidate : candidates)
    {
        cout << "Index: " << get<0>(candidate) << ", Distance: " << get<2>(candidate) << endl;
        vector<float> &ts = get<1>(candidate);
        cout << "Time Series: [";
        for (size_t i = 0; i < ts.size(); ++i)
        {
            cout << ts[i];
            if (i < ts.size() - 1)
            {
                cout << ", ";
            }
        }
        cout << "]" << endl;
        cout << "--------------------------------------" << endl;
    }
}

int main()
{
    int N = 20;
    int gridDimension = 5;
    int cellSize = 10;
    string datapath = "/home/renatoseb/2023-2/eda/proyecto/In-Memory-iSax-ParSketch/Dataset/datasets/synthetic/random_walk/random_walk_dataset.txt";
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

    vector<vector<vector<int>>> sketches; // Almacena los sketches

    for (auto &ts : data)
    {
        auto sketch = searcher.tsToSketch(ts, R);

        auto progrSketch = searcher.tsProgrSketch(ts, R, breakpoints);

        sketches.push_back(progrSketch);
    }

    int threshold = 0;
    float fraction = 0.2;
    int k = 3;
    vector<float> queryTS = data[2];
    auto ans = searcher.search(queryTS, R, breakpoints, threshold, k, fraction);
    printCandidates(ans);

    return 0;
}