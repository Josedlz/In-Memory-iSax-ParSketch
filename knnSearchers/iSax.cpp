#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"
#include <queue>
#include <cmath>
#include <iostream>

Node* handle_split(Leaf* node, int turnSplit){
    auto [leftNode, rightNode] = node->split(turnSplit);

    if (leftNode == nullptr || rightNode == nullptr){
        return node;
    }

    auto childPrefix = node->getPrefix();
    auto nextTurnSplit = (turnSplit + 1) % WORD_LENGTH;
    auto newInternal = new Internal(childPrefix, leftNode, rightNode, nextTurnSplit);
    return newInternal;
}

// ROOT
explicit Root::Root() {
    children.resize(1 << WORD_LENGTH, nullptr);

    // We create all the children of the root in advance
    for(int i = 0; i < 1 << WORD_LENGTH; i++) {
        std::vector<iSAXSymbol> childPrefix;

        for (int j = 0; j < WORD_LENGTH; j++) {
            if (i & (1 << j)) {
                childPrefix.emplace_back(1, 1);
            } else {
                childPrefix.emplace_back(0, 1);
            }
        }

        children[i] = new Leaf(childPrefix, turnSplit=0);
    }
}

bool Root::covers(std::vector<iSAXSymbol>& timeSeriesWord) const {
    std::cout << "Warning: Root should not be invoking covers" << std::endl;
    return true;
}

void Root::insert(TimeSeries ts) {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    bool inserted = false;
    for (int i=0;i<children.size();i++){
        if (children[i]->covers(timeSeriesWord)){
            children[i]->insert(ts);
            if (children[i]->isLeaf() && static_cast<Leaf*>(children[i])->size() > THRESHOLD){
                auto newInternal = handle_split(static_cast<Leaf*>(children[i]), turnSplit);
                delete children[i];
                children[i] = newInternal;
            }
            inserted = true;
            break;
        }
    }

    if (!inserted) {
        throw std::runtime_error("No child from the root covers the time series");
    }
}

std::pair<Node*, Node*> Root::split (int turnSplit) {
    throw std::runtime_error("Root nodes should not be invoking split");
}

TimeSeries Root::search(TimeSeries& ts) const {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    for (auto& child: children){
        if (child->covers(timeSeriesWord)){
            return child->search(ts);
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

void Internal::insert(TimeSeries ts){
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    std::vector<Node*> children = {leftChild, rightChild};

    bool inserted = false;
    for (auto& child: children){
        if (child->covers(timeSeriesWord)){
            child->insert(ts);
            inserted = true;
            if (child->isLeaf() && static_cast<Leaf*>(child)->size() > THRESHOLD){
                auto newInternal = handle_split(static_cast<Leaf*>(child), turnSplit);
                delete child;
                child = newInternal;
            }
            break;
        }
    }
    if (!inserted) {
        throw std::runtime_error("No child from the internal node covers the time series");
    }
}

std::pair<Node*, Node*> Internal::split (int turnSplit) {
    throw std::runtime_error("Internal nodes should not be invoking split");
}

TimeSeries Internal::search(TimeSeries& ts) const {
    std::vector<iSAXSymbol> timeSeriesWord;
    auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);

    for (auto& p: iSAXRepresentation) {
        timeSeriesWord.emplace_back(p.first, p.second);
    }

    std::vector<Node*> children = {leftChild, rightChild};

    for (auto& child: children){
        if (child->covers(timeSeriesWord)){
            return child->search(ts);
        }
    }
}

Internal::~Internal() {
    delete leftChild;
    delete rightChild;
}

// LEAF

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


void Leaf::insert(TimeSeries ts) {
    data.push_back(ts);
}

std::pair<Node*, Node*> Leaf::split (int turnSplit) {
    if (pow(2, prefix[turnSplit].level) < CARDINALITY){

        int nextTurnSplit = (turnSplit + 1) % WORD_LENGTH;

        auto leftPrefix = prefix;
        leftPrefix[turnSplit].level += 1;
        leftPrefix[turnSplit].symbol <<= 1;

        auto rightPrefix = prefix;
        rightPrefix[turnSplit].level += 1;
        rightPrefix[turnSplit].symbol <<= 1;
        rightPrefix[turnSplit].symbol += 1;

        auto leftNode = new Leaf(leftPrefix, nextTurnSplit);
        auto rightNode = new Leaf(rightPrefix, nextTurnSplit);

        for (auto& ts: data){
            auto iSAXRepresentation = ts.tsToiSAX(WORD_LENGTH, CARDINALITY);
            std::vector<iSAXSymbol> word;

            for (auto& p: iSAXRepresentation) {
                word.emplace_back(p.first, p.second);
            }

            if (leftNode->covers(word)){
                leftNode->insert(ts);
            } else {
                rightNode->insert(ts);
            }
        }

        return std::make_pair(leftNode, rightNode);
    }
    return std::make_pair(nullptr, nullptr);
}

TimeSeries Leaf::search(TimeSeries& ts) const {
    TimeSeries bestMatch;
    double bestDist = std::numeric_limits<double>::max();

    for (auto& ts2: data){
        double dist = ts.euclideanDist(ts2);
        if (dist < bestDist){
            bestDist = dist;
            bestMatch = ts2;
        }
    }

    return bestMatch;
}

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
        auto candidate = candidates.top();
        candidates.pop();

        if (candidate.first > result[k-1].first){
            break;
        }

        if (candidate.second->isLeaf()){
            for (auto& ts: static_cast<Leaf*>(candidate.second)->getData()){
                if (q.euclideanDist(ts) < result[k-1].first){
                    result.push(std::make_pair(q.euclideanDist(ts), ts));
                }
            }

        } else {
            std::vector<Node*> children = {static_cast<Internal*>(candidate.second)->leftChild, static_cast<Internal*>(candidate.second)->rightChild};
            for (auto& child: children){
                if (child->covers(word) or candidate.second->isRoot()){
                    candidates.push(std::make_pair(q.minDist(child->getPrefix(), cardinality, wordLength), child));
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