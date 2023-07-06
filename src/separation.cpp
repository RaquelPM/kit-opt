#include "separation.h"

extern vector <vector<int> > MaxBack(double** x, int n){
    vector<vector<int>> subtours;
    vector<int> vertices;

    for(int i =0; i<n; i++){
        vertices.push_back(i);
    }

    int z = rand() % vertices.size();
    vector<int> S;

    while(!vertices.empty()){
        while(S.size() < n){
           
        }
    }


    return subtours;
}

extern vector <vector<int> > MinCut(double** x, int n){
    cout << "MinCut" << endl;
    return {{}};
}