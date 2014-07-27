#ifndef SYNTACTIC_H
#define SYNTACTIC_H
#include <iostream>
#include <list>
#include <sstream>
#include <assert.h>
#include <stdlib.h>
#include <map>
#include "lex.h"
struct evl_statement
{
	enum statement_type {MODULE, WIRE, COMPONENT, ENDMODULE, ASSIGN};
	statement_type type;
	evl_tokens tokens;
};
struct evl_pin
{
	std::string name;
	int bus_msb;
	int bus_lsb;
};
typedef std::list<evl_pin> evl_pins;
struct evl_component//can exchange sequence of these structs, one is build upon another
{
	std::string type;//for now, a string for type is ok, later may introduced enum
	std::string name;
	evl_pins pins;
};
typedef std::list<evl_statement> evl_statements;
typedef std::map<std::string,int> evl_wires;//<name,width>
typedef std::list<evl_component> evl_components;
bool group_tokens_into_statements(evl_statements &, evl_tokens &, evl_wires &, evl_components &, std::ofstream &);
bool move_tokens_to_statement(evl_tokens &, evl_tokens &);//pop out and insert to the statement
void display_statements(std::ofstream &, evl_statements &);//for output
bool pin_check(evl_wires &,evl_pin &);//checking semantics of pins, using current evl_wires, which may not have later wire item, also it's detecting logical error
bool process_assign_statement(evl_statement &,evl_components &,evl_wires&);
bool process_wire_statement(evl_wires &, evl_statement &);//from string to object
std::string make_net_name(std::string wire_name, int i);
void display_wires(const evl_wires &, std::ofstream &);
void display_components(const evl_components &,std::ofstream &);
void count_tokens_by_types(const evl_tokens &);//not required in this project
bool process_component_statement(evl_components &, evl_statement &,evl_wires &);
#endif
