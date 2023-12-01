#include "ParSketch.h"

/*std::vector<TimeSeries> randomSample(vector<TimeSeries>& data, int L) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<TimeSeries> sampledData;
        std::sample(data.begin(), data.end(), std::back_inserter(sampledData), L, gen);

        return sampledData;
    }

std::vector<TimeSeries> createSample(const std::vector<TimeSeries>& ts, int sampleSize) {
    std::vector<TimeSeries> sampleInput;
    std::sample(ts.begin(), ts.end(), std::back_inserter(sampleInput), sampleSize, std::mt19937{std::random_device{}()});
    return sampleInput;
}

std::vector<TimeSeries> transpose(std::vector<TimeSeries>& matrix) {

    size_t rows = matrix.size();
    size_t cols = matrix[0].getLength();
    
    vector<vector<float>> a,b;
    for(auto p: matrix){
        a.push_back(p.getValues());
    }
    std::vector<TimeSeries> result(cols, std::vector<float>(rows));

    for (size_t i = 0; i < rows; ++i) {
        for (size_t j = 0; j < cols; ++j) {
            b[j][i] = a[i][j];
        }
    }

    for(int i=0;i<b.size();i++){
       result[i].setValues(b[i]);
    }

    return result;
}

std::vector<TimeSeries> createBreakpoints(vector<TimeSeries>& sampleProject, int gridSize) {
    // Transponer la matriz para trabajar con las dimensiones como columnas
   vector<TimeSeries> sampleProjectTransposed = transpose(sampleProject);

    // Calcular el tamaño de cada segmento en la proyección
    int segmentSize = sampleProjectTransposed[0].getLength() / gridSize;

    // Calcular los breakpoints para cada dimensión en la proyección
    std::vector<TimeSeries> breakpoints;
    for (auto& dimension : sampleProjectTransposed) {
        // Tomar una porción del array y ordenar
        std::vector<float> sortedDimension(dimension.begin(), dimension.end());
        std::sort(sortedDimension.begin(), sortedDimension.end());

        // Crear un array de breakpoints tomando el último elemento de cada segmento
        std::vector<float> dimensionBreakpoints;
        for (int i = 0; i < gridSize; ++i) {
            dimensionBreakpoints.push_back(sortedDimension[(i + 1) * segmentSize - 1]);
        }

        breakpoints.push_back(dimensionBreakpoints);
    }

    return breakpoints;
}
*/

int main() {
    int N = 100; 
    string datapath = "/home/luischahua/In-Memory-iSax-ParSketch/Dataset/datasets/synthetic/random_dataset/white_noise_dataset.txt";
    ParSketchSearcher searcher(datapath);
    searcher.initialize();
    cout<<searcher.getSeries().size();
    /*std::vector<TimeSeries> R = searcher.ranD(N, searcher.getSeries().size());

    int L = 20; 
    std::vector<TimeSeries> randvectors = searcher.getSeries();
    
    int gridDimension = 10;
    std::vector<TimeSeries> sampleInput = searcher.randomSample(randvectors, L);

    vector<TimeSeries> sampleProject;
    for (auto& t : sampleInput) {
        sampleProject.push_back(searcher.mult(t, R));
    }
*/

    return 0;
}

