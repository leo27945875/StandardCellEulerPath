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
        if (name == ".ENDS") break;
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
    m_PMOSNet.recordNoDummyInfo();
    m_NMOSNet.recordNoDummyInfo();
}

void EulerPathsHandler::outputToFile(
    const std::string&              outputFilename,
    const std::vector<std::string>& instanceEulerPathP,
    const std::vector<std::string>& instanceEulerPathN,
    const std::vector<std::string>& nodeGateEulerPathP,
    const std::vector<std::string>& nodeGateEulerPathN,
    double                          HPWL
){
    std::ofstream f(outputFilename);
    f << HPWL << std::endl;
    f << concatStringVector(instanceEulerPathP) << std::endl;
    f << concatStringVector(nodeGateEulerPathP) << std::endl;
    f << concatStringVector(instanceEulerPathN) << std::endl;
    f << concatStringVector(nodeGateEulerPathN) << std::endl;
    f.close();
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

void EulerPathsHandler::randomTwoEulerPathStrategy(const std::string& outputFilename){
    srand(time(NULL));
    int loop = 1;
    do {
        LOOP_START(loop)

        std::vector<std::string> nodePathP, gatePathP, instancePathP, nodeGatePathP;
        std::vector<std::string> nodePathN, gatePathN, instancePathN, nodeGatePathN;
        m_PMOSNet.randomEulerPath(nodePathP, gatePathP, instancePathP, nodeGatePathP);
        m_NMOSNet.randomEulerPath(nodePathN, gatePathN, instancePathN, nodeGatePathN);
        
        // END_LINE
        // LOG("[PMOS Instance Path] :")
        // printVector(instancePathP);
        // LOG("[NMOS Instance Path] :")
        // printVector(instancePathN);
        // END_LINE
        // LOG("[PMOS NodeGate Path] :")
        // printVector(nodeGatePathP);
        // LOG("[NMOS NodeGate Path] :")
        // printVector(nodeGatePathN);

        std::vector<std::string> newInstanceDummiesP, newNodeGateDummiesP;
        std::vector<std::string> newInstanceDummiesN, newNodeGateDummiesN;

        alignInstanceDummies(instancePathP, instancePathN, newInstanceDummiesP, newInstanceDummiesN);
        LOG("[PMOS Instance Path with Dummies] :")
        printVector(newInstanceDummiesP);
        LOG("[NMOS Instance Path with Dummies] :")
        printVector(newInstanceDummiesN);
        END_LINE
        alignNodeGateDummies(nodeGatePathP, nodeGatePathN, newNodeGateDummiesP, newNodeGateDummiesN);
        LOG("[PMOS NodeGate Path with Dummies] :")
        printVector(newNodeGateDummiesP);
        LOG("[NMOS NodeGate Path with Dummies] :")
        printVector(newNodeGateDummiesN);
        double HPWL = getHPWL(newNodeGateDummiesP, newNodeGateDummiesN);
        LOGKV(HPWL)

        if (isSameGateOrder(newNodeGateDummiesP, newNodeGateDummiesN)){
            END_LINE DDASH_LINE
            LOG("[Found !!!]")
            LOG("[PMOS NodeGate Path with Dummies] :")
            printVector(newNodeGateDummiesP);
            LOG("[NMOS NodeGate Path with Dummies] :")
            printVector(newNodeGateDummiesN);
            LOGKV(HPWL)
            END_LINE

            outputToFile(
                outputFilename,
                newInstanceDummiesP,
                newInstanceDummiesN,
                newNodeGateDummiesP,
                newNodeGateDummiesN,
                HPWL
            );

            break;
        }

        loop++;

    } while(true);
}

void EulerPathsHandler::alignInstanceDummies(
    const std::vector<std::string>& dummiesP, 
    const std::vector<std::string>& dummiesN,
          std::vector<std::string>& newDummiesP,
          std::vector<std::string>& newDummiesN
){
    std::list<std::string> dummyListP(dummiesP.begin(), dummiesP.end());
    std::list<std::string> dummyListN(dummiesN.begin(), dummiesN.end());
    auto itP = dummyListP.begin();
    auto itN = dummyListN.begin();
    int n = MAX(dummyListP.size(), dummyListN.size());
    for (int i = 0; i < n; i++){
        const std::string& instP = *itP;
        const std::string& instN = *itN;
        if      (instP == DUMMY_NAME && instN != DUMMY_NAME){dummyListN.insert(itN, DUMMY_NAME); itP++;}
        else if (instP != DUMMY_NAME && instN == DUMMY_NAME){dummyListP.insert(itP, DUMMY_NAME); itN++;}
        else                                                {itP++; itN++;}
        n = MAX(dummyListP.size(), dummyListN.size());
    }
    newDummiesP = std::vector<std::string>(dummyListP.begin(), dummyListP.end());
    newDummiesN = std::vector<std::string>(dummyListN.begin(), dummyListN.end());
}

void EulerPathsHandler::alignNodeGateDummies(
    const std::vector<std::string>& dummiesP, 
    const std::vector<std::string>& dummiesN,
          std::vector<std::string>& newDummiesP,
          std::vector<std::string>& newDummiesN
){
    std::list<std::string> dummyListP(dummiesP.begin(), dummiesP.end());
    std::list<std::string> dummyListN(dummiesN.begin(), dummiesN.end());
    auto itP = ++(dummyListP.begin());
    auto itN = ++(dummyListN.begin());
    int n = MAX(dummyListP.size(), dummyListN.size());
    for (int i = 1; i < n; i+=2){
        const std::string& gateP = *itP;
        const std::string& gateN = *itN;
        if (gateP == DUMMY_NAME && gateN != DUMMY_NAME){
            dummyListN.insert(itN, DUMMY_NAME); 
            dummyListN.insert(itN, *std::prev(itN, 2)); 
            std::advance(itP, 2);
        }
        else if (gateP != DUMMY_NAME && gateN == DUMMY_NAME){
            dummyListP.insert(itP, DUMMY_NAME); 
            dummyListP.insert(itP, *std::prev(itP, 2)); 
            std::advance(itN, 2);
        }
        else {
            std::advance(itP, 2); std::advance(itN, 2);
        }
        n = MAX(dummyListP.size(), dummyListN.size());
    }
    newDummiesP = std::vector<std::string>(dummyListP.begin(), dummyListP.end());
    newDummiesN = std::vector<std::string>(dummyListN.begin(), dummyListN.end());
}

double EulerPathsHandler::getHPWL(const std::vector<std::string>& nodeGateEulerPathP, 
                                  const std::vector<std::string>& nodeGateEulerPathN
){
    auto cumDistance = [](int i, int n, const std::string& name, double& x) -> bool {
        if (i % 2 == 0){
            if (i == 0 || i == n - 1)
                x += NODE_EXT_LENGTH * 0.5;
            else
                x += NODE_MID_LENGTH * 0.5;

            return false;
        } 
        else {
            if (name == DUMMY_NAME)
                x += GATE_LENGTH * 2. + NODE_MID_LENGTH;
            else
                x += GATE_LENGTH;

            if (i == 1)
                x += NODE_EXT_LENGTH * 0.5;
            else
                x += NODE_MID_LENGTH * 0.5;

            return true;
        }
    };

    auto findMaxDistance = [&](const std::vector<std::string>& nodeGateEulerPath, const std::string& srcName, int n, int i, double srcX, double dstX, double& maxDistance) -> bool {
        bool isMaxDistanceUpdated = false;
        for (int j = i; j < n; j++){
            const std::string& dstName = nodeGateEulerPath[j];
            bool isGate = cumDistance(j, n, dstName, dstX);
            if (isGate || dstName != srcName) 
                continue;
            double distance = ABS(dstX - srcX);
            if (distance > maxDistance){
                maxDistance = distance;
                isMaxDistanceUpdated = true;
            }
        }
        return isMaxDistanceUpdated;
    };

    double totalHPWL = 0.;
    double vertical  = VERTICAL_LENGTH + m_PMOSNet.getHalfHeight() + m_NMOSNet.getHalfHeight();
    double srcX, maxDistance;

    int n = nodeGateEulerPathP.size();

    bool isMaxDistanceUpdated;

    std::unordered_set<std::string> foundSet;

    // Src = PMOS, Dst = NMOS
    srcX = 0.;
    for (int i = 0; i < n; i++){
        const std::string& srcName = nodeGateEulerPathP[i];
        bool isGate = cumDistance(i, n, srcName, srcX);
        if (isGate || foundSet.count(srcName))
            continue;
        else
            foundSet.emplace(srcName);

        maxDistance = 0.; 
        findMaxDistance(nodeGateEulerPathP, srcName, n, i + 1, srcX, srcX, maxDistance);
        if (m_NMOSNet.m_foundNodeMap.count(srcName)){
            findMaxDistance(nodeGateEulerPathN, srcName, n, 0, srcX, 0.  , maxDistance);
            totalHPWL += vertical;
        }
        totalHPWL += maxDistance;
    }

    // Src = NMOS, Dst = PMOS
    srcX = 0.;
    for (int i = 0; i < n; i++){
        const std::string& srcName = nodeGateEulerPathN[i];
        bool isGate = cumDistance(i, n, srcName, srcX);
        if (isGate || foundSet.count(srcName))
            continue;
        else
            foundSet.emplace(srcName);

        maxDistance = 0.; 
        findMaxDistance(nodeGateEulerPathN, srcName, n, i + 1, srcX, srcX, maxDistance);
        if (m_PMOSNet.m_foundNodeMap.count(srcName)){
            findMaxDistance(nodeGateEulerPathP, srcName, n, 0, srcX, 0.  , maxDistance);
            totalHPWL += vertical;
        }
        totalHPWL += maxDistance;
    }

    return totalHPWL;
}

bool EulerPathsHandler::isSameGateOrder(const std::vector<std::string>& nodeGateEulerPathP, 
                                        const std::vector<std::string>& nodeGateEulerPathN
){
    for (int i = 1; i < nodeGateEulerPathP.size(); i+=2)
        if (nodeGateEulerPathP[i] != nodeGateEulerPathN[i])
            return false;
    return true;
}

/*############################################################################################################################*/

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
    std::vector<std::string>& instanceEulerPath,
    std::vector<std::string>& nodeGateEulerPath
){
    int               edgeIdx;
    int               numGate = m_instances.size();
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
            // std::cout << "[Forward] ";
            visitGateCount++;

            gate = m_node2node[node][edgeIdx]->gate;
            next = m_node2node[node][edgeIdx]->next;
            // LOGFORWARD(node, gate, next, m_nodeFeasibleNumGate)
            updateFeasibleInfo(node, edgeIdx);
            node = next;

            tracebackNodes.push(node);
            tracebackGates.push(gate);
        } 
        else {
            do {
                // std::cout << "[Backward] " << "Node: " << node->name;
                pathNodes.push_front(node);
                tracebackNodes.pop();
                if (tracebackNodes.empty())
                    break;

                pathGates.push_front(tracebackGates.top());
                tracebackGates.pop();

                next = tracebackNodes.top();
                // std::cout << ", Back: " << next->name << std::endl;
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
    makeNodeGateEulerPath(nodeEulerPath, gateEulerPath, nodeGateEulerPath);
    removeDummies();
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
    if (numOddDegreeNode == 0)
        return randomNode();
    else if (numOddDegreeNode == 2)
        return randomOddDegreeNode(true);

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
    m_gate2inst[gatePtr->name] = instPtr;
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
            instanceEulerPath.emplace_back(DUMMY_NAME);
        else
            instanceEulerPath.emplace_back(
                m_instanceMap[nodeEulerPath[i]][gateEulerPath[i]][nodeEulerPath[i+1]]->name
            );
    }
}

void EulerPath::makeNodeGateEulerPath(
    const std::vector<std::string>& nodeEulerPath, 
    const std::vector<std::string>& gateEulerPath, 
          std::vector<std::string>& nodeGateEulerPath
){
    int nGate = gateEulerPath.size();
    for (int i = 0; i < nGate; i++){
        if (nodeEulerPath[i] == DUMMY_NAME)
            continue;
        if (gateEulerPath[i] == DUMMY_NAME){
            nodeGateEulerPath.push_back(nodeEulerPath[i]);
            nodeGateEulerPath.push_back(DUMMY_NAME);
        }
        else {
            nodeGateEulerPath.push_back(nodeEulerPath[i]);
            nodeGateEulerPath.push_back(gateEulerPath[i]);
        }
    }
    nodeGateEulerPath.push_back(nodeEulerPath.back());
}

void EulerPath::makeNodeGateEulerPath(
    const std::vector<std::string>& otherGateEulerPath, 
          std::vector<std::string>& nodeGateEulerPath
){
    auto randomDrainSourcePtr = [&](Instance* instPtr, Node* nodePtr0, Node* nodePtr1){
        if (RANDOM_BOOL) {
            nodePtr0 = instPtr->drain;
            nodePtr1 = instPtr->source;
        } else {
            nodePtr0 = instPtr->source;
            nodePtr1 = instPtr->drain;
        }
    };

    auto findFirstPairNodesOrder = [&](const std::string& gateName0, const std::string& gateName1, std::string& nodeNameOne, std::string& nodeNameTwo, std::string& nodeNameThree) -> bool {
        const std::string& nodeName00 = m_gate2inst[gateName0]->drain->name;
        const std::string& nodeName01 = m_gate2inst[gateName0]->source->name;
        const std::string& nodeName10 = m_gate2inst[gateName1]->drain->name;
        const std::string& nodeName11 = m_gate2inst[gateName1]->source->name;

        if (nodeName00 == nodeName10){
            nodeNameOne   = nodeName01;
            nodeNameTwo   = nodeName00;
            nodeNameThree = nodeName11;
        } else if (nodeName00 = nodeName11){
            nodeNameOne   = nodeName01;
            nodeNameTwo   = nodeName00;
            nodeNameThree = nodeName10;
        } else if (nodeName01 = nodeName10){
            nodeNameOne   = nodeName00;
            nodeNameTwo   = nodeName01;
            nodeNameThree = nodeName11;
        } else if (nodeName01 = nodeName11){
            nodeNameOne   = nodeName00;
            nodeNameTwo   = nodeName01;
            nodeNameThree = nodeName10;
        } else 
            return false;

        return true;
    };

    auto findNodeOrderFromLast = [&](const std::string& last, const std::string& gateName, std::string& nodeNameNext) -> bool{
        const std::string& nodeName0 = m_gate2inst[gateName]->drain->name;
        const std::string& nodeName1 = m_gate2inst[gateName]->source->name;
        if (nodeName0 == last)
            nodeNameNext = nodeName0;
        else if (nodeName1 == last)
            nodeNameNext = nodeName1;
        else 
            return false;
        
        return true;
    };

    std::string last = "";
    bool isLastDummy = true;
    for (int i = 0; i < otherGateEulerPath.size() - 1; i++){
        const std::string& gateName0 = otherGateEulerPath[i];
        const std::string& gateName1 = otherGateEulerPath[i + 1];
        if (isLastDummy){
            Node *nodePtr0, *nodePtr1;
            if (gateName1 == DUMMY_NAME){
                randomDrainSourcePtr(m_gate2inst[gateName0], nodePtr0, nodePtr1);
                nodeGateEulerPath.push_back(nodePtr0->name);
                nodeGateEulerPath.push_back(gateName0);
                nodeGateEulerPath.push_back(nodePtr1->name);
                nodeGateEulerPath.push_back(DUMMY_NAME);
            } else {
                std::string nodeNameOne, nodeNameTwo, nodeNameThree;
                if (findFirstPairNodesOrder(gateName0, gateName1, nodeNameOne, nodeNameTwo, nodeNameThree)){
                    nodeGateEulerPath.push_back(nodeNameOne);
                    nodeGateEulerPath.push_back(gateName0);
                    nodeGateEulerPath.push_back(nodeNameTwo);
                    nodeGateEulerPath.push_back(gateName1);
                    nodeGateEulerPath.push_back(nodeNameThree);
                } else {
                    randomDrainSourcePtr(m_gate2inst[gateName0], nodePtr0, nodePtr1);
                    nodeGateEulerPath.push_back(nodePtr0->name);
                    nodeGateEulerPath.push_back(gateName0);
                    nodeGateEulerPath.push_back(nodePtr1->name);
                    nodeGateEulerPath.push_back(DUMMY_NAME);
                    randomDrainSourcePtr(m_gate2inst[gateName1], nodePtr0, nodePtr1);
                    nodeGateEulerPath.push_back(nodePtr0->name);
                    nodeGateEulerPath.push_back(gateName1);
                    nodeGateEulerPath.push_back(nodePtr1->name);
                }
            }
            isLastDummy = false;
        } 
        else {
            if (gateName0 == DUMMY_NAME){
                isLastDummy = true;
                continue;
            } 
            else if (gateName1 == DUMMY_NAME){
                nodeGateEulerPath.push_back(DUMMY_NAME);
            }
            else {

            }
        }
    }
}

void EulerPath::recordNoDummyInfo(){
    m_instancesNoDummy   = m_instances;
    m_nodesNoDummy       = m_nodes;
    m_gatesNoDummy       = m_gates;
    m_node2gateNoDummy   = m_node2gate;
    m_gate2nodeNoDummy   = m_gate2node;
    m_node2nodeNoDummy   = m_node2node;
    m_instanceMapNoDummy = m_instanceMap;
}

void EulerPath::removeDummies(){
    m_instances   = m_instancesNoDummy;
    m_nodes       = m_nodesNoDummy;
    m_gates       = m_gatesNoDummy;
    m_node2gate   = m_node2gateNoDummy;
    m_gate2node   = m_gate2nodeNoDummy;
    m_node2node   = m_node2nodeNoDummy;
    m_instanceMap = m_instanceMapNoDummy;
    m_isHasDummy  = false;
}

double EulerPath::getHalfHeight(){
    return m_instances[0]->width / 2.;
}