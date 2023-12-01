#include <iostream>
#include "knnSearchers/iSax.h"

using namespace std;

int main()
{
    std::string datapath = "Dataset/datasets/synthetic/fourier_slice/fourier_slice_dataset.txt";
    iSAXSearcher searcher(datapath);
    searcher.initialize();

    return 0;
}