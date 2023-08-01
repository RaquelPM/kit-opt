#pragma once

#include "Data.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>
#include <cmath>

#define bigM 1000000
#define EPSILON 0.00000001
using namespace std;

class MasterProblem{
    private:
        Data* data;
        IloEnv env;
        IloModel model;
        IloCplex solver;

        IloNumVarArray lambdas;
        IloRangeArray restrictions;
        IloObjective obj;

        vector<vector<bool>> itemIJ;

    public:
        IloNumArray duals;
        MasterProblem(Data* data);
        double solve();
        void addColumn(vector<bool> column);
        pair<int, int> getPairFractioned();
        vector<double> getLambdas();
        vector<vector<bool>> getItemIJ();
        void attLambdas(vector<int> lambdasProhibited, vector<vector<bool>> A);
        vector<int> getLambdasPairTogether(pair<int, int> pairFractioned);
        vector<int> getLambdasPairSeparated(pair<int, int> pairFractioned);
};