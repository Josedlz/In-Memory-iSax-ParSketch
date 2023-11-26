#ifndef PARSKETCH_H
#define PARSKETCH_H

#include "knnSearcher.h"

class ParSketchSearcher: public knnSearcher {
    private:
    // attributes

    public:
        ParSketchSearcher() = default;
        ~ParSketchSearcher() = default;
};

#endif