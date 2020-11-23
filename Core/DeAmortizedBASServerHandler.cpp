//
// Created by shangqi on 2020/11/21.
//

#include "DeAmortizedBASServerHandler.h"

#include <utility>

DeAmortizedBASServerHandler::DeAmortizedBASServerHandler(int index) {
    for (int i = 0; i < index; i++) {
        // initialise l instance
        data.emplace_back();
        for (int j = 0; j < 4; j++) { // OLDEST, OLDER, OLD, NEW
            auto* curData = new EachSet();
            data[i].emplace_back(curData);
        }
        // initialise OMAP
        bytes<AES_BLOCK_SIZE> key{0};
        OMAP* omap = new OMAP(max((int) pow(2, index - i), 4), key);
        omaps.emplace_back(omap);
    }
}

bool DeAmortizedBASServerHandler::exist(int instance, int index) {
    return !data[instance][index]->setData.empty();
}

prf_type DeAmortizedBASServerHandler::get(int instance, int index, int pos) {
    auto iter = data[instance][index]->setData.begin();
    for (int i = 0; i < pos; i++) {
        iter++;
    }
    return (*iter).second;
}

void DeAmortizedBASServerHandler::add(int instance, int index, pair<prf_type, prf_type> pair) {
    data[instance][index]->setData.insert(pair);
}

string DeAmortizedBASServerHandler::read_omap(int instance, const Bid &bid) {
    return omaps[instance]->find(bid);
}

void DeAmortizedBASServerHandler::put_omap(int instance, const Bid &bid, string value) {
    if(!value.empty()) {
        omaps[instance]->insert(bid, std::move(value));
    }
}

int DeAmortizedBASServerHandler::move(int instance) {
    // |NEW_i| == 2^i
    if(pow(2, instance) == data[instance][3]->setData.size()) {
        // OLD->OLDEST
        data[instance - 1][0]->setData.clear();
        data[instance - 1][0]->setData.insert(data[instance - 1][2]->setData.begin(), data[instance - 1][2]->setData.end());
        // clear OLDER
        data[instance - 1][1]->setData.clear();
        data[instance - 1][2]->setData.clear();
        // move db in i-level
        return finalise(instance);
    }
    // no move
    return -1;
}

int DeAmortizedBASServerHandler::finalise(int instance){
    if (data[instance][0]->setData.empty()) {
        data[instance][0]->setData.insert(data[instance][3]->setData.begin(), data[instance][3]->setData.end());
        data[instance][3]->setData.clear();
        return 0;
    } else if (data[instance][1]->setData.empty()) {
        data[instance][1]->setData.insert(data[instance][3]->setData.begin(), data[instance][3]->setData.end());
        data[instance][3]->setData.clear();
        return 1;
    } else {
        data[instance][2]->setData.insert(data[instance][3]->setData.begin(), data[instance][3]->setData.end());
        data[instance][3]->setData.clear();
        return 2;
    }
}

vector<prf_type> DeAmortizedBASServerHandler::search(int instance, int index, prf_type token) {
    vector<prf_type> res;
    bool exist;
    int cnt = 0;
    do {
        prf_type mapKey;
        hmac_digest((unsigned char*) &cnt, sizeof(int), token.data(), AES_BLOCK_SIZE, mapKey.data());
        if (data[instance][index]->setData.count(mapKey) != 0) {
            res.push_back(data[instance][index]->setData[mapKey]);
            exist = true;
            cnt++;
        } else {
            exist = false;
        }
    } while (exist);
    return res;
}