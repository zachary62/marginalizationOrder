#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

// multi-level hash map
struct hNode{
    unordered_map<int,hNode*> children;
    // assume that leaf node has non-zero value
    int value = 0;
};

struct row
{
  vector<int> attr;
  // aggregation value
  int value;
};

class Relation
{
public:
    Relation();
    Relation(std::string dir);
    ~Relation();
    void print();
    void buildIdx();
    int getAttIdx(string att);
    int size(){
      return relation->size();
    };

    vector<row>* relation;
    // original attributes from the file
    vector<string> schema;


    hNode* trie;
    // ordered attr for the trie
    vector<string> orderedAttr;
    // idx of orderedAttr
    vector<int> idx;

    
};

