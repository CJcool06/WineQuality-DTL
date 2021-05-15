#pragma once

#include <utility>
#include <string>
#include <vector>

enum attribute {
    f_acid,
    v_acid,
    c_acid,
    res_sugar,
    chlorides,
    fs_dioxide,
    ts_dioxide,
    density,
    pH,
    sulphates,
    alcohol,
    none
};

struct pair_data {
    std::vector<double> input;
    double output;
};

struct attribute_data {
    std::vector<double> values;
};

struct node {
    double label;
    int attr;
    double splitval;
    node* left = NULL;
    node* right = NULL;
    bool leaf = false;
};

struct less_than_pair {
    int attr;
    less_than_pair(int attr) {
        this->attr = attr;
    }
    inline bool operator() (const pair_data& struct1, const pair_data& struct2)
    {
        return (struct1.input[attr] < struct2.input[attr]);
    }
};

class algorithm {

private:
    std::pair<int, double> choose_split(std::vector<pair_data> d);

public:
    node* dtl(std::vector<pair_data> d, int minleaf);
    double predict_dtl(node* n, attribute_data d);
    
    void test(std::vector<pair_data> d, std::vector<attribute_data> d2, int minleaf);
    void print_tree(node* n, int depth);
};