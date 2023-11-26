#ifndef KNNSEARCHER_H
#define KNNSEARCHER_H

#include <iostream>
#include <vector>
#include <string>

#include "Timeseries.h"
#include "Dataset.h"

class knnSearcher {
    private:

    public:
        knnSearcher();
        ~knnSearcher();

        void loadDataset(std::string filename);

        void search(TimeSeries q, int k);

        void search(const std::vector<TimeSeries>& queries, int k);
};
#endif