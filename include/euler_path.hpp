#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "utils.hpp"
#include "structure.hpp"

#define DUMMY_NAME "Dummy"

#define VERTICAL_LENGTH 27
#define NODE_MID_LENGTH 34
#define NODE_EXT_LENGTH 25
#define GATE_LENGTH     20

#define TO_MICRO_SEC  1000000l
#define MAX_FIND_TIME 3590l * 1000000l


class EulerPath {
public:
    using Node2GateMap    = std::unordered_map<Node*, std::vector<Gate*>>;
    using Gate2NodeMap    = std::unordered_map<Gate*, std::vector<Node*>>;
    using Node2NodeMap    = std::unordered_map<Node*, std::vector<Edge*>>;
    using Gate2Instance   = std::unordered_map<std::string, std::vector<Instance*>>;

    using NodeNameMap     = std::unordered_map<std::string, Node*>;
    using GateNameMap     = std::unordered_map<std::string, Gate*>;

    using NodeFeasibleMap = std::unordered_map<Node*, int>;

    using InstanceMap     = std::unordered_map<std::string, 
                            std::unordered_map<std::string, 
                            std::unordered_map<std::string, Instance*>>>;

public:
    MOS                     m_type;

    std::vector<Instance*>  m_instances, m_instancesNoDummy;
    std::vector<Node*>      m_nodes, m_nodesNoDummy;
    std::vector<Gate*>      m_gates, m_gatesNoDummy;

    InstanceMap             m_instanceMap, m_instanceMapNoDummy;
    Node2GateMap            m_node2gate, m_node2gateNoDummy;
    Gate2NodeMap            m_gate2node, m_gate2nodeNoDummy;
    Node2NodeMap            m_node2node, m_node2nodeNoDummy;
    Gate2Instance           m_gate2inst;

    std::vector<Node*>      m_nodesStartEnd;
    NodeNameMap             m_foundNodeMap;
    GateNameMap             m_foundGateMap;
    NodeFeasibleMap         m_nodeFeasibleNumGate;

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

#if DEBUG == 1
    void      print                  ();
    void      printInstances         ();
    void      printNodes             ();
    void      printGates             ();
    void      printNode2Gate         ();
    void      printGate2Node         ();
    void      printNode2Node         ();
    void      printGate2Instance     ();
#endif

    void      randomEulerPath        (std::vector<std::string>& nodeEulerPath,
                                      std::vector<std::string>& gateEulerPath, 
                                      std::vector<std::string>& instanceEulerPath,
                                      std::vector<std::string>& nodeGateEulerPath);
    Node*     randomNode             ();
    Node*     randomOddDegreeNode    (bool isPop = false);
    int       randomFeasibleEdge     (Node* node);
    
    Node*     initStartDummyNodes    ();
    void      initGateVisit          ();
    void      initFeasibleNodeNumGate();
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
    void      makeNodeGateEulerPath  (const std::vector<std::string>& nodeEulerPath, 
                                      const std::vector<std::string>& gateEulerPath, 
                                      std::vector<std::string>& nodeGateEulerPath);
    void      makeNodeGateEulerPath  (const std::vector<std::string>& otherGateEulerPath,
                                      std::vector<std::string>& nodeGateEulerPath,
                                      std::vector<std::string>& nodeEulerPath,
                                      std::vector<std::string>& gateEulerPath);
    void      recordNoDummyInfo      ();
    void      removeDummies          ();

    double    getHalfHeight          ();
};


class EulerPathsHandler {
public:
    EulerPath m_PMOSNet = EulerPath(MOS::P);
    EulerPath m_NMOSNet = EulerPath(MOS::N);

public:
    EulerPathsHandler(std::string inputFilename);

public:

#if DEBUG == 1
    void   print                     ();
    void   print                     (MOS type);
#endif

    void   randomTwoEulerPathStrategy(const std::string& outputFilename);
    void   randomOneEulerPathStrategy(const std::string& outputFilename);

    double getHPWL                   (const std::vector<std::string>& nodeGateEulerPathP,
                                      const std::vector<std::string>& nodeGateEulerPathN);
    bool   isSameGateOrder           (const std::vector<std::string>& nodeGateEulerPathP, 
                                      const std::vector<std::string>& nodeGateEulerPathN);
    void   alignInstanceDummies      (const std::vector<std::string>& dummiesP, 
                                      const std::vector<std::string>& dummiesN,
                                            std::vector<std::string>& newDummiesP,
                                            std::vector<std::string>& newDummiesN);
    void   alignNodeGateDummies      (const std::vector<std::string>& dummiesP, 
                                      const std::vector<std::string>& dummiesN,
                                            std::vector<std::string>& newDummiesP,
                                            std::vector<std::string>& newDummiesN);
    void   outputToFile              (const std::string& outputFilename,
                                      const std::vector<std::string>& instanceEulerPathP,
                                      const std::vector<std::string>& instanceEulerPathN,
                                      const std::vector<std::string>& nodeGateEulerPathP,
                                      const std::vector<std::string>& nodeGateEulerPathN,
                                      double HPWL);
};
