//
// Created by shangqi on 2020/11/23.
//

#ifndef SDD_SSESERVER_H
#define SDD_SSESERVER_H

#include <thrift/server/TSimpleServer.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/transport/TSocket.h>

#include "DeAmortizedBASServerHandler.h"
#include "../gen-cpp/SSEService.h"

using namespace server;
using namespace apache::thrift;
using namespace apache::thrift::concurrency;
using namespace apache::thrift::server;
using namespace apache::thrift::transport;

class SSEServer : public SSEServiceIf {
private:
    DeAmortizedBASServerHandler *handler;

public:
    SSEServer() = default;
    void initialise(const int32_t index) override;
    bool exist(const int32_t instance, const int32_t index) override;
    void add(const int32_t instance, const int32_t index, const std::string& key_str, const std::string& value_str) override;
    void get(std::string& _return, const int32_t instance, const int32_t index, const int32_t pos) override;
    void read_omap(std::string& _return, const int32_t instance, const serialised_bid& bid) override;
    void put_omap(const int32_t instance, const serialised_bid& bid, const std::string& value) override;
    int32_t move(const int32_t instance) override;
    int32_t finalise(const int32_t instance) override;
    void search(std::vector<std::string> & _return, const int32_t instance, const int32_t index, const std::string& token) override;
};


#endif //SDD_SSESERVER_H
