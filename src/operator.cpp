#include "relation.h"
#include "operator.h"

#include <unordered_map>
#include <unordered_set>
#include <iostream>



static const char COMMENT_CHAR = '#';
static const char PARAMETER_SEPARATOR_CHAR = ',';

static const bool VERBOSE = true;

using namespace std;

Operator::Operator()
{   
}

Operator::~Operator()
{   

}

// helper function to marginalize
// given the node, build the row, and add the row to relation
void nodeToRelation(vector<row*>* relation, vector<string*> curRow, hNode* curNode){
    // base case
    if(curNode->value != 0){
        row* r = new row;
        r->attr = new vector<string*>(curRow);
        r->value = curNode->value;
        relation->push_back(r);

        // the node is no longer used
        delete &curNode;
        return;
    }

    // iterate through the hashmap in the node
    unordered_map<string,hNode*>::iterator it;
    for (it = curNode->children.begin(); it != curNode->children.end(); it++)
    {
        // copy a new vector (as the old one will be used for next value)
        vector<string*> next = curRow;
        string * att = new string(it->first);
        next.push_back(att);
        nodeToRelation(relation, next, it->second);
        
    }
    // the node is no longer used
    delete &curNode;
}



Relation* Operator::marginalize(Relation* input, unordered_set<string> attrs)
{
    Relation* output = new Relation();
    // indics of attributes not marginalized
    unordered_set<int> indics;

    // check attrs in the input relaiton
    for(unsigned int i = 0; i < input->schema->size(); i++){
        // get each attribute in the input relaiton
        string cur_attr = *input->schema->at(i);

        // if not in the marginalization list
        if(attrs.find(cur_attr) == attrs.end()){
            string * att = new string(cur_attr);
            // add not marginalized attrs to new relation
            output->schema->push_back(att);
            // add its index
            indics.insert(i);
        }
    }

    // build multi level hash map
    hNode* root = new hNode();
    // for each row in the input relaiton
    for(row* r: *input->relation){
        // start with root node
        hNode* cur = root;

        // for each attribute value in this row
        for(unsigned int i = 0; i < r->attr->size(); i++){
            // check if current attribute is marginalized (not found in indics)
            if(indics.find(i) == indics.end()){
                // skip marginalized attributes
                continue;
            }

            // if not marginalized, get the current attribute value
            string cur_attr = *r->attr->at(i);
            // cout << cur_attr <<"\n";

            // check if previous node has been built
            if(cur->children.find(cur_attr) == cur->children.end()){
                // if not, add the hash node
                // cout << "not found" <<"\n";
                cur->children[cur_attr] = new hNode();
            }

            // move to the child node
            cur = cur->children[cur_attr];
        }

        // after iterating over all attribute values, cur points to the leaf node
        // calculate value for leaf node
        cur->value += r->value;
        // cout << "value: " << cur->value <<"\n";
    }
    
    vector<string*> empty;
    // build relation from the built multi-level map
    nodeToRelation(output->relation, empty, root);

    return output;

}

