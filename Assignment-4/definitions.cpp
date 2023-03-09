#include <iostream>
#include <iomanip>
#include "definitions.hpp"
using namespace std;

uint64_t Action::actionCount = 0;
Action::Action() : timestamp(time(NULL)) {}
Action::Action(int user, ACTION_TYPE type) : userId(user), actionId(actionCount), actionType(type), timestamp(time(NULL)) {}
Action::Action(const Action &action) : userId(action.userId), actionId(action.actionId), actionType(action.actionType), timestamp(action.timestamp) {}
Action::~Action() {}
Action &Action::operator=(const Action &action)
{
    if (this != &action)
    {
        userId = action.userId;
        actionId = action.actionId;
        actionType = action.actionType;
        timestamp = action.timestamp;
    }
    return *this;
}

ostream &operator<<(ostream &os, const Action &a)
{
    os << "user#" << a.userId << " ";
    os << (a.actionType == Action::POST ? "posted" : (a.actionType == Action::LIKE ? "liked" : "commented"));
    os << " (ID:" << a.actionId << ") ";
    os << put_time(localtime(&a.timestamp), "at %H:%M:%S on %B %d, %Y") << endl;
    return os;
}

istream &operator>>(istream &is, Action &a)
{
    string type;
    is >> a.userId;
    is >> a.actionId;
    is >> type;
    if (tolower(type[0]) == 'p')
        a.actionType = Action::POST;
    else if (tolower(type[0]) == 'l')
        a.actionType = Action::LIKE;
    else if (tolower(type[0]) == 'c')
        a.actionType = Action::COMMENT;
    else
        cerr << "Invalid Action Type" << endl;
    a.timestamp = time(NULL);
    return is;
}

Node::Friend::Friend() : node(NULL), priority(0) {}
Node::Friend::Friend(Node *n, int p) : node(n), priority(p) {}
Node::Friend::Friend(const Friend &a) : node(a.node), priority(a.priority) {}
Node::Friend &Node::Friend::operator=(const Friend &a)
{
    if (this != &a)
    {
        node = a.node;
        priority = a.priority;
    }
    return *this;
}

Node::Node() : userId(time(NULL)), preference(CHRONOLOGICAL){};
Node::Node(int user, PREFERENCE pref) : userId(user), preference(pref), wall([](const Action &a, const Action &b) -> bool
                                                                             { return a.timestamp < b.timestamp; })
{
    Comparator comp;
    if (preference == PRIORITY)
        comp = [this](const Action &a, const Action &b) -> bool
        { return friendList[a.userId].priority < friendList[b.userId].priority; };
    else
        comp = [](const Action &a, const Action &b) -> bool
        { return a.timestamp < b.timestamp; };

    feed = ActionQueue(comp);
}

Node::Node(const Node &node) : userId(node.userId), preference(node.preference), friendList(node.friendList), wall(node.wall), feed(node.feed) {}

Node::~Node()
{
    friendList.clear();
}

void Node::initPriority()
{
        // using Comparator_ = function<bool(pair<int, Friend> a,pair<int, Friend> b)>;
        // Comparator_ comp;
        // comp = [](pair<int, Friend> a, pair<int, Friend> b) -> bool
        // { return a.second.node->userId < b.second.node->userId; };
        // map<int, Friend>::iterator ls;
        // map<int, Friend> mp;

        // for (const pair<int, Friend> &it : friendList)
        // {
        //     ls = std::set_intersection(friendList.begin(), friendList.end(), nodes[it.first]->friendList.begin(), nodes[it.first]->friendList.end(), mp.begin(), comp);

        // }
        using Comparator_ = function<bool(pair<int, Friend> a,pair<int, Friend> b)>;
        Comparator_ comp;
        comp = [](pair<int, Friend> a, pair<int, Friend> b) -> bool
        { return a.second.node->userId < b.second.node->userId; };

        vector<pair<int, Friend>> this_Friends, output;
        for (const pair<int, Friend> &it : friendList)
        {
            this_Friends.push_back(it);
        }
        sort(this_Friends.begin(), this_Friends.end(), comp);
        vector<pair<int, Friend>>::iterator ls;

        for (const pair<int, Friend> &it : friendList)
        {
            vector<pair<int, Friend>> that_Friends;
            for (const pair<int, Friend> &it2 : it.second.node->friendList)
            {
                that_Friends.push_back(it2);
            }
            sort(that_Friends.begin(), that_Friends.end(), comp);
            ls = std::set_intersection(this_Friends.begin(), this_Friends.end(), that_Friends.begin(), that_Friends.end(), output.begin(), comp);
            friendList[it.first].priority = ls - output.begin();
        }
}

ostream &operator<<(ostream &os, const Node &a)
{
    os << "User: " << a.userId << endl;
    os << "Prefers: " << ((a.preference == Node::CHRONOLOGICAL) ? "latest feed" : "feed from close friends") << endl;
    os << "Friend List: ";
    for (const pair<int, Node::Friend> &it : a.friendList)
    {
        os << it.first << " ";
    }
    os << endl;

    Node::ActionQueue Q = a.wall;
    os << "Wall: ";
    while (!Q.empty())
    {
        os << Q.top().actionId << " ";
        Q.pop();
    }
    os << endl;

    Q = a.feed;
    os << "Feed: ";
    while (!Q.empty())
    {
        os << Q.top().actionId << " ";
        Q.pop();
    }
    os << endl;

    return os;
}
