//
// Created by shangqi on 2020/11/23.
//

#include "SSEClient.h"

SSEClient::SSEClient(int N) {
    // establish connection with the server
    shared_ptr<TTransport> socket(new TSocket("localhost", 9090));
    transport = shared_ptr<TTransport>(new TFramedTransport(socket));
    shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));
    server = new SSEServiceClient(protocol);
    transport->open();
    // open server
    this->l = ceil(log2(N));
    server->initialise(l);

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

SSEClient::~SSEClient() {
    // delete SSE handler and shutdown the connection
    delete server;
    transport->close();
}

void SSEClient::update(OP op, const string& keyword, int ind) {
    // increase update count
    update_cnt++;
    for (int i = l - 1; i > 0; i--) {
        // fetch EDB here
        if(server->exist(i - 1, 0) && server->exist(i - 1, 1)) {
            // fetch a tuple based on the counter_i
            string x;
            prf_type value;
            if(cnt[i] < pow(2, i - 1)) {
                server->get(x, i - 1, 0, cnt[i]);
                memcpy(value.data(), x.data(), AES_BLOCK_SIZE);
                Utilities::decode(value, value, keys[i - 1][0]);
            } else {
                server->get(x, i - 1, 1, cnt[i] % (int) pow(2, i - 1));
                memcpy(value.data(), x.data(), AES_BLOCK_SIZE);
                Utilities::decode(value, value, keys[i - 1][1]);
            }
            // parse the tuple
            string cur_keyword((char*) value.data());
            // increase counter
            cnt[i]++;
            // compute num
            int num = (int) ceil((update_cnt - (6 * pow(2, i - 1) - 2)) / pow(2, i)) + 1;
            // update OMAP
            serialised_bid bid;
            bid.input = cur_keyword;
            bid.count = num;
            string c_w;
            server->read_omap(c_w, i, bid);
            int c;
            if(c_w.empty()) {
                c = 0;
            } else {
                c = stoi(c_w) + 1;
            }
            server->put_omap(i, bid, to_string(c));
            // update NEW_i
            auto updated_pair = map(std::move(cur_keyword), value, c, keys[i][3]);
            server->add(i, 3, updated_pair.first, updated_pair.second);
            // move database
            int key_pos = server->move(i);
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
    auto new_pair = map(keyword, value, new_cnt, keys[0][3]);
    // add to the server
    server->add(0, 3, new_pair.first, new_pair.second);
    // move EDB_0 on server and move the key on server
    int key_pos = server->finalise(0);
    memcpy(keys[0][key_pos], keys[0][3], AES_BLOCK_SIZE);
}

vector<int> SSEClient::search(const string& keyword) {
    vector<prf_type> res_list;
    vector<int> final_res;
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < 3; ++j) {
            prf_type token = Utilities::encode(keyword, keys[i][j]);
            string token_str = string((char*) token.data(), AES_BLOCK_SIZE);
            vector<string> res;
            server->search(res, i, j, token_str);
            // recover results
            for (const auto& cipher : res) {
                prf_type plaintext;
                memcpy(plaintext.data(), cipher.data(), AES_BLOCK_SIZE);
                Utilities::decode(plaintext, plaintext, keys[i][j]);
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

pair<string, string> SSEClient::map(string keyword, prf_type value, int counter, unsigned char *key) {
    prf_type mapKey, mapValue;
    prf_type K1 = Utilities::encode(std::move(keyword), key);
    hmac_digest((unsigned char*) &counter, sizeof(int), K1.data(), AES_BLOCK_SIZE, mapKey.data());
    mapValue = Utilities::encode(string((char*) value.data(), AES_BLOCK_SIZE), key);
    return pair<string, string>(string((char*)mapKey.data(), AES_BLOCK_SIZE),
                                    string((char*)mapValue.data(), AES_BLOCK_SIZE));
}