#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "relation.h"
#include "database.h"
#include "operator.h"
#include <unordered_set>

using namespace std;

// marg data/marg/R.csv
void test_marginalization(string directory){
    


    Relation* R = new Relation(directory);
    // R->print();
    // delete R;

    clock_t start;
    double duration;
    start = clock();

    Operator op;
    unordered_set<string> set;
    //marginalize more attributes is actually cheaper

    set.insert("A");
    set.insert("B");
    set.insert("C");
    set.insert("D");
    // set.insert("E");
    Relation* R2 = op.marginalize(R, set);

    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to marginalization: " << duration << "\n"; 
    // R2->print();
}

void join_example(){
    Relation* R = new Relation("data/small/R.csv");
    R->print();
    Relation* S = new Relation("data/small/S.csv");
    S->print();
    Relation* T = new Relation("data/small/T.csv");
    T->print();

    vector<Relation*> relations;
    relations.push_back(R);
    relations.push_back(S);
    relations.push_back(T);

    Operator op;
    Relation* R2 = op.join(relations);
    R2->print();
}

int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);
    test_marginalization(directory);
    // join_example();
    // cout<<CLOCKS_PER_SEC;

}


