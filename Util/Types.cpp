//
// Created by shangqi on 2020/11/21.
//

#include "Types.h"


bytes<AES_BLOCK_SIZE> generate_IV() {
    bytes<AES_BLOCK_SIZE> iv;
    RAND_bytes(iv.data(), iv.size());

    return iv;
}

block encrypt(bytes<AES_BLOCK_SIZE> key, block plaintext, size_t cipher_length, size_t plain_length) {
    bytes<AES_BLOCK_SIZE> iv = generate_IV();

    uint8_t output[cipher_length];
    aes_encrypt(plaintext.data(), plain_length, key.data(), iv.data(), output);

    // Put randomised IV at the front of the ciphertext
    block ciphertext(output, output + cipher_length);
    ciphertext.insert(ciphertext.end(), iv.begin(), iv.end());
    return ciphertext;
}

block decrypt(bytes<AES_BLOCK_SIZE> key, block ciphertext, size_t cipher_length) {
    bytes<AES_BLOCK_SIZE> iv;
    std::copy(ciphertext.end() - AES_BLOCK_SIZE, ciphertext.end(), iv.begin());

    // Perform the decryption
    uint8_t output[cipher_length];
    int plain_length = aes_decrypt(ciphertext.data(), cipher_length, key.data(), iv.data(), output);

    block plaintext(output, output + plain_length);
    return plaintext;
}