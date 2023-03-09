#ifndef DEFINITIONS_H
#define DEFINITIONS_H
#include <iostream>
#include <pthread.h>
#include <queue>
#include <functional>
#include <map>
#include <fstream>
#include <algorithm>
#include <iterator>

class Action;
class Node;
extern const int MAXNODES;
extern const int MAXEDGES;
extern const int MAX_PUSH_UPDATE_THREADS;
extern const int MAX_READ_POST_THREADS;
extern Node *nodes[];
extern pthread_mutex_t feedQmutex[];
extern std::queue<Action *> actionQueue;
extern pthread_mutex_t actionQmutex;
extern pthread_cond_t newActionGenerated;
extern std::queue<Node *> feedsUpdatedQueue;
extern pthread_mutex_t feedsUpdatedQmutex;
extern pthread_cond_t newUpdatesPushed;
extern std::ofstream logfile;
extern pthread_mutex_t fmutex;
extern pthread_mutex_t omutex;
extern long long PUops0[], PUops1[], RDops[];

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
    Action();
    Action(int user, ACTION_TYPE type);
    Action(const Action &action);
    ~Action();
    Action &operator=(const Action &action);
    friend std::ostream &operator<<(std::ostream &os, const Action &a);
    friend std::istream &operator>>(std::istream &is, Action &a);

private:
};

class Node
{
private:
    using Comparator = std::function<bool(const Action &a, const Action &b)>;
    using ActionQueue = std::priority_queue<Action, std::vector<Action>, Comparator>;

public:
    using Friend = struct Friend
    {
        Node *node;
        int priority;
        Friend();
        Friend(Node *n, int p = 0);
        Friend(const Friend &a);
        Friend &operator=(const Friend &a);
    };

    enum PREFERENCE
    {
        PRIORITY,
        CHRONOLOGICAL
    };

    int userId;
    const PREFERENCE preference;
    std::map<int, Friend> friendList;
    ActionQueue wall;
    ActionQueue feed;

    Node();
    Node(int user, PREFERENCE pref);
    Node(const Node &node);
    ~Node();

    inline size_t getDegree() const
    {
        return friendList.size();
    }

    void initPriority();

    friend std::ostream &operator<<(std::ostream &os, const Node &a);
};

inline int getRandom(int a, int b)
{
    return a + rand() % (b - a + 1);
}
void *userSimulatorRunner(void *);
void *pushUpdateRunner(void *param);
void *readPostRunner(void *param);

#endif