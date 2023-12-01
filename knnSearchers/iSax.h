#ifndef ISAX_H
#define ISAX_H

#include <limits>

#include "knnSearcher.h"
#include "../TimeSeries/TimeSeries.h"


class Node {
    private:
        int THRESHOLD = 5;
        int WORD_LENGTH = 4;
        int CARDINALITY = 4;
    public:
        std::vector<Node*> children;
        std::vector<TimeSeries> datapoints;
        std::vector<iSAXSymbol> symbols;
        int turnSplit = 0;
        //int maxCard;
        int wordLength;
        int threshold;
        int cardinality;
        Node* parent;

        Node() {
            this->threshold = THRESHOLD;
            this->wordLength = WORD_LENGTH;
            this->cardinality = CARDINALITY;

            this->symbols.resize(wordLength, iSAXSymbol(0, cardinality));
        };

        Node(TimeSeries ts, std::vector<iSAXSymbol> nodeSymbols) {
            this->threshold = THRESHOLD;
            this->wordLength = WORD_LENGTH;
            this->cardinality = CARDINALITY;

            this->symbols = nodeSymbols;

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
        Internal* root;
        int maxCard;
        int wordLength;
        int threshold;
        int cardinality;
    public:

        iSAXSearcher(std::string filename) : knnSearcher(filename) {
            this->root = new Internal();
        }

        ~iSAXSearcher() = default;

        std::vector<TimeSeries> search(TimeSeries q, int k) override;
        std::vector<TimeSeries> search(const std::vector<TimeSeries>& queries, int k) override;
        void insert(TimeSeries ts) override;
        void createIndex() override;
};

template <typename T>
class indexablePQ {
    private:
        std::vector<std::pair<double, T>> sortedList;
        int k = 0; 
    public:
        indexablePQ(int sz) {
            this->k = sz;
          this->sortedList.resize(sz, std::make_pair(std::numeric_limits<double>::infinity(), T()));
        }

        std::pair<double, T> top() {
            return sortedList.front();
        }

        void push(std::pair<double, T> p) {
            // Custom comparator that compares only the first elements of the pairs
            auto comp = [](const std::pair<double, T>& a, const std::pair<double, T>& b) {
                return a.first < b.first;
            };

            // Find the position to insert using binary search
            auto it = std::lower_bound(sortedList.begin(), sortedList.end(), p, comp);
  
            // If the vector is not full yet, append a placeholder at the end
            if (sortedList.size() < k) {
                sortedList.emplace_back();
            }
  
            // Determine the starting point for shifting elements to the right
            auto shiftStart = std::distance(sortedList.begin(), it);
            // Shift elements to the right to make space for the new element
            for (size_t i = std::min(int(sortedList.size() - 1), int(k - 1)); i > shiftStart; --i) {
                sortedList[i] = sortedList[i - 1];
            }
  
            // Insert the new element
            *it = p;
        }

       std::pair<double, T>  operator[](int index) {
            return this->sortedList[index];
        }

        int size() {
            return sortedList.size();
        }

        typename std::vector<std::pair<double, T>>::iterator begin() {
            return sortedList.begin();
        }
        
        typename std::vector<std::pair<double, T>>::iterator end() {
            return sortedList.end();
        }

        ~indexablePQ() = default;
};

#endif