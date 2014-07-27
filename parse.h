#ifndef _PARSE_H
#define _PARSE_H
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "lex.h"
#include "syntactic.h"


bool parse_evl_file(std::string,evl_tokens &, evl_wires &, evl_statements &, evl_components &);
#endif 
