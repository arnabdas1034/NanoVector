#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include "NanoVectorDB.h"

using namespace std;

int main() {
    cout << "==========================================" << endl;
    cout << "          NANOVECTOR DB SHELL             " << endl;
    cout << "==========================================" << endl;

    // Boot up the unified database wrapper
    NanoVectorDB db("nanovector_data.bin");

    cout << "\nAvailable Commands:" << endl;
    cout << "  INSERT <id> <val1> <val2> ... : Add a new vector" << endl;
    cout << "  SEARCH <k> <val1> <val2> ...  : Find top K nearest neighbors" << endl;
    cout << "  STATS                         : Show database size" << endl;
    cout << "  EXIT                          : Save and shutdown" << endl;

    string line;
    // THE CLI LOOP
    while (true) {
        cout << "\nnano> ";
        if (!getline(cin, line)) break; // Catch Ctrl+D or EOF
        if (line.empty()) continue;

        // Use stringstream to chop the input line into separate words
        stringstream ss(line);
        string command;
        ss >> command;

        // Convert command to uppercase so it's case-insensitive
        for(auto &c : command) c = toupper(c);

        try {
            if (command == "EXIT") {
                cout << "Saving memory to disk..." << endl;
                db.commit();
                cout << "Shutdown complete. Goodbye!" << endl;
                break;
            }
            else if (command == "STATS") {
                cout << "Total vectors in database: " << db.size() << endl;
            }
            else if (command == "INSERT") {
                string id;
                ss >> id; // Next word is the ID

                vector<float> vals;
                float val;
                while (ss >> val) { // The rest are floats
                    vals.push_back(val);
                }

                if (vals.empty()) {
                    cout << "Error: You must provide float values. Example: INSERT my_vec 1.0 2.0" << endl;
                    continue;
                }

                db.insert(Vector(id, vals));
                cout << "-> Inserted '" << id << "' (" << vals.size() << "D)" << endl;
            }
            else if (command == "SEARCH") {
                int k;
                ss >> k; // Next word is K

                vector<float> vals;
                float val;
                while (ss >> val) { // The rest are floats for the query
                    vals.push_back(val);
                }

                if (vals.empty()) {
                    cout << "Error: You must provide query values. Example: SEARCH 3 1.0 2.0" << endl;
                    continue;
                }

                Vector query("query", vals);
                auto results = db.search(query, k);

                cout << "--- Top " << k << " Results ---" << endl;
                cout << fixed << setprecision(4);
                for (size_t i = 0; i < results.size(); ++i) {
                    cout << " " << i + 1 << ". " << results[i].id
                         << " (Distance: " << results[i].distance << ")" << endl;
                }
            }
            else {
                cout << "Error: Unknown command '" << command << "'" << endl;
            }
        }
        // Catch any dimensional mismatch errors from Phase 1!
        catch (const exception& e) {
            cout << "SYSTEM ERROR: " << e.what() << endl;
        }
    }

    return 0;
}