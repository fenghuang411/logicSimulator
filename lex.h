#ifndef LEX_H
#define LEX_H


#include <iostream>
#include <fstream>
#include <string>
#include <ctype.h>
#include <list>
#include <assert.h>
struct evl_token{
	enum token_type {NAME, NUMBER, SINGLE};
	token_type type;
	std::string str;
	int line_no;
};
typedef std::list<evl_token> evl_tokens;
bool extract_tokens_from_file(std::string, evl_tokens&);
bool extract_tokens_from_line(std::string, int,evl_tokens&);
void display_tokens(std::ostream &,const evl_tokens &);
void display_tokens(std::ofstream &, evl_tokens &);
#endif
