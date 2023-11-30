#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"


std::vector<TimeSeries> iSAXSearcher::search(TimeSeries q, int k) {
    //root->search(q, k);

    return std::vector<TimeSeries>();
}

std::vector<TimeSeries> iSAXSearcher::search(const std::vector<TimeSeries>& queries, int k){
    //for (auto&q: queries){
    //    root->search(q, k);
    //}
    return std::vector<TimeSeries>();
}

void iSAXSearcher::insert(TimeSeries ts) {  
    root->insert(ts);
}

void iSAXSearcher::createIndex() {
    for (auto&ts: dataset){
        this->insert(ts);
    }
}
