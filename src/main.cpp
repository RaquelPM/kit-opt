#include <iostream>
#include "Data.h"
#include "MasterProblem.h"
#include "Subproblem.h"
#include <cmath>

using namespace std;

int main(int argc, char** argv){
    Data data(argv[1]);

    MasterProblem master(&data);
    master.solve();

    Subproblem sub(&data);
    sub.attObjective(master.duals);

    double subOpt = sub.solve();

    double bins;

    while(1 - subOpt < 0){
        master.addColumn(sub.solution);
        master.solve();

        sub.attObjective(master.duals);
        subOpt = sub.solve();
    }

    cout << ceil(master.solve()) << endl;

    return 0;
}