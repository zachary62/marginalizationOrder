#include "relation.h"


#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>


static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ',';

static const bool VERBOSE = true;

using namespace std;

Relation::Relation()
{ 
    schema = new vector<string*>;
    relation = new vector<row*>;
}

Relation::Relation(string dir)
{   
    ifstream input(dir);
    string line;
    stringstream ssLine;

    if (!input)
	{
		cout<< dir << "table" <<" does not exist. \n";
		exit(1);
	}

    // get the schema
    getline(input, line);
    ssLine << line;

    schema = new vector<string*>;
    string temp;
    
    // read each attribute name
    while(getline(ssLine, temp, PARAMETER_SEPARATOR_CHAR)){
        string * att = new string(temp);
        schema->push_back(att);
    }
    ssLine.clear();

    // read relation
    relation = new vector<row*>;

    while(getline(input, line)){
        ssLine << line;
        row* r = new row;
        r->attr = new vector<string*>;
        r->value = 1;
        // read each attribute value
        while(getline(ssLine, temp, PARAMETER_SEPARATOR_CHAR)){
            string * att = new string(temp);
            r->attr->push_back(att);
        }

        if (r->attr->size() != schema->size())
        {
            cout<< dir << " Different number of attributes. \n";
            exit(1);
        }
        relation->push_back(r);
        ssLine.clear();
    }
}

Relation::~Relation()
{   
    for(string* s: *schema){
        delete s;
    }
    delete schema;
    for(row* r: *relation){
        for(string* s: *r->attr){
            delete s;
        }
        delete r;
    }
    delete relation;
}

void Relation::print()
{   
    for(string* s: *schema){
        cout << *s <<",";
    }
    cout << "\n";
    for(row* r: *relation){
        for(string* s: *r->attr){
            cout << *s <<",";
        }
        cout << r->value;
        cout << "\n";
    }
}

