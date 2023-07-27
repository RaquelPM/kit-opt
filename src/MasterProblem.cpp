#include "MasterProblem.h"

MasterProblem::MasterProblem(Data* data){
    this->data = data;
    int n = data->getQuantItems();
    
    model = IloModel(env);
    solver = IloCplex(model);

    itemIJ = vector<vector<bool>>(n, vector<bool>(n, false));
    lambdas = IloNumVarArray(env, n, 0, IloInfinity);
    restrictions = IloRangeArray(env);

    IloExpr sum(env);

    for(int i =0; i < n; i++){
        char name[50];
        sprintf(name, "λ%d", i);
        lambdas[i].setName(name);

        sum += bigM * lambdas[i];
        restrictions.add(lambdas[i] == 1);

        itemIJ[i][i] = true;
    }

    obj = IloMinimize(env, sum);

    model.add(obj);
    model.add(restrictions);

    sum.end();
}

double MasterProblem::solve(){
    solver = IloCplex(model);
    solver.setOut(env.getNullStream());
    solver.solve();

    int n = data->getQuantItems();

    duals = IloNumArray(env, n);

    for(int i = 0; i < n; i++){
        try{
            duals[i] = solver.getDual(restrictions[i]);
        }
        catch(IloException& e){
            cout << "ERRO: " << e << endl;
            e.end();
        }
    }
   
   return solver.getObjValue();
}

void MasterProblem::addColumn(vector<bool> column){
    int n = data->getQuantItems();
    IloNumColumn col = obj(1);

    for(int i = 0; i < n; i++){
        if(column[i]){
            col += restrictions[i](1);
        }
    }

    IloNumVar newLambda(col, 0.0, IloInfinity);

    char name[30];
    sprintf(name, "λ%d", lambdas.getSize());
    newLambda.setName(name);

    lambdas.add(newLambda);

    itemIJ.push_back(column);
}