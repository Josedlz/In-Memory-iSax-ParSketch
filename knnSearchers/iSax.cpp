#include "iSax.h"
#include "../TimeSeries/TimeSeries.h"
#include <queue>
#include <cmath>
#include <iostream>

std::string toBinary(int symbol, int level) {
    std::string binary = "";
    for (int i = level - 1; i >= 0; --i) {
        binary += (symbol & (1 << i)) ? '1' : '0';
    }
    return binary;
}


bool Node::covers(std::vector<iSAXSymbol> tsSymbols) {
    std::cout << "Checking if node covers" << std::endl;


    std::cout << "Node symbols: " << std::endl;
    for (auto& p: symbols) {
        std::cout << toBinary(p.symbol, p.level) << "(" << p.level << ") ";
    }
    std::cout << std::endl;

    std::cout << "TimeSeries symbols: " << std::endl;
    for (auto& p: tsSymbols) {
        std::cout << toBinary(p.symbol, p.level) << "(" << p.level << ") ";
    }
    std::cout << std::endl;

    
    for (int i=0;i<tsSymbols.size();i++){
        int sy = tsSymbols[i].level; // 4
        for (int j=symbols[i].level;j>=1;j--){ 
            int sim1 = (tsSymbols[i].symbol & (1<<(sy-1))) ? 1 : 0;
            int sim2 = (symbols[i].symbol & (1<<(j-1))) ? 1 : 0;
            if (sim1 != sim2){
                std::cout << "Does not cover" << std::endl;
                return false;
            }
            sy--;
        }
    }
    std::cout << "Covers" << std::endl;
    return true; 
}

void Internal::insert(TimeSeries ts){
    //std::vector <iSAXSymbol> tsSymbols = ts.tsToSAX();
    std::vector<iSAXSymbol> tsSymbols;
    auto iSAXRepresentation = ts.tsToiSAX(this->wordLength, this->cardinality);

    std::cout << "iSAXRepresentation:" << std::endl;
    for (auto& p: iSAXRepresentation) {
        std::cout << p.first << "(" << p.second << ") ";
    }
    std::cout << std::endl;

    for (auto& p: iSAXRepresentation) {
        tsSymbols.emplace_back(p.first, p.second);
    }
    if(isRoot()){
        std::cout << "Inserting in the root" << std::endl;
        std::vector<iSAXSymbol> pref;
        for (int i=0; i < tsSymbols.size(); i++){
            pref.push_back(iSAXSymbol((tsSymbols[i].symbol>>(tsSymbols[i].level-1))&1, 1));
        }

        bool inserted = false;
        for (int i=0; i < children.size();i++){
            if (children[i]->covers(tsSymbols)){
                children[i]->insert(ts);
                inserted = true;
                break;
            }
        }
        if (!inserted) {
            Leaf* newLeaf = new Leaf(ts, pref);
            children.push_back(newLeaf);
            newLeaf->parent = this;
        }
    } else {
        std::cout << "Inserting in an internal node" << std::endl;
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
    std::cout << "Inserting in leaf" << std::endl;
    std::cout << "ID: " << std::endl;

    for (auto& p: symbols) {
        std::cout << p.symbol << "(" << p.level << ") ";
    }
    std::cout << "number of timeseries in node: " << datapoints.size() << std::endl;
    this->datapoints.push_back(ts);
    if (this->datapoints.size() > threshold){
        split(turnSplit);
        turnSplit = (turnSplit + 1) % wordLength;
    } 
}

void Leaf::split (int turnSplit) {
    if (pow(2, symbols[turnSplit].level) < cardinality){

        auto& my_parents_children = this->parent->children;
        auto it = std::find(my_parents_children.begin(), my_parents_children.end(), this);
        if (it != my_parents_children.end()) {
            std::cout << "Do I delete it?" << std:: endl;
            my_parents_children.erase(it);  
            std::cout << reinterpret_cast<uintptr_t>(this) << std::endl;

            std::cout << reinterpret_cast<uintptr_t>(*it) << std::endl;
        }
        

        Internal* newNode = new Internal();

        std::vector<iSAXSymbol> newSymbols0, newSymbols1;

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

        // redistribuyo los datos
        for (auto& ts: datapoints){
            std::vector<iSAXSymbol> tsSymbols;
            auto iSAXRepresentation = ts.tsToiSAX(20, 4);

            for (auto& p: iSAXRepresentation) {
                tsSymbols.emplace_back(p.first, p.second);
            }
            if (newChild0->covers(tsSymbols)){
                newChild0->insert(ts);
            } else {
                newChild1->insert(ts);
            }
        }

        newNode->children.push_back(newChild0);
        newNode->children.push_back(newChild1);

        my_parents_children.push_back(newNode);

        delete this; 
    }
}

std::vector<TimeSeries> iSAXSearcher::search(TimeSeries q, int k) {
    // best first search
    indexablePQ<TimeSeries> result(k);
    auto iSAXRepresentation = q.tsToiSAX(20, 4);
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
            std::cout << "We are in an internal node called " << std::endl;
            for (auto& child: candidate.second->children){
                std::cout << "ID ";
                auto childSymbols = child->symbols;
                for (auto& s: childSymbols){
                    std::cout << s.symbol << "(" << s.level << ") ";
                }
                std::cout << "\nDoes child cover? " << child->covers(word) << std::endl;
                /*
                if (child->covers(word) or candidate.second->isRoot()){
                    std::cout << "Found child that covers" << std::endl;
                    candidates.push(std::make_pair(q.minDist(child->symbols, cardinality, wordLength), child));
                }
                */
                std::cout << "Found child that covers" << std::endl;
                candidates.push(std::make_pair(q.minDist(child->symbols, cardinality, wordLength), child));
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
        std::cout << "Inserting " << i << std::endl;
        root->insert(ts);
        std::cout << "Inserted " << i++ << std::endl;

        if(i == 50) break;
    }
    //print();
}

void iSAXSearcher::print() const{
    if (root) {
        root->print();
    } else {
        std::cout << "Empty tree." << std::endl;
    }
}

void Node::print(size_t indent) {
    std::cout << "Hello" << std::endl;
    for (size_t i = 0; i < indent; ++i) {
        std::cout << "  ";
    }

    // Imprime información del nodo.
    std::cout << "Symbols Node: ";
    for (const iSAXSymbol& s : symbols) {
        std::cout << s.symbol << " (" << s.level << ") ";
    }
    if (this->isLeaf()) {
        Leaf* leaf = dynamic_cast<Leaf*>(this);
        std::cout << ", Points: [ ";
        for (auto& t : leaf->datapoints) {
            auto isax = t.tsToiSAX(this->wordLength, this->cardinality);

            for (const auto& s : isax) {
                std::cout << s.first << "(" << s.second << ") ";
            }
            std::cout << std::endl;
        }
        std::cout << "]";
    } else {
        std::cout << std::endl;
        Internal* inner = dynamic_cast<Internal*>(this);
        std::cout << "Internal Node: " << std::endl;
        std::cout << inner->children.size() << std::endl;
        std::cout << "I crossed" << std::endl;
        for ( Node* child : inner->children) {
            std::cout << "Child:" << std::endl;
            child->print(indent + 1); 
        }
    }
    std::cout << std::endl;
}