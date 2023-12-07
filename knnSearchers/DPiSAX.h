#ifndef DPISAX_H
#define DPISAX_H

#include "iSax.h"
#include "utils.h"
#include <string>
#include <cstring>
#include <vector>

std::vector<char> serializeTimeSeries(const std::vector<float> values);

void broadcastSerializedQuery(const std::vector<char>& serializedQuery, int rank, int size);

std::vector<float> deserializeTimeSeries(std::vector<char> serializedQuery);

std::vector<TimeSeries> gatherResults(std::vector<TimeSeries> localResults, int rank, int size);

std::vector<TimeSeries> combineResults(const TimeSeries& q, std::vector<TimeSeries> globalResults, int k);


class DPiSAX {
private:
    std::vector<std::string> filePaths;
    int k;
public:
    DPiSAX(std::vector<std::string> filenames, int k);
    ~DPiSAX();

    std::vector<TimeSeries> parallelSearch(TimeSeries q, int k); 

};


#endif