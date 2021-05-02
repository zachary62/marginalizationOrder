#include "database.h"

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "operator.h"

using namespace std;

Database::Database(vector<Relation*> relations)
{  
    this->relations = relations;
}

Database::~Database()
{   

}

void Database::eliminate(string attribute){

    // indices of relations 
    vector<int> idx;

    for(unsigned int i = 0; i < this->relations.size(); i++){
        Relation* relation = this->relations[i];
        // check if this relation contains attribute
        if(find(relation->schema->begin(), relation->schema->end(), attribute) != relation->schema->end()){
            // if contains
            idx.push_back(i);
        }
    }

    vector<Relation*> ToJoin;

    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        ToJoin.push_back(relation);
    }


    // remove these relations from original relations
    for(int i = idx.size() - 1; i >= 0; i--){
        this->relations.erase(this->relations.begin() + idx[i]);
    }

    Operator op;
    Relation* joinResult = op.join(ToJoin);
    // cout << joinResult->size() << "\n";

    unordered_set<string> set;
    set.insert(attribute);
    Relation* margResult = op.marginalize(joinResult, set);
    cout << margResult->size() << "\n";
    this->relations.push_back(margResult);

    


}