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

string History::getHistory(DIREC dir)
{
    string temp;
    
    if (dir == UP) {
        temp = arr[index];
        if (index < arr.size() - 1) index++;

    }
    else if (dir == DOWN) {
        temp = arr[index];
        if (index > 0) index--;
        else temp = "";
    }
    return temp;
}

void History::resetHistory()
{
    index = 0;
}