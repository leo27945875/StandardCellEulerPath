#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <list>
#include "include/euler_path.hpp"
#include "include/utils.hpp"


EulerPathsHandler::EulerPathsHandler(std::string inputFilename){
    std::string   line;
    std::ifstream inputFile(inputFilename);

    getline(inputFile, line, '\n');
    while (getline(inputFile, line, '\n')){
        std::stringstream ss;
        std::string       name, drain, gate, source, substrate, mosType, wStr, lStr;

        ss << line;
        ss >> name;
        if (name == ".ENDS") return;
        ss >> drain >> gate >> source >> substrate >> mosType >> wStr >> lStr;

        switch (mosType[0]){
        case 'n':
            m_NMOSNet.parseOneLine(name, drain, gate, source, substrate, mosType, wStr, lStr);
            break;
        case 'p':
            m_PMOSNet.parseOneLine(name, drain, gate, source, substrate, mosType, wStr, lStr);
            break;
        }
    }
    m_NMOSNet.m_foundNodeMap.clear();
    m_PMOSNet.m_foundNodeMap.clear();
}

void EulerPathsHandler::print(){
    END_LINE
    DASH_LINE
    LOG("NMOS network")
    DASH_LINE
    m_NMOSNet.print();
    END_LINE
    DASH_LINE
    LOG("PMOS network")
    DASH_LINE
    m_PMOSNet.print();
}
void EulerPathsHandler::print(MOS type){
    END_LINE
    switch (type){
    case MOS::N:
        DASH_LINE
        LOG("NMOS network")
        DASH_LINE
        m_NMOSNet.print();
        break;
    case MOS::P:
        DASH_LINE
        LOG("PMOS network")
        DASH_LINE
        m_PMOSNet.print();
        break;
    }
}

EulerPath::EulerPath(std::string inputFilename){
    std::string   line;
    std::ifstream inputFile(inputFilename);

    getline(inputFile, line, '\n');
    while (getline(inputFile, line, '\n')){
        std::stringstream ss;
        std::string       name, drain, gate, source, substrate, mosType, wStr, lStr;
        ss << line;
        ss >> name;
        if (name == ".ENDS") return;
        ss >> drain >> gate >> source >> substrate >> mosType >> wStr >> lStr;
        parseOneLine(name, drain, gate, source, substrate, mosType, wStr, lStr);
    }
}

void EulerPath::parseOneLine(
    const std::string& name,
    const std::string& drain, 
    const std::string& gate, 
    const std::string& source, 
    const std::string& substrate, 
    const std::string& mosType, 
    const std::string& wStr, 
    const std::string& lStr
){
    if (!m_foundNodeMap.count(drain)){
        Node* ptr = makeNode(drain);
        m_foundNodeMap[drain] = ptr;
    }
    if (!m_foundNodeMap.count(source)){
        Node* ptr = makeNode(source);
        m_foundNodeMap[source] = ptr;
    }
    if (!m_foundGateMap.count(gate)){
        Gate* ptr = makeGate(gate);
        m_foundGateMap[gate] = ptr;
    }

    Node* drainPtr  = m_foundNodeMap[drain];
    Node* sourcePtr = m_foundNodeMap[source];
    Gate* gatePtr   = m_foundGateMap[gate];

    link(drainPtr, gatePtr, sourcePtr);
    makeInstance(
        drainPtr, 
        gatePtr, 
        sourcePtr, 
        name.substr(1), 
        getMOSTypeByString(mosType), 
        getNumberByString(wStr), 
        getNumberByString(lStr)
    );
}

void EulerPath::print(){
    printInstances();
    printNodes();
    printGates();
    printNode2Gate();
    printGate2Node();
    printNode2Node();
}

void EulerPath::printInstances(){
    END_LINE
    LOG("Instances :")
    for (const auto& inst : m_instances){
        std::cout << "[" << inst->name << "] " 
        << inst->drain->name << " (" << inst->drain << ") "
        << inst->gate->name << " (" << inst->gate << ") "
        << inst->source->name << " (" << inst->source << ") " << " | " 
        << (inst->type == MOS::N? "NMOS": "PMOS") << " | "
        << inst->width << " "
        << inst->length 
        << std::endl;
    }
}

void EulerPath::printNodes(){
    END_LINE
    LOG("Nodes :")
    for (const auto& ptr : m_nodes)
        std::cout << "[Node " << ptr->name << "]" << "(" << ptr << ")" << std::endl; 
}

void EulerPath::printGates(){
    END_LINE
    LOG("Gates :")
    for (const auto& ptr : m_gates)
        std::cout << "[Gate " << ptr->name << "]" << "(" << ptr << ")" << std::endl; 
}

void EulerPath::printNode2Gate(){
    printNode2Gate_(m_node2gate);
}

void EulerPath::printGate2Node(){
    printGate2Node_(m_gate2node);
}

void EulerPath::printNode2Node(){
    printNode2Node_(m_node2node);
}

void EulerPath::randomEulerPath(
    std::vector<std::string>& nodeEulerPath,
    std::vector<std::string>& gateEulerPath,
    std::vector<std::string>& instanceEulerPath
){
    int               edgeIdx;
    int               numGate        = m_instances.size();
    int               visitGateCount = 0;
    Node*             node;
    Node*             next;
    Gate*             gate;
    NodeFeasibleMap   feasibleMap;
    std::stack<Node*> tracebackNodes;
    std::stack<Gate*> tracebackGates;
    std::list<Node*>  pathNodes;
    std::list<Gate*>  pathGates;

    node = initStartDummyNodes();
    initGateVisit();
    initFeasibleNodeNumGate();

    tracebackNodes.push(node);
    while (!tracebackNodes.empty())
    {
        edgeIdx = randomFeasibleEdge(node);
            
        if (edgeIdx >= 0){
            std::cout << "[Forward] ";
            visitGateCount++;

            gate = m_node2node[node][edgeIdx]->gate;
            next = m_node2node[node][edgeIdx]->next;
            LOGFORWARD(node, gate, next, m_nodeFeasibleNumGate)
            updateFeasibleInfo(node, edgeIdx);
            node = next;

            tracebackNodes.push(node);
            tracebackGates.push(gate);
        } 
        else {
            do {
                std::cout << "[Backward] " << "Node: " << node->name;
                pathNodes.push_front(node);
                tracebackNodes.pop();
                if (tracebackNodes.empty())
                    break;

                pathGates.push_front(tracebackGates.top());
                tracebackGates.pop();

                next = tracebackNodes.top();
                std::cout << ", Back: " << next->name << std::endl;
                node = next;
            } while (m_nodeFeasibleNumGate[node] <= 0);
        }
    }

    nodeEulerPath.resize(pathNodes.size());
    int i = 0;
    while (!pathNodes.empty()){
        nodeEulerPath[i] = pathNodes.front()->name;
        pathNodes.pop_front();
        i++;
    }
    gateEulerPath.resize(pathGates.size());
    i = 0;
    while (!pathGates.empty()){
        gateEulerPath[i] = pathGates.front()->name;
        pathGates.pop_front();
        i++;
    }
    makeInstanceEulerPath(nodeEulerPath, gateEulerPath, instanceEulerPath);
}

Node* EulerPath::randomNode(){
    return m_nodes[RANDOM_INT(m_nodes.size())];
}

Node* EulerPath::randomOddDegreeNode(bool isPop){
    int idx = RANDOM_INT(m_nodesStartEnd.size());
    if (!isPop)
        return m_nodesStartEnd[idx];
    
    Node* outNode = m_nodesStartEnd[idx];
    m_nodesStartEnd.erase(m_nodesStartEnd.begin() + idx);
    return outNode;
}

Node* EulerPath::randomFeasibleNode(Gate* gate){
    std::unordered_set<Node*>& set = m_gateFeasibleNodeSet[gate];
    int idx = RANDOM_INT(set.size());
    return *std::next(set.begin(), idx);
}

int EulerPath::randomFeasibleEdge(Node* node){
    int numFeasible = m_nodeFeasibleNumGate[node];
    if (numFeasible <= 0)
        return -1;
    
    const std::vector<Edge*>& gates = m_node2node[node];
    int idxFeasible = RANDOM_INT(numFeasible);
    int i = -1;
    int count = -1;
    while (count < idxFeasible){
        i++;
        if (m_node2node[node][i]->isVisited)
            continue;
        count++;
    }
    // END_LINE
    // LOGKV(m_nodeFeasibleNumGate[node])
    // LOGKV(m_node2node[node][i]->isVisited)
    // LOGKV(idxFeasible)
    // LOGKV(count)
    // LOGKV(i)
    // END_LINE
    return i;
}

Node* EulerPath::initStartDummyNodes(){

    m_nodesStartEnd.clear();

    int numOddDegreeNode = 0;
    for (auto& it : m_node2node){
        if (it.second.size() % 2 != 0){
            m_nodesStartEnd.push_back(it.first);
            numOddDegreeNode++;
        }
    }
    if (numOddDegreeNode <= 2)
        return randomNode();

    Node* startNode = randomOddDegreeNode(true);
    Node* endNode   = randomOddDegreeNode(true);
    
    while (!m_nodesStartEnd.empty())
    {
        Node* n0 = randomOddDegreeNode(true);
        Node* n1 = randomOddDegreeNode(true);
        makeDummyNodeGates(n0, n1);
    }
    return startNode;
}

void EulerPath::initFeasibleNodeNumGate(){
    for (const auto& it : m_node2node)
        m_nodeFeasibleNumGate[it.first] = it.second.size();
}

void EulerPath::initFeasibleGateNodeSet(){
    for (const auto& it : m_gate2node)
        m_gateFeasibleNodeSet.emplace(
            it.first, std::unordered_set<Node*>(it.second.begin(), it.second.end())
        );
}

void EulerPath::initGateVisit(){
    for (auto& it   : m_node2node)
    for (auto& edge : it.second  )
        edge->isVisited = false;
}

void EulerPath::updateFeasibleInfo(Node* node, int edgeIdx){
    visit(node, edgeIdx);
}

void EulerPath::visit(Node* node, int edgeIdx){
    Edge* eT = m_node2node[node    ][edgeIdx];
    Edge* eB = m_node2node[eT->next][eT->reverseIdx];

    eT->isVisited = true;
    eB->isVisited = true;

    if (m_nodeFeasibleNumGate[node] > 0){
        m_nodeFeasibleNumGate[node]--;
    }
    if (m_nodeFeasibleNumGate[eT->next] > 0){
        m_nodeFeasibleNumGate[eT->next]--;
    }
}

void EulerPath::link(Node* drainPtr, Gate* gatePtr, Node* sourcePtr){
    int reverseIdx0 = m_node2node[sourcePtr].size(), reverseIdx1 = m_node2node[drainPtr].size();
    m_node2node[drainPtr ].push_back(new Edge(gatePtr, sourcePtr, reverseIdx0));
    m_node2node[sourcePtr].push_back(new Edge(gatePtr, drainPtr , reverseIdx1));
    if (!isInVector<Gate*>(m_node2gate[drainPtr], gatePtr))
        m_node2gate[drainPtr].push_back(gatePtr);
    if (!isInVector<Gate*>(m_node2gate[sourcePtr], gatePtr))
        m_node2gate[sourcePtr].push_back(gatePtr);
    if (!isInVector<Node*>(m_gate2node[gatePtr], drainPtr))
        m_gate2node[gatePtr].push_back(drainPtr);
    if (!isInVector<Node*>(m_gate2node[gatePtr], sourcePtr))
        m_gate2node[gatePtr].push_back(sourcePtr);
}

Instance* EulerPath::makeInstance(Node* drainPtr, Gate* gatePtr, Node* sourcePtr, const std::string& name, MOS type, double w, double l){
    Instance* instPtr = new Instance {
        name, drainPtr, sourcePtr, gatePtr, type, w, l
    };
    m_instances.push_back(instPtr);
    m_instanceMap[drainPtr->name][gatePtr->name][sourcePtr->name] = instPtr;
    m_instanceMap[sourcePtr->name][gatePtr->name][drainPtr->name] = instPtr;
    return instPtr;
}

Node* EulerPath::makeNode(const std::string& name, bool isDummy){
    Node* nodePtr = new Node(name, isDummy);
    m_nodes.push_back(nodePtr);
    return nodePtr;
}

Gate* EulerPath::makeGate(const std::string& name, bool isDummy){
    Gate* gatePtr = new Gate(name, isDummy);
    m_gates.push_back(gatePtr);
    return gatePtr;
}

Node* EulerPath::makeDummyNode(){
    if (!m_isHasDummy){
        m_nodes.push_back(m_dummyNode);
        m_isHasDummy = true;
    }
    return m_dummyNode;
}

Gate* EulerPath::makeDummyGate(){
    m_gates.push_back(m_dummyGate);
    return m_dummyGate;
}

void EulerPath::makeDummyNodeGates(Node* node0, Node* node1){
    Node* dummyNode  = makeDummyNode();
    Gate* dummyGate0 = makeDummyGate(); //makeGate(DUMMY_NAME, true);
    Gate* dummyGate1 = makeDummyGate(); //makeGate(DUMMY_NAME, true);
    link(node0, dummyGate0, dummyNode);
    link(dummyNode, dummyGate1, node1);
    makeInstance(node0, dummyGate0, dummyNode, DUMMY_NAME, m_type);
    makeInstance(dummyNode, dummyGate1, node1, DUMMY_NAME, m_type);
}

void EulerPath::makeInstanceEulerPath(
    const std::vector<std::string>& nodeEulerPath, 
    const std::vector<std::string>& gateEulerPath, 
          std::vector<std::string>& instanceEulerPath
){
    int nGate = gateEulerPath.size();
    instanceEulerPath.reserve(nGate);
    for (int i = 0; i < nGate; i++){
        if (nodeEulerPath[i] == DUMMY_NAME)
            continue;
        if (gateEulerPath[i] == DUMMY_NAME)
            instanceEulerPath.emplace_back(DUMMY_OUTPUT_NAME);
        else
            instanceEulerPath.emplace_back(
                m_instanceMap[nodeEulerPath[i]][gateEulerPath[i]][nodeEulerPath[i+1]]->name
            );
    }
}
