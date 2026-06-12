#pragma once
#include<iostream>
#include<string>
using namespace std;

class Timestamp
{
public:
    Timestamp();
    Timestamp(int64_t microSecondsSinceEpoch);
    static Timestamp now();
    string toString() const;
private:
    int64_t _microSecondsSinceEpoch;

};