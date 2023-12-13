#include "include/structure.hpp"


#if DEBUG == 1

void printNode2Gate_(const std::unordered_map<Node*, std::vector<Gate*>>& map){
    END_LINE
    LOG("Node2Gate :")
    for (auto& it : map){
        std::cout << "[Node " << it.first->name << "] : ";
        for (Gate* ptr : it.second){
            std::cout << ptr->name << " ";
        }
        END_LINE
    }
}

void printGate2Node_(const std::unordered_map<Gate*, std::vector<Node*>>& map){
    END_LINE
    LOG("Gate2Node :")
    for (auto& it : map){
        std::cout << "[Gate " << it.first->name << "] : ";
        for (Node* ptr : it.second){
            std::cout << ptr->name << " ";
        }
        END_LINE
    }
}

void printNode2Node_(const std::unordered_map<Node*, std::vector<Edge*>>& map){
    END_LINE
    LOG("Node2Node :")
    for (auto& it0 : map){
        std::cout << "------" << std::endl;
        std::cout << "[Parent Node " << it0.first->name << "] : " << std::endl;
        for (const Edge* edge : it0.second){
            std::cout << "  [Gate " << edge->gate->name << "] : " << edge->next->name << "(rev = " << edge->reverseIdx << ")" << std::endl;
        }
    }
}

void printGate2Instance_(const std::unordered_map<std::string, std::vector<Instance*>>& map){
    END_LINE
    LOG("Gate2Instance :")
    for (auto& it0 : map){
        std::cout << "[Gate " << it0.first << "] : ";
        for (Instance* inst : it0.second){
            std::cout << inst->name << " ";
        }
        END_LINE
    }
}

#endif