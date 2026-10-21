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

const int SIMULATIONS = 15, SLICES = 2, EVENTS = 4, CONTAINERS = 3;
const int CURRENT = 0, TOTALS = 1;

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
void run_simulation(long long current[EVENTS][CONTAINERS]);
void accumulate_times(long long timings[SLICES][EVENTS][CONTAINERS]);
void print_results(const long long timings[SLICES][EVENTS][CONTAINERS]);

int main() {
    // slice 0 holds the current run and slice 1 holds running totals
    // rows are read, sort, insert, delete and columns are vector, list, set
    long long timings[SLICES][EVENTS][CONTAINERS]{};

    try {
        for (int run = 0; run < SIMULATIONS; ++run) {
            run_simulation(timings[CURRENT]);
            accumulate_times(timings);
        }
        print_results(timings);
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
    // a set is already sorted so its sorting time stays at zero
    RaceResult sorting{"Sorting", 0, 0, 0};
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

void run_simulation(long long current[EVENTS][CONTAINERS]) {
    // fresh containers keep earlier runs from changing the next race
    vector<string> vector_codes;
    list<string> list_codes;
    set<string> set_codes;
    RaceResult reading{"Reading", 0, 0, 0};
    reading.vector_time = read_codes(vector_codes, "names.txt");
    reading.list_time = read_codes(list_codes, "names.txt");
    reading.set_time = read_codes(set_codes, "names.txt");
    RaceResult races[EVENTS] = {
        reading,
        sort_codes(vector_codes, list_codes),
        insert_codes(vector_codes, list_codes, set_codes),
        delete_codes(vector_codes, list_codes, set_codes)
    };

    for (int event = 0; event < EVENTS; ++event) {
        current[event][0] = races[event].vector_time;
        current[event][1] = races[event].list_time;
        current[event][2] = races[event].set_time;
    }
}

void accumulate_times(long long timings[SLICES][EVENTS][CONTAINERS]) {
    for (int event = 0; event < EVENTS; ++event) {
        for (int container = 0; container < CONTAINERS; ++container)
            timings[TOTALS][event][container] += timings[CURRENT][event][container];
    }
}

void print_results(const long long timings[SLICES][EVENTS][CONTAINERS]) {
    const string operations[EVENTS] = {"Read", "Sort", "Insert", "Delete"};
    cout << "Number of simulations: " << SIMULATIONS << "\n";
    cout << right << setw(12) << "Operation" << setw(16) << "Vector"
         << setw(16) << "List" << setw(16) << "Set" << endl;
    for (int event = 0; event < EVENTS; ++event) {
        cout << setw(12) << operations[event];
        for (int container = 0; container < CONTAINERS; ++container)
            cout << setw(16) << timings[TOTALS][event][container] / SIMULATIONS;
        cout << endl;
    }
    cout << "Average times in nanoseconds\n";
}
