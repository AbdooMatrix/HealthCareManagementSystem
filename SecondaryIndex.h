#ifndef HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H

#include <bits/stdc++.h>

using namespace std;

// node has a --> vector
class SecondaryIndexNode {
public:
    string secondaryKey;
    vector<string> primaryKeys;

    SecondaryIndexNode(const string &secondaryKey, vector<string> &vec) {
        this->secondaryKey = secondaryKey;
        this->primaryKeys = vec;
    }

    bool operator<(const SecondaryIndexNode &other) const {
        return (this->secondaryKey < other.secondaryKey);
    }
};

class SecondaryIndex {
private:
    vector<SecondaryIndexNode> secondaryIndex;

public:
    void loadSecondaryIndexInMemory(const string &fileName) {
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
            string secondaryKey;
            string primarykey;

            getline(recordStream, secondaryKey, '|');


            vector<string> pks; // vector to store the primary keys.
            while (getline(recordStream, primarykey, ',')) {
                pks.emplace_back(primarykey);
            }
            secondaryIndex.emplace_back(secondaryKey, pks);
        }

        file.close();
    }

    void updateSecondaryIndexFile(const string &fileName) {
        fstream outFile(fileName, ios::out | ios::trunc);
        if (!outFile.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        for (const auto &ele: secondaryIndex) {
            outFile << ele.secondaryKey << '|';
            for (auto &pk: ele.primaryKeys) {
                outFile << pk;
                if (pk != ele.primaryKeys.back()) {
                    outFile << ',';
                }
            }
            outFile << '\n';
        }
        outFile.close();
    }

    void sortSecondaryIndex() {
        sort(secondaryIndex.begin(), secondaryIndex.end());
        for (auto &node: secondaryIndex) {
            sort(node.primaryKeys.begin(), node.primaryKeys.end());
        }
    }

    void addSecondaryNode(const string &secondaryKey, const string &primaryKey, const string &fileName) {
        vector<string> vec;
        vec.push_back(primaryKey);
        secondaryIndex.emplace_back(secondaryKey, vec);
        sortSecondaryIndex();
        updateSecondaryIndexFile(fileName);
    }

    void addPrimaryKeyInSecondaryNode(const string &secondaryKey, const string &primarykey, const string &fileName) {
        int index = binarySearchSecondaryIndex(secondaryKey);
        if (index == -1) {
            addSecondaryNode(secondaryKey, primarykey, fileName);
        } else {
            secondaryIndex[index].primaryKeys.emplace_back(primarykey);
            sortSecondaryIndex();
            updateSecondaryIndexFile(fileName);
        }
    }

    void removeSecondaryNode(const string &secondaryKey, const string &fileName) {
        // Perform binary search
        int left = 0, right = secondaryIndex.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;

            if (secondaryIndex[mid].secondaryKey == secondaryKey) {
                // Found the node, remove it
                secondaryIndex.erase(secondaryIndex.begin() + mid);
                sortSecondaryIndex();
                updateSecondaryIndexFile(fileName);
                return;
            } else if (secondaryIndex[mid].secondaryKey < secondaryKey) {
                left = mid + 1;
            } else {
                right = mid - 1;
            }
        }

        cerr << "Error: Secondary key not found.\n";
    }

    void
    removePrimaryKeyFromSecondaryNode(const string &secondaryKey, const string &primaryKey, const string &fileName) {
        int indexNode = binarySearchSecondaryIndex(secondaryKey); // Find node
        if (indexNode == -1) {
            cerr << "Error: Secondary key not found in index.\n";
            return;
        }

        int indexKey = binarySearchPrimaryKeys(indexNode, primaryKey); // Find key
        if (indexKey == -1) {
            cerr << "Error: Primary key " << primaryKey << " not found in secondary node with key " << secondaryKey
                 << ".\n";
            return;
        }

        // Erase the primary key
        secondaryIndex[indexNode].primaryKeys.erase(secondaryIndex[indexNode].primaryKeys.begin() + indexKey);

        // If no primary keys are left, remove the entire secondary node
        if (secondaryIndex[indexNode].primaryKeys.empty()) {
            removeSecondaryNode(secondaryKey, fileName);
        }

        sortSecondaryIndex(); // Ensure the index is sorted
        updateSecondaryIndexFile(fileName); // Persist changes
    }


    int binarySearchSecondaryIndex(const string &secondaryKey) {
        int left = 0;
        int right = secondaryIndex.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (secondaryIndex[mid].secondaryKey == secondaryKey) {
                return mid;
            } else if (secondaryKey < secondaryIndex[mid].secondaryKey) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }

    int binarySearchPrimaryKeys(int indexNode, const string &primaryKey) {
        const vector<string> &keys = secondaryIndex[indexNode].primaryKeys;
        int left = 0;
        int right = keys.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (keys[mid] == primaryKey) {
                return mid; // Found the primary key
            } else if (keys[mid] < primaryKey) {
                left = mid + 1; // Search in the right half
            } else {
                right = mid - 1; // Search in the left half
            }
        }
        return -1; // Not found
    }

    vector<string> getPrimaryKeys(int indexNode) const {
        if (indexNode >= 0 && indexNode < secondaryIndex.size()) {
            return secondaryIndex[indexNode].primaryKeys;
        }
        return {};
    }

    vector<string> getPrimaryKeysBySecondaryKey(const string& secondaryKey) {
        int index = binarySearchSecondaryIndex(secondaryKey);  // Binary search for the secondary key
        if (index != -1) {
            // If found, return the associated primary keys
            return secondaryIndex[index].primaryKeys;
        }
        return {};  // Return an empty vector if not found
    }

};


#endif //HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
