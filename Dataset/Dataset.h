#ifndef DATASET_H
#define DATASET_H

#include <vector>
#include <string>
#include "Timeseries.h"


class Dataset {
    private:
        std::vector<TimeSeries> dataset;
    public:
        Dataset(std::string filename);
        Dataset(std::vector<TimeSeries> dataset);
        ~Dataset();

        void addTimeSeries(TimeSeries t);
        void addTimeSeries(const std::vector<float>& values);

        std::vector<TimeSeries> getDataset();
};
#endif