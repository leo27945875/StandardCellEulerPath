#pragma once

#include <iostream>
#include <random>
#include <vector>
#include <list>
#include <algorithm>
#include <stdexcept>

#define DEBUG 1

#if DEBUG == 1
    #define LOG(x)                 std::cout << x << std::endl;
    #define LOGKV(x)               std::cout << #x << " = " << (x) <<  std::endl;
    #define LOGFORWARD(n, g, t, m) std::cout << "Node: " << (n->name) << "(" << (m[n]) << ")" << ", Gate: " << (g->name) << ", Next: " << (t->name) << "(" << (m[t]) << ")" << std::endl;
    #define LOOP_START(x)          std::cout << "========================== " << "Loop " << (x) << " ==========================\n" << std::endl;
    #define DDASH_LINE             std::cout << "==============================================================" << std::endl;
    #define DASH_LINE              std::cout << "--------------------------------------------------------------" << std::endl;
    #define MOVE_INFO(x)           std::cout << "Move " << x->m_number << "  (from: " << (x->m_group + 1) << " to: " << ((!x->m_group) + 1) << ")" << std::endl;
    #define END_LINE               std::cout << std::endl;
    #define AAA                    std::cout << "AAAAAAAAAAAAAAAAAAAA" << std::endl;
    #define BBB                    std::cout << "BBBBBBBBBBBBBBBBBBBB" << std::endl;
#else
    #define LOG(x) 
    #define LOGKV(x) 
    #define LOGFORWARD(n, g, t, m) 
    #define LOOP_START(x) 
    #define DDASH_LINE 
    #define DASH_LINE 
    #define MOVE_INFO(x) 
    #define END_LINE 
    #define AAA 
    #define BBB 
#endif
 
#define FIXLOG(x)      std::cout << x << std::endl;
#define FIXLOGKV(x, y) std::cout << x << " = " << y <<  std::endl;
#define FIX_END_LINE   std::cout << std::endl;

#define RANDOM_INT(MAX)          (rand() % MAX)
#define RANDOM_BETWEEN(MIN, MAX) (rand() % (MAX - MIN) + MIN)
#define RANDOM_BOOL              (static_cast<bool>(rand() % 2))
#define RANDOM_0TO1              (static_cast<double>(rand()) / RAND_MAX)

enum class MOS : int8_t {
    P, N
};

MOS    getMOSTypeByString(std::string str);
double getNumberByString (std::string str);

template<class T>
bool isInVector(std::vector<T>& vec, const T& val){
    return std::find(vec.begin(), vec.end(), val) != vec.end();
}

template<class T>
void printVector(const std::vector<T>& vec){
    for (int i = 0; i < vec.size(); i++){
        if (i != vec.size() - 1)
            std::cout << vec[i] << ", ";
        else
            std::cout << vec[i];
    }
    FIX_END_LINE
}

template<class T>
void listToVector(const std::list<T>& list, std::vector<T>& vec){
    vec.resize(list.size());
    int i = 0;
    while (!list.empty()){
        vec[i] = list.front();
        list.pop_front();
        i++;
    }
}