//
// Created by shangqi on 2020/11/21.
//

#ifndef SDD_DEAMORTIZEDBASSERVER_H
#define SDD_DEAMORTIZEDBASSERVER_H

#include <string>
#include <map>
#include <vector>
#include <array>
#include <iostream>
#include <sstream>

#include "OMAP.h"

using namespace std;

typedef array<uint8_t, AES_BLOCK_SIZE> prf_type;
struct PRFHasher {

    std::size_t operator()(const prf_type &key) const {
        std::hash<byte_t> hasher;
        size_t result = 0; // I would still seed this.
        for (size_t i = 0; i < AES_BLOCK_SIZE; ++i) {
            result = (result << 1) ^ hasher(key[i]); // ??
        }
        return result;
    }
};

class EachSet {
public:
    unordered_map<prf_type, prf_type, PRFHasher> setData;
};

class DeAmortizedBASServerHandler {
private:
    vector<vector< EachSet* > > data;   //OLDEST, OLDER, OLD, NEW;
    vector<OMAP*> omaps;                // omap
//    void getAESRandomValue(unsigned char* keyword, int cnt, unsigned char* result);

public:
    explicit DeAmortizedBASServerHandler(int index);
    bool exist(int instance, int index);
    ~DeAmortizedBASServerHandler() = default;
    prf_type get(int instance, int index, int pos);
    void add(int instance, int index, pair<prf_type, prf_type> pair);
    string read_omap(int instance, const Bid &bid);
    void put_omap(int instance, const Bid &bid, string value = "");
    int move(int instance);
    int finalise(int instance);
    vector<prf_type> search(int instance, int index, prf_type token);
};


#endif //SDD_DEAMORTIZEDBASSERVER_H
