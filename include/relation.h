#pragma once

#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct row
{
  vector<string> attr;
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
    int size(){
      return relation->size();
    };

    vector<row>* relation;
    // attribute names
    vector<string>* schema;

};

