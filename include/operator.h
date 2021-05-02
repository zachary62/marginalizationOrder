#pragma once
#include "relation.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;



class Operator
{
public:
    Operator();
    ~Operator();
    hNode* buildHashNode(Relation* input, vector<int> idx);
    Relation* marginalize(Relation* R, unordered_set<string> attrs);
    Relation* join(vector<Relation*> relations, vector<string> ordered_attributes);
    Relation* generializedProject(Relation* input, vector<string> attrs);
    void trieToRelation(vector<row>* relation, vector<string> curRow, hNode* curNode);

};

