#pragma once

#include <string>
#include <fstream>
#include <vector>

using namespace std;

class Data{
    private:

        int items;
        int capacity;
        vector<int> weights;

    public:

    Data(char* instance);

    int getQuantItems();
    int getCapacity();
    int getWeight(int i);

};