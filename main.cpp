#include <iostream>
#include "include/structure.hpp"
#include "include/euler_path.hpp"

int main(){
    EulerPathsHandler algo("data/input2.in");

    END_LINE
    std::vector<std::string> nodePathN;
    std::vector<std::string> gatePathN;
    std::vector<std::string> instancePathN;
    algo.m_NMOSNet.randomEulerPath(nodePathN, gatePathN, instancePathN);

    nodePathN.clear();
    gatePathN.clear();
    instancePathN.clear();
    algo.m_NMOSNet.randomEulerPath(nodePathN, gatePathN, instancePathN);

    END_LINE
    algo.print(MOS::N);
    END_LINE
    LOG("NMOS Node Euler Path :")
    printVector(nodePathN);
    END_LINE
    LOG("NMOS Gate Euler Path :")
    printVector(gatePathN);
    END_LINE
    LOG("NMOS Instance Euler Path :")
    printVector(instancePathN);
    END_LINE

    // algo.print(MOS::P);
    // END_LINE
    // std::vector<std::string> nodePathP;
    // std::vector<std::string> gatePathP;
    // std::vector<std::string> instancePathP;
    // algo.m_PMOSNet.randomEulerPath(nodePathP, gatePathP, instancePathP);
    // END_LINE
    // LOG("PMOS Node Euler Path :")
    // printVector(nodePathP);
    // END_LINE
    // LOG("PMOS Gate Euler Path :")
    // printVector(gatePathP);
    // END_LINE
    // LOG("PMOS Instance Euler Path :")
    // printVector(instancePathP);
    // END_LINE

    return 0;
}