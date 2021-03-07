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


void test_marginalization(){
    
}

int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);

    Relation* R = new Relation(directory);
    R->print();
    // delete R;
    Operator op;
    unordered_set<string> set;
    set.insert("A");
    Relation* R2 = op.marginalize(R, set);
    R2->print();


}


