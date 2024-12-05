#ifndef HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H
#define HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H

#include <iostream>

using namespace std;

class AvailListNode {
public:
    int offset;
    int size;
    AvailListNode *next;

    AvailListNode(int offset, int size) : offset(offset), size(size), next(nullptr) {}
};

class AvailList {
private:
    string availListFileName;
    AvailListNode *header;

public:
    AvailList() : header(nullptr) {}

    void setAvailListFileName(const string& fileName) {
        this->availListFileName = fileName;
        loadAvailListInMemory();
    }

    bool empty() const {
        return header == nullptr;
    }

    void insert(AvailListNode *newNode) {
        if (header == nullptr) { // Empty list
            header = newNode;
        } else { // Insert in sorted order
            AvailListNode *prev = nullptr;
            AvailListNode *curr = header;

            // Traverse to find the correct position
            while (curr != nullptr && curr->size < newNode->size) {
                prev = curr;
                curr = curr->next;
            }

            if (prev == nullptr) { // Insert at the beginning
                newNode->next = header;
                header = newNode;
            } else { // Insert in the middle or end
                prev->next = newNode;
                newNode->next = curr;
            }
        }
        updateAvailListFile();
    }

    void remove(AvailListNode *nodeToRemove, const string &fileName) {
        if (header == nullptr || nodeToRemove == nullptr) {
            return; // List is empty or invalid node
        }

        // Case 1: Node to remove is the head of the list
        if (header == nodeToRemove) {
            header = header->next;
            delete nodeToRemove;
            updateAvailListFile();  // Update the file after removing the node
            return;
        }

        // Case 2: Node to remove is not the head
        AvailListNode *prev = nullptr;
        AvailListNode *curr = header;

        // Traverse the list to find the node to remove
        while (curr != nullptr && curr != nodeToRemove) {
            prev = curr;
            curr = curr->next;
        }

        // If the node is found, remove it
        if (curr == nodeToRemove) {
            prev->next = curr->next;
            delete curr;
            updateAvailListFile();  // Update the file after removing the node
        }
    }

    AvailListNode *bestFit(int newSize) { // offset - size
        AvailListNode *curr = header;
        while (curr != nullptr && curr->size < newSize) {
            curr = curr->next;
        }
        return curr;
    }

    void loadAvailListInMemory() {
        fstream availListFile(availListFileName, ios::in);

        if (!availListFile.is_open()) {
            cerr << "Error opening file: doctors.txt\n";
            return;
        }
        if (isFileEmpty(availListFileName)) {
            return;
        }

        string line;

        while (getline(availListFile, line)) {
            istringstream stream(line);
            string offset, size;
            getline(stream, offset, '|');
            getline(stream, size, '|');

            AvailListNode *newNode = new AvailListNode(stoi(offset), stoi(size));
            insert(newNode);
        }
    }

    void updateAvailListFile() {
        // Open the file in output mode (overwrites the file)
        fstream availFile(availListFileName, ios::out);

        if (!availFile.is_open()) {
            cerr << "Error opening file: " << availListFileName << endl;
            return;
        }

        AvailListNode *curr = header;

        // Traverse through the list and write each node's data to the file
        while (curr != nullptr) {
            // Assuming status '*' for available nodes, and writing the size
            availFile << curr->offset << "|" << curr->size << '\n';
            curr = curr->next;
        }
        availFile.close();  // Ensure to close the file after writing
    }

    void printAvailList() {
        AvailListNode *curr = header;

        // Traverse through the list and write each node's data to the file
        while (curr != nullptr) {
            // Assuming status '*' for available nodes, and writing the size
            cout << curr->offset << "->" << curr->size << '\n';
            curr = curr->next;
        }
    }

    ~AvailList() {
        while (header) {
            AvailListNode *temp = header;
            header = header->next;
            delete temp;
        }
    }

};

#endif //HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H
