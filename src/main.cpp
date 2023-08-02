#include "Data.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <cmath>

using namespace std;

struct InsertionInfo {
    int noInserido;
    int arestaRemovida;
    double custo;
};

struct Solution {
    vector<int> sequencia;
    double valorObj;
};

void showSolution (Solution *s){
    cout << "Exemplo de Solucao s = ";
    for (int i = 0; i < s->sequencia.size(); i++) {
        cout << s->sequencia[i] << " -> ";
    }
    cout << "Custo de S: " << s->valorObj << endl;
};

void registerSolutionCost (Solution *s, Data& data){
    s->valorObj =0.0;
    int n = s->sequencia.size();
    for (int i = 0; i < n -1; i++) {
        s->valorObj += data.getDistance(s->sequencia[i], s->sequencia[i+1]);
    }
};


vector<InsertionInfo> calcCostInsertion (Solution *s, Data& data, vector<int>& CL){

    vector<InsertionInfo> costInsertion ((s->sequencia.size()-1) * CL.size());

    int count=0;
    int l =0;
  
  for(int a= 0, b= 1; count < s->sequencia.size() - 1; a++, b++){
    
    int i= s->sequencia[a];
    int j= s->sequencia[b];

    for (auto k : CL){
      costInsertion[l].custo= data.getDistance(i, k) + data.getDistance(j, k) - data.getDistance(i, j);
      costInsertion[l].noInserido= k;
      costInsertion[l].arestaRemovida= a;
      l++;
    }
    count++;
    
  }
  
  return costInsertion;    
};

bool check(InsertionInfo i, InsertionInfo j){
  return i.custo < j.custo;
}


void Construction(Solution *s, Data& data){
    vector<int> CL;
    for (size_t i = 1; i <= data.getDimension(); i++) {
        if(i != 1) CL.push_back(i);
    }

    int maxIterIls = data.getDimension() >= 150 ? data.getDimension()/2 : data.getDimension();

    s->sequencia.push_back(1);
    for(int i =0; i<3; i++){
     double alpha = (double) rand() / RAND_MAX;
     alpha += 0.000001;
     int index = rand() % ((int) ceil(alpha * CL.size()));

     s->sequencia.push_back(CL[index]);
     CL.erase(CL.begin() + index);
    }
    s->sequencia.push_back(1);

    while(!CL.empty()){
        vector<InsertionInfo> costInsertion = calcCostInsertion(s, data, CL);

        sort(costInsertion.begin(), costInsertion.end(), check);

        double alpha = (double) rand() / RAND_MAX;
        int index = rand() % ((int) ceil(alpha * costInsertion.size()));

        s->sequencia.insert(s->sequencia.begin()+ costInsertion[index].arestaRemovida + 1, costInsertion[index].noInserido);

        std::vector<int>::iterator clIndex = find(CL.begin(), CL.end(), costInsertion[index].noInserido);

        CL.erase(clIndex);
    };
};

bool bestImprovementSwap(Solution *s, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i =1; i< s->sequencia.size() -1; i++){
      int vi = s->sequencia[i];
      int vi_next = s->sequencia[i+1];
      int vi_prev = s->sequencia[i-1];

      for(int j =i+1; j< s->sequencia.size() -1; j++){
        int vj = s->sequencia[j];
        int vj_next = s->sequencia[j+1];
        int vj_prev = s->sequencia[j-1];

        double delta;

        if(j == i+1) 
            delta = - data.getDistance(vi, vi_prev) - data.getDistance(vj, vj_next) + data.getDistance(vi, vj_next) + data.getDistance(vj, vi_prev);

        else delta = -data.getDistance(vi, vi_next)-data.getDistance(vi, vi_prev) - data.getDistance(vj, vj_next) - data.getDistance(vj, vj_prev) + data.getDistance(vi, vj_next) + data.getDistance(vi, vj_prev) + data.getDistance(vj, vi_prev) + data.getDistance(vj, vi_next);

        if(delta < bestDelta){
            bestDelta = delta;
            best_i = i;
            best_j = j;
        }
      }
    }

    if(bestDelta < 0){
        std::swap(s->sequencia[best_i], s->sequencia[best_j]);
        s->valorObj += bestDelta;
        return true;
    }
    return false;
}

bool bestImprovementReinsertion(Solution *s, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i =1; i< s->sequencia.size() -1; i++){
      int vi = s->sequencia[i];
      int vi_next = s->sequencia[i+1];
      int vi_prev = s->sequencia[i-1];

      double i_distance, j_distance;

      i_distance =  data.getDistance(vi_prev, vi_next) - data.getDistance(vi, vi_next) - data.getDistance(vi, vi_prev);

      for(int j =i+1; j< s->sequencia.size() -1; j++){
        int vj = s->sequencia[j];
        int vj_next = s->sequencia[j+1];

        j_distance = data.getDistance(vi, vj) + data.getDistance(vi, vj_next) -data.getDistance(vj, vj_next); 

        if(i_distance + j_distance < bestDelta){
            bestDelta = i_distance + j_distance;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    if(bestDelta < 0){
        int i_best = s->sequencia[best_i];
        s->sequencia.erase(s->sequencia.begin()+best_i);
        s->sequencia.insert(s->sequencia.begin()+ best_j, i_best);


        s->valorObj += bestDelta;
        return true;
    }
    return false;
}

bool bestImprovementReinsertion2(Solution *s, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i = 1; i< s->sequencia.size() -2; i++){
      int vi1 = s->sequencia[i];
      int vi2 = s->sequencia[i+1];
      int vi_next = s->sequencia[i+2];
      int vi_prev = s->sequencia[i-1];

      double i_distance, j_distance;

      i_distance =  data.getDistance(vi_next, vi_prev) -data.getDistance(vi1, vi_prev) -data.getDistance(vi2, vi_next);

      for(int j =i+2; j< s->sequencia.size() - 1; j++){
        int vj = s->sequencia[j];
        int vj_next = s->sequencia[j+1];

        j_distance = data.getDistance(vi1, vj) + data.getDistance(vi2, vj_next) -data.getDistance(vj, vj_next); 

        if(i_distance + j_distance < bestDelta){
            bestDelta = i_distance + j_distance;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    if(bestDelta < 0){
        int i1_best = s->sequencia[best_i];
        int i2_best = s->sequencia[best_i+1];
        
        s->sequencia.erase(s->sequencia.begin()+best_i);
        s->sequencia.erase(s->sequencia.begin()+best_i);

        s->sequencia.insert(s->sequencia.begin()+ best_j-1, i2_best);
        s->sequencia.insert(s->sequencia.begin()+ best_j-1, i1_best);

        s->valorObj += bestDelta;
        return true;
    }
    return false;
}

bool bestImprovementReinsertion3(Solution *s, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i = 1; i< s->sequencia.size() - 3; i++){
      int vi1 = s->sequencia[i];
      int vi3 = s->sequencia[i+2];
      int vi_next = s->sequencia[i+3];
      int vi_prev = s->sequencia[i-1];

      double i_distance, j_distance;

      i_distance =  data.getDistance(vi_next, vi_prev) -data.getDistance(vi1, vi_prev) -data.getDistance(vi3, vi_next);

      for(int j =i+3; j< s->sequencia.size() - 1; j++){
        int vj = s->sequencia[j];
        int vj_next = s->sequencia[j+1];

        j_distance = data.getDistance(vi1, vj) + data.getDistance(vi3, vj_next) -data.getDistance(vj, vj_next); 

        if(i_distance + j_distance < bestDelta){
            bestDelta = i_distance + j_distance;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    if(bestDelta < 0){
        int i1_best = s->sequencia[best_i];
        int i2_best = s->sequencia[best_i+1];
        int i3_best = s->sequencia[best_i+2];
        
        s->sequencia.erase(s->sequencia.begin()+best_i);
        s->sequencia.erase(s->sequencia.begin()+best_i);
        s->sequencia.erase(s->sequencia.begin()+best_i);

        s->sequencia.insert(s->sequencia.begin()+ best_j-2, i3_best);
        s->sequencia.insert(s->sequencia.begin()+ best_j-2, i2_best);
        s->sequencia.insert(s->sequencia.begin()+ best_j-2, i1_best);


        s->valorObj += bestDelta;
        return true;
    }
    return false;
}

bool bestImprovement20pt(Solution *s, Data& data){
    double bestDelta = 0;
    int best_i, best_j;

    for(int i = 1; i< s->sequencia.size() - 1; i++){
      int vi = s->sequencia[i];
      int vi_next = s->sequencia[i+1];

      for(int j =i+2; j< s->sequencia.size() - 2; j++){
        int vj = s->sequencia[j];
        int vj_next = s->sequencia[j+1];

        double delta = data.getDistance(vi, vj) +data.getDistance(vi_next, vj_next) -data.getDistance(vi, vi_next) - data.getDistance(vj, vj_next);

        if(delta < bestDelta){
            bestDelta = delta;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    if(bestDelta < 0){
        for(int i = best_i+1, j = best_j;i<j; i++, j--){
            int aux = s->sequencia[j];
            s->sequencia[j] = s->sequencia[i];
            s->sequencia[i] = aux;
        }

        s->valorObj += bestDelta;
        return true;
    }
    return false;
}

void BuscaLocal(Solution *s, Data& data){
    vector<int> n = {1, 2, 3, 4, 5};

    bool improved = false;

    s->sequencia.erase(s->sequencia.begin());
    s->sequencia.erase(s->sequencia.begin()+s->sequencia.size()-1);

    while(!n.empty()){
        int z = rand() % n.size();
        switch (n[z]){
            case 1:
                improved = bestImprovementSwap(s, data);
                break;
            case 2:
                improved = bestImprovement20pt(s, data);
                break;
            case 3:
                improved = bestImprovementReinsertion(s, data);
                break;
            case 4:
                improved = bestImprovementReinsertion2(s, data);
                break;
            case 5:
                improved = bestImprovementReinsertion3(s, data);
                break;
        }

        if(improved)
            n = {1, 2, 3, 4, 5};
        else 
            n.erase(n.begin() + z);
    }

    s->sequencia.push_back(1);
    s->sequencia.insert(s->sequencia.begin(), 1);
}

void Pertubacao(Solution *s){
    s->sequencia.erase(s->sequencia.begin());
    s->sequencia.erase(s->sequencia.begin()+s->sequencia.size()-1);

    int t = s->sequencia.size()/10;

    int segmentOneFirst, segmentTwoFirst = -1, insertSegmentTwo, insertSegmentOne;
    vector<int> segmentOne = {}, segmentTwo ={};

    int tOne = rand() % t + 2;
    int tTwo = rand() % t + 2;

    segmentOneFirst = rand() % (s->sequencia.size() -1 - tOne);
    
    for(int i =0; i < tOne; i++){
        segmentOne.push_back(s->sequencia[segmentOneFirst+i]);
    }

    while(segmentTwoFirst == -1){
      segmentTwoFirst = rand() % (s->sequencia.size() -1 - tTwo);


      for(int i =0; i < segmentOne.size(); i++){
        for(int j =0; j < tTwo; j++){
            if(s->sequencia[segmentTwoFirst+j] == segmentOne[i]) {
                segmentTwoFirst = -1;
                break;
            }
        }
      }
    }

    for(int i =0; i < tTwo; i++){
        segmentTwo.push_back(s->sequencia[segmentTwoFirst+i]);
    }

    for(int i =0; i < segmentTwo.size(); i++){
        if(segmentTwoFirst < segmentOneFirst){
            std::vector<int>::iterator index = find(s->sequencia.begin(), s->sequencia.end(), segmentTwo[i]);
            s->sequencia.erase(index);
            s->sequencia.insert(s->sequencia.begin()+segmentOneFirst, segmentTwo[i]);
        }
        else {
            std::vector<int>::iterator index = find(s->sequencia.begin(), s->sequencia.end(), segmentTwo[tTwo-1-i]);
            s->sequencia.erase(index);
            s->sequencia.insert(s->sequencia.begin()+segmentOneFirst, segmentTwo[tTwo-1-i]);
        }
    }
    for(int i =0; i < segmentOne.size(); i++){
        if(segmentTwoFirst < segmentOneFirst){
            std::vector<int>::iterator index = find(s->sequencia.begin(), s->sequencia.end(), segmentOne[tOne-1-i]);
            s->sequencia.erase(index);
            s->sequencia.insert(s->sequencia.begin()+segmentTwoFirst, segmentOne[tOne-1-i]);
        }
        else{
            std::vector<int>::iterator index = find(s->sequencia.begin(), s->sequencia.end(), segmentOne[i]);
            s->sequencia.erase(index);
            s->sequencia.insert(s->sequencia.begin()+segmentTwoFirst, segmentOne[i]);
        }
        
    }

    s->sequencia.push_back(1);
    s->sequencia.insert(s->sequencia.begin(), 1);
}

Solution SolutionILS(int maxIter, int maxIterIls, vector<int> &CL, Data& data){
    Solution bestOfAll;
    bestOfAll.valorObj = INFINITY;
    Solution s;

    for(int i =0; i < maxIter; i++){
        s = {{}, 0};

        Construction(&s, data);
        registerSolutionCost(&s, data);

        Solution best = s;

        int iterIls = 0;

        while(iterIls <= maxIterIls){
            BuscaLocal(&s, data);
            registerSolutionCost(&s, data);
            if(s.valorObj < best.valorObj){
                best = s;
                iterIls = 0;
            }

            Pertubacao(&s);
           
            iterIls++;
        }

        if(best.valorObj < bestOfAll.valorObj)
            bestOfAll = best;
    }

    return bestOfAll;
}

int main(int argc, char** argv) {
    clock_t start= clock();
    srand((unsigned)time(NULL));
    vector<int> CL;
    Solution sLinha;

    double media =0;

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();

    // cout << "Dimension: " << n << endl;
    // cout << "DistanceMatrix: " << endl;
    // data.printMatrixDist();

    int maxIterIls = data.getDimension() >= 150 ? data.getDimension()/2 : data.getDimension();

    for(int i =0; i<10;i++){
        Solution bestSolution = SolutionILS(50, maxIterIls, CL, data);
        registerSolutionCost(&bestSolution, data);
        media += bestSolution.valorObj;
    }

    clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "Custo: " << media/10 << "  Tempo: " << time << endl; 

    return 0;
}




