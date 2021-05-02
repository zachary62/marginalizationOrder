#include "relation.h"


#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <fstream>
#include <sstream>


// based on https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c

static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ',';

static const bool VERBOSE = true;

using namespace std;

Relation::Relation()
{ 
    schema = new vector<string>;
    relation = new vector<row>;
}

enum class CSVState {
    UnquotedField,
    QuotedField,
    QuotedQuote
};

row readCSVRow(const string &line) {
    CSVState state = CSVState::UnquotedField;
    row r;
    r.value = 1;
    r.attr.push_back("");
    size_t i = 0; // index of the current field
    for (char c : line) {
        switch (state) {
            case CSVState::UnquotedField:
                switch (c) {
                    case ',': // end of field
                              r.attr.push_back(""); i++;
                              break;
                    case '"': state = CSVState::QuotedField;
                              break;
                    default:  r.attr[i].push_back(c);
                              break; }
                break;
            case CSVState::QuotedField:
                switch (c) {
                    case '"': state = CSVState::QuotedQuote;
                              break;
                    default:  r.attr[i].push_back(c);
                              break; }
                break;
            case CSVState::QuotedQuote:
                switch (c) {
                    case ',': // , after closing quote
                              r.attr.push_back(""); i++;
                              state = CSVState::UnquotedField;
                              break;
                    case '"': // "" -> "
                              r.attr[i].push_back('"');
                              state = CSVState::QuotedField;
                              break;
                    default:  // end of quote
                              state = CSVState::UnquotedField;
                              break; }
                break;
        }
    }
    return r;
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

    schema = new vector<string>;
    string temp;    

    // read each attribute name
    while(getline(ssLine, temp, PARAMETER_SEPARATOR_CHAR)){
        schema->push_back(temp);
    }
    ssLine.clear();
    
    // read relation
    relation = new vector<row>;

    while(getline(input, line)){

        if (line[0] == COMMENT_CHAR || line == "")
                continue;
        
        row r = readCSVRow(line);

        if (r.attr.size() != schema->size())
        {
            cout<< dir << " Different number of attributes. \n";
            exit(1);
        }
        
        relation->push_back(r);
    }
}



Relation::~Relation()
{   
    // for(string* s: *schema){
    //     delete s;
    // }
    delete schema;
    // for(row* r: *relation){
    //     for(string* s: *r->attr){
    //         delete s;
    //     }
    //     delete r;
    // }
    delete relation;
}

void Relation::print()
{   
    for(string s: *schema){
        cout << s <<",";
    }
    cout << "\n";
    for(row r: *relation){
        for(string s: r.attr){
            cout << s <<",";
        }
        cout << r.value;
        cout << "\n";
    }
}

