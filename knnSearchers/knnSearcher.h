#ifndef KNNSEARCHER_H
#define KNNSEARCHER_H

#include <iostream>
#include <vector>
#include <string>

#include "../TimeSeries/TimeSeries.h"
#include "../Dataset/Dataset.h"

class knnSearcher {
    private:
        void loadDataset(std::string filename);

    protected:
        Dataset dataset;

    public:
        knnSearcher(std::string filename);
        ~knnSearcher() = default;

        virtual std::vector<TimeSeries> search(TimeSeries q, int k) = 0;

        virtual std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k) = 0;

        virtual void insert(TimeSeries t) = 0;

        virtual void createIndex() = 0;

        void initialize();
};

#endif