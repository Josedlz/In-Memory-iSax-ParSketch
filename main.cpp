#include <iostream>
#include "knnSearchers/iSax.h"
#include "knnSearchers/ParSketch.h"
using namespace std;

int main()
{
    std::string datapath = "Dataset/datasets/synthetic/fourier_slice/fourier_slice_dataset.txt";
    iSAXSearcher searcher1(datapath);
    ParSketchSearcher searcher2(datapath);
    searcher1.initialize();
    searcher2.initialize();

    return 0;
}