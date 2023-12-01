#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"
#include <queue>
#include <iostream>

bool Node::covers(std::vector<iSAXSymbol> tsSymbols) {
    bool covered = true;
    for (int i=0;i<tsSymbols.size();i++){
        int sy = tsSymbols[i].level;
        for (int j=tsSymbols[i].level;j>=0;j--){
            int sim1 = (tsSymbols[i].symbol & (1<<j)) ? 1 : 0;
            int sim2 = (symbols[i].symbol & (1<<(sy))) ? 1 : 0;
            if (sim1 != sim2){
                covered = false;
                break;
            }
            sy--;
        }
    }
    return covered; 
}

void Internal::insert(TimeSeries ts){
    //std::vector <iSAXSymbol> tsSymbols = ts.tsToSAX();
    std::cout << "instantiating tsSymbols" << std::endl;
    std::vector<iSAXSymbol> tsSymbols;
    std::cout << "Converting to iSAX" << std::endl;
    auto iSAXRepresentation = ts.tsToiSAX(this->cardinality, this->wordLength);
    std::cout << "Converted to iSAX" << std::endl;

    for (auto& p: iSAXRepresentation) {
        tsSymbols.emplace_back(p.first, p.second);
    }
    if(isRoot()){
        //std::cout << "We are in the root" << std::endl;
        std::vector<iSAXSymbol> pref;
        for (int i=0;tsSymbols.size();i++){
            pref.push_back(iSAXSymbol((tsSymbols[i].symbol>>(tsSymbols[i].level-1))&1,1));
        }
        bool inserted = false;
        for (int i=0;i<children.size();i++){
            if (children[i]->covers(pref)){
                children[i]->insert(ts);
                inserted = true;
                break;
            }
        }
        if (!inserted) children.push_back(new Leaf(ts));
    } else {
        //std::cout << "We are in an internal node" << std::endl;
        bool inserted = false;
        for (int i=0;i<children.size();i++){
            if (children[i]->covers(tsSymbols)){
                children[i]->insert(ts);
                inserted = true;
                break;
            }
        }
    }
}

void Leaf::insert(TimeSeries ts) {
    //std::cout << "Inserting in leaf" << std::endl;
    this->datapoints.push_back(ts);
    if (this->datapoints.size() > threshold){
        split(turnSplit);
        turnSplit = (turnSplit + 1) % wordLength;
    } 
}

void Leaf::split (int turnSplit) {
    if (symbols[turnSplit].level < cardinality){
        std::vector<iSAXSymbol> newSymbols0, newSymbols1;
        Internal* newNode = new Internal();
        newNode->parent = this->parent;
        newSymbols0 = symbols;
        newSymbols0[turnSplit].level++;
        newSymbols0[turnSplit].symbol *= 2;
        newSymbols1 = symbols;
        newSymbols1[turnSplit].level++;
        newSymbols1[turnSplit].symbol = newSymbols1[turnSplit].symbol * 2 + 1;

        Node* newChild0 = new Leaf();
        Node* newChild1 = new Leaf();
        newChild0->parent = newNode;
        newChild1->parent = newNode;
        newChild0->symbols = newSymbols0;
        newChild1->symbols = newSymbols1;
        newNode->children.push_back(newChild0);
        newNode->children.push_back(newChild1);


        // redistribuyo los datos
        for (auto& ts: datapoints){
            std::vector<iSAXSymbol> tsSymbols;
            auto iSAXRepresentation = ts.tsToiSAX(3, 3);

            for (auto& p: iSAXRepresentation) {
                tsSymbols.emplace_back(p.first, p.second);
            }

            if (newChild0->covers(tsSymbols)){
                newChild0->insert(ts);
            } else {
                newChild1->insert(ts);
            }
        }
    }
}


std::vector<TimeSeries> iSAXSearcher::search(TimeSeries q, int k) {
    // best first search
    indexablePQ<TimeSeries> result(k);
    auto iSAXRepresentation = q.tsToiSAX(3, 3);
    std::vector<iSAXSymbol> word;

    for (auto& p: iSAXRepresentation) {
        word.emplace_back(p.first, p.second);
    }

    std::priority_queue<std::pair<double, Node*>> candidates;

    candidates.push(std::make_pair(0, root));
    

    while (!candidates.empty() && result.size() < k){
        auto candidate = candidates.top();
        candidates.pop();

        if (candidate.first > result[k-1].first){
            break;
        }

        if (candidate.second->isLeaf()){
            for (auto& ts: candidate.second->datapoints){
                if (q.euclideanDist(ts) < result[k-1].first){
                    result.push(std::make_pair(q.euclideanDist(ts), ts));
                }
            }
        } else {
            for (auto& child: candidate.second->children){
                if (child->covers(word)){
                    candidates.push(std::make_pair(q.minDist(child->symbols, cardinality, wordLength), child));
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
    //for (auto&q: queries){
    //    root->search(q, k);
    //}
    return std::vector<TimeSeries>();
}

void iSAXSearcher::insert(TimeSeries ts) {  
    root->insert(ts);
}

void iSAXSearcher::createIndex() {
    int i = 0;
    for (auto&ts: dataset){
        //std::cout << "Inserting " << i << std::endl;
        root->insert(ts);
        //std::cout << "Inserted " << i++ << std::endl;
    }
}
