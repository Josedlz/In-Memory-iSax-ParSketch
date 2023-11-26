#ifndef TIMESERIES_H
#define TIMESERIES_H
#include <vector>

class TimeSeries {
    private:
        int length;
        std::vector<float> values;
    public:
        TimeSeries() = default;
        TimeSeries(std::vector<float> values);

        ~TimeSeries() = default;

        void setValues(const std::vector<float>& values);

        int getLength();

        std::vector<float> getValues();

        std::vector <float>::iterator begin();
        std::vector <float>::iterator end();
};

TimeSeries::TimeSeries(std::vector<float> values) {
    this->values = values;
    this->length = values.size();
}

void TimeSeries::setValues(const std::vector<float>& values) {
    this->values = values;
    this->length = values.size();
}

int TimeSeries::getLength() {
    return this->length;
}

std::vector<float> TimeSeries::getValues() {
    return this->values;
}

std::vector <float>::iterator TimeSeries::begin() {
    return this->values.begin();
}

std::vector <float>::iterator TimeSeries::end() {
    return this->values.end();
}


#endif