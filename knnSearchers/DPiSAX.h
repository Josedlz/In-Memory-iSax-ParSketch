#ifndef DPISAX_H
#define DPISAX_H

#include "knnSearcher.h"

class DPiSAX {
private:
    // Attributes for DPiSAX, such as partitioning information, SAX parameters, etc.

public:
    DPiSAX(std::string filename);
    ~DPiSAX();

    void DPiSAX::parallelIndexBuild();

    void DPiSAX::parallelSearch(TimeSeries q, int k); 

};


#endif