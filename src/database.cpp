#include "database.h"

#include <iostream>
#include <unordered_map>
#include <map>
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

bool sortByDg(const pair<string, int> &a, 
               const pair<string, int> &b) 
{ 
    return (a.second > b.second); 
} 

void Database::GetNodeOrder(){

    // get all attributes without duplication
    // join attribute first
    map<string,int> attributes;
    for(Relation* relation: relations){
        for(string att: relation->schema){
            if (attributes.find(att) == attributes.end()) {
                attributes[att] = 1;
            }
            else{
                attributes[att] ++;
            }
        }
    }

        // sort the attribute by degree

    // create a empty vector of pairs
	vector<pair<string, int>> vec;

    // copy key-value pairs from the map to the vector
    map<string, int> :: iterator it2;
    for (it2=attributes.begin(); it2!=attributes.end(); it2++) 
    {
        vec.push_back(make_pair(it2->first, it2->second));
    }

    // // sort the vector by increasing order of its pair's second value
    sort(vec.begin(), vec.end(), sortByDg); 

    for (auto& p: vec){
        nodeOrder.push_back(p.first);
        // cout << p.first <<"\n";
    }
}



vector<string> Database::orderAttrs(vector<string> attrs){
    unordered_set<string> attrs_set;

    for(string attr: attrs){
        attrs_set.insert(attr);
    }

    vector<string> result;

    for(string attr: nodeOrder){
        if(attrs_set.find(attr) != attrs_set.end()){
            result.push_back(attr);
        }
    }

    return result;
}



void Database::buildTrie(Relation* relation){
    relation->orderedAttr = orderAttrs(relation->schema);
    relation->buildIdx();
    
    Operator op;
    relation->trie = op.buildHashNode(relation, relation->idx);
}

void Database::Preprocess(){
    GetNodeOrder();
    
    for(Relation* relation: relations){
        buildTrie(relation);
    }
}

Relation*  Database::join(vector<Relation*> relations){
    Operator op;

    unordered_set<string> attrSet;
    for(Relation* r: relations){
        for(string attr: r->schema){
            attrSet.insert(attr);
        }
    }

    vector<string> attrVec;
    for(string attr: attrSet){
        attrVec.push_back(attr);
    }

    Relation* joinResult = op.join(relations, orderAttrs(attrVec));
    buildTrie(joinResult);
    return joinResult;
}

Relation*  Database::marginalize(Relation* relation, string attr){

    vector<string> newAttrs = relation->orderedAttr;
    int remove = -1;
    for(int i = 0; i < (int)newAttrs.size(); i++){
        if(newAttrs[i] == attr){
            remove = i;
            break;
        }
    }

    if(remove == -1){
        cout << "Can't find the attribute in the relation to marginalize!\n"; 
        exit(1);
    }

    newAttrs.erase(newAttrs.begin() + remove);

    Operator op;
    Relation* margResult = op.generializedProject(relation, newAttrs);

    vector<int> empty;
    // build relation from the built multi-level map
    op.trieToRelation(margResult->relation, empty, margResult->trie);

    margResult->schema = newAttrs;
    margResult->buildIdx();

    return margResult;
}

void Database::eliminate(string attribute){

    // indices of relations 
    vector<int> idx;

    for(unsigned int i = 0; i < this->relations.size(); i++){
        
        Relation* relation = this->relations[i];
        
        // check if this relation contains attribute
        if(find(relation->schema.begin(), relation->schema.end(), attribute) != relation->schema.end()){
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
    Relation* joinResult = join(ToJoin);

    // cout << joinResult->size() << "\n";


    Relation* margResult = marginalize(joinResult, attribute);
    // margResult->print();
    cout << margResult->size() << "\n";


    // for(Relation* r: ToJoin){
    //     delete r;
    // }

    // if(ToJoin.size() > 1){
    //     delete joinResult;
    // }

    this->relations.push_back(margResult);


}