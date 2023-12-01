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

        virtual std::vector<TimeSeries> search(TimeSeries q, int k);

        virtual std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k);

        virtual void insert(TimeSeries t);

        virtual void createIndex();

        void initialize();

        virtual ~knnSearcher() = default;
};

#endif