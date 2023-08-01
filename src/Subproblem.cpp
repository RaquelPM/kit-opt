#include "Subproblem.h"
#include "minknap.c"

Subproblem::Subproblem(Data* data){
    this->data = data;
    int n = data->getQuantItems();
    
    modelSub = IloModel(envSub);
    solver = IloCplex(modelSub);

    x = IloBoolVarArray(envSub, n);
    IloExpr sum(envSub);

    for(int i =0; i < n; i++){
        sum += data->getWeight(i)*x[i];
    }

    obj = IloMaximize(envSub);

    modelSub.add(sum <= data->getCapacity());
    modelSub.add(obj);

    sum.end();
}

void Subproblem::attObjective(IloNumArray duals){
    int n = data->getQuantItems();
    IloExpr sum(envSub);

    p = new long int[n];

    for(int i = 0; i < n; i++){
        sum += duals[i]*x[i];
        p[i] = duals[i]*bigM;
    }

    obj.setExpr(sum);
    sum.end();
}

double Subproblem::solve(bool raiz){
    int n = data->getQuantItems();
    double objResult;

    if(!raiz){
        solver = IloCplex(modelSub);
        solver.setOut(envSub.getNullStream());

        solver.setParam(IloCplex::Param::TimeLimit, 2*60);
        solver.setParam(IloCplex::Param::Threads, 1);
        solver.setParam(IloCplex::Param::MIP::Tolerances::MIPGap, 1e-08);

        solver.solve();



        solution  = vector<bool>(n, 0);

        for(int i = 0; i < n; i++){
            solution[i] = solver.getValue(x[i]) >= 0.9 - EPSILON ? 1 : 0;
        }

        objResult = solver.getObjValue();
    }
    else {
        int *w = new int[n];
        int *x = new int[n];

        for(int i =0; i <n; i++){
            w[i] = data->getWeight(i);
        }

        long double obj = minknap(n, p, w, x, data->getCapacity());

        delete w;

        solution = vector<bool>(n, 0);

        for(int i = 0; i < n; i++){
            solution[i] = x[i];
        }

        delete x;
        objResult = obj/bigM;
    }
    
    return objResult;
}

void Subproblem::attRestrictionsTogether(vector<pair<int, int>> pairT){
    for(int i = 0; i < pairT.size(); i++){
        modelSub.add(x[pairT[i].first] == x[pairT[i].second]);
    }
}

void Subproblem::attRestricitionsSeparated(vector<pair<int, int>> pairS){
    for(int i = 0; i < pairS.size(); i++){
        modelSub.add(x[pairS[i].first] + x[pairS[i].second] <= 1);
    }
}

