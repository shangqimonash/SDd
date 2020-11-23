//
// Created by shangqi on 2020/11/21.
//

#include "RAMStore.h"

#include <utility>

using namespace std;

RAMStore::RAMStore(size_t count, size_t size)
        : store(count), size(size) {
}

block RAMStore::Read(int pos) {
    return store[pos];
}

void RAMStore::Write(int pos, block b) {
    store[pos] = std::move(b);
}
