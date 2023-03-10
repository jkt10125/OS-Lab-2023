#include <iostream>
#include <iomanip>
#include "definitions.hpp"
using namespace std;

uint64_t Action::postCount = 0;
uint64_t Action::commentCount = 0;
uint64_t Action::likeCount = 0;
Action::Action() : timestamp(time(NULL)) {}
Action::Action(int user, ACTION_TYPE type) : userId(user), actionId((type == POST) ? ++postCount : ((type == COMMENT) ? ++commentCount : ++likeCount)), actionType(type), timestamp(time(NULL)) {}
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
    os << " (ID: " << a.actionId << ") ";
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

    vector<int> this_Friends;
    for (const pair<int, Friend> &it : friendList)
    {
        this_Friends.push_back(it.second.node->userId);
    }
    sort(this_Friends.begin(), this_Friends.end());

    for (const pair<int, Friend> &it : friendList)
    {
        if (nodes[userId]->friendList[it.second.node->userId].priority >= 0)
            continue;

        vector<int> that_Friends;
        for (const pair<int, Friend> &it2 : it.second.node->friendList)
        {
            that_Friends.push_back(it2.second.node->userId);
        }
        sort(that_Friends.begin(), that_Friends.end());
        int i = 0, j = 0, p = 0;
        while (i < this_Friends.size() && j < that_Friends.size())
        {
            if (this_Friends[i] < that_Friends[j])
                i++;
            else if (this_Friends[i] > that_Friends[j])
                j++;
            else
            {
                i++, j++, p++;
            }
        }
        nodes[userId]->friendList[it.second.node->userId].priority = p;
        nodes[it.second.node->userId]->friendList[userId].priority = p;
    }
}

ostream &operator<<(ostream &os, const Node &a)
{
    os << "User: " << a.userId << endl;
    os << "Prefers: " << ((a.preference == Node::CHRONOLOGICAL) ? "latest feed" : "feed from close friends") << endl;
    os << "Friend List: \n";
    for (const pair<int, Node::Friend> &it : a.friendList)
    {
        os << "\t" << it.first << "[" << it.second.priority << " mutual friends]" << endl;
    }

    Node::ActionQueue Q = a.wall;
    if (!Q.empty())
        os << "Wall: ";
    while (!Q.empty())
    {
        os << Q.top().actionId << " ";
        Q.pop();
    }
    os << endl;

    Q = a.feed;
    if (!Q.empty())
        os << "Feed: ";
    while (!Q.empty())
    {
        os << Q.top().actionId << " ";
        Q.pop();
    }
    os << endl;

    return os;
}
