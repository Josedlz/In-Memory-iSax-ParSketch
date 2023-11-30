
#include "Dataset/Dataset.h"

#include <fstream>

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
