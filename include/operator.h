#pragma once
#include "relation.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;



// multi-level hash map
struct hNode{
    unordered_map<string,hNode*> children;
    // assume that leaf node has non-zero value
    int value = 0;
};

class Operator
{
public:
    Operator();
    ~Operator();
    Relation* marginalize(Relation* R, unordered_set<string> attrs);

};

