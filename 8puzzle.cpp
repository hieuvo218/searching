#include <iostream>
#include <math.h>
#include <vector>
#include <map>

using namespace std;

enum slide {top, right, bottom, left};

struct Node
{
public:
    vector<int> state;
    int cost;
    int action;
    Node * parent;
    Node * next;
    Node(const vector<int> & table, Node * parent, int action, int cost): state(table), parent(parent), action(action), cost(cost), next(nullptr) {}
    Node(const vector<int> & table): state(table), cost(0), action(0), parent(nullptr), next(nullptr) {}
};

class Problem
{
public:
    vector<int> initial {4, 5, 1, 8, 0, 3, 2, 7, 6};
    vector<slide> action(const vector<int> & state);
    bool isGoal(const vector<int> & state);
    vector<int> result(vector<int> & state, int action);
};

int manhattan(int pos, int des)
{
    int temp = abs(des - pos);
    return temp/3 + temp%3;
}

int heuristic(const vector<int> & table)
{
    int h = 0;
    for(int i = 0; i < table.size(); i++) {
        h += manhattan(i, table[i]);
    }
    return h;
}

class PriorityQueue
{
private:
    Node * head;

    void insert(Node **head, Node * node) 
    {
        if(!*head || (*head)->cost + heuristic((*head)->state) >= node->cost + heuristic(node->state)) {
            node->next = *head;
            *head = node;
            return;
        }
        Node *current = *head;
        while (current->next != nullptr && current->next->cost + heuristic(current->next->state) < node->cost + heuristic(node->state))
            current = current->next;
        
        node->next = current->next;
        current->next = node;
    }

public:
    PriorityQueue(): head(nullptr) {}
    PriorityQueue(Node * node): head(node) {}

    void insert(Node * node) 
    {
        insert(&head, node);
    }

    bool empty() const
    {
        return head;
    }

    Node pop()
    {
        Node * temp = head;
        head = head->next;
        return *temp;
    }
};

vector<slide> s(int pos, const vector<int> & table)
{
    vector<slide> ret;
    if(pos % 3 < 2)
        ret.push_back(slide::right);
    if(pos / 3 > 0)
        ret.push_back(bottom);
    if(pos % 3 > 0)
        ret.push_back(slide::left);
    if(pos / 3 < 2)
        ret.push_back(top);
    return ret;
}


vector<int> swap(const vector<int> & table, slide d)
{
    int pos = 0;
    for(; pos < table.size(); pos++) {
        if(table[pos] == 0) 
            break;
    }
    int tile;
    if(d == top) tile = pos - 3;
    else if (d == slide::left) tile = pos - 1;
    else if (d == slide::right) tile = pos + 1;
    else tile = pos + 3;
    vector<int> new_table = table;
    new_table[pos] = new_table[tile];
    new_table[tile] = 0;
    return new_table;
}

vector<Node> expand(Problem p, Node node)
{
    vector<int> s = node.state;
    vector<slide> actions = p.action(s);
    vector<Node> children;
    for(int i = 0; i < actions.size(); i++) {
        Node new_node(swap(s, actions[i]), &node, actions[i], node.cost + 1);
        children.push_back(new_node);
    }
    return children;
}

vector<vector<int>> aStarSearch(Problem & p, int (*func)(const vector<int> &))
{  
    vector<vector<int>> solution;
    PriorityQueue frontier; 
    Node _node = Node(p.initial);
    frontier.insert(&_node);
    map<vector<int>, Node*> reached;
    reached.insert ( pair<vector<int>, Node*> (p.initial, &_node) );
    while ( !frontier.empty() ) {
        Node node = frontier.pop();
        if ( p.isGoal(node.state) )
            return solution;
        solution.push_back(node.state);
        vector<Node> children = expand(p, node);
        for (Node child : children) {
            vector<int> s = child.state;
            if (!reached.count(s) || child.cost < reached[s]->cost) {
                reached[s] = &child;
                frontier.insert(&child);
            }
        }
    }
    return solution;
}

vector<slide> Problem::action(const vector<int> & state)
{
    int pos;
    for(int i = 0; i < state.size(); i++) 
        if(state[i] == 0)
            pos = i;
    vector<slide> res = s(pos, state);
    return res;
}

bool Problem::isGoal(const vector<int> & state)
{
    for(int i = 0; i < state.size(); i++) {
        if(i != state[i]) return false;
    }
    return true;
}

int main()
{
    Problem puzzle8;
    vector<int> table {4, 5, 1, 8, 0, 3, 2, 7, 6};
    vector<vector<int>> proc = aStarSearch(puzzle8, heuristic);
    for(vector<int> table : proc)
        for(int i : table)
            cout << i;
}