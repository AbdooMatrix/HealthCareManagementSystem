#ifndef HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H
#define HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H

#include <bits/stdc++.h>

using namespace std;

// node has a --> vector
class SecondaryIndexNode {
public:
    string secondaryKey;
    vector<string> primaryKeys;

    SecondaryIndexNode(const string &secondaryKey, vector<string> & vec) {
        this->secondaryKey = secondaryKey ;
        this->primaryKeys = vec ;
    }
    bool operator<(const SecondaryIndexNode &other) const {
        return (this->secondaryKey < other.secondaryKey);
    }
};

class SecondaryIndex {
private:
    vector<SecondaryIndexNode> secondaryIndex;

public:
    void loadSecondaryIndexInMemory(const string &fileName)
    {
        ifstream file(fileName, ios::in);
        if (!file.is_open()) {
            cerr << "Error opening file: PrimaryIndex.txt\n";
            return;
        }
        if (isFileEmpty(fileName)) {
            return;
        }

        string line;
        while (getline(file, line)) { // omar mohamed|1,2,3

            istringstream recordStream(line);
            string secondaryKey ;
            string primarykey ;

            getline(recordStream, secondaryKey , '|');


            vector<string>pks ; // vector to store the primary keys.
            while(getline(recordStream ,primarykey  , ',')){
                pks.emplace_back(primarykey) ;
            }
            secondaryIndex.emplace_back(secondaryKey , pks) ;
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
            outFile << ele.secondaryKey << '|' ;
            for(auto& pk : ele.primaryKeys ){
                outFile << pk  ;
                if(pk != ele.primaryKeys.back()){
                    outFile << ',' ;
                }
            }
        }
        outFile.close();
    }

    void sortSecondaryIndex(){
        sort(secondaryIndex.begin() , secondaryIndex.end()) ;
        for (auto &node : secondaryIndex) {
            sort(node.primaryKeys.begin(), node.primaryKeys.end());
        }
    }

    void addSecondaryNode(const string &secondaryKey, vector<string> & vec , const string &fileName) {
        secondaryIndex.emplace_back(secondaryKey , vec) ;
        sortSecondaryIndex();
        updateSecondaryIndexFile(fileName);
    }

    void addPrimaryKey(const string &secondaryKey, string primarykey , const string &fileName)
    {
        int index = binarySearchSecondaryIndex(secondaryKey) ;
        secondaryIndex[index].primaryKeys.emplace_back(primarykey) ;
        sortSecondaryIndex() ;
        updateSecondaryIndexFile(fileName) ;
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

    void removePrimaryKey(const string & secondaryKey ,const string & primaryKey , const string & fileName){
        int indexNode = binarySearchSecondaryIndex(secondaryKey) ; // find node
        int indexKey = binarySearchPrimaryKeys(indexNode,primaryKey) ; // find key
        secondaryIndex[indexNode].primaryKeys.erase(secondaryIndex[indexNode].primaryKeys.begin() + indexKey) ;
        sortSecondaryIndex() ;
        updateSecondaryIndexFile(fileName) ;
    }

    int binarySearchSecondaryIndex(const string &secondaryKey) {
        int left = 0;
        int right = secondaryIndex.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (secondaryIndex[mid].secondaryKey == secondaryKey) {
                return mid ;
            } else if (secondaryKey < secondaryIndex[mid].secondaryKey) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }

    int binarySearchPrimaryKeys(int indexNode , const string & primaryKey) {

        int left = 0;
        int right = secondaryIndex[indexNode].primaryKeys.size() - 1;
        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (secondaryIndex[indexNode].primaryKeys[mid] == primaryKey) {
                return mid ;
            } else if (secondaryIndex[indexNode].primaryKeys[mid] < primaryKey) {
                right = mid - 1;
            } else {
                left = mid + 1;
            }
        }
        return -1;
    }

};


#endif //HEALTHCAREMANAGEMENTSYSTEM_SECONDARYINDEX_H