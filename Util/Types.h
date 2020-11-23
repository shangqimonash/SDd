//
// Created by shangqi on 2020/11/21.
//

#ifndef SDD_TYPES_H
#define SDD_TYPES_H

#include <array>
#include <vector>
#include <iostream>

extern "C" {
#include "CommonUtil.h"
};


// The main type for passing around raw file data
#define ID_SIZE 16

using byte_t = uint8_t;
using block = std::vector<byte_t>;

template <size_t N>
using bytes = std::array<byte_t, N>;

// A bucket contains a number of Blocks
constexpr int Z = 4;

enum Op {
    READ,
    WRITE
};

enum OP {
    INS, DEL
};

template< typename T >
std::array< byte_t, sizeof (T) > to_bytes(const T& object) {
    std::array< byte_t, sizeof (T) > bytes;

    const auto* begin = reinterpret_cast<const byte_t*> (std::addressof(object));
    const byte_t* end = begin + sizeof (T);
    std::copy(begin, end, std::begin(bytes));

    return bytes;
}

template< typename T >
T& from_bytes(const std::array< byte_t, sizeof (T) >& bytes, T& object) {
    auto *begin_object = reinterpret_cast<byte_t *> (std::addressof(object));
    std::copy(std::begin(bytes), std::end(bytes), begin_object);

    return object;
}

bytes<AES_BLOCK_SIZE> generate_IV();

block encrypt(bytes<AES_BLOCK_SIZE> key, block plaintext, size_t cipher_length, size_t plain_length);

block decrypt(bytes<AES_BLOCK_SIZE> key, block ciphertext, size_t cipher_length);

#endif //SDD_TYPES_H
