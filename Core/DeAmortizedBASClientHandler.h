//
// Created by shangqi on 2020/11/21.
//

#ifndef SDD_DEAMORTIZEDBASCLIENTHANDLER_H
#define SDD_DEAMORTIZEDBASCLIENTHANDLER_H

#include "DeAmortizedBASServerHandler.h"
#include "Utilities.h"

class DeAmortizedBASClientHandler {
private:
    // DB size and handler
    int l;
    DeAmortizedBASServerHandler *handler;

    vector<vector<unsigned char*>> keys;// key list
    vector<int> cnt;                    // counters
    int update_cnt;                     // update count

    static pair<prf_type, prf_type> map(string keyword, prf_type value, int counter, unsigned char *key);

public:
    explicit DeAmortizedBASClientHandler(int N);
    void update(OP op, string keyword, int ind);
    vector<int> search(const string& keyword);
};


#endif //SDD_DEAMORTIZEDBASCLIENTHANDLER_H
