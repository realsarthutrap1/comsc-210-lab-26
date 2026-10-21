// COMSC-210 | Lab 26 | Sarthak Pani
// adapted from the instructor race starter and chrono demo

#include <iostream>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
using namespace std::chrono;

struct RaceResult {
    string operation;
    long long vector_time;
    long long list_time;
    long long set_time;
};

template <typename Container>
long long read_codes(Container& codes, const string& filename);
RaceResult sort_codes(vector<string>& vector_codes, list<string>& list_codes);
RaceResult insert_codes(vector<string>& vector_codes, list<string>& list_codes,
                        set<string>& set_codes);
template <typename Container>
long long delete_middle(Container& codes);
RaceResult delete_codes(vector<string>& vector_codes, list<string>& list_codes,
                        set<string>& set_codes);
void print_results(const vector<RaceResult>& results);

int main() {
    vector<string> vector_codes;
    list<string> list_codes;
    set<string> set_codes;
    vector<RaceResult> results;

    try {
        RaceResult reading{"Reading", 0, 0, 0};
        reading.vector_time = read_codes(vector_codes, "names.txt");
        reading.list_time = read_codes(list_codes, "names.txt");
        reading.set_time = read_codes(set_codes, "names.txt");
        results.push_back(reading);
        results.push_back(sort_codes(vector_codes, list_codes));
        results.push_back(insert_codes(vector_codes, list_codes, set_codes));
        results.push_back(delete_codes(vector_codes, list_codes, set_codes));
        print_results(results);
    } catch (const runtime_error& error) {
        cerr << error.what() << endl;
        return 1;
    }

    return 0;
}

template <typename Container>
long long read_codes(Container& codes, const string& filename) {
    ifstream input(filename);
    if (!input)
        throw runtime_error("Could not open " + filename);

    // each contestant reads the file with its own stopwatch
    string code;
    auto start = steady_clock::now();
    while (input >> code)
        codes.insert(codes.end(), code);
    auto end = steady_clock::now();

    if (input.bad() || !input.eof())
        throw runtime_error("Could not finish reading " + filename);
    if (codes.empty())
        throw runtime_error("No codes found in " + filename);

    return duration_cast<nanoseconds>(end - start).count();
}

RaceResult sort_codes(vector<string>& vector_codes, list<string>& list_codes) {
    // a set is already sorted so its sorting time stays at -1
    RaceResult sorting{"Sorting", 0, 0, -1};
    auto start = steady_clock::now();
    sort(vector_codes.begin(), vector_codes.end());
    auto end = steady_clock::now();
    sorting.vector_time = duration_cast<nanoseconds>(end - start).count();

    start = steady_clock::now();
    list_codes.sort();
    end = steady_clock::now();
    sorting.list_time = duration_cast<nanoseconds>(end - start).count();
    return sorting;
}

RaceResult insert_codes(vector<string>& vector_codes, list<string>& list_codes,
                        set<string>& set_codes) {
    RaceResult inserting{"Inserting", 0, 0, 0};
    // finding the middle is included in each operation's time
    auto start = steady_clock::now();
    auto vector_middle = next(vector_codes.begin(), vector_codes.size() / 2);
    vector_codes.insert(vector_middle, "TESTCODE");
    auto end = steady_clock::now();
    inserting.vector_time = duration_cast<nanoseconds>(end - start).count();

    start = steady_clock::now();
    auto list_middle = next(list_codes.begin(), list_codes.size() / 2);
    list_codes.insert(list_middle, "TESTCODE");
    end = steady_clock::now();
    inserting.list_time = duration_cast<nanoseconds>(end - start).count();

    start = steady_clock::now();
    set_codes.insert("TESTCODE");
    end = steady_clock::now();
    inserting.set_time = duration_cast<nanoseconds>(end - start).count();
    return inserting;
}

template <typename Container>
long long delete_middle(Container& codes) {
    auto start = steady_clock::now();
    if (!codes.empty()) {
        auto middle = next(codes.begin(), codes.size() / 2);
        codes.erase(middle);
    }
    auto end = steady_clock::now();
    return duration_cast<nanoseconds>(end - start).count();
}

RaceResult delete_codes(vector<string>& vector_codes, list<string>& list_codes,
                        set<string>& set_codes) {
    RaceResult deleting{"Deleting", 0, 0, 0};
    deleting.vector_time = delete_middle(vector_codes);
    deleting.list_time = delete_middle(list_codes);
    deleting.set_time = delete_middle(set_codes);
    return deleting;
}

void print_results(const vector<RaceResult>& results) {
    cout << "Data structure races (nanoseconds)\n";
    cout << left << setw(16) << "Operation" << right
         << setw(16) << "Vector" << setw(16) << "List" << setw(16) << "Set" << endl;
    for (const RaceResult& result : results) {
        cout << left << setw(16) << result.operation << right
             << setw(16) << result.vector_time << setw(16) << result.list_time
             << setw(16) << result.set_time << endl;
    }
    cout << "-1 means the set is already sorted\n";
}
