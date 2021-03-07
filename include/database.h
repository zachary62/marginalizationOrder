#pragma once
#include <relation.h>
#include <string>
#include <vector>
#include <unordered_map>


class Database
{
public:
    Database(std::string dir);
    ~Database();

    vector<Relation*>* relations;
    

};

