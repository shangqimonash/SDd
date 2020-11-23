#include <iostream>
#include <chrono>

#include "SSEClient.h"

using namespace chrono;

int main() {
    SSEClient client(10000);

    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;
    for (int i = 0; i < 10000; ++i) {
        client.update(INS, "test", i);
    }
    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;

    for (int i = 0; i < 100; ++i) {
        client.update(DEL, "test", i);
    }

    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;
    vector<int> results = client.search("test");
    cout << duration_cast<microseconds>(system_clock::now().time_since_epoch()).count() << endl;
    for (int res : results) {
        cout << res << endl;
    }
    return 0;
}
