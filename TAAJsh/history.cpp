#include "./history.hpp"
#include <unistd.h>
using namespace std;

History::History() : index(-1)
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
    arr.clear();
}

void History::addHistory(const std::string __str)
{
    if (arr.size() == CACHE_SIZE)
        arr.pop_back();
    arr.push_front(__str);
}

string History::getHistory(DIREC dir)
{
    string temp;

    if (dir == UP)
    {
        if (index < static_cast<int>(arr.size()-1))
            temp = arr[++index];
        else
            temp = arr[index];
    }
    else if (dir == DOWN)
    {
        if (index > 0)
            temp = arr[--index];
        else{
            index = -1;
            temp = "";
        }
    }
    return temp;
}

void History::resetHistory()
{
    index = -1;
}

void History::updateHistory()
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