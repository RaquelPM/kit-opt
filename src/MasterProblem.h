#pragma once

#include "Data.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

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

};