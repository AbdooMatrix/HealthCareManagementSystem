#ifndef HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H

#include <bits/stdc++.h>

using namespace std;

class PrimaryKeyNode {
public:
    string primaryKey;
    string nextIndex;  // Change from int to string

    // Constructor
    PrimaryKeyNode(const string& pk, const string& next) : primaryKey(pk), nextIndex(next) {}
};

class SecondaryIndex {
private:
    string secondaryIndexFileName;
    string labelIdListFileName;
    map<string, int> secondaryIndexMap; // Maps secondary key to index of the head of linked list
    vector<PrimaryKeyNode> primaryKeyList; // Vector storing PrimaryKeyNodes as a linked list

public:
    int getFreeLabelIndex() {
        for (int index = 0; index < primaryKeyList.size(); ++index) {
            if (primaryKeyList[index].nextIndex == "##") {
                return index;
            }
        }
        primaryKeyList.emplace_back("##", "##");
        return primaryKeyList.size() - 1;
    }

    void releaseLabelId(int index) {
        if (index >= 0 && index < primaryKeyList.size()) {
            primaryKeyList[index].primaryKey = "##";
            primaryKeyList[index].nextIndex = "##"; // Mark as free
        }
    }

    void setSecondaryIndexAndLabelIdListFileNames(const string& secondaryIndex, const string& labelIdFileName) {
        this->secondaryIndexFileName = secondaryIndex;
        this->labelIdListFileName = labelIdFileName;
        loadSecondaryIndexAndLabelIdList();
    }

    void loadSecondaryIndexAndLabelIdList() {
        // Load DoctorSecondaryIndex
        ifstream secFile(secondaryIndexFileName);
        if (!secFile.is_open()) {
            cerr << "Error opening file: " << secondaryIndexFileName << "\n";
            return;
        }

        string line;
        while (getline(secFile, line)) {
            istringstream recordStream(line);
            string secondaryKey;
            string headIndex;

            // Parse format: "<SecondaryKey>|<HeadPointer>"
            getline(recordStream, secondaryKey, '|');
            getline(recordStream , headIndex , '|') ;

            secondaryIndexMap[secondaryKey] = stoi(headIndex) ;
        }
        secFile.close();

        // Load DoctorLabelIdList
        ifstream labelFile(labelIdListFileName);
        if (!labelFile.is_open()) {
            cerr << "Error opening file: " << labelIdListFileName << "\n";
            return;
        }

        // Clearing primaryKeyList to prepare for direct insertion
        primaryKeyList.clear();

        string recNoStr;
        string id;
        string nextPtrStr;
        while (getline(labelFile, line)) {
            istringstream recordStream(line);

            // Parse format: "<recNo>|<ID>,<NextPointer>"
            // Extract recNo (fixed width 2 bytes)
            getline(recordStream, recNoStr, '|');
            // Extract ID (fixed width 2 bytes)
            getline(recordStream, id, ',');
            // Extract next pointer as string
            getline(recordStream, nextPtrStr);

            // Add nodes sequentially without worrying about resizing
            primaryKeyList.emplace_back(id, nextPtrStr);
        }
        labelFile.close();
    }

    void updateSecondaryIndexAndLabelIdList() {
        // Update DoctorSecondaryIndex
        ofstream secFile(secondaryIndexFileName);
        if (!secFile.is_open()) {
            cerr << "Error opening file: " << secondaryIndexFileName << "\n";
            return;
        }
        for (const auto &entry : secondaryIndexMap) {
            secFile << entry.first << "|"
            << setw(2) << setfill('0') << entry.second << '\n';
        }
        secFile.close();

        // Update DoctorLabelIdList
        ofstream labelFile(labelIdListFileName);
        if (!labelFile.is_open()) {
            cerr << "Error opening file: " << labelIdListFileName << "\n";
            return;
        }
        int recNo = 0;
        for (const auto &node : primaryKeyList) {
            // Ensure fixed width formatting for recNo (2 bytes) and nextPtr (2 bytes)
            labelFile << setw(2) << setfill('0') << recNo << "|"
                      << setw(2) << setfill('0') << node.primaryKey << ","
                      << setw(2) << setfill('0') << node.nextIndex << '\n';  // Writing nextIndex as a string
            recNo++;
        }
        labelFile.close();
    }

    void addPrimaryKeyToSecondaryNode(const string &secondaryKey, const string &primaryKey) { // name - id
        int freeLabelId = getFreeLabelIndex();  // Get a free label ID
        if (secondaryIndexMap.find(secondaryKey) == secondaryIndexMap.end()) {
            primaryKeyList[freeLabelId] = PrimaryKeyNode(primaryKey, "-1"); // Use free label ID for the new node
            secondaryIndexMap[secondaryKey] = freeLabelId; // Head is the new entry
        } else {
            int currentIndex = secondaryIndexMap[secondaryKey]; // first current index will point to head index
            if(currentIndex == -1){
                secondaryIndexMap[secondaryKey] = freeLabelId ;
            }
            else{
                while (primaryKeyList[currentIndex].nextIndex != "-1") {
                    currentIndex = stoi(primaryKeyList[currentIndex].nextIndex);  // Convert string back to int
                }
                primaryKeyList[currentIndex].nextIndex = to_string(freeLabelId);  // Store index as string
            }
            primaryKeyList[freeLabelId] = PrimaryKeyNode(primaryKey, "-1");
        }
        updateSecondaryIndexAndLabelIdList();
    }

    void removePrimaryKeyFromSecondaryNode(const string &secondaryKey, const string &primaryKey) {
        if (secondaryIndexMap.find(secondaryKey) == secondaryIndexMap.end()) {
            cerr << "Error: Secondary key not found.\n";
            return;
        }

        string* prevPtr = &primaryKeyList[secondaryIndexMap[secondaryKey]].nextIndex; // address of head node's nextIndex
        int currentIndex = secondaryIndexMap[secondaryKey]; // value of head index

        while (currentIndex != -1) {
            if (primaryKeyList[currentIndex].primaryKey == primaryKey) {
                if (currentIndex == secondaryIndexMap[secondaryKey]) {
                    secondaryIndexMap[secondaryKey] = stoi(primaryKeyList[currentIndex].nextIndex);
                }
                *prevPtr = primaryKeyList[currentIndex].nextIndex; // Update the previous node's nextIndex to current node's nextIndex
                releaseLabelId(currentIndex);  // Release the label ID of the removed node
                break;
            }
            prevPtr = &primaryKeyList[currentIndex].nextIndex; // address of current node's nextIndex
            currentIndex = stoi(primaryKeyList[currentIndex].nextIndex); // value of current node's index
        }

        if (currentIndex == -1) {
            cerr << "Error: Primary key not found.\n";
        }

        updateSecondaryIndexAndLabelIdList();
    }

    vector<string> getPrimaryKeysBySecondaryKey(const string &secondaryKey) {
        vector<string> primaryKeys;

        if (secondaryIndexMap.find(secondaryKey) == secondaryIndexMap.end()) {
            cerr << "Error: Secondary key not found.\n";
            return primaryKeys;
        }

        int index = secondaryIndexMap[secondaryKey];
        unordered_set<int> visitedIndices;

        while (index != -1) {
            if (visitedIndices.find(index) != visitedIndices.end()) {
                cerr << "Error: Circular reference detected.\n";
                break;
            }
            if (index >= primaryKeyList.size()) {
                cerr << "Error: Index out of bounds.\n";
                break;
            }

            visitedIndices.insert(index);
            primaryKeys.push_back(primaryKeyList[index].primaryKey);

            try {
                index = stoi(primaryKeyList[index].nextIndex);
            } catch (const invalid_argument&) {
                cerr << "Error: Invalid nextIndex format.\n";
                break;
            } catch (const out_of_range&) {
                cerr << "Error: nextIndex out of range.\n";
                break;
            }
        }

        return primaryKeys;
    }

};

#endif //HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
