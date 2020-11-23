#ifndef SDD_BID_H
#define SDD_BID_H

#include <array>
#include <string>

#include "Types.h"

using namespace std;

class Bid {
public:
    std::array< byte_t, ID_SIZE> id;
    Bid();
    Bid(int value);
    Bid(std::array< byte_t, ID_SIZE> value);
    Bid(string value);
    virtual ~Bid();
    Bid operator++();
    Bid& operator=(int other);
    bool operator!=(const int rhs) const;
    bool operator!=(const Bid rhs) const;
    bool operator==(const int rhs) const;
    bool operator==(const Bid rhs) const;
    Bid& operator=(std::vector<byte_t> other);
    Bid& operator=(Bid const &other);
    bool operator<(const Bid& b) const;
    bool operator<(int b) const;
    bool operator>(const Bid& b) const;
    bool operator<=(const Bid& b) const;
    bool operator>=(const Bid& b) const;
    bool operator>=(int b) const;
    int getValue();
    void setValue(int value);
    friend ostream& operator<<(ostream &o, Bid& id);
    static Bid get_bid(string input, int cnt);
};

#endif //SDD_BID_H
