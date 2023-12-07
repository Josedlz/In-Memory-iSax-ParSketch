#include <iostream>
#include <fstream>
#include "knnSearchers/iSax.h"

using namespace std;

void search_one(iSAXSearcher& searcher, std::vector<float>& test_time_series) {
    auto ts = searcher.search(test_time_series);

    std::cout << "Results:\n";
    for (auto& val : ts) {
        std::cout << val << " ";
    }
    std::cout << std::endl;
}

void searchKNN(iSAXSearcher& searcher, std::vector<float>& test_time_series, int k) {
    auto ts = searcher.search(test_time_series, k);

    std::cout << "Results:\n";
    for (auto& val : ts) {
        for (auto& val2 : val) {
            std::cout << val2 << " ";
        }
        std::cout << std::endl;
    }
}

int main()
{
    std::string datapath = "Dataset/datasets/synthetic/fourier_slice/fourier_slice_dataset.txt";
    std::cout << "Loading data from " << datapath << std::endl;
    iSAXSearcher searcher(datapath);
    std::cout << "Data loaded" << std::endl;

    searcher.initialize();

    //Getting the first time series from datapath
    std::ifstream file(datapath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << datapath << std::endl;
    }
    int idx = 110;
    while(idx) {
        std::getline(file, line);
        idx--;
    }

    std::istringstream iss(line);
    std::vector<float> test_time_series;
    float num;

    while (iss >> num) {
        test_time_series.push_back(num);
    }

    file.close();

    std::cout << "Searching for the 5 nearest neighbors of the first time series in the dataset" << std::endl;

    for(auto& val: test_time_series){
        std::cout << val << " ";
    }
    std::cout << std::endl;

    search_one(searcher, test_time_series);
    return 0;
}