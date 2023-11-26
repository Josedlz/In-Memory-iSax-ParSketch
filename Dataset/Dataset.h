#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include "Timeseries.h"

class Dataset {
    private:
        std::vector<TimeSeries> dataset;
    public:
        Dataset() = default;
        Dataset(std::string filename);
        Dataset(std::vector<TimeSeries> dataset);
        ~Dataset() = default;

        std::vector<TimeSeries> getDataset();
        int size() { return this->dataset.size(); }

        std::vector<TimeSeries>::iterator begin();
        std::vector<TimeSeries>::iterator end();

        TimeSeries operator[](int index);
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

std::vector<TimeSeries>::iterator Dataset::begin() {
    return this->dataset.begin();
}

std::vector<TimeSeries>::iterator Dataset::end() {
    return this->dataset.end();
}

TimeSeries Dataset::operator[](int index) {
    return this->dataset[index];
}

#endif