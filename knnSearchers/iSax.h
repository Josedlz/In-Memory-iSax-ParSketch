#ifndef ISAX_H
#define ISAX_H

#include "TimeSeries.h"

class iSAXSymbol {
    public:
        int symbol;
        int level;
        iSAXSymbol(int symbol, int level){
            this->symbol = symbol;
            this->level = level;
        }
        iSAXSymbol() = default;
        ~iSAXSymbol() = default;
};


class Node {
    public:
        std::vector<Node*> children;
        std::vector<TimeSeries> ts;
        std::vector<iSAXSymbol> symbols;
        int turnSplit;
        int maxCard;
        int dimension;
        int threshold;
        int maxWith;
        Node* parent;

        Node() = default;
        bool isRoot(){
            return parent == nullptr;
        }
        bool isLeaf(){
            return false;
        }
        void insert(TimeSeries ts){
            std::vector <iSAXSymbol> tsSymbols = ts.tsToSAX();
            if (isLeaf()){
                this->ts.push_back(ts);
                if (children.size() > threshold){
                    split(turnSplit);
                    turnSplit = (turnSplit + 1) % dimension;
                } 
            } else {
                if(isRoot){
                    std::vector<iSAXSymbol> pref;
                    for (int i=0;tsSymbols.size();i++){
                        pref.push_back(iSAXSymbol(((1<<maxWith) & tsSymbols[i].symbol),1));
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
        
        bool covers(std::vector<iSAXSymbol> tsSymbols){
            bool covered = true;
            for (int i=0;i<tsSymbols.size();i++){
                int sy = tsSymbols[i].level;
                for (int j=tsSymbols[i].level;j>=0;j--){
                    if ((tsSymbols[i].symbol & (1<<j)) != (symbols[i].symbol & (1<<(sy)))){
                        covered = false;
                        break;
                    }
                }
            }
            return covered; 
        }
        virtual void split (int turnSplit) = 0;
        ~Node() = default;
};

class Leaf: public Node {
    private:
        std::vector<TimeSeries> ts;
        Node* parent;
    public:
        Leaf() = default;
        bool isLeaf(){
            return true;
        }
        void insert(TimeSeries ts){
            this->ts.push_back(ts);
        }
        void split (int turnSplit) {
            if (symbols[turnSplit].level < maxWith){
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
            }
        }
        ~Leaf() = default;
};

class iSaxSearcher: public knnSearcher {
    private:
        Node* root;
        int maxCard;
        int dimension;
        int threshold;
    public:
        iSaxSearcher() = default;
        void insert(TimeSeries ts){
            root->insert(ts);
        }
        ~iSaxSearcher() = default;
};

#endif