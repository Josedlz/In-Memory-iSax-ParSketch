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
        virtual std::vector<iSAXSymbol> insert(TimeSeries ts) = 0;

        virtual TimeSeries search(TimeSeries& ts) const = 0;

        virtual bool covers(std::vector<iSAXSymbol>& tsSymbols) const = 0; 

        virtual bool isRoot() = 0;

        virtual bool isLeaf() = 0;

        virtual ~Node() = default;
};

class Root: public Node {
    std::vector<Leaf*> children;
    std::vector<iSAXSymbol> prefix;
    int turnSplit = 0;
    public:
        explicit Root() {
            children.resize(1 << WORD_LENGTH, nullptr);
        }
        std::vector<iSAXSymbol> insert(TimeSeries ts) override;
        TimeSeries search(TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        bool isRoot() override {
            return true;
        }

        bool isLeaf() override {
            return false;
        }

        ~Root();
};

class Internal: public Node {
    Node* leftChild;
    Node* rightChild;
    std::vector<iSAXSymbol> prefix;
    int turnSplit = 0;

    public:

        //explicit Internal(std::vector<iSAXSymbol> prefix) : prefix(prefix), leftChild(nullptr), rightChild(nullptr) {}
        explicit Internal(std::vector<iSAXSymbol> prefix, Node* left, Node* right) : prefix(prefix), leftChild(left), rightChild(right) {}

        std::vector<iSAXSymbol> insert(TimeSeries ts) override;
        TimeSeries search(TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        bool isRoot() override {
            return false;
        }

        bool isLeaf() override {
            return false;
        }

        ~Internal();
};

class Leaf: public Node {
    std::vector<TimeSeries> data;
    std::vector<iSAXSymbol> prefix;

    int M;
    int turnSplit = 0;

    public:
        explicit Leaf(std::vector<iSAXSymbol> prefix) : prefix(prefix) {
            M = THRESHOLD;
        }

        std::vector<iSAXSymbol> insert(TimeSeries ts) override;
        std::vector<iSAXSymbol> split (int turnSplit);
        TimeSeries search(TimeSeries& ts) const override;

        bool covers(std::vector<iSAXSymbol>& tsSymbols) const override;

        bool isRoot() override {
            return false;
        }

        bool isLeaf() override {
            return true;
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

        std::vector<TimeSeries> search(TimeSeries q, int k) override;
        std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k) override;
        void insert(TimeSeries ts) override;
        void createIndex() override;

};

#endif