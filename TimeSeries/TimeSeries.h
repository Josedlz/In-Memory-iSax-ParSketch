#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <algorithm>
#include <vector>
#include <climits>

class iSAXSymbol {
    public:
        int symbol;
        int level;

        iSAXSymbol() = default;
        iSAXSymbol(int symbol, int level){
            this->symbol = symbol;
            this->level = level;
        }

        double minDistToSymbol(iSAXSymbol o);

        ~iSAXSymbol() = default;
};


std::vector<std::vector<double>> getSAXdistanceMatrix(int cardinality);

class TimeSeries {
    private:
        std::vector<float> values;

        std::vector<float> getPAARepresentation(int wordLength) const;
        std::vector<std::pair<int, int>> getiSAXRepresentation(const std::vector<float>& paaRepresentation, int wordLength, int cardinality) const;

    public:
        TimeSeries() = default;
        TimeSeries(std::vector<float> values);

        ~TimeSeries() = default;

        void setValues(const std::vector<float>& values);

        int getLength() const;

        std::vector<float> getValues() const;

        std::vector <float>::iterator begin();
        std::vector <float>::iterator end();

        std::vector<std::pair<int, int>> tsToiSAX(int wordLength, int cardinality) const;

        int minDist(const std::vector<iSAXSymbol>& o, int maxWith, int wordLength) const;

        double euclideanDist(const TimeSeries& o) const;
};


#endif