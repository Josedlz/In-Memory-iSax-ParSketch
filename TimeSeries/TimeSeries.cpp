#include "TimeSeries.h"
#include <iostream>
#include <cmath>



TimeSeries::TimeSeries(std::vector<float> values) {
    this->values = values;
}

void TimeSeries::setValues(const std::vector<float>& values) {
    this->values = values;
}

int TimeSeries::getLength() {
    return this->values.size();
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

std::vector<float> TimeSeries::getPAARepresentation(int wordLength) const {
    std::vector<float> breakpoints, paaRepresentation;
    paaRepresentation.resize(wordLength, 0.0f);
    breakpoints.resize(wordLength);

    for (int i = 0; i < wordLength; i++) {
        breakpoints[i] = 0 + int(this->values.size() / wordLength) * (i + 1);
    }

    /*
    std::cout << "Breakpoints: " << std::endl;
    for (auto& breakpoint : this->breakpoints) {
        std::cout << breakpoint << " ";
    }
    std::cout << std::endl;
    */

    int currentIndexCurrentBucket = 0;
    int lastIndexPreviousBucket = 0;

    for (int segment = 0; segment < wordLength; segment++) {

        while (currentIndexCurrentBucket < breakpoints[segment]) {
            paaRepresentation[segment] += this->values[currentIndexCurrentBucket];
            currentIndexCurrentBucket++;
        }

        paaRepresentation[segment] /= currentIndexCurrentBucket - lastIndexPreviousBucket + 1;
        lastIndexPreviousBucket = currentIndexCurrentBucket;
    }

    /*
    std::cout << "PAA: " << std::endl;
    for (auto& paa : this->paaRepresentation) {
        std::cout << paa << " ";
    }
    std::cout << std::endl;
    */

   return paaRepresentation;
}

std::vector<std::pair<int, int>> TimeSeries::getiSAXRepresentation(const std::vector<float>& paaRepresentation, int wordLength, int cardinality) const {

    float min = *std::min_element(this->values.begin(), this->values.end());
    float max = *std::max_element(this->values.begin(), this->values.end());

    float range = max - min;

    float step = range / (cardinality);

    std::vector<std::pair<int, int>> iSAXRepresentation;
    iSAXRepresentation.resize(wordLength, std::make_pair(0, cardinality));

    for (int i = 0; i < wordLength; i++) {
        auto PAA = paaRepresentation[i];

        for (int j = 0; j < cardinality; j++) {
            if (PAA >= min + j * step && PAA <= min + (j + 1) * step) {
                iSAXRepresentation[i] = std::make_pair(j, log2(cardinality));
                break;
            }
        }
    }

    /*
    std::cout << "iSAX" << std::endl;
    for (auto& isax : this->iSAXRepresentation) {
        std::cout << isax.first << "(" << isax.second << ") ";
    }
    std::cout << std::endl;
    */

   return iSAXRepresentation;
}

std::vector<std::pair<int, int>> TimeSeries::tsToiSAX(int wordLength, int cardinality) const {

    auto paaRepresentation = getPAARepresentation(wordLength);
    return getiSAXRepresentation(paaRepresentation, wordLength, cardinality);
}

int TimeSeries::minDist(const std::vector<iSAXSymbol>& o, int cardinality, int wordLength) const {
    auto thisiSAX = this->tsToiSAX(cardinality, wordLength);

    int dist = 0;

    for (int i = 0; i < wordLength; i++) {
        double temp = iSAXSymbol(thisiSAX[i].first, thisiSAX[i].second).minDist(o[i]);
        dist += temp * temp;
    }

    return dist;
}

double TimeSeries::euclideanDist(const TimeSeries& o) const {
    double dist = 0;

    for (int i = 0; i < this->values.size(); i++) {
        double temp = this->values[i] - o.values[i];
        dist += temp * temp;
    }

    return dist;
}
