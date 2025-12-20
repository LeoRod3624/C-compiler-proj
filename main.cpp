#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cassert>
#include <list>
#include "leocc.hpp"

using namespace std;

int main(int argc, char* argv[]){
    if((argc != 2) && (argc != 3)) {
        printf("wrong number of arguments\n");
        exit(1);
    }

    char* p;

    if(argc == 2) {
        p = argv[1];
    }
    else if(argc == 3) {
        p = argv[2];
    }

    tokenize(p);

    tokens_i = 0;
    current_tok = tokens[tokens_i];
    //usage: ./leocc --print-cst "program"
    // if((argc == 3) && (std::string(argv[1]) == "--print-cst")) {
    //     CNode* concrete_tree = concrete_parse();
    //     concrete_tree->print_cst(0);
        
    //     std::cout << "exiting from --print-cst option in main\n";
    //     exit(0);
    // }



    Node* abstract_tree = abstract_parse();
    


    //implement switcher here

    const char* which = std::getenv("LEO_BACKEND");
    assert(which && "LEO_BACKEND must be defined, either arm or llvm");
    bool use_llvm = (which && std::string(which) == "llvm");
    bool use_arm =  (which && std::string(which) == "arm");


    if (use_arm) {
        do_codegen(abstract_tree);
    }
    else if(use_llvm){
        do_codegen_llvm(abstract_tree);
    }
    else{
        assert(false && "Didnt define a backend or didnt choose 1 of 2 options. Must be llvm or arm as those are the only targets right now.");
    }

    return 0;
}