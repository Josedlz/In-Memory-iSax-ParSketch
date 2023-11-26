#ifndef KNNSEARCHER_H
#define KNNSEARCHER_H

#include <iostream>
#include <vector>
#include <string>

#include "Timeseries.h"
#include "Dataset.h"

class knnSearcher {
    private:
        Dataset dataset;
        void loadDataset(std::string filename);

    public:
        knnSearcher(std::string filename);
        ~knnSearcher() = default;

        void search(TimeSeries q, int k);

        void search(const std::vector<TimeSeries>& queries, int k);
};

void knnSearcher::loadDataset(std::string filename) {
    this->dataset = Dataset(filename);
}

knnSearcher::knnSearcher(std::string filename) {
    this->loadDataset(filename);
}

#endif