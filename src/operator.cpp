#include "relation.h"
#include "operator.h"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>
#include <chrono>

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
void Operator::trieToRelation(vector<row>* relation, vector<int> curRow, hNode* curNode){
    // base case
    if(curNode->value != 0){
        row r;
        r.attr = curRow;
        r.value = curNode->value;

        relation->push_back(r);

        return;
    }

    // iterate through the hashmap in the node
    unordered_map<int,hNode*>::iterator it;
    for (it = curNode->children.begin(); it != curNode->children.end(); it++)
    {
        // copy a new vector (as the old one will be used for next value)
        vector<int> next = curRow;
        int att = it->first;
        next.push_back(att);
        trieToRelation(relation, next, it->second);
        
    }
}




// first build multi hash, then to relation
// use generalized project instead
Relation* Operator::marginalize(Relation* input, unordered_set<string> attrs)
{
    Relation* output = new Relation();
    // indics of attributes not marginalized
    unordered_set<int> indics;

    // check attrs in the input relaiton
    for(unsigned int i = 0; i < input->schema.size(); i++){
        // get each attribute in the input relaiton
        string cur_attr = input->schema[i];

        // if not in the marginalization list
        if(attrs.find(cur_attr) == attrs.end()){
            // add not marginalized attrs to new relation
            output->schema.push_back(cur_attr);
            // add its index
            indics.insert(i);
        }
    }

    // build multi level hash map
    hNode* root = new hNode();
    // for each row in the input relaiton
    for(row r: *input->relation){
        // start with root node
        hNode* cur = root;

        // for each attribute value in this row
        for(unsigned int i = 0; i < r.attr.size(); i++){
            // check if current attribute is marginalized (not found in indics)
            if(indics.find(i) == indics.end()){
                // skip marginalized attributes
                continue;
            }

            // if not marginalized, get the current attribute value
            int cur_attr = r.attr[i];
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
        cur->value += r.value;
        // cout << "value: " << cur->value <<"\n";
    }
    
    vector<int> empty;
    // build relation from the built multi-level map
    trieToRelation(output->relation, empty, root);
    return output;

}



hNode* Operator::buildHashNode(Relation* input, vector<int> idx){
    // build multi level hash map
    hNode* root = new hNode();
    // for each row in the input relaiton
    for(row r: *input->relation){
        // start with root node
        hNode* cur = root;

        // for each attribute value in this row
        for(int i: idx){

            // get the current attribute value
            int cur_attr = r.attr[i];

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
        cur->value += r.value;
        // cout << "value: " << cur->value <<"\n";
    }
    return root;
}

// attrs must follow the global order
Relation* Operator::generializedProject(Relation* input, vector<string> attrs){

    Relation* output = new Relation();
    output->orderedAttr = attrs;

    vector<int> idx;

    // iterate through ordered global attributes 
    for(string curAtt: attrs){
        int curIdx = input->getAttIdx(curAtt);
        // if found this attribute
        if(curIdx != -1){
            idx.push_back(curIdx);
        }
    }

    output->trie = buildHashNode(input, idx);

    return output;
}

// given the nodes, build the row, and add the row to relation
void nodesJoinToRelation(vector<row>* output, vector<int> curRow, vector<hNode*> curNodes, vector<int> levels, 
                        int curAttId, int maxAttId, vector<string> ordered_attributes, vector<vector<string>> attrs){

    // base case
    if(curAttId >= maxAttId){
        row r;
        r.attr = curRow;

        int value = 1;
        for(hNode* curNode: curNodes){
            value = value * curNode->value;
        }

        r.value = value;
        output->push_back(r);
        return;
    }

    // current attributes to join
    string curAtt = ordered_attributes[curAttId];

    // find the relation which contains this attribute, and has the smallest size
    bool first = true;
    hNode* node;
    unsigned int size;

    for(unsigned int i = 0; i < curNodes.size(); i++){
        // if not this attribute, skip to next
        if(attrs[i][levels[i]] != curAtt){
            continue;
        }
        
        // check if this nodes is smaller
        hNode* curNode = curNodes[i];
        if(first || (curNode->children.size() < size)){
            first = false;
            size = curNode->children.size();
            node = curNode;
        }
    }


    // iterate through all the attribute value in the smallest relation
    unordered_map<int,hNode*>::iterator it;
    for (it = node->children.begin(); it != node->children.end(); it++)
    {
        int attrValue = it->first;

        // for all relations with this attribute, check if they all contain this attribute value
        bool equal = true;

        for(unsigned int i = 0; i < curNodes.size(); i++){
            // if not this attribute, skip to next
            if(attrs[i][levels[i]] != curAtt){
                continue;
            }

            // check if contains this attribute
            hNode* curNode = curNodes[i];
            if(curNode->children.find(attrValue) == curNode->children.end()){
                // if not contain, stop the loop
                equal = false;
                break;
            }
        }

        // don't satisfy the join condition
        // go to the next attribute value
        if(!equal){
            continue;
        }

        // otherwise, go to the deeper attribute
        // copy a new vector (as the old one will be used for next value)
        vector<int> nextRow = curRow;
        nextRow.push_back(it->first);

        // for relation with this attribute, node goes to next level (update level and nodes)
        vector<hNode*> nextNode = curNodes;
        vector<int> nextlevels = levels;

        for(unsigned int i = 0; i < curNodes.size(); i++){
            // if not this attribute, skip to next
            if(attrs[i][levels[i]] != curAtt){
                continue;
            }

            nextlevels[i] = nextlevels[i] + 1;
            nextNode[i] = nextNode[i]->children[attrValue];
        }

        nodesJoinToRelation(output, nextRow, nextNode,nextlevels, curAttId + 1, maxAttId, ordered_attributes, attrs);
        
    }

}

bool sortByVal(const pair<string, int> &a, 
               const pair<string, int> &b) 
{ 
    return (a.second > b.second); 
} 


Relation* Operator::join(vector<Relation*> relations, vector<string> ordered_attributes){

    
    if(relations.size() == 1){
        return relations[0];
    }


    Relation* output = new Relation();
    output->schema = ordered_attributes;
    


    // root hash node for each relation
    vector<hNode*> nodes;
    // for each relation, ordered attrs (wrt global order) and their indics to original order
    vector<vector<string>> attrs;
    vector<vector<int>> attrsIdx;
    // for each relation, current level (used for join later)
    vector<int> levels;
    
    for(Relation* relation: relations){

        attrs.push_back(relation->orderedAttr);
        attrsIdx.push_back(relation->idx);

        if (relation->trie == NULL){
            cout << "Trie hasn't been built for the relation\n"; 
            exit(1);
        }

        nodes.push_back(relation->trie);
        levels.push_back(0);
    }

    vector<int> empty;
    nodesJoinToRelation(output->relation, empty, nodes, levels, 0, (int)ordered_attributes.size(), ordered_attributes, attrs);

    return output;

}