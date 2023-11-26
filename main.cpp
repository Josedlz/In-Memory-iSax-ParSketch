#include <iostream>
#include "knnSearcher.h"

using namespace std;

int main()
{
    TimeSeries t;
    t.setValues({1, 2, 3, 4, 5});

    for(auto v: t) {
        cout << v << " ";
    }
    cout << endl;
}