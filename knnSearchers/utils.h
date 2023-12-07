#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <limits>

std::string toBinary(int symbol, int level);


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