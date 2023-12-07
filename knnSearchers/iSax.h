#ifndef ISAX_H
#define ISAX_H

#include <limits>
#include "knnSearcher.h"
#include "../TimeSeries/TimeSeries.h"
#include "config.h"
#include "utils.h"

class Node {
    protected:
    public:
        virtual void insert(TimeSeries ts) = 0;

        virtual std::pair<Node*, Node*> split (int turnSplit) = 0;

        virtual TimeSeries search(const TimeSeries& ts) const = 0;

        virtual bool covers(std::vector<iSAXSymbol>& tsSymbols) const = 0; 

        virtual bool isRoot() = 0;

        virtual bool isLeaf() = 0;

        virtual std::vector<iSAXSymbol> getPrefix() = 0;

        virtual ~Node() = default;

};

class Root: public Node {
    std::vector<Node*> children;
    std::vector<iSAXSymbol> prefix;
    int turnSplit = 0;

    public:
        explicit Root(); 
        void insert(TimeSeries ts) override;

        std::pair<Node*, Node*> split (int turnSplit) override;

        TimeSeries search(const TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        bool isRoot() override {
            return true;
        }

        bool isLeaf() override {
            return false;
        }

        std::vector<iSAXSymbol> getPrefix() override {
            return prefix;
        }

        ~Root();
};

class Internal: public Node {
    std::vector<iSAXSymbol> prefix;
    int turnSplit;

    public:
        Node* leftChild;
        Node* rightChild;

        //explicit Internal(std::vector<iSAXSymbol> prefix) : prefix(prefix), leftChild(nullptr), rightChild(nullptr) {}
        explicit Internal(std::vector<iSAXSymbol> prefix, Node* left, Node* right, int turnSplit) : prefix(prefix), leftChild(left), rightChild(right), turnSplit(turnSplit) {}

        void insert(TimeSeries ts) override;

        std::pair<Node*, Node*> split (int turnSplit) override;

        TimeSeries search(const TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        bool isRoot() override {
            return false;
        }

        bool isLeaf() override {
            return false;
        }

        std::vector<iSAXSymbol> getPrefix() override {
            return prefix;
        }

        ~Internal();
};

class Leaf: public Node {
    std::vector<TimeSeries> data;
    std::vector<iSAXSymbol> prefix;

    int M;
    int turnSplit;

    public:
        explicit Leaf(std::vector<iSAXSymbol> prefix, int turnSplit) : prefix(prefix), turnSplit(turnSplit) {
            M = THRESHOLD;
        }

        void insert(TimeSeries ts) override;

        std::pair<Node*, Node*> split (int turnSplit);

        TimeSeries search(const TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        size_t size() const {
            return data.size();
        }

        bool isRoot() override {
            return false;
        }

        bool isLeaf() override {
            return true;
        }

        std::vector<iSAXSymbol> getPrefix() override {
            return prefix;
        }

        std::vector<TimeSeries> getData() {
            return data;
        }   

        ~Leaf() = default;
};

class iSAXSearcher: public knnSearcher {
    private:
        Root* root;
        int maxCard;
        int wordLength;
        int threshold;
        int cardinality;
    public:

        iSAXSearcher(std::string filename) : knnSearcher(filename) {
            this->root = new Root();
        }

        ~iSAXSearcher() {
            delete root;
        };

        TimeSeries search(const TimeSeries& q);

        std::vector<TimeSeries> search(const TimeSeries& q, int k) override;

        void insert(const TimeSeries& ts) override;

        void createIndex() override;

};

#endif