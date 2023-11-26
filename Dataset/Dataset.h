#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include "Timeseries.h"

class Dataset {
    private:
        std::vector<TimeSeries> dataset;
    public:
        Dataset(std::string filename);
        Dataset(std::vector<TimeSeries> dataset);
        ~Dataset() = default;

        std::vector<TimeSeries> getDataset();
};

Dataset::Dataset(std::string filename) {
    std::ifstream file(filename);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<float> values;
        float num;

        while (iss >> num) {
            values.push_back(num);
        }

        this->dataset.emplace_back(values);
    }
}

std::vector<TimeSeries> Dataset::getDataset() {
    return this->dataset;
}

#endif