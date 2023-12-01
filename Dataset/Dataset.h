#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include "../TimeSeries/TimeSeries.h"

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


#endif