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

    int n = data->getQuantItems();
    
    solver.solve();

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

    itemIJ.push_back(column);
    lambdas.add(newLambda);

    cout << "size: adicionando colunas" << itemIJ.size() << " " << lambdas.getSize() << endl;
}

pair<int, int> MasterProblem::getPairFractioned(){
    int n = data->getQuantItems();
    
    pair<int, int> fractionedPair;
    double fractionedValue = bigM;

    vector<vector<double>> pairs(n, vector<double>(n, 0));
    
    // show itemIJ
    cout << "itemIJ: " << endl;
    for(int i = 0; i < itemIJ.size(); i++){
        for(int j = 0; j < n; j++){
            cout << itemIJ[i][j] << " ";
        }
        cout << " lambda: " << solver.getValue(lambdas[i]) << endl;
    }

    for(int i = 0; i < itemIJ.size(); i++){
        for(int j = 0; j < n-1; j++){
            for(int w = j+1; w < n; w++){
                if(itemIJ[i][j] && itemIJ[i][w]){
                    pairs[j][w] += solver.getValue(lambdas[i]);
                }
            }
            
        }
    }

    cout << endl;

    // show pairs
    cout << "pairs: " << endl;
    for(int i = 0; i < pairs.size(); i++){
        for(int j = 0; j < pairs[i].size(); j++){
            cout << pairs[i][j] << " ";
        }
        cout << endl;
    }

    for(int i = 0; i < pairs.size(); i++){        
        for(int j = i+1; j < pairs[i].size(); j++){
            if(pairs[i][j] == 0) continue;
            cout << i << " " << j << " " << pairs[i][j] << endl;
            if(abs(pairs[i][j] - 0.5) < fractionedValue){
                fractionedPair = make_pair(i, j);
                fractionedValue = abs(pairs[i][j] - 0.5);
            }
        }
    }

    cout << "fractionedPair: " << fractionedPair.first << " " << fractionedPair.second << endl;
    if(fractionedPair.first == 0 && fractionedPair.second == 0) exit(0);

    return fractionedPair;
}

vector<int> MasterProblem::getLambdasPairTogether(pair<int, int> fractionedPair){
    vector<int> lambdasPairTogether;

    for(int i = 0; i < itemIJ.size(); i++){
        if(itemIJ[i][fractionedPair.first] && itemIJ[i][fractionedPair.second]){
            lambdasPairTogether.push_back(i);
        }
    }

    return lambdasPairTogether;
}

vector<int> MasterProblem::getLambdasPairSeparated(pair<int, int> fractionedPair){
    vector<int> lambdasPairSeparated;

    for(int i = 0; i < itemIJ.size(); i++){
        if(itemIJ[i][fractionedPair.first] && !itemIJ[i][fractionedPair.second]){
            lambdasPairSeparated.push_back(i);
        }
        else if(!itemIJ[i][fractionedPair.first] && itemIJ[i][fractionedPair.second]){
            lambdasPairSeparated.push_back(i);
        }
    }

    return lambdasPairSeparated;
}

vector<double> MasterProblem::getLambdas(){
    vector<double> l;
    for(int i =0; i< lambdas.getSize(); i++){
        l.push_back(solver.getValue(lambdas[i]));
    }

    return l;
}

vector<vector<bool>> MasterProblem::getItemIJ(){
    return itemIJ;
}

void MasterProblem::attLambdas(vector<int> lambdasProhibited, vector<vector<bool>> A){
    itemIJ = A;

    for(int i =0; i < lambdas.getSize(); i++){
        if(lambdas[i].getUB() == 0) lambdas[i].setUB(IloInfinity);
    }

    for(int i =0; i <lambdasProhibited.size(); i++){
        lambdas[lambdasProhibited[i]].setUB(0);
    }
}