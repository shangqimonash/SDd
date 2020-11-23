//
// Created by shangqi on 2020/11/23.
//

#ifndef SDD_SSECLIENT_H
#define SDD_SSECLIENT_H

#include <thrift/protocol/TBinaryProtocol.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "DeAmortizedBASServerHandler.h"
#include "Utilities.h"
#include "../gen-cpp/SSEService.h"

using namespace server;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::protocol;
using namespace apache::thrift::transport;

class SSEClient {
private:
    // DB size and handler
    int l;

    vector<vector<unsigned char*>> keys;// key list
    vector<int> cnt;                    // counters
    int update_cnt;                     // update count

    static pair<string, string> map(string keyword, prf_type value, int counter, unsigned char *key);
    shared_ptr<TTransport> transport;
    SSEServiceClient *server;

public:
    explicit SSEClient(int N);
    ~SSEClient();

    void update(OP op, const string& keyword, int ind);
    vector<int> search(const string& keyword);
};



#endif //SDD_SSECLIENT_H
