#include "./history.hpp"

using namespace std;

History::History() : index(0)
{
    ifstream file(HISTORY_CACHE);
    if (!file.is_open())
    {
        perror("can't open history file");
    }

    string line;
    while (getline(file, line, '\n'))
    {
        arr.push_back(line);
    }
    file.close();
}
History::~History()
{
    ofstream file(HISTORY_CACHE);
    if (!file.is_open())
    {
        perror("can't open history file");
    }

    for (auto line : arr)
    {
        file << line << "\n";
    }

    file.close();
}

void History::addHistory(const std::string __str)
{
    if (arr.size() == CACHE_SIZE)
        arr.pop_back();
    arr.push_front(__str);
}

string History::getHistory(DIR dir)
{
    string temp = arr[index];
    index = (index + 1) % arr.size();
    return temp;
}

void History::resetHistory()
{
    index = 0;
}