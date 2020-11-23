//
// Created by shangqi on 2020/11/21.
//

#include "DeAmortizedBASClientHandler.h"

#include <utility>

DeAmortizedBASClientHandler::DeAmortizedBASClientHandler(int N) {
    this->l = ceil(log2(N));
    this->handler = new DeAmortizedBASServerHandler(l);

    for(int i = 0; i < l; i++) {
        // initialise counter
        cnt.emplace_back(0);
        // initialise key array
        keys.emplace_back(vector<unsigned char*>());
        for(int j = 0; j < 4; j++) {
            auto* tmpKey = new unsigned char[AES_BLOCK_SIZE]();
            for (int k = 0; k < AES_BLOCK_SIZE; k++) {
                tmpKey[k] = (unsigned char) rand() % 256;
            }
            keys[i].emplace_back(tmpKey);
        }
    }
    // global update counter
    update_cnt = 0;
}

void DeAmortizedBASClientHandler::update(OP op, string keyword, int ind) {
    // increase update count
    update_cnt++;
    for (int i = l - 1; i > 0; i--) {
        // fetch EDB here
        if(handler->exist(i - 1, 0) && handler->exist(i - 1, 1)) {
            // fetch a tuple based on the counter_i
            prf_type x;
            prf_type value;
            if(cnt[i] < pow(2, i - 1)) {
                x = handler->get(i - 1, 0, cnt[i]);
                Utilities::decode(x, value, keys[i - 1][0]);
            } else {
                x = handler->get(i - 1, 1, cnt[i] % (int) pow(2, i - 1));
                Utilities::decode(x, value, keys[i - 1][1]);
            }
            // parse the tuple
            string cur_keyword((char*) value.data());
            // increase counter
            cnt[i]++;
            // compute num
            int num = (int) ceil((update_cnt - (6 * pow(2, i - 1) - 2)) / pow(2, i)) + 1;
            // update OMAP
            Bid bid = Bid::get_bid(cur_keyword, num);
            string c_w = handler->read_omap(i, bid);
            int c;
            if(c_w.empty()) {
                c = 0;
            } else {
               c = stoi(c_w) + 1;
            }
            handler->put_omap(i, bid, to_string(c));
            // update NEW_i
            auto updated_pair = map(std::move(cur_keyword), value, c, keys[i][3]);
            handler->add(i, 3, updated_pair);
            // move database
            int key_pos = handler->move(i);
            if(key_pos != -1) {
                memcpy(keys[i - 1][0], keys[i - 1][2], AES_BLOCK_SIZE);
                memcpy(keys[i][key_pos], keys[i][3], AES_BLOCK_SIZE);
                cnt[i] = 0;
                // select a new key
                for (int j = 0; j < AES_BLOCK_SIZE; j++) {
                    keys[i][3][j] = (unsigned char) rand() % 256;
                }
            }
        }
    }
    // select a new key
    for (int j = 0; j < AES_BLOCK_SIZE; j++) {
        keys[0][3][j] = (unsigned char) rand() % 256;
    }
    // pre-process the key-value pair
    int new_cnt = 0;
    prf_type value;
    std::fill(value.begin(), value.end(), 0);
    std::copy(keyword.begin(), keyword.end(), value.begin());
    memcpy(value.data() + keyword.size() + 1, &ind, sizeof(int));
    value[keyword.size() + sizeof(int) + 1] = (op == OP::INS ? (uint8_t) 0 : (uint8_t) 1);
    // compute the encrypted key-value
    auto new_pair = map(std::move(keyword), value, new_cnt, keys[0][3]);
    // add to the server
    handler->add(0, 3, new_pair);
    // move EDB_0 on server and move the key on server
    int key_pos = handler->finalise(0);
    memcpy(keys[0][key_pos], keys[0][3], AES_BLOCK_SIZE);
}

vector<int> DeAmortizedBASClientHandler::search(const string& keyword) {
    vector<prf_type> res_list;
    vector<int> final_res;
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < 3; ++j) {
            prf_type token = Utilities::encode(keyword, keys[i][j]);
            auto res = handler->search(i, j, token);
            // recover results
            for (auto cipher : res) {
                prf_type plaintext;
                Utilities::decode(cipher, plaintext, keys[i][j]);
                res_list.push_back(plaintext);
            }
        }   
    }


    std::map<int, int> remove;
    for (auto decodedString : res_list) {
        int plaintext = *(int*) (decodedString.data() + keyword.size() + 1);
        remove[plaintext] += *(int*)(decodedString.data() + keyword.size() + sizeof(int) + 1);
    }

    for (auto const& cur : remove) {
        if (cur.second == 0) {
            final_res.emplace_back(cur.first);
        }
    }

    return final_res;
}

pair<prf_type, prf_type> DeAmortizedBASClientHandler::map(string keyword, prf_type value, int counter, unsigned char *key) {
    prf_type mapKey, mapValue;
    prf_type K1 = Utilities::encode(std::move(keyword), key);
    hmac_digest((unsigned char*) &counter, sizeof(int), K1.data(), AES_BLOCK_SIZE, mapKey.data());
    mapValue = Utilities::encode(string((char*) value.data(), AES_BLOCK_SIZE), key);
    return pair<prf_type, prf_type>(mapKey, mapValue);
}