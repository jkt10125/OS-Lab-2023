#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unordered_map>
#include <functional>
#include <map>
#include <fstream>
#include <sstream>

using namespace std;
const int MAXNODES = 37700;
const int MAXEDGES = 289003;
const string INPUT_FILE = "musae_git_edges.csv";

class Action
{
public:
    static uint64_t actionCount;
    int userId;
    int actionId;
    enum ACTION_TYPE
    {
        POST,
        COMMENT,
        LIKE
    } actionType;
    time_t timestamp;
    Action() {}
    Action(int user, ACTION_TYPE type) : userId(user), actionId(actionCount), actionType(type), timestamp(time(NULL)) {}
    Action(const Action &action) : userId(action.userId), actionId(action.actionId), actionType(action.actionType), timestamp(action.timestamp) {}
    ~Action() {}
    Action &operator=(const Action &action)
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

    friend ostream &operator<<(ostream &os, const Action &a)
    {
        os << "User: " << a.userId << endl;
        os << "ID: " << a.actionId << endl;
        os << "Type: " << (a.actionType == Action::POST ? "Post" : (a.actionType == Action::LIKE ? "Like" : "Comment")) << endl;
        os << "Time: " << asctime(localtime(&(a.timestamp))) << endl;
        return os;
    }

    friend istream &operator>>(istream &is, Action &a)
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

private:
};

class Node
{
private:
    using Comparator = function<bool(const Action &a, const Action &b)>;
    using ActionQueue = priority_queue<Action, vector<Action>, Comparator>;

public:
    using Friend = struct Friend
    {
        Node *node;
        int priority;
        Friend(){};
        Friend(Node *n, int p = 0) : node(n), priority(p) {}
        Friend(const Friend &a) : node(a.node), priority(a.priority) {}
    };
    enum PREFERENCE
    {
        PRIORITY,
        CHRONOLOGICAL
    };

    int userId;
    const PREFERENCE preference;
    map<int, Friend> friendList;
    ActionQueue wall;
    ActionQueue feed;

    Node() : userId(time(NULL)), preference(CHRONOLOGICAL){};
    Node(int user, PREFERENCE pref) : userId(user), preference(pref), wall([](const Action &a, const Action &b) -> bool
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

    Node(const Node &node) : userId(node.userId), preference(node.preference), friendList(node.friendList), wall(node.wall), feed(node.feed) {}

    ~Node()
    {
        friendList.clear();
    }

    void initPriority()
    {
        for (const pair<int, Friend> &it : friendList)
        {
        }
    }

    friend ostream &operator<<(ostream &os, const Node &a)
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
};

Node **readGraph(const string &filename)
{
    Node **nodes = new Node *[MAXNODES];
    for (int i = 0; i < MAXNODES; i++)
    {
        nodes[i] = new Node(i, (rand() % 2) ? Node::PRIORITY : Node::CHRONOLOGICAL);
    }
    ifstream in(filename);
    string line;
    int u, v;
    getline(in, line);
    while (getline(in, line))
    {
        stringstream ss(line);
        ss >> u;
        ss.ignore();
        ss >> v;
        nodes[u]->friendList[v] = Node::Friend(nodes[u]);
        nodes[v]->friendList[u] = Node::Friend(nodes[u]);
    }
    for (int i = 0; i < MAXNODES; i++)
    {
        nodes[i]->initPriority();
    }
    return nodes;
}

Node **nodes;
uint64_t Action::actionCount = 0;

int main()
{
    nodes = readGraph(INPUT_FILE);
    for (int i = 0; i < MAXNODES; i++)
    {
        cout << *nodes[i] << endl;
    }
    return 0;
}