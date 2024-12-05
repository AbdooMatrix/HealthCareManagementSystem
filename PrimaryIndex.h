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
    string primaryIndexFileName;
    vector<PrimaryIndexNode> primaryIndex;

public:
    void setPrimaryIndexFileName(const string& fileName) {
        this->primaryIndexFileName = fileName;
        loadPrimaryIndexInMemory();
    }

    string getNewId() {
        if (primaryIndex.empty()) {
            return "01";  // Start with a two-digit ID
        } else {
            int newId = stoi(primaryIndex.back().primaryKey) + 1;
            return (newId < 10) ? "0" + to_string(newId) : to_string(newId);
        }
    }

    void loadPrimaryIndexInMemory() {
        ifstream file(primaryIndexFileName, ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        if (isFileEmpty(primaryIndexFileName)) {
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

    void updatePrimaryIndexFile() {
        fstream outFile(primaryIndexFileName, ios::out | ios::trunc);
        if (!outFile.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        for (const auto &ele: primaryIndex) {
            outFile << ele.primaryKey << '|' << ele.offset << '\n';
        }
        outFile.close();
    }

    void addPrimaryNode(const string &primaryKey, int offset, const string &fileName) {
        primaryIndex.emplace_back(primaryKey, offset);
        sortPrimaryIndex();
        updatePrimaryIndexFile();
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
                updatePrimaryIndexFile();
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

    void printPrimaryIndex() {
        for (const auto &index: primaryIndex) {
            cout << index.primaryKey << "-->" << index.offset << endl;
        }
    }
};


#endif //HEALTHCAREMANAGEMENTSYSTEM_PRIMARYINDEX_H
