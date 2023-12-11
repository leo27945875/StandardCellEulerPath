#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "utils.hpp"


struct Node {
    std::string name;
    bool        isDummy = false;
    Node(std::string name): name(name) {}
    Node(std::string name, bool isDummy): name(name), isDummy(isDummy) {}
};


struct Gate {
    std::string name;
    bool        isDummy = false;
    Gate(std::string name): name(name) {}
    Gate(std::string name, bool isDummy): name(name), isDummy(isDummy) {}
};


struct Instance
{
    std::string name;
    Node*       drain;
    Node*       source;
    Gate*       gate;
    MOS         type;
    double      width, length;

    Instance(std::string name, Node* drain, Node* source, Gate* gate, MOS type, double width, double length)
        : name(name), drain(drain), source(source), gate(gate), type(type), width(width), length(length)
    {}
};

struct Edge {
    Gate* gate;
    Node* next;
    int   reverseIdx;
    bool  isVisited = false;
    Edge(Gate* gate, Node* next, int reverseIdx): gate(gate), next(next), reverseIdx(reverseIdx) {}
};


void printNode2Gate_(const std::unordered_map<Node*, std::vector<Gate*>>& map);
void printGate2Node_(const std::unordered_map<Gate*, std::vector<Node*>>& map);
void printNode2Node_(const std::unordered_map<Node*, std::vector<Edge*>>& map);