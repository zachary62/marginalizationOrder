#pragma once
#include <relation.h>
#include <string>
#include <vector>
#include <unordered_map>


class Database
{
public:
    Database(std::string dir);
    Database(vector<Relation*> relations);

    // Order by Degree as recommended in EmptyHeaded
    void GetNodeOrder();
    // Order the given attrs based on node order
    vector<string> orderAttrs(vector<string> attrs);

    void buildTrie(Relation* relation);

    // get the basic stats and build trie for each relation,
    void Preprocess();

    // join and build trie for the join result
    Relation* join(vector<Relation*> relations);

    // join and build trie for the join result
    Relation* marginalize(Relation* relation, string attr);

    ~Database();

    void eliminate(string attribute);


    vector<Relation*> relations;
    vector<string> nodeOrder;
};

