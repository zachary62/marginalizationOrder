#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "relation.h"
#include "database.h"
#include "operator.h"
#include <unordered_set>

using namespace std;

// marg data/marg/R.csv
void test_marginalization(string directory){
    


    Relation* R = new Relation(directory);
    // R->print();
    // delete R;

    clock_t start;
    double duration;
    start = clock();

    Operator op;
    unordered_set<string> set;
    //marginalize more attributes is actually cheaper

    set.insert("A");
    set.insert("B");
    set.insert("C");
    set.insert("D");
    // set.insert("E");
    Relation* R2 = op.marginalize(R, set);

    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to marginalization: " << duration << "\n"; 
    // R2->print();
}

void join_example(){
    Relation* R = new Relation("data/small/R.csv");
    R->print();
    Relation* S = new Relation("data/small/S.csv");
    S->print();
    Relation* T = new Relation("data/small/T.csv");
    T->print();

    vector<Relation*> relations;
    relations.push_back(R);
    relations.push_back(S);
    relations.push_back(T);

    Operator op;
    Relation* R2 = op.join(relations);
    R2->print();
}

void logo_example(){
    Relation* colors = new Relation("data/lego/colors.csv");
    Relation* inventory_parts = new Relation("data/lego/inventory_parts.csv");
    Relation* inventory_sets = new Relation("data/lego/inventory_sets.csv");
    Relation* part_categories = new Relation("data/lego/part_categories.csv");
    Relation* parts = new Relation("data/lego/parts.csv");
    Relation* sets = new Relation("data/lego/sets.csv");
    Relation* themes = new Relation("data/lego/themes.csv");

    vector<Relation*> relations;
    relations.push_back(colors);
    relations.push_back(inventory_parts);
    relations.push_back(inventory_sets);
    relations.push_back(part_categories);
    relations.push_back(parts);
    relations.push_back(sets);
    relations.push_back(themes);

    clock_t start;
    double duration;
    start = clock();
    Database db(relations);

    // db.eliminate("part_category_name");
    // db.eliminate("color_name");
    // db.eliminate("rgb");
    // db.eliminate("is_trans");
    // db.eliminate("theme_name");
    // db.eliminate("parent_id");
    // db.eliminate("set_name");
    // db.eliminate("num_parts");
    // db.eliminate("theme_id");
    // db.eliminate("set_num");
    // db.eliminate("year");
    // db.eliminate("inventory_set_quantity");
    // db.eliminate("part_name");
    // db.eliminate("part_num");
    // db.eliminate("inventory_id");
    // db.eliminate("inventory_part_quantity");
    // db.eliminate("color_id");
    // db.eliminate("part_cat_id");
    // db.eliminate("is_spare");

    db.eliminate("part_category_name");
    db.eliminate("color_name");
    db.eliminate("rgb");
    db.eliminate("is_trans");
    db.eliminate("theme_name");
    db.eliminate("parent_id");
    db.eliminate("inventory_id");
    db.eliminate("color_id");
    db.eliminate("inventory_part_quantity");
    db.eliminate("part_num");
    db.eliminate("part_name");
    db.eliminate("part_cat_id");
    db.eliminate("inventory_set_quantity");
    db.eliminate("is_spare");
    db.eliminate("set_num");
    db.eliminate("set_name");
    db.eliminate("num_parts");
    db.eliminate("theme_id");
    db.eliminate("year");



    duration = (clock() - start) / (double)CLOCKS_PER_SEC;
    cout << "time to marginalization: " << duration << "\n"; 
    
    // R->print();
    cout << "Good!\n"; 
}

int main(int argc, char *argv[])
{
    if(argc <=1 ){
        cout << "Please specify data directory!\n"; 
        exit(1);
    }
    
    string directory(argv[1]);
    logo_example();
    // test_marginalization(directory);
    // join_example();
    // cout<<CLOCKS_PER_SEC;

}


