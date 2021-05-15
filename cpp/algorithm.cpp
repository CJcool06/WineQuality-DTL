#include "algorithm.h"
#include <map>
#include <iostream>
#include <math.h>
#include <float.h>
#include <algorithm>
#include <set>

double get_unique_val(std::vector<pair_data> d);
bool is_all_input_same(std::vector<pair_data> d);
bool is_all_output_same(std::vector<pair_data> d);
attribute get_attribute(int index);
int get_attribute_index(attribute a);
bool is_leaf_node(node n);
double information_content(std::vector<pair_data> d);
double remainder(std::vector<pair_data> d, int attr, double splitval);
std::vector<pair_data> get_data_for_output(std::vector<pair_data> d, int attr, double output);
void print_tree(node* n, int depth);

node* algorithm::dtl(std::vector<pair_data> d, int minleaf) {
    if (d.size() <= minleaf || is_all_input_same(d) || is_all_output_same(d)) {
        node* n = new node();

        // If there is a unique value, it will get it. If none found, will be set to -1 (undefined)
        n->label = get_unique_val(d);
        n->leaf = true;
        
        return n;
    }

    std::vector<pair_data> copy(d);
    std::pair<int, double> attr_splitval = choose_split(d);
    node* n = new node();
    n->attr = attr_splitval.first;
    n->splitval = attr_splitval.second;

    std::vector<pair_data> left;
    std::vector<pair_data> right;

    for (pair_data data : d) {
        if (data.input.size() < 11) {
            continue;
        }
        if (data.input[n->attr] <= n->splitval) {
            left.push_back(data);
        }
        else {
            right.push_back(data);
        }
    }

    n->left = dtl(left, minleaf);
    n->right = dtl(right, minleaf);

    return n;
}

std::pair<int, double> algorithm::choose_split(std::vector<pair_data> d) {
    double best_splitval = 0;
    int best_attr = -1;
    double best_gain = -DBL_MAX;
    
    for (int j = 0; j < d[0].input.size(); j++) {
        std::vector<pair_data> sorted_attr(d);
        std::sort(sorted_attr.begin(), sorted_attr.end(), less_than_pair(j));

        std::set<double> sett;
        for (pair_data data : sorted_attr) {
            sett.insert(data.input[j]);
        }

        for (std::set<double>::iterator itr = sett.begin(); itr != sett.end(); itr++) {
            auto x = itr;
            auto y = itr;
            if(++y == sett.end()) {
                break;
            }

            double splitval = 0.5 * ((*x) + (*y));
            double gain = information_content(sorted_attr) + remainder(sorted_attr, j, splitval);
            if (gain > best_gain) {
                best_attr = j;
                best_splitval = splitval;
                best_gain = gain;
            }
        }
    }

    return std::pair<int, double>(best_attr, best_splitval);
}

double algorithm::predict_dtl(node* n, attribute_data d) {
    while (!n->leaf) {
        if (d.values[n->attr] <= n->splitval) {
            n = n->left;
        }
        else {
            n = n->right;
        }
    }

    return n->label;
}

double information_content(std::vector<pair_data> d) {
    std::map<double, int> data_map;

    for (pair_data data : d) {
        std::map<double, int>::iterator it = data_map.find(data.output);
        if (it != data_map.end()) {
            //element found;
            it->second++;
        }
        else {
            data_map.insert({ data.output, 1});
        }
    }

    double value = 0;
    for (auto const& x : data_map) {
        value -= ((double)x.second / (double)d.size()) * log2((double)x.second / (float)d.size());
    }

    return value;
}

double remainder(std::vector<pair_data> d, int attr, double splitval) {
    std::vector<pair_data> left;
    std::vector<pair_data> right;

    for (pair_data data : d) {
        if (data.input[attr] <= splitval) {
            left.push_back(data);
        }
        else {
            right.push_back(data);
        }
    }

    return (-((double)left.size() / (double)d.size()) * information_content(left)) - (((double)right.size() / (double)d.size()) * information_content(right));
}

std::vector<pair_data> get_data_for_output(std::vector<pair_data> d, int attr, double input) {
    std::vector<pair_data> out;

    for (pair_data data : d) {
        if (data.input[attr] == input) {
            out.push_back(data);
        }
    }

    return out;
}

bool is_leaf_node(node n) {
    return n.label != -2;
}

double get_unique_val(std::vector<pair_data> d) {
    std::map<double, int> data_map;

    for (pair_data data : d) {
        std::map<double, int>::iterator it = data_map.find(data.output);
        if (it != data_map.end()) {
            it->second++;
        }
        else {
            data_map.insert({ data.output, 1});
        }
    }

    int max_key = -1;
    double max = 0;
    bool multiple = false;
    for (auto const& x : data_map) {
        if (x.second > max) {
            max_key = x.first;
            max = x.second;
            multiple = false;
        }
        else if (x.second == max) {
            multiple = true;
        }
    }

    return multiple ? -1 : max_key;
}

bool is_all_input_same(std::vector<pair_data> d) {
    // If size is less than 2, it must be true.
    if (d.size() <= 1) {
        return true;
    }

    try {
        // Check if all input data is equal to the first input data.
        for (int i = 1; i < d.size(); i++) {
            for (int j = 0; j < d[i].input.size(); j++) {
                // If not equal, data is not the same.
                if (d[0].input[j] != d[i].input[j]) {
                    return false;
                }
            }
        }
    } catch (...) {
        // If there's an out of bounds error, then it's not the same.
        return false;
    }
    // Data is the same
    return true;
}

bool is_all_output_same(std::vector<pair_data> d) {
    // If size is less than 2, it must be true.
    if (d.size() <= 1) {
        return true;
    }

    try {
        // Check if all output data is equal to the first output data.
        for (int i = 1; i < d.size(); i++) {
            // If not equal, data is not the same.
            if (d[0].output != d[i].output) {
                return false;
            }
        }
    } catch (...) {
        // If there's an out of bounds error, then it's not the same.
        return false;
    }
    return true;
}

attribute get_attribute(int index) {
    switch (index) {
        case 0:
            return f_acid;
        case 1:
            return v_acid;
        case 2:
            return c_acid;
        case 3:
            return res_sugar;
        case 4:
            return chlorides;
        case 5:
            return fs_dioxide;
        case 6:
            return ts_dioxide;
        case 7:
            return density;
        case 8:
            return pH;
        case 9:
            return sulphates;
        case 10:
            return alcohol;
        default:
            return none;
    }
}

int get_attribute_index(attribute a) {
    switch (a) {
        case f_acid:
            return 0;
        case v_acid:
            return 1;
        case c_acid:
            return 2;
        case res_sugar:
            return 3;
        case chlorides:
            return 4;
        case fs_dioxide:
            return 5;
        case ts_dioxide:
            return 6;
        case density:
            return 7;
        case pH:
            return 8;
        case sulphates:
            return 9;
        case alcohol:
            return 10;
        default:
            return -1;
    }
}

void algorithm::test(std::vector<pair_data> d, std::vector<attribute_data> d2, int minleaf) {
    node* n = dtl(d, minleaf);
    print_tree(n, 0);
}

void algorithm::print_tree(node* n, int depth) {
    if (!n) {
        return;
    }
    
    std::cout << "Depth: " << depth << std::endl;
    std::cout << "Label: " << n->label << std::endl;
    std::cout << "Attr: " << n->attr << std::endl;
    std::cout << "Splitval: " << n->splitval << std::endl;
    std::cout << "Left: " << (n->left ? "y" : "n") << std::endl;
    std::cout << "Right: " << (n->right ? "y" : "n") << std::endl;

    print_tree(n->left, depth + 1);
    print_tree(n->right, depth + 1);
}