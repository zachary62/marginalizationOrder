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

    return project(relation, newAttrs);
}

Relation*  Database::project(Relation* relation, vector<string> newAttrs){
    Operator op;
    Relation* margResult = op.generializedProject(relation, newAttrs);

    vector<int> empty;
    // build relation from the built multi-level map
    op.trieToRelation(margResult->relation, empty, margResult->trie);

    margResult->schema = newAttrs;
    margResult->buildIdx();
    return margResult;
}


unordered_set<string> intersection(unordered_set<string> a, vector<string> b){
    unordered_set<string> result;
    for(string s : b){
        if(a.find(s) != a.end()){
            result.insert(s);
        }
    }
    return result;

}

// given attribute, return the indices of all relations with this attribute
vector<int> Database::findRelatedRelations(string attribute){
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
    return idx;
}

void Database::computeAllStats(){
    for(string s: nodeOrder){
        computeStats(s);
    }
    // cout<< nodeOrder[0] <<"\n";
    // computeStats(nodeOrder[0]);

}

void Database::computeStats(string attribute){
    // indices of relations 
    vector<int> idx = findRelatedRelations(attribute);

    vector<Relation*> ToJoin;

    // add these relations to the join list
    for(int i = idx.size() - 1; i >= 0; i--){
        Relation* relation = this->relations[idx[i]];
        ToJoin.push_back(relation);
    }

    // get the set of all attributes in ToJoin
    unordered_set<string> attrs;
    for(Relation* r : ToJoin){
        for(string attr: r->schema){
            attrs.insert(attr);
        }
    }

    
    // inside-outside algorithm
    // prebuild projection
    // find relations whose schema intersects with related relations
    
    for(unsigned int i = 0; i < this->relations.size(); i++){
        
        Relation* r = this->relations[i];

        if(find(idx.begin(), idx.end(), (int)i) != idx.end()){
            // cout<<"skip because already in toJoin\n";
            continue;
        }

        unordered_set<string> inters = intersection(attrs, r->schema);
        if(inters.size() == 0){
            continue;
        }

        // cout << r->id <<"intersects!\n";
        vector<string> inters_vec(inters.begin(), inters.end());
        vector<string> ordered_attrs = orderAttrs(inters_vec);

        // get projection
        string hash = "";
        hash.append(to_string(r->id));
        hash.append(":");
        for(string s: ordered_attrs){
            hash.append(s);
            hash.append(",");
        }
        // cout<<hash<<"\n";

        if(cached_projection.find(hash) != cached_projection.end()){
            // cout<<"hashed!\n";
        }else{
            Relation* projection = project(r, ordered_attrs);
            cached_projection[hash] = projection;
            
        }
        ToJoin.push_back(cached_projection[hash]);
    }

}




void Database::eliminate(string attribute){

    cout << attribute <<"\n";
    // indices of relations 
    vector<int> idx = findRelatedRelations(attribute);

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

    // get the set of all attributes in ToJoin
    unordered_set<string> attrs;
    for(Relation* r : ToJoin){
        for(string attr: r->schema){
            attrs.insert(attr);
        }
    }

    // inside-outside algorithm
    // find relations whose schema intersects with related relations
    for(Relation* r: this->relations){
        unordered_set<string> inters = intersection(attrs, r->schema);
        if(inters.size() == 0){
            continue;
        }

        // cout << r->id <<"intersects!\n";
        vector<string> inters_vec(inters.begin(), inters.end());
        vector<string> ordered_attrs = orderAttrs(inters_vec);

        // get projection
        string hash = "";
        hash.append(to_string(r->id));
        hash.append(":");
        for(string s: ordered_attrs){
            hash.append(s);
            hash.append(",");
        }
        // cout<<hash<<"\n";

        if(cached_projection.find(hash) != cached_projection.end()){
            // cout<<"hashed!\n";
        }else{
            Relation* projection = project(r, ordered_attrs);
            cached_projection[hash] = projection;
            
        }
        ToJoin.push_back(cached_projection[hash]);
    }


    Operator op;
    Relation* joinResult = join(ToJoin);

    cout << joinResult->size() << "\n";
    // if(attribute == "theme_id"){
    //     joinResult->print();
    // }


    Relation* margResult = marginalize(joinResult, attribute);
    // margResult->print();
    cout << margResult->size() << "\n";
    // if(attribute == "theme_id"){
    //     margResult->print();
    // }

    // for(Relation* r: ToJoin){
    //     delete r;
    // }

    // if(ToJoin.size() > 1){
    //     delete joinResult;
    // }

    this->relations.push_back(margResult);


}


void Database::Preprocess(){
    GetNodeOrder();
    
    for(Relation* relation: relations){
        buildTrie(relation);
    }

    computeAllStats();
}