#include "Data.h"

Data::Data(char* instance){

    fstream fp;
    string linha;
    int weight;
    fp.open(instance, fstream::in);


    getline(fp, linha);
    items = stoi(linha);
    getline(fp, linha);
    capacity = stoi(linha);

    for(int i = 0; i < items; i++){
        getline(fp, linha);
        weight = stoi(linha);
        weights.push_back(weight);
    }
}

int Data::getQuantItems(){
    return items;
}

int Data::getCapacity(){
    return capacity;
}

int Data::getWeight(int i){
    return weights[i];
}
