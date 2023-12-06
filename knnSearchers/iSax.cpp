#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"
#include <queue>
#include <cmath>
#include <iostream>


// ROOT
explicit Root::Root() {
    children.resize(1 << WORD_LENGTH, nullptr);

    // We create all the children of the root in advance
    for(int i = 0; i < 1 << WORD_LENGTH; i++) {
        std::vector<iSAXSymbol> child_prefix;

        for (int j = 0; j < WORD_LENGTH; j++) {
            if (i & (1 << j)) {
                child_prefix.emplace_back(1, 1);
            } else {
                child_prefix.emplace_back(0, 1);
            }
        }

        children[i] = new Leaf(child_prefix);
    }
}

bool Root::covers(std::vector<iSAXSymbol>& timeSeriesWord) const {
    std::cout << "Warning: Root should not be invoking covers" << std::endl;
    return true;
}

std::vector<iSAXSymbol> Root::insert(TimeSeries ts) {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    bool inserted = false;
    std::vector<iSAXSymbol> inserted_node_prefix;
    for (int i=0;i<children.size();i++){
        if (children[i]->covers(timeSeriesWord)){
            inserted_node_prefix = children[i]->insert(ts);
            inserted = true;
            break;
        }
    }
    if (!inserted) {
        throw std::runtime_error("No child from the root covers the time series");
    }

    return inserted_node_prefix;
}

TimeSeries Root::search(TimeSeries& ts) const {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    for (int i=0;i<children.size();i++){
        if (children[i]->covers(timeSeriesWord)){
            return children[i]->search(ts);
        }
    }
}

Root::~Root() {
    for (auto& child: children) {
        delete child;
    }
}

// INTERNAL

bool Internal::covers(std::vector<iSAXSymbol>& timeSeriesWord) const {
    for (int i=0;i<timeSeriesWord.size();i++){
        int sy = timeSeriesWord[i].level; 
        for (int j=prefix[i].level;j>=1;j--){ 
            int sim1 = (timeSeriesWord[i].symbol & (1<<(sy-1))) ? 1 : 0;
            int sim2 = (prefix[i].symbol & (1<<(j-1))) ? 1 : 0;
            if (sim1 != sim2){
                return false;
            }
            sy--;
        }
    }
    return true; 
}


std::vector<iSAXSymbol> Internal::insert(TimeSeries ts){
    //std::vector <iSAXSymbol> timeSeriesWord = ts.tsToSAX();
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    std::vector<iSAXSymbol> inserted_node_prefix;

    if (leftChild->covers(timeSeriesWord)){
        inserted_node_prefix = leftChild->insert(ts);
    } else if (rightChild->covers(timeSeriesWord)){
        inserted_node_prefix = rightChild->insert(ts);
    } else {
        throw std::runtime_error("No child from the internal node covers the time series");
    }

    return inserted_node_prefix;
}

TimeSeries Internal::search(TimeSeries& ts) const {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    if (leftChild->covers(timeSeriesWord)){
        return leftChild->search(ts);
    } else if (rightChild->covers(timeSeriesWord)){
        return rightChild->search(ts);
    } else {
        throw std::runtime_error("No child from the internal node covers the time series");
    }
}

Internal::~Internal() {
    delete leftChild;
    delete rightChild;
}

// Leaf

bool Leaf::covers(std::vector<iSAXSymbol>& timeSeriesWord) const {
    for (int i=0;i<timeSeriesWord.size();i++){
        int sy = timeSeriesWord[i].level; 
        for (int j=prefix[i].level;j>=1;j--){ 
            int sim1 = (timeSeriesWord[i].symbol & (1<<(sy-1))) ? 1 : 0;
            int sim2 = (prefix[i].symbol & (1<<(j-1))) ? 1 : 0;
            if (sim1 != sim2){
                return false;
            }
            sy--;
        }
    }
    return true; 
}


std::vector<iSAXSymbol> Leaf::insert(TimeSeries ts) {
    data.push_back(ts);
    if (data.size() > THRESHOLD){
        split(turnSplit);
        turnSplit = (turnSplit + 1) % WORD_LENGTH;
    } 
}

std::vector<iSAXSymbol> Leaf::split (int turnSplit) {
    if (pow(2, prefix[turnSplit].level) < CARDINALITY){
        //TODO
    }
}

/*
std::vector<TimeSeries> iSAXSearcher::search(TimeSeries q, int k) {
    // best first search
    indexablePQ<TimeSeries> result(k);
    auto iSAXRepresentation = q.tsToiSAX(WORD_LENGTH, CARDINALITY);
    std::vector<iSAXSymbol> word;

    for (auto& p: iSAXRepresentation) {
        word.emplace_back(p.first, p.second);
    }

    std::priority_queue<std::pair<double, Node*>> candidates;

    candidates.push(std::make_pair(0, root));
    

    while (!candidates.empty()){
        std::cout << "In loop" << std::endl;
        auto candidate = candidates.top();
        candidates.pop();

        std::cout << "Candidate: " << candidate.first << std::endl;        
        std::cout << "Result kth: " << result[k-1].first << std::endl;

        if (candidate.first > result[k-1].first){
            break;
        }

        if (candidate.second->isLeaf()){
            std::cout << "We are in a leaf" << std::endl;
            for (auto& ts: candidate.second->datapoints){
                if (q.euclideanDist(ts) < result[k-1].first){
                    result.push(std::make_pair(q.euclideanDist(ts), ts));
                }
            }
            std::cout << "Result so far:" << std::endl;
            for (auto& p: result){
                std::cout << p.first << " ";
            }
            std::cout << std::endl;

        } else {
            for (auto& child: candidate.second->children){
                if (child->covers(word) or candidate.second->isRoot()){
                    candidates.push(std::make_pair(q.minDist(child->isax_word, cardinality, wordLength), child));
                }
            }
        }
    }

    std::vector<TimeSeries> ret;

    for (auto& p: result){
        ret.push_back(p.second);
    }

    return ret;

}

std::vector<TimeSeries> iSAXSearcher::search(const std::vector<TimeSeries>& queries, int k){
    for (auto&q: queries){
        root->search(q, k);
    }
}
*/

void iSAXSearcher::insert(TimeSeries ts) {  
    root->insert(ts);
}

void iSAXSearcher::createIndex() {
    int i = 0;
    for (auto&ts: dataset){
        std::cout << "Inserting " << i << std::endl;
        root->insert(ts);
        std::cout << "Inserted " << i++ << std::endl;

        if(i == 50) break;
    }
}