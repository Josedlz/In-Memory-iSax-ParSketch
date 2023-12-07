#include "TimeSeries.h"
#include <iostream>
#include <cmath>

std::vector<std::vector<double>> getSAXdistanceMatrix(int cardinality) {
    std::vector<std::vector<double>> distanceMatrix(cardinality, std::vector<double>(cardinality, 0));

    // Assuming linear breakpoints for simplicity
    for (int i = 0; i < cardinality; ++i) {
        for (int j = 0; j < cardinality; ++j) {
            distanceMatrix[i][j] = std::abs(i - j); // Distance is the difference in breakpoints
        }
    }

    return distanceMatrix;
}

double iSAXSymbol::minDistToSymbol(iSAXSymbol o) {
    iSAXSymbol a, b;
    if (level < o.level) {
        a = *this;
        b = o;
    } else {
        b = *this;
        a = o;
    }
    if (a.symbol < 0 || a.symbol >= (1 << a.level) || b.symbol < 0 || b.symbol >= (1 << b.level)) {
        // Handle error: symbol out of bounds
        throw std::out_of_range("Symbol out of bounds");
    }
    std::vector<std::vector<double>> distMat = getSAXdistanceMatrix(1 << b.level);

    if (a.level == b.level) {
        return distMat[a.symbol][b.symbol];
    } else {
        // Different cardinalities, need to "promote" the lower cardinality symbol
        int widthDiff = b.level - a.level;
        int rsBLoad = b.symbol >> widthDiff;

        int promotedASymbol = a.symbol << widthDiff;
        for (int i = 0; i < (1 << widthDiff); ++i) {
            int bitMask = 1 << (widthDiff - 1 - i);
            if ((a.symbol & bitMask) == (rsBLoad & bitMask)) {
                // S^k_i forms a prefix for T^8_i
                promotedASymbol |= (b.symbol & bitMask);
            } else if ((a.symbol & bitMask) < (rsBLoad & bitMask)) {
                // S^k_i is lexicographically smaller than T^8_i
                promotedASymbol |= bitMask;
            } else {
                // Else, leave the bit as 0 (already set by the left shift)
            }
        }

        // Calculate the distance using the promoted symbol
        return distMat[b.symbol][promotedASymbol];
    }
}

TimeSeries::TimeSeries(std::vector<float> values) {
    this->values = values;
}

void TimeSeries::setValues(const std::vector<float>& values) {
    this->values = values;
}

int TimeSeries::getLength() const{
    return this->values.size();
}

std::vector<float> TimeSeries::getValues() const{
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

    double totalDist = 0.0;

    for (int i = 0; i < wordLength; i++) {
        iSAXSymbol thisSymbol(thisiSAX[i].first, thisiSAX[i].second);

        // Calculate the distance between each pair of symbols
        double temp = thisSymbol.minDistToSymbol(o[i]);
        totalDist += temp * temp; // Squaring the distance as per the original implementation
    }

    return static_cast<int>(totalDist);
}

double TimeSeries::euclideanDist(const TimeSeries& o) const {
    double dist = 0;

    for (int i = 0; i < this->values.size(); i++) {
        double temp = this->values[i] - o.values[i];
        dist += temp * temp;
    }

    return dist;
}
