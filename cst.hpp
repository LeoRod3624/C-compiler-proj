// // #include "main.hpp"

// // ConcreteNode
// class CNode;
// class CExpr;
// class CMul;
// class CNum;
// class CMulList;
// class CNumList;



// // -> 1 * 2 + 3 
// //-expr 
// // |-mul
// //  |-num 
// //   |-1
// //  |-optional_num_list
// //   |-*
// //   |-num
// //    |-2
// //   |-optional_num_list
// //    |-NULL
// // |-optional_mul_list
// //  |-+
// //  |-mul
// //   |-num
// //    |-3
// //   |-optional_num_list
// //    |-NULL
// //  |-optional_mul_list
// //   |-NULL

// class CNode {
//     virtual void print_tree(int depth) = 0;
// };

// class CExpr : public CNode {
//     void print_tree(int depth);
//     CMul* mul;
//     CMulList* mul_list;
// };

// class CMul : public CNode {
//     CNum* num;
//     CNumList* num_list;
// };

// class CMulList : public CNode {
//     CNode* plus_or_minus;
//     CMul* mul;
//     CMulList* mul_list;
// };

// class CNum : public CNode { 
//     unsigned int num_literal;
// };

// class CNumList : public CNode {
//     CNode* times_or_divides;
//     CNum* num;
//     CNumList* num_list;
// };





// // grammar without any tricks like * lists
// // expr = mul optional_mul_list
// // mul = num optional_num_list
// // optional_mul_list =  "" 
// //                    | "+" mul optional_mul_list 
// //                    | "-" mul optional_mul_list
// // optional_num_list = "" 
// //                   | "*" num optional_num_list 
// //                   | "/" num optional_num_list
// CExpr* C_expr();

// CNode* concrete_parse() {
//     cout << "called concrete_parse\n";
//     for(int i = 0; i < tokens.size(); i++) {
//         tokens[i]->print();
//     }
//     CExpr* expr = C_expr();
//     return expr;
// }

// CExpr* C_expr() {
//     return nullptr;
// }