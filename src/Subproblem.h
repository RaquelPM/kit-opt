#pragma once

#include "Data.h"
#include <ilcplex/ilocplex.h>
#include <iostream>
#include <vector>

#define bigM 1000000
#define EPSILON 0.00000001
using namespace std;

class Subproblem{
    private: 
        Data* data;
        IloEnv envSub;
        IloModel modelSub;
        IloCplex solver;

        IloRangeArray restrictions;
        IloNumArray duals;
        IloObjective obj;
        IloBoolVarArray x;

        long int *p;
        
    public:
        vector<bool> solution;
        Subproblem(Data* data);
        void attObjective(IloNumArray duals);
        void attRestrictionsTogether(vector<pair<int, int>> pairT);
        void attRestricitionsSeparated(vector<pair<int, int>> pairS);
        double solve(bool raiz);
};