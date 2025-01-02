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

    tokens_i = 0;
    current_tok = tokens[tokens_i];

    Node* abstract_tree = abstract_parse();
    
    do_codegen(abstract_tree);


    return 0;
}