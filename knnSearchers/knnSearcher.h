#ifndef KNNSEARCHER_H
#define KNNSEARCHER_H

#include <iostream>
#include <vector>
#include <string>

#include "TimeSeries.h"
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