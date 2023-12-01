#ifndef ISAX_H
#define ISAX_H

#include "knnSearcher.h"
#include "../TimeSeries/TimeSeries.h"

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
        std::vector<TimeSeries> datapoints;
        std::vector<iSAXSymbol> symbols;
        int turnSplit;
        int maxCard;
        int dimension;
        int threshold;
        int maxWidth;
        Node* parent;

        Node() = default;
        Node(TimeSeries ts) {
            this->datapoints.push_back(ts);
        } 

        bool isRoot(){
            return parent == nullptr;
        }
        bool isLeaf(){
            return false;
        }

        virtual void insert(TimeSeries ts) = 0;
        
        bool covers(std::vector<iSAXSymbol> tsSymbols);

        virtual ~Node() = default;
};

class Leaf: public Node {
    public:
        using Node::Node;
        bool isLeaf(){
            return true;
        }

        void insert(TimeSeries ts) override;
        void split (int turnSplit);

        ~Leaf() = default;
};

class Internal: public Node {
    public:
        using Node::Node;

        void insert(TimeSeries ts) override;

        ~Internal() = default;
};

class iSAXSearcher: public knnSearcher {
    private:
        Node* root;
        int maxCard;
        int dimension;
        int threshold;
    public:
        using knnSearcher::knnSearcher;

        ~iSAXSearcher() = default;

        std::vector<TimeSeries> search(TimeSeries q, int k) override;
        std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k) override;
        void insert(TimeSeries ts) override;
        void createIndex() override;
};

#endif