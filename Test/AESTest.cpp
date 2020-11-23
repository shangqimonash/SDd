//
// Created by shangqi on 2020/6/17.
//
#include <cstring>
#include <iostream>

extern "C" {
#include "CommonUtil.h"
}


using namespace std;

int main() {
    auto *key = (unsigned char*) "0123456789123456";
    auto *iv = (unsigned char*) "0123456789123456";
    auto *plaintext = (unsigned char*) "The test segmentation for AES_CBC mode";

    unsigned char ciphertext[128];

    // also consider \n here
    int ciphertext_len = aes_encrypt(plaintext, strlen((char*) plaintext) + 1,
                                     key, iv,
                                     ciphertext);

    cout << "Input size:" << ciphertext_len << endl;

    unsigned char recover[128];
    int plaintext_len = aes_decrypt(ciphertext, ciphertext_len,
                                    key, iv, recover);

    cout << "Output size:" << plaintext_len << endl;
    cout <<"Recovered string:" << recover << endl;
}