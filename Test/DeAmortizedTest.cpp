//
// Created by shangqi on 2020/11/21.
//
#include <iostream>

#include "DeAmortizedBASClientHandler.h"

int main() {
    DeAmortizedBASClientHandler client(100);

    for (int i = 0; i < 10; ++i) {
        client.update(INS, "test", i);
    }

    for (int i = 0; i < 5; ++i) {
        client.update(DEL, "test", i);
    }

    vector<int> results = client.search("test");
    for (int res : results) {
        cout << res << endl;
    }
    return 0;
}