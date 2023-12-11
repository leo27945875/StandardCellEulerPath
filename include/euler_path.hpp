#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "utils.hpp"
#include "structure.hpp"

#define DUMMY_NAME        "_Dummy_"
#define DUMMY_OUTPUT_NAME "Dummy"


class EulerPath {
public:
    using Node2GateMap    = std::unordered_map<Node*, std::vector<Gate*>>;
    using Gate2NodeMap    = std::unordered_map<Gate*, std::vector<Node*>>;
    using Node2NodeMap    = std::unordered_map<Node*, std::vector<Edge*>>;

    using NodeNameMap     = std::unordered_map<std::string, Node*>;
    using GateNameMap     = std::unordered_map<std::string, Gate*>;

    using NodeFeasibleMap = std::unordered_map<Node*, int>;
    using GateFeasibleMap = std::unordered_map<Gate*, std::unordered_set<Node*>>;

    using InstanceMap     = std::unordered_map<std::string, 
                            std::unordered_map<std::string, 
                            std::unordered_map<std::string, Instance*>>>;

public:
    MOS                     m_type;

    std::vector<Instance*>  m_instances;
    std::vector<Node*>      m_nodes;
    std::vector<Gate*>      m_gates;

    std::vector<Node*>      m_nodesStartEnd;

    Node2GateMap            m_node2gate;
    Gate2NodeMap            m_gate2node;
    Node2NodeMap            m_node2node;

    NodeNameMap             m_foundNodeMap;
    GateNameMap             m_foundGateMap;

    NodeFeasibleMap         m_nodeFeasibleNumGate;
    GateFeasibleMap         m_gateFeasibleNodeSet;

    InstanceMap             m_instanceMap;

    Node* const             m_dummyNode  = new Node(DUMMY_NAME, true);
    Gate* const             m_dummyGate  = new Gate(DUMMY_NAME, true);
    bool                    m_isHasDummy = false;

public:
    EulerPath(MOS mosType): m_type(mosType) {};
    EulerPath(std::string inputFilename);

public:
    void parseOneLine         (
        const std::string& name,
        const std::string& drain, 
        const std::string& gate, 
        const std::string& source, 
        const std::string& substrate, 
        const std::string& mosType, 
        const std::string& wStr, 
        const std::string& lStr
    );

    void      print                  ();
    void      printInstances         ();
    void      printNodes             ();
    void      printGates             ();
    void      printNode2Gate         ();
    void      printGate2Node         ();
    void      printNode2Node         ();

    void      randomEulerPath        (std::vector<std::string>& nodeEulerPath,
                                      std::vector<std::string>& gateEulerPath, 
                                      std::vector<std::string>& instanceEulerPath);
    Node*     randomNode             ();
    Node*     randomOddDegreeNode    (bool isPop = false);
    Node*     randomFeasibleNode     (Gate* gate);
    int       randomFeasibleEdge     (Node* node);
    
    Node*     initStartDummyNodes    ();
    void      initGateVisit          ();
    void      initFeasibleNodeNumGate();
    void      initFeasibleGateNodeSet();
    void      updateFeasibleInfo     (Node* node, int edgeIdx);
    void      visit                  (Node* node, int edgeIdx);
    void      link                   (Node* drainPtr, Gate* gatePtr, Node* sourcePtr);
    Instance* makeInstance           (Node* drainPtr, Gate* gatePtr, Node* sourcePtr, const std::string& name, MOS type, double w = 0., double l = 0.);
    Node*     makeNode               (const std::string& name, bool isDummy = false);
    Gate*     makeGate               (const std::string& name, bool isDummy = false);
    Node*     makeDummyNode          ();
    Gate*     makeDummyGate          ();
    void      makeDummyNodeGates     (Node* node0, Node* node1);
    void      makeInstanceEulerPath  (const std::vector<std::string>& nodeEulerPath, 
                                      const std::vector<std::string>& gateEulerPath, 
                                      std::vector<std::string>& instanceEulerPath);
};


class EulerPathsHandler {
public:
    EulerPath m_NMOSNet = EulerPath(MOS::N);
    EulerPath m_PMOSNet = EulerPath(MOS::P);

public:
    EulerPathsHandler(std::string inputFilename);

public:
    void print();
    void print(MOS type);

    void randomTwoEulerPathStrategy(const std::string& outputFilename);

    void alignInstanceDummies(const std::vector<std::string>& dummiesN, 
                              const std::vector<std::string>& dummiesP,
                                    std::vector<std::string>& newDummiesN,
                                    std::vector<std::string>& newDummiesP);
    void alignNodeGateDummies(const std::vector<std::string>& dummiesN, 
                              const std::vector<std::string>& dummiesP,
                                    std::vector<std::string>& newDummiesN,
                                    std::vector<std::string>& newDummiesP);
    void outputToFile(const std::string& outputFilename);
};
