#include <iostream>
#include <fstream>

#include "algorithm.h"

using namespace std;

int main(int argc, char** argv) {

    /**
     * Begin input parsing.
     */

    string training_file_str = string(argv[1]);
    string testing_file_str = string(argv[2]);
    int minleaf = stoi(string(argv[3]));

    fstream training_file(training_file_str, ios_base::in);
    fstream testing_file(testing_file_str, ios_base::in);

    string inp;
    for (int i = 0; i < 12; i++) {
        training_file >> inp;
    }
    for (int i = 0; i < 11; i++) {
        testing_file >> inp;
    }

    vector<pair_data> training_input;
    vector<attribute_data> testing_input;
    int count = 0;
    pair_data d;
    double val;

    // cout << "Getting training data..." << endl;

    while (training_file >> val) {
        d.input.push_back(val);
        count++;

        if (count >= 11) {
            training_file >> val;
            d.output = val;
            training_input.push_back(d);
            d = pair_data();
            count = 0;
        }
    }

    // cout << "Getting testing data..." << endl;

    count = 0;
    attribute_data ad;
    while (testing_file >> val) {
        ad.values.push_back(val);
        count++;

        if (count >= 11) {
            testing_input.push_back(ad);
            ad = attribute_data();
            count = 0;
        }
    }

    // DEBUG OUTPUT

    // for (pair_data data : training_input) {
    //     for (double d : data.input) {
    //         cout << d << " ";
    //     }
    //     cout << data.output << endl;
    // }
    // for (attribute_data data : testing_input) {
    //     for (double d : data.values) {
    //         cout << d << " ";
    //     }
    //     cout << endl;
    // }

    // for (pair_data data : training_input) {
    //     std::cout << "data size: " << data.input.size() << std::endl;
    // }

    /**
     * End input parsing.
     */


    algorithm algo;

    // cout << "Doing algo." << endl;

    // cout << "training size: " << training_input.size() << endl;

    node* n = algo.dtl(training_input, minleaf);

    // cout << "Printing tree." << endl;
    // algo.print_tree(n, 0);

    // cout << "Doing predictions." << endl;

    for (attribute_data data : testing_input) {
        // cout << "Predicting..." << endl;
        double ans = algo.predict_dtl(n, data);
        if (ans == -1) {
            cout << "unknown" << endl;
        }
        else {
            cout << ans << endl;
            // cout << to_string(ans) << endl;
        }
    }

    //  cout << "Done." << endl;

    // cout << "Running algo..." << endl;

    // algo.test(training_input, testing_input, minleaf);
    // cout << "Hi!" << endl;
}