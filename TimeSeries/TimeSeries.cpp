#include "TimeSeries.h"



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

void TimeSeries::getPAARepresentation(int segments) {
    this->paaRepresentation.resize(segments, 0.0f);

    this->cutPointsSegments.resize(segments);

    for (int i = 0; i < segments - 1; i++) {
        this->cutPointsSegments[i] = 0 + int(this->values.size() / segments) * (i + 1);
    }

    int currentIndexCurrentBucket = 0;
    int lastIndexPreviousBucket = 0;

    for (int segment = 0; segment < segments; segment++) {

        while (currentIndexCurrentBucket < cutPointsSegments[segment]) {
            this->paaRepresentation[segment] += this->values[currentIndexCurrentBucket];
            currentIndexCurrentBucket++;
        }

        this->paaRepresentation[segment] /= currentIndexCurrentBucket - lastIndexPreviousBucket + 1;
        lastIndexPreviousBucket = currentIndexCurrentBucket;
    }
}

void TimeSeries::getiSAXRepresentation(int segments, int wordLength) {

    float min = *std::min_element(this->values.begin(), this->values.end());
    float max = *std::max_element(this->values.begin(), this->values.end());

    float range = max - min;

    float step = range / (1<<wordLength);

    this->iSAXRepresentation.resize(segments, std::make_pair(0, wordLength));

    for (int i = 0; i < segments; i++) {
        auto PAA = this->paaRepresentation[i];

        for (int j = 0; j < (1<<wordLength); j++) {
            if (PAA >= min + j * step && PAA <= min + (j + 1) * step) {
                this->iSAXRepresentation[i] = std::make_pair(j, wordLength);
                break;
            }
        }
    }
}

std::vector<std::pair<int, int>> TimeSeries::tsToiSAX(int wordLength, int segments) {

    getPAARepresentation(segments);
    getiSAXRepresentation(segments, wordLength);


    for (auto isax : this->iSAXRepresentation) {
        this->iSAXRepresentation.emplace_back(isax.first, isax.second);
    }

    return this->iSAXRepresentation;
}

int TimeSeries::minDist(std::vector<iSAXSymbol> o, int maxWith, int segments) {
    auto thisiSAX = this->tsToiSAX(maxWith, segments);

    int dist = 0;

    for (int i = 0; i < segments; i++) {
        double temp = iSAXSymbol(thisiSAX[i].first, thisiSAX[i].second).minDist(o[i]);
        dist += temp * temp;
    }

    return dist;
}
