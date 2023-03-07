#include <iostream>
#include "definitions.hpp"
using namespace std;

uint64_t Action::actionCount = 0;
Action::Action() {}
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
    os << "User: " << a.userId << endl;
    os << "ID: " << a.actionId << endl;
    os << "Type: " << (a.actionType == Action::POST ? "Post" : (a.actionType == Action::LIKE ? "Like" : "Comment")) << endl;
    os << "Time: " << asctime(localtime(&(a.timestamp))) << endl;
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
    for (const pair<int, Friend> &it : friendList)
    {
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
        cout << Q.top().actionId << " ";
        Q.pop();
    }
    cout << endl;

    Q = a.feed;
    cout << "Feed: ";
    while (!Q.empty())
    {
        cout << Q.top().actionId << " ";
        Q.pop();
    }
    cout << endl;

    return os;
}

int getRandom(int a, int b)
{
    return a + rand() % (a - b + 1);
}