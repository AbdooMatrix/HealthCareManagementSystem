#ifndef HEALTHCAREMANAGEMENTSYSTEM_PRIMARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_PRIMARYINDEX_H

#include<string>

using namespace std;

static bool isFileEmpty(const string &filename) {
    ifstream file(filename, ios::in);
    return file.peek() == ifstream::traits_type::eof();  // Check if the file is empty
}

class PrimaryIndexNode {
public:
    string primaryKey;
    int offset;

    PrimaryIndexNode(const string &primaryKey, int offset) {
        this->primaryKey = primaryKey;
        this->offset = offset;
    }

    bool operator<(const PrimaryIndexNode &other) const {
        return (this->primaryKey < other.primaryKey);
    }
};

class PrimaryIndex {
    vector<PrimaryIndexNode> primaryIndex;

public:
    string getNewId() {
        if (primaryIndex.empty()) {
            return "1";
        } else {
            int newId = stoi(primaryIndex.back().primaryKey);
            newId++;
            return to_string(newId);
        }
    }

    void addPrimaryNode(const string &primaryKey, int offset, const string &fileName) {
        primaryIndex.emplace_back(primaryKey, offset);
        sortPrimaryIndex();
        updatePrimaryIndexFile(fileName);
    }

    void removePrimaryNode(const string &primaryKey, const string &fileName) {
        // Perform binary search
        int left = 0, right = primaryIndex.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (primaryIndex[mid].primaryKey == primaryKey) {
                // Found the node, remove it
                primaryIndex.erase(primaryIndex.begin() + mid);
                sortPrimaryIndex();
                updatePrimaryIndexFile(fileName);
                return;
            } else if (primaryIndex[mid].primaryKey < primaryKey) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        cerr << "Error: Primary key not found.\n";
    }

    void sortPrimaryIndex() {
        sort(primaryIndex.begin(), primaryIndex.end());
    }

    int binarySearchPrimaryIndex(const string &primaryKey) {
        int left = 0;
        int right = primaryIndex.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (primaryIndex[mid].primaryKey == primaryKey) {
                return primaryIndex[mid].offset;
            } else if (primaryKey < primaryIndex[mid].primaryKey) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }

    void loadPrimaryIndexInMemory(const string &fileName) {
        ifstream file(fileName, ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        if (isFileEmpty(fileName)) {
            return;
        }

        string line;
        while (getline(file, line)) {

            istringstream recordStream(line);
            string primaryKey, offset;

            getline(recordStream, primaryKey, '|');
            getline(recordStream, offset, '|');

            primaryIndex.emplace_back(primaryKey, stoi(offset)); // Save the correct offset.
        }

        file.close();
    }

    void updatePrimaryIndexFile(const string &fileName) {
        fstream outFile(fileName, ios::out | ios::trunc);
        if (!outFile.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        for (const auto &ele: primaryIndex) {
            outFile << ele.primaryKey << '|' << ele.offset << '\n';
        }
        outFile.close();
    }

    void printPrimaryIndex() {
        for (const auto &index: primaryIndex) {
            cout << index.primaryKey << "-->" << index.offset << endl;
        }
    }
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_PRIMARYINDEX_H
