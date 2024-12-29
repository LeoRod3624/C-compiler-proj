#include "leocc.hpp"
#include <cassert>

vector<Token*> tokens;
Token* current_tok = nullptr;
int tokens_i = 0;
Token::Token(TokenKind k, long val) 
: kind(k), num(val) {

};
Token::Token(TokenKind k, string p) 
: kind(k) {
    if(kind == TK_PUNCT) {
        punct = p;
    }
    else if(kind == TK_ID) {
        id = p;
        if(id == "return") {
            kind = TK_KW;
            kw_kind = KW_RET;
        }
        else if(id == "while"){
            kind = TK_KW;
            kw_kind = KW_WHILE;
        }
        else if(id == "for"){
            kind = TK_KW;
            kw_kind = KW_FOR;
        }
        else if(id == "int"){
            kind = TK_KW;
            kw_kind = KW_INT;
        }
    }
};
Token::Token(TokenKind kind) 
: kind(kind) {

}


// void Token::print() {
//     if(kind == TK_NUM) {
//         cout << "<";
//         cout << "TK_NUM: " << num;
//         cout << ">" << endl;
//     }
//     else if(kind == TK_PUNCT) {
//         cout << "<";
//         cout << "TK_PUNCT: " << punct;
//         cout << ">" << endl;
//     }
//     else if(kind == TK_ID) {
//         cout << "<";
//         cout << "TK_ID: " << id;
//         cout << ">" << endl;
//     }
//     else if(kind == TK_KW) {
//         cout << "<";
//         cout << "TK_KW: " << id;
//         cout << "> return" << endl;
//     }
//     else if(kind == TK_EOF) {
//         cout << "<";
//         cout << "TK_EOF " << punct;
//         cout << ">" << endl;
//     }
// }

void tokenize(char* p) {
    while(*p) {
        if(isspace(*p)) {
            p++;// do nothing 
        }
        else if(isdigit(*p)) {
            char* q = p;
            long val = strtol(p, &p, 10);
            tokens.push_back(new Token(TK_NUM, val));
        }
        else if((*p == '+') || (*p == '-') || (*p == '&')){
            // cout << "*p is " << *p << endl;
            string s = "";
            s.push_back(*p);
            // cout << "s is " << s << endl;
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else if((*p == '*') || (*p == '/') || (*p == '&')){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else if((*p == '(') || (*p == ')')){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else if(((*p == '<') || (*p == '>') || (*p == '!') || (*p == '=')) 
                 && (*(p+1) == '=') ){
            string s = "";
            s.push_back(*p);
            s.push_back(*(p+1));
            tokens.push_back(new Token(TK_PUNCT, s));
            p+=2;
        }
        else if((*p == '<') || (*p == '>')) {
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p += 1;
        }
        else if(*p == ';'){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p+=1;
        }
        else if(*p == '='){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p+=1;
        }
        // variables start with a letter or _ and can have numbers
        else if(isalpha(*p) || *p == '_'){
            string s = "";
            while(isalnum(*p) || *p == '_') {
                s.push_back(*p);
                p++;
            }
            tokens.push_back(new Token(TK_ID, s)); // Token constructor corrects TK_ID if it's a keyword
        }
        else if(*p == '{' || *p == '}'){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else if(*p == ','){
            string s = "";
            s.push_back(*p);
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else {
            assert(false && "shouldn't reach here");
        }
    }
    tokens.push_back(new Token(TK_EOF));

    // Debugging: Print all tokens
    // cout << "Tokenization Result:" << endl;
    // for (auto token : tokens) {
    //     token->print();
    // }
}
