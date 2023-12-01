#include "knnSearcher.h"


void knnSearcher::loadDataset(std::string filename){
    this->dataset = Dataset(filename);
}

knnSearcher::knnSearcher(std::string filename){
    loadDataset(filename);
}

void knnSearcher::initialize() {
    createIndex();
}