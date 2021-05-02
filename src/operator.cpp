#include "relation.h"
#include "operator.h"

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iostream>
#include <algorithm>

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
void nodeToRelation(vector<row>* relation, vector<string> curRow, hNode* curNode){
    // base case
    if(curNode->value != 0){
        row r;
        r.attr = curRow;
        r.value = curNode->value;

        relation->push_back(r);

        // the node is no longer used
        delete curNode;
        return;
    }

    // iterate through the hashmap in the node
    unordered_map<string,hNode*>::iterator it;
    for (it = curNode->children.begin(); it != curNode->children.end(); it++)
    {
        // copy a new vector (as the old one will be used for next value)
        vector<string> next = curRow;
        string att = it->first;
        next.push_back(att);
        nodeToRelation(relation, next, it->second);
        
    }
    // the node is no longer used
    delete curNode;
}



Relation* Operator::marginalize(Relation* input, unordered_set<string> attrs)
{
    Relation* output = new Relation();
    // indics of attributes not marginalized
    unordered_set<int> indics;

    // check attrs in the input relaiton
    for(unsigned int i = 0; i < input->schema->size(); i++){
        // get each attribute in the input relaiton
        string cur_attr = input->schema->at(i);

        // if not in the marginalization list
        if(attrs.find(cur_attr) == attrs.end()){
            // add not marginalized attrs to new relation
            output->schema->push_back(cur_attr);
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
            string cur_attr = r.attr[i];
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
    
    vector<string> empty;
    // build relation from the built multi-level map
    nodeToRelation(output->relation, empty, root);

    return output;

}

// get index of attributes in the schema
// return -1 if not existed
int getAttIdx(vector<string>* schema, string att){
    for(unsigned int i = 0; i < schema->size(); i++){
        if(schema->at(i) == att){
            return i;
        }
    }
    return -1;
}

hNode* buildHashNode(Relation* input, vector<int> idx){
    // build multi level hash map
    hNode* root = new hNode();
    // for each row in the input relaiton
    for(row r: *input->relation){
        // start with root node
        hNode* cur = root;

        // for each attribute value in this row
        for(int i: idx){

            // get the current attribute value
            string cur_attr = r.attr[i];

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


// given the nodes, build the row, and add the row to relation
void nodesJoinToRelation(vector<row>* output, vector<string> curRow, vector<hNode*> curNodes, vector<int> levels, 
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
    unordered_map<string,hNode*>::iterator it;
    for (it = node->children.begin(); it != node->children.end(); it++)
    {
        string attrValue = it->first;

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
        vector<string> nextRow = curRow;
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

// release the memory after join
void deleteNode(hNode* curNode){
    // base case
    if(curNode->value != 0){
        // the node is no longer used
        delete &curNode;
        return;
    }

    // iterate through the hashmap in the node
    unordered_map<string,hNode*>::iterator it;
    for (it = curNode->children.begin(); it != curNode->children.end(); it++)
    {
        deleteNode(it->second);
    }
    // the node is no longer used
    delete &curNode;
}


Relation* Operator::join(vector<Relation*> relations){


    if(relations.size() == 1){
        return relations[0];
    }

    Relation* output = new Relation();

    // define a global attribute order for join

    // get all attributes without duplication
    // join attribute first
    map<string,int> attributes;
    for(Relation* relation: relations){
        for(string att: *relation->schema){
            if (attributes.find(att) == attributes.end()) {
                attributes[att] = 1;
            }
            else{
                attributes[att] ++;
            }
        }
    }

    // create a empty vector of pairs
	vector<pair<string, int>> vec;

    // copy key-value pairs from the map to the vector
    map<string, int> :: iterator it2;
    for (it2=attributes.begin(); it2!=attributes.end(); it2++) 
    {
        vec.push_back(make_pair(it2->first, it2->second));
    }

    // // sort the vector by increasing order of its pair's second value
    sort(vec.begin(), vec.end(), sortByVal); 

    // find some global order
    vector<string> ordered_attributes;

    for (auto& p: vec){
        ordered_attributes.push_back(p.first);
    }

    for(string att: ordered_attributes){
        // cout<<att<<"\n";
        output->schema->push_back(att);
    }
    
    
    // build hash index for each relation

    // root hash node for each relation
    vector<hNode*> nodes;
    // for each relation, ordered attrs (wrt global order) and their indics to original order
    vector<vector<string>> attrs;
    vector<vector<int>> attrsIdx;
    // for each relation, current level (used for join later)
    vector<int> levels;
    
    for(Relation* relation: relations){
        // attribute and indices for this relation
        vector<string> attr;
        vector<int> idx;

        // iterate through ordered global attributes 
        for(string curAtt: ordered_attributes){
            int curIdx = getAttIdx(relation->schema, curAtt);
            // if found this attribute
            if(curIdx != -1){
                attr.push_back(curAtt);
                idx.push_back(curIdx);
            }
        }

        // for debug only
        // for(string s: attr){
        //     cout<< s << " ";
        // }
        // cout<< "\n";

        // for(int s: idx){
        //     cout<< s << " ";
        // }
        // cout<< "\n";

        attrs.push_back(attr);
        attrsIdx.push_back(idx);

        hNode* root = buildHashNode(relation,idx);
        nodes.push_back(root);
        levels.push_back(0);
    }
    
    vector<string> empty;
    nodesJoinToRelation(output->relation, empty, nodes, levels, 0, (int )ordered_attributes.size(), ordered_attributes, attrs);
    
    for(hNode* node:nodes){
        deleteNode(node);
    }

    return output;

}