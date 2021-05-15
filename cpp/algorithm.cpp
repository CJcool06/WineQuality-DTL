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
    // std::cout << "d size0: " << d.size() << std::endl;
    if (d.size() <= minleaf || is_all_input_same(d) || is_all_output_same(d)) {
        // std::cout << "leaf" << std::endl;
        node* n = new node();

        // If there is a unique value, it will get it. If none found, will be set to -1 (undefined)
        // if unique mode
            // n.label = mode
        // else
            // n.label = -1 (unknown)
        n->label = get_unique_val(d);
        n->leaf = true;
        // std::cout << "L: " << n.label << std::endl;

        // std::cout << "finished leaf: " << n->label << std::endl;
        
        return n;
    }

    // std::cout << "d size1: " << d.size() << std::endl;

    // std::cout << "not leaf" << std::endl;

    std::vector<pair_data> copy(d);
    std::pair<int, double> attr_splitval = choose_split(d);
    node* n = new node();
    n->attr = attr_splitval.first;
    n->splitval = attr_splitval.second;

    // std::cout << "split done" << std::endl;
    // std::cout << "attr: " << n->attr << std::endl;
    // std::cout << "split: " << n->splitval << std::endl;

    std::vector<pair_data> left;
    std::vector<pair_data> right;

    // std::cout << "d size2: " << d.size() << std::endl;
    // std::cout << "copy size2: " << copy.size() << std::endl;
    for (pair_data data : d) {
        // std::cout << "data: " << std::endl;
        if (data.input.size() < 11) {
            continue;
        }
        // std::cout << "data size: " << data.input.size() << std::endl;
        if (data.input[n->attr] <= n->splitval) {
            // std::cout << "left" << std::endl;
            left.push_back(data);
            // std::cout << "left done" << std::endl;
        }
        else {
            // std::cout << "right" << std::endl;
            right.push_back(data);
            // std::cout << "right done" << std::endl;
        }
    }

    // std::cout << "doing children" << std::endl;

    n->left = dtl(left, minleaf);
    n->right = dtl(right, minleaf);
    // n.left = &node_left;
    // n.right = &node_right;

    // std::cout << "done children" << std::endl;

    // if (n->left->leaf) {
    //     std::cout << "In left leaf." << std::endl;
    //     std::cout << "LL: " << n->left->label << std::endl; 
    // }
    // if (n->right->leaf) {
    //     std::cout << "In right leaf." << std::endl;
    //     std::cout << "LR: " << n->right->label << std::endl; 
    // }

    // print_tree(n.left, 0);
    // print_tree(n.right, 0);

    // std::cout << "not leaf return" << std::endl;

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

        // std::vector<pair_data> sorted_attr;
        // for (int i = 0; i < sorted_attr_old.size(); i++) {
        //     if (i > 0 && sorted_attr_old[i].input[j] == sorted_attr_old[i - 1].input[j])
        //         continue;
        //     sorted_attr.push_back(sorted_attr_old[i]);
        // }

        for (std::set<double>::iterator itr = sett.begin(); itr != sett.end(); itr++) {
            auto x = itr;
            auto y = itr;
            if(++y == sett.end()) {
                break;
            }

            double splitval = 0.5 * ((*x) + (*y));

            // std::cout << "ATTR: " << j << std::endl;
            // std::cout << "SPLIT: " << splitval << std::endl;
            // std::cout << "IC: " << information_content(sorted_attr) << std::endl;
            double gain = information_content(sorted_attr) + remainder(sorted_attr, j, splitval);
            // double gain = 1 + remainder(sorted_attr, j, splitval);
            // std::cout << "GAIN: " << gain << std::endl;
            if (gain > best_gain) {
                best_attr = j;
                best_splitval = splitval;
                best_gain = gain;
            }
        }

        // for (int i = 0; i < d.size() - 1; i++) {
        //     // double splitval = 0.5 * (sorted_attr[i].input[j] + sorted_attr[i+1].input[j]);

        //     // std::cout << "ATTR: " << j << std::endl;
        //     // std::cout << "SPLIT: " << splitval << std::endl;
        //     // std::cout << "IC: " << information_content(sorted_attr) << std::endl;
        //     double gain = information_content(sorted_attr) + remainder(sorted_attr, j, splitval);
        //     // double gain = 1 + remainder(sorted_attr, j, splitval);
        //     // std::cout << "GAIN: " << gain << std::endl;
        //     if (gain > best_gain) {
        //         best_attr = j;
        //         best_splitval = splitval;
        //         best_gain = gain;
        //     }
        // }
    }

    // std::cout << "split [attr, val]: " << best_attr << ", " << best_splitval << std::endl;
    // std::cout << "GAIN: " << best_gain << std::endl;

    return std::pair<int, double>(best_attr, best_splitval);
}

double algorithm::predict_dtl(node* n, attribute_data d) {
    // std::cout << "data: ";
    // for (double val : d.values) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;
    
    // if (n->leaf) {
    //     return n->label;
    // }

    // if (d.values[n->attr] <= n->splitval) {
    //     return predict_dtl(n->left, d);
    // }
    // else {
    //     return predict_dtl(n->right, d);
    // }

    // std::cout << "1" << std::endl;
    while (!n->leaf) {
        // std::cout << "attr: " << n.attr << std::endl;
        // std::cout << "split: " << n.splitval << std::endl;
        // std::cout << "2" << std::endl;
        // std::cout << "for attribute " << n->attr << " and splitval " << n->splitval << ": ";
        if (d.values[n->attr] <= n->splitval) {
            // std::cout << "left: " << (n->left ? "notnull" : "null") << std::endl;
            // std::cout << "left for value " << d.values[n->attr] << std::endl;
            n = n->left;
        }
        else {
            // std::cout << "right: " << (n->left ? "notnull" : "null") << std::endl;
            // std::cout << "right for value " << d.values[n->attr] << std::endl;
            n = n->right;
        }
    }

    // std::cout << "found leaf" << std::endl;

    // std::cout << "attr: " << n->attr << std::endl;
    // std::cout << "split: " << n->splitval << std::endl;
    // std::cout << "label: " << n->label << std::endl;

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
    // std::cout << "V: " << value << std::endl;
    for (auto const& x : data_map) {
        // std::cout << "first: " << x.first << std::endl;
        // std::cout << "second: " << x.second << std::endl;
        // std::cout << "size: " << d.size() << std::endl;
        // double v = log2((float(x.second)) / ((float)d.size()));
        // std::cout << "v: "<< v << std::endl;
        // std::cout << "p: " << (x.second / (double)d.size()) << std::endl;
        // std::cout << "log: " << log2(x.second / (float)d.size()) << std::endl;
        value -= ((double)x.second / (double)d.size()) * log2((double)x.second / (float)d.size());
        // std::cout << "V2: " << value << std::endl;
    }

    return value;


    // int countleft = 0;

    // for (pair_data data : d) {
    //     for (int i = 0; i < data.size(); i++) {
    //         if (data.input[i] <= splitval) {
    //             countleft++;
    //         }
    //     }
    // }

    // return -((countleft / d.size()) * log2(countleft / d.size())) - (((d.size() - countleft) / d.size()) * log2((d.size() - countleft) / d.size()));
}

double remainder(std::vector<pair_data> d, int attr, double splitval) {


    // - P(left) * IC(left) - P(right) * IC(right);
    // return 

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


    // std::map<double, int> data_map;

    // for (pair_data data : d) {
    //     std::map<double, int>::iterator it = data_map.find(data.input[attr]);
    //     if (it != data_map.end()) {
    //         //element found;
    //         it->second++;
    //     }
    //     else {
    //         data_map.insert({ data.input[attr], 1});
    //     }
    // }

    // double value = 0;
    // for (auto const& x : data_map) {
    //     // std::cout << "Map: " << x.first << ", " << x.second << std::endl;
    //     value -= (x.second / (double)data_map.size()) * information_content(get_data_for_output(d, attr, x.first));
    // }

    // // std::cout << "VAL: " << value << std::endl;

    // return value;
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
    // return !(n.left || n.right);
    return n.label != -2;
}

double get_unique_val(std::vector<pair_data> d) {
    std::map<double, int> data_map;

    for (pair_data data : d) {
        // std::cout << "data: " << data.output << std::endl;
        std::map<double, int>::iterator it = data_map.find(data.output);
        if (it != data_map.end()) {
            //element found;
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
        // std::cout << "map: " << x.first << ", " << x.second << std::endl;
        if (x.second > max) {
            max_key = x.first;
            max = x.second;
            multiple = false;
        }
        else if (x.second == max) {
            multiple = true;
        }
    }

    // std::cout << "result: " << (multiple ? -1 : max_key) << std::endl;

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
    // Successfully tested
    // std::cout << "Unique val: " << get_unique_val(d) << std::endl;
    // std::cout << "Input same: " << is_all_input_same(d) << std::endl;
    // std::cout << "Output same: " << is_all_output_same(d) << std::endl;

    node* n = dtl(d, minleaf);
    print_tree(n, 0);

    // std::cout << "IC: " << information_content(d) << std::endl;
    // std::cout << "REM: " << remainder(d) << std::endl;

    // std::pair<double, int> attr_spl = choose_split(d);
    // std::cout << "Best attr: " << attr_spl.first << std::endl;
    // std::cout << "Best split: " << attr_spl.second << std::endl;
    
    // for (attribute_data data : d2) {
    //     std::cout << "Predict: " << predict_dtl(&n, data) << std::endl;
    // }
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