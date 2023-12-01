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
        double minDist(iSAXSymbol o) {
            iSAXSymbol a, b;
            if (level < o.level) {
                a = *this;
                b = o;
            } else {
                b = *this;
                a = o;
            }

            if (a.level == b.level) {
                std::vector<std::vector<double>> distMat; //= getDistanceMatrix(1 << a.level);
                return distMat[a.symbol][b.symbol];
            } else {
                std::vector<std::vector<double>> distMat; //= getDistanceMatrix(1 << b.level);
                int widthDiff = b.level - a.level;
                int rsBLoad = b.symbol >> widthDiff;
                if (a.symbol > rsBLoad) {
                    int lsALoad = (a.symbol << widthDiff) & (INT_MAX << widthDiff);
                    return distMat[b.symbol][lsALoad];
                } else if (a.symbol < rsBLoad) {
                    int lsALoad = (a.symbol << widthDiff) | (INT_MAX >> (sizeof(int) * 8 - widthDiff));
                    return distMat[b.symbol][lsALoad];
                } else {
                    return 0;
                }
            }
        }
        ~iSAXSymbol() = default;
};


class TimeSeries {
    private:
        int length;
        std::vector<float> values;
        std::vector<float> breakpoints;
        std::vector<float> curPointsValues;
        std::vector<float> paaRepresentation;
        std::vector<std::pair<int, int>> iSAXRepresentation;

        void getPAARepresentation(int wordLength);
        void getiSAXRepresentation(int wordLength, int cardinality);

    public:
        TimeSeries() = default;
        TimeSeries(std::vector<float> values);

        ~TimeSeries() = default;

        void setValues(const std::vector<float>& values);

        int getLength();

        std::vector<float> getValues();

        std::vector <float>::iterator begin();
        std::vector <float>::iterator end();

        std::vector<std::pair<int, int>> tsToiSAX(int cardinality, int wordLength);

        int minDist(std::vector<iSAXSymbol> o, int maxWith, int wordLength);

        double euclideanDist(TimeSeries o);
};


#endif