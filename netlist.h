#ifndef _NETLIST_H
#define _NETLIST_H
#include <list>
#include <vector>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <ctype.h>
#include <map>
#include "lex.h"
#include "syntactic.h"
class netlist;
class gate;
class net;
class pin;
// gate.output->gate.inputpin->net.outputpin->backtobegin
//class net
class net{
	net(std::string &net_name){name = net_name;}
	friend class netlist;
	net(const net &){};
	net &operator=(const net &){};
	std::list<pin *> connections_;
	std::string name;
public:
	int value_;
	void append_pin(pin *);
	int retrieve_logic_value();
};
//class pin
class pin{
	friend class gate;
	friend class netlist;
	pin(){};
	pin(const pin &){};
	pin &operator=(const pin &){};
	bool create(gate *, size_t, const evl_pin &, const std::map<std::string,net *> &);
	std::string make_net_name(std::string, int);
public:
	net* get_net(int);
	gate *gate_;
	size_t pin_index_;
	void set_as_input() { direction = 1;}
	void set_as_output() { direction = 0;}
	int direction;	//1:input,0:output
	std::vector<net *> nets_;
	int get_value(int);
};
//class gate
class gate{
	friend class netlist;
	gate(){};
	gate(const gate &){};
	gate &operator=(const gate &){};
	bool create_pin(const evl_pin &, size_t, const std::map<std::string,net *> &);
	bool create(const evl_component &, const std::map<std::string, net *> &);
public:
	std::string type;
	std::string get_type() const;
	std::string get_name() const;
	virtual bool validate_structural_semantics() = 0;
	virtual int compute(int ) {assert(false); return false;}
//protected:
	std::vector<pin *> pins_;
	std::string name;
	~gate();
	gate(std::string type,std::string name):type(type),name(name){}
	virtual bool get_next_state(){assert(false); return false;}
};
//class netlist
class netlist{
public:
	std::string evl_name;
	netlist(std::string name):evl_name(name){}
	netlist(const netlist &){};
	netlist &operator=(const netlist &){};
	~netlist();
	std::list<gate *> gates_;
	std::map<std::string, net *> nets_;
	bool create_gate(const evl_component &);
	bool create_gates(const evl_components &);
	bool create_net(std::string);
	bool create_nets(const evl_wires &);
	bool create(const evl_wires &, const evl_components &);
	void save(std::string &);
	std::string make_net_name(std::string, int);
	bool simulate(int,std::string & );
	std::string conv_output(std::vector<int> & );
	bool simulate_with_input(std::string &,std::list<gate*>::iterator ,std::list<gate*>::iterator , std::ofstream &, int);
	bool simulate_without_input(std::list<gate*>::iterator, std::ofstream &, int);
};


	std::vector<int> conv_input(evl_token &,size_t);
	bool input_to_tokens(std::string &, evl_tokens &);
#endif
