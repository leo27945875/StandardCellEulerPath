#include <iostream>
#include "include/structure.hpp"
#include "include/euler_path.hpp"

int main(int argc, char* args[]){

    std::string inputfilename  = args[1];
    std::string outputfilename = args[2];

    EulerPathsHandler algo(inputfilename);

    algo.randomOneEulerPathStrategy(outputfilename);

    return 0;
}