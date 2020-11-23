//
// Created by shangqi on 2020/11/23.
//

#include "SSEServer.h"

void SSEServer::initialise(const int32_t index) {
    this->handler = new DeAmortizedBASServerHandler(index);
}

bool SSEServer::exist(const int32_t instance, const int32_t index) {
    return handler->exist(instance, index);
}
void SSEServer::add(const int32_t instance, const int32_t index, const std::string& key_str, const std::string& value_str) {
    prf_type key, value;
    memcpy(key.data(), key_str.data(), AES_BLOCK_SIZE);
    memcpy(value.data(), value_str.data(), AES_BLOCK_SIZE);
    handler->add(instance, index, pair<prf_type, prf_type>(key, value));
}

void SSEServer::get(std::string& _return, const int32_t instance, const int32_t index, const int32_t pos) {
    _return = string((char*) (handler->get(instance, index, pos).data()), AES_BLOCK_SIZE);
}

void SSEServer::read_omap(std::string& _return, const int32_t instance, const serialised_bid& bid) {
    Bid id = Bid::get_bid(bid.input, bid.count);
    _return = handler->read_omap(instance, id);
}

void SSEServer::put_omap(const int32_t instance, const serialised_bid& bid, const std::string& value) {
    Bid id = Bid::get_bid(bid.input, bid.count);
    handler->put_omap(instance, id, value);
}

int32_t SSEServer::move(const int32_t instance) {
    return handler->move(instance);
}

int32_t SSEServer::finalise(const int32_t instance) {
    return handler->finalise(instance);
}

void SSEServer::search(std::vector<std::string> & _return, const int32_t instance, const int32_t index, const std::string& token) {
    prf_type search_token;
    memcpy(search_token.data(), token.data(), AES_BLOCK_SIZE);
    vector<prf_type> res_list = handler->search(instance, index, search_token);
    // convert result to string
    for(prf_type res : res_list) {
        _return.emplace_back(string((char*) res.data(), AES_BLOCK_SIZE));
    }
}

int main() {
    TSimpleServer server(
            make_shared<SSEServiceProcessor>(make_shared<SSEServer>()),
            make_shared<TServerSocket>("localhost", 9090),
            make_shared<TFramedTransportFactory>(),
            make_shared<TBinaryProtocolFactory>()
    );
    // start service
    server.serve();
}