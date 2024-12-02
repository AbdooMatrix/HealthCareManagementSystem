#ifndef HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H
#define HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H

#include <iostream>
#include "AvailListNode.h"

using namespace std;

class AvailList {
private:
    AvailListNode *header;
public:
    AvailList() : header(nullptr) {}

    void insert(AvailListNode *newNode) {
        if (header == nullptr) { // Empty list
            header = newNode;
        }
        else { // Insert in sorted order
            AvailListNode *prev = nullptr;
            AvailListNode *curr = header;
            while (curr != nullptr && curr->size < newNode->size) {
                prev = curr;
                curr = curr->next;
            }
            prev->next = newNode;
            newNode->next = curr;
        }
    }


    AvailListNode* bestFit(int newSize){ // offset - size
        AvailListNode* curr = header ;
        while(curr != nullptr && curr->size < newSize){
            curr = curr->next ;
        }
        return curr ;
    }

    bool empty() const { return header == nullptr; }

    ~AvailList() {
        while (header) {
            AvailListNode *temp = header;
            header = header->next;
            delete temp;
        }
    }

    void remove(AvailListNode *nodeToRemove) {
        if (header == nullptr || nodeToRemove == nullptr) {
            return; // List is empty or invalid node
        }

        if (header == nodeToRemove) { // Node to remove is the head
            header = header->next;
            delete nodeToRemove;
            return;
        }

        AvailListNode *prev = nullptr;
        AvailListNode *curr = header;

        while (curr != nullptr && curr != nodeToRemove) {
            prev = curr;
            curr = curr->next;
        }

        if (curr == nodeToRemove) { // Node found
            prev->next = curr->next;
            delete curr;
        }
    }


};

#endif //HEALTHCAREMANAGEMENTSYSTEM_AVAILLIST_H
