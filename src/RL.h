#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>
#include <queue>

using namespace std;

#include "data.h"
#include "Kruskal.h"

class RL {

public:
    RL();
    vector<double> getBestLambdas();
    vector<pair<int, int>> getBestEdges();
    double getNewUB();
    int doRL(vector<vector<double>> dist, Data& data, double UB, vector<double>& lambdas);

private:
    vector<double> best_lambdas;
    vector <pair<int, int>> best_edges;
    double new_UB;
};