#include "leocc.hpp"
#include <cassert>

vector<Token*> tokens;
Token* current_tok = nullptr;
int tokens_i = 0;

Token::Token(TokenKind kind, long val) 
: kind(kind), num(val) {

};
Token::Token(TokenKind kind, string p) 
: kind(kind), punct(p) {

};
Token::Token(TokenKind kind) 
: kind(kind) {

}

void Token::print() {
    if(kind == TK_NUM) {
        cout << "<";
        cout << "TK_NUM: " << num;
        cout << ">" << endl;
    }
    else if(kind == TK_PUNCT) {
        cout << "<";
        cout << "TK_PUNCT: " << punct;
        cout << ">" << endl;
    }
    else if(kind == TK_EOF) {
        cout << "<";
        cout << "TK_EOF " << punct;
        cout << ">" << endl;
    }
}

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
        else if((*p == '+') || (*p == '-')){
            // cout << "*p is " << *p << endl;
            string s = "";
            s.push_back(*p);
            // cout << "s is " << s << endl;
            tokens.push_back(new Token(TK_PUNCT, s));
            p++;
        }
        else if((*p == '*') || (*p == '/')){
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
        else {
            assert(false && "shouldn't reach here");
        }
    }

    tokens.push_back(new Token(TK_EOF));
}
