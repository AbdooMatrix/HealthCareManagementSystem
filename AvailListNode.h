#ifndef HEALTHCAREMANAGEMENTSYSTEM_AVAILLISTNODE_H
#define HEALTHCAREMANAGEMENTSYSTEM_AVAILLISTNODE_H

class AvailListNode {
public:
    int offset;
    int size;
    AvailListNode *next;

    AvailListNode(int offset, int size) : offset(offset), size(size), next(nullptr) {}
};

#endif //HEALTHCAREMANAGEMENTSYSTEM_AVAILLISTNODE_H
