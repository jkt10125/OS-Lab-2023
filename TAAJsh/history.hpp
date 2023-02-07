#ifndef HISTORY_HPP
#define HISTORY_HPP
#include <iostream>
#include <deque>
#include <fstream>
const std::string HISTORY_CACHE = ".taajshcache";
const int CACHE_SIZE = 1000;
enum DIR
{
    UP,
    DOWN
};

class History
{
    std::deque<std::string> arr;
    size_t index;

public:
    History();
    ~History();
    void addHistory(const std::string __str);
    std::string getHistory(DIR dir);
    void resetHistory();
};
#endif