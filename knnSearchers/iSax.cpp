#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"

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

void Node::insert(TimeSeries ts){
    //std::vector <iSAXSymbol> tsSymbols = ts.tsToSAX();
    std::vector<iSAXSymbol> tsSymbols;
    auto iSAXRepresentation = ts.tsToiSAX(3, 3);

    for (auto& p: iSAXRepresentation) {
        tsSymbols.emplace_back(p.first, p.second);
    }

    if (isLeaf()){
        this->datapoints.push_back(ts);
        if (this->datapoints.size() > threshold){
            split(turnSplit);
            turnSplit = (turnSplit + 1) % dimension;
        } 
    } else {
        if(isRoot()){
            std::vector<iSAXSymbol> pref;
            for (int i=0;tsSymbols.size();i++){
                pref.push_back(iSAXSymbol(((1<<maxWidth) & tsSymbols[i].symbol),1));
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
        }
    }
}

void Leaf::split (int turnSplit) {
    if (symbols[turnSplit].level < maxWidth){
        std::vector<iSAXSymbol> newSymbols0, newSymbols1;
        // convierto a interno
        newSymbols0 = symbols;
        newSymbols0[turnSplit].level++;
        newSymbols0[turnSplit].symbol *= 2;
        newSymbols1 = symbols;
        newSymbols1[turnSplit].level++;
        newSymbols1[turnSplit].symbol = newSymbols1[turnSplit].symbol * 2 + 1;

        Node* newChild0 = new Leaf();
        Node* newChild1 = new Leaf();
        newChild0->parent = this;
        newChild1->parent = this;
        newChild0->symbols = newSymbols0;
        newChild1->symbols = newSymbols1;

        // redistribuyo los datos
        for (auto& ts: datapoints){
            if (newChild0->covers(ts.tsToiSAX(3, 3))){
                newChild0->insert(ts);
            } else {
                newChild1->insert(ts);
            }
        }
    }
}


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
