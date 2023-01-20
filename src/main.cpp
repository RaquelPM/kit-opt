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
    double custo;
};

struct Solution {
    vector<int> sequencia;
    double valorObj;
};

struct Subsequence {
    int W;
    double T, C;
    int first, last;
    inline static Subsequence Concatenate(Subsequence &sigma1, Subsequence &sigma2, Data& data){
        Subsequence sigma;
        double temp = data.getDistance(sigma1.last, sigma2.first);
        sigma.W = sigma1.W + sigma2.W;
        sigma.T = sigma1.T + temp + sigma2.T;
        sigma.C = sigma1.C + (sigma2.W * (sigma1.T + temp)) + sigma2.C;
        sigma.first = sigma1.first;
        sigma.last = sigma2.last;
        return sigma;
    }
};

void showSolution (Solution *s){
    cout << "Exemplo de Solucao s = ";
    for (int i = 0; i < s->sequencia.size(); i++) {
        cout << s->sequencia[i] << " -> ";
    }
    cout << "Custo de S: " << s->valorObj << endl;
};

void registerSolutionCost (Solution *s, vector<vector<Subsequence>>& subseq_matrix, Data& data){
    s->valorObj = subseq_matrix[0][s->sequencia.size()-1].C;
};


vector<InsertionInfo> calcCostInsertion (Data& data, int r, vector<int>& CL){

  vector<InsertionInfo> costInsertion (CL.size());

    int i =0;
    for (auto k : CL){
      costInsertion[i].custo = data.getDistance(r, k);
      costInsertion[i].noInserido = k;
      i++;
    }
    
  return costInsertion;    
};

bool check(InsertionInfo i, InsertionInfo j){
  return i.custo < j.custo;
}


void Construction(Solution *s, Data& data){
    vector<int> CL;
    int r = 1;
    for (size_t i = 1; i <= data.getDimension(); i++) {
        if(i != 1) CL.push_back(i);
    }

    s->sequencia.push_back(1);

    while(!CL.empty()){
        vector<InsertionInfo> costInsertion = calcCostInsertion(data, r, CL);

        sort(costInsertion.begin(), costInsertion.end(), check);

        double alpha = rand() / (RAND_MAX + 1.0);
        alpha += 0.0000000000000001;
        int index = rand() % ((int) ceil(alpha * costInsertion.size()));

        s->sequencia.push_back(costInsertion[index].noInserido);
        r = costInsertion[index].noInserido;

        std::vector<int>::iterator clIndex = find(CL.begin(), CL.end(), costInsertion[index].noInserido);

        CL.erase(clIndex);
    };

    s->sequencia.push_back(1);
};

void UpdateAllSubseq(Solution *s, vector<vector<Subsequence>>& subseq_matrix, Data& data){

    int n = s->sequencia.size();

    for(int i=0; i<n; i++){
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s->sequencia[i];
        subseq_matrix[i][i].last = s->sequencia[i];
    };

    for(int i =0; i<n;i++){
        for(int j=i+1; j<n;j++){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j], data);
        }
    };

    for(int i=n-1; i>= 0;i--){
        for(int j=i-1; j>=0;j--){
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j], data);
        }
    }

}

void UpdateIndexSubseq(Solution *s, vector<vector<Subsequence>>& subseq_matrix, Data& data, int best_i, int best_j){
    int n = s->sequencia.size();

    for(int i=0; i<n; i++){
        subseq_matrix[i][i].W = (i > 0);
        subseq_matrix[i][i].C = 0;
        subseq_matrix[i][i].T = 0;
        subseq_matrix[i][i].first = s->sequencia[i];
        subseq_matrix[i][i].last = s->sequencia[i];
    };

    for(int i = 0; i <= best_j; i++){
        for(int j = best_i; j < n; j++){
            if(i>= j){
                j = i+1;
            }
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j-1], subseq_matrix[j][j], data);
        }
    };

    for(int i = n-1; i >= best_i; i--){
        for(int j = best_j; j >= 0; j--){
            if(i <= j){
                j = i-1;
            }
            subseq_matrix[i][j] = Subsequence::Concatenate(subseq_matrix[i][j+1], subseq_matrix[j][j], data);
        }
    };

}

bool bestImprovement20pt(Solution *s, Data& data,  vector<vector<Subsequence>>& subseq_matrix){
    double bestDelta = 0;
    int best_i, best_j;
    int n = s->sequencia.size() -1;

    for(int i = 1; i< n; i++){
      for(int j =i+1; j< n; j++){
        Subsequence sigma_1 = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][i], data);
        Subsequence sigma_2 = Subsequence::Concatenate(sigma_1, subseq_matrix[j+1][n], data);

        double delta = sigma_2.C - subseq_matrix[0][n].C;

        if(delta < bestDelta){
            best_i = i;
            best_j = j;
            bestDelta = delta;
        }
      }
    }

    if(bestDelta < 0){
        for(int i = best_i, j = best_j;i<j; i++, j--){
            int aux = s->sequencia[j];
            s->sequencia[j] = s->sequencia[i];
            s->sequencia[i] = aux;
        }
        UpdateIndexSubseq(s, subseq_matrix, data, best_i, best_j);
        return true;
    }

    return false;
}

bool bestImprovementSwap(Solution *s, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    int n = s->sequencia.size() -1;
    double bestDelta = 0;
    int best_i, best_j;

    Subsequence sigma4;

    for(int i =1; i< n; i++){
      for(int j=i+1; j< n; j++){

        Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[j][j], data);
        Subsequence sigma2 = Subsequence::Concatenate(subseq_matrix[i][i], subseq_matrix[j+1][n], data);

        if(j != i+1){
            Subsequence sigma3 = Subsequence::Concatenate(subseq_matrix[i+1][j-1], sigma2, data);
            sigma4 = Subsequence::Concatenate(sigma1, sigma3, data);
        } else if(j == i+1)
            sigma4 = Subsequence::Concatenate(sigma1, sigma2, data);
        
        double delta = sigma4.C - subseq_matrix[0][n].C;
        if(delta < bestDelta){
            best_i = i;
            best_j = j;
            bestDelta = delta;
        }
      }
    }

    if(bestDelta < 0){
        std::swap(s->sequencia[best_i], s->sequencia[best_j]);
        UpdateIndexSubseq(s, subseq_matrix, data, best_i, best_j);

        return true;
    }
   
    return false;
}

bool bestImprovementReinsertion(Solution *s, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    double bestDelta = 0;
    int best_i, best_j;
    Subsequence sigma3;

    int n = s->sequencia.size() -1;

    for(int i = 1; i< n; i++){
      for(int j = 1; j< n; j++){
        if(j > i){
            Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+1][j], data);
            Subsequence sigma2 = Subsequence::Concatenate(sigma1, subseq_matrix[i][i], data);

            sigma3 = Subsequence::Concatenate(sigma2, subseq_matrix[j+1][n], data);

        } else if (i > j) {
            Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[j][i-1], subseq_matrix[i+1][n], data);
            Subsequence sigma2 = Subsequence::Concatenate(subseq_matrix[i][i], sigma1, data);

            sigma3 = Subsequence::Concatenate(subseq_matrix[0][j-1], sigma2, data);
        }

        double delta = sigma3.C - subseq_matrix[0][n].C;

        if(delta < bestDelta && i != j && i != j+1){
            bestDelta = delta;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    if(bestDelta < 0){
        int i_best = s->sequencia[best_i];
        s->sequencia.erase(s->sequencia.begin()+best_i);
        s->sequencia.insert(s->sequencia.begin()+ best_j, i_best);

        if(best_i > best_j){
            UpdateIndexSubseq(s, subseq_matrix, data, best_j, best_i);
        } else UpdateIndexSubseq(s, subseq_matrix, data, best_i, best_j);

        return true;
    }
    return false;
}

bool bestImprovementReinsertion2(Solution *s, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    double bestDelta = 0;
    int best_i, best_j;

    int n = s->sequencia.size() -1;


    for(int i = 2; i < n-1; i++){
      for(int j = i-1; j > 0; j--){
        Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[j][i-1], subseq_matrix[i+2][n], data);
        Subsequence sigma2 = Subsequence::Concatenate(subseq_matrix[i][i+1], sigma1, data);

        Subsequence sigma3 = Subsequence::Concatenate(subseq_matrix[0][j-1], sigma2, data);

        double delta = sigma3.C - subseq_matrix[0][n].C;

        if(delta < bestDelta){
            bestDelta = delta;
            best_i = i;
            best_j = j;
        }
      }
    }

    for(int i= 1; i< n-1; i++){
      for(int j= i+2; j< n; j++){
        Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+2][j], data);
        Subsequence sigma2 = Subsequence::Concatenate(sigma1, subseq_matrix[i][i+1], data);

        Subsequence sigma3 = Subsequence::Concatenate(sigma2, subseq_matrix[j+1][n], data);

        double delta = sigma3.C - subseq_matrix[0][n].C;

        if(delta < bestDelta){
            bestDelta = delta;
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

        if(best_i > best_j){
            s->sequencia.insert(s->sequencia.begin()+ best_j, i2_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j, i1_best);
        } else {
            s->sequencia.insert(s->sequencia.begin()+ best_j-1, i2_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j-1, i1_best);
        }

        if(best_i > best_j){
            UpdateIndexSubseq(s, subseq_matrix, data, best_j, best_i+1);
        } else UpdateIndexSubseq(s, subseq_matrix, data, best_i, best_j);

        return true;
    }
    return false;
}

bool bestImprovementReinsertion3(Solution *s, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    double bestDelta = 0;
    int best_i, best_j;

    int n = s->sequencia.size() -1;

    for(int i=1; i< n-2; i++){
      for(int j =i+3; j< n; j++){
        Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[0][i-1], subseq_matrix[i+3][j], data);
        Subsequence sigma2 = Subsequence::Concatenate(sigma1, subseq_matrix[i][i+2], data);

        Subsequence sigma3 = Subsequence::Concatenate(sigma2, subseq_matrix[j+1][n], data);

        double delta = sigma3.C - subseq_matrix[0][n].C;

        if(delta < bestDelta){
            bestDelta = delta;
            best_i = i;
            best_j = j;
        }
      }
    
    }

    for(int i = 3; i < n-2; i++){
      for(int j = i-2; j > 0; j--){
        Subsequence sigma1 = Subsequence::Concatenate(subseq_matrix[j][i-1], subseq_matrix[i+3][n], data);
        Subsequence sigma2 = Subsequence::Concatenate(subseq_matrix[i][i+2], sigma1, data);

        Subsequence sigma3 = Subsequence::Concatenate(subseq_matrix[0][j-1], sigma2, data);

        double delta = sigma3.C - subseq_matrix[0][n].C;

        if(delta < bestDelta){
            bestDelta = delta;
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

        if(best_i > best_j){
            s->sequencia.insert(s->sequencia.begin()+ best_j, i3_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j, i2_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j, i1_best);
        } else {
            s->sequencia.insert(s->sequencia.begin()+ best_j-2, i3_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j-2, i2_best);
            s->sequencia.insert(s->sequencia.begin()+ best_j-2, i1_best);

        }

        if(best_i > best_j){
            UpdateIndexSubseq(s, subseq_matrix, data, best_j, best_i+2);
        } else UpdateIndexSubseq(s, subseq_matrix, data, best_i, best_j);

        return true;
    }
    return false;
}

void BuscaLocal(Solution& s, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    vector<int> n = {1, 2, 3, 4, 5};

    bool improved = false;

    while(!n.empty()){
        int z = rand() % n.size();
        switch (n[z]){
            case 1:
                improved = bestImprovementSwap(&s, data, subseq_matrix);
                break;
            case 2:
                improved = bestImprovement20pt(&s, data, subseq_matrix);
                break;
            case 3:
                improved = bestImprovementReinsertion(&s, data, subseq_matrix);
                break;
            case 4:
                improved = bestImprovementReinsertion2(&s, data, subseq_matrix);
                break;
            case 5:
                improved = bestImprovementReinsertion3(&s, data, subseq_matrix);
                break;
        }

        if(improved){
            n = {1, 2, 3, 4, 5};
        }
        else 
            n.erase(n.begin() + z);
    }

}

void Pertubacao(Solution *s, vector<vector<Subsequence>>& subseq_matrix, Data& data){
    s->sequencia.erase(s->sequencia.begin());
    s->sequencia.erase(s->sequencia.begin()+s->sequencia.size()-1);

    int t = (s->sequencia.size()+1)/10;
    int tOne, tTwo;

    int segmentOneFirst, segmentTwoFirst = -1, insertSegmentTwo, insertSegmentOne;
    vector<int> segmentOne = {}, segmentTwo ={};

    if(t > 2){
        tOne = rand() % (t - 1) + 2;
        tTwo = rand() % (t - 1) + 2;
    }
    else{
        tOne = tTwo = 2;
    }

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

Solution SolutionILS(int maxIter, int maxIterIls, Data& data, vector<vector<Subsequence>>& subseq_matrix){
    Solution bestOfAll;
    bestOfAll.valorObj = INFINITY;
    Solution s;

    size_t n = data.getDimension();

    vector<vector<Subsequence>> subseq_matrix_s = vector<vector<Subsequence>>(n+1, vector<Subsequence>(n+1));

    for(int i =0; i < maxIter; i++){
        s = {{}, 0};

        Construction(&s, data);
        UpdateAllSubseq(&s, subseq_matrix_s, data);
        registerSolutionCost(&s, subseq_matrix_s, data);

        Solution best = s;

        int iterIls = 0;

        while(iterIls <= maxIterIls){
            BuscaLocal(s, data, subseq_matrix_s);
            registerSolutionCost(&s, subseq_matrix_s, data);
            if(s.valorObj < best.valorObj){
                best = s;
                iterIls = 0;
            }

            Pertubacao(&s, subseq_matrix_s, data);
            UpdateAllSubseq(&s, subseq_matrix_s, data);
            registerSolutionCost(&s, subseq_matrix_s, data);
           
            iterIls++;
        }

        if(best.valorObj < bestOfAll.valorObj){
            bestOfAll = best;  
        }
    }

    return bestOfAll;
}


int main(int argc, char** argv) {
    clock_t start= clock();
    srand((unsigned)time(NULL));

    double media =0;

    auto data = Data(argc, argv[1]);
    data.read();
    size_t n = data.getDimension();
    
    Solution sLinha;
    vector<vector<Subsequence>> subseq_matrix = vector<vector<Subsequence>>(n+1, vector<Subsequence>(n+1));


    int maxIterIls = data.getDimension() < 100 ? data.getDimension() : 100;

    for(int i =0; i<10;i++){
        Solution bestSolution = SolutionILS(10, maxIterIls, data, subseq_matrix);
        UpdateAllSubseq(&bestSolution, subseq_matrix, data);
        registerSolutionCost(&bestSolution, subseq_matrix, data);

        media += bestSolution.valorObj;
    }

    clock_t end= clock();
    double time= ((double) (end - start)) / CLOCKS_PER_SEC;

    cout << "Custo: " << media/10 << "  Tempo: " << time/10 << endl; 


    return 0;
}




