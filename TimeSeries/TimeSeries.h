#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <algorithm>
#include <vector>
#include <limits>

class TimeSeries {
    private:
        int length;
        std::vector<float> values;
        std::vector<float> cutPointsSegments;
        std::vector<float> curPointsValues;
        std::vector<float> paaRepresentation;
        std::vector<std::pair<int, int>> iSAXRepresentation;

        void getPAARepresentation(int segments);
        void getiSAXRepresentation(int segments, int wordLength);

    public:
        TimeSeries() = default;
        TimeSeries(std::vector<float> values);

        ~TimeSeries() = default;

        void setValues(const std::vector<float>& values);

        int getLength();

        std::vector<float> getValues();

        std::vector <float>::iterator begin();
        std::vector <float>::iterator end();

        std::vector<iSAXSymbol> tsToSAX(int wordLength, int buckets);
};


#endif