//
// Created by shangqi on 2020/11/21.
//

#ifndef SDD_OMAP_H
#define SDD_OMAP_H

#include <iostream>
#include <functional>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <iostream>

#include "AVLTree.h"
#include "ORAM.h"
using namespace std;

class OMAP {
private:
    Bid rootKey;
    int rootPos;

public:
    AVLTree* treeHandler;
    OMAP(int maxSize, bytes<AES_BLOCK_SIZE> key);
    virtual ~OMAP();
    void insert(Bid key, string value);
    string find(Bid key);
    void printTree();
    void batchInsert(map<Bid, string> pairs);
    void setupInsert(map<Bid, string> pairs);
    vector<string> batchSearch(vector<Bid> keys);
    void setDummy(int i);
    string incrementCnt(Bid key);
};
#endif //SDD_OMAP_H
