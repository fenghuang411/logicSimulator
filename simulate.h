#ifndef _SIMULATE_H
#define _SIMULATE_H
#include "netlist.h"
#include "lex.h"

class lut_gate : public gate{
public:
	evl_tokens tokens;
	std::string evl_file;	
	lut_gate(std::string name,std::string file):gate("lut",name),evl_file(file){}
	int compute(int);
	bool read_rom(std::string &, evl_tokens &);
	int  get_data(int,int,int);
	bool validate_structural_semantics();
};


class tris_gate : public gate{
public:
	tris_gate(std::string name):gate("tris",name){}
	int compute(int);
	bool validate_structural_semantics();
};


class and_gate : public gate{
public:
	and_gate(std::string name):gate("and",name){}
	int compute(int);
	bool validate_structural_semantics();
};

class or_gate : public gate{
public:
	or_gate(std::string name):gate("or",name){}
	int compute(int);
	bool validate_structural_semantics();
};

class xor_gate : public gate{
public:
	xor_gate(std::string name):gate("xor",name){}
	int compute(int);
	bool validate_structural_semantics();
};

class buf_gate : public gate{
public:
	buf_gate(std::string name):gate("buf",name){}
	int compute(int);
	bool validate_structural_semantics();
};

class flip_flop : public gate{
	bool state_,next_state_;
public:
	flip_flop(std::string name):gate("dff",name),state_(false),next_state_(false){}
	int compute(int);
	bool validate_structural_semantics();
	bool get_next_state(){return next_state_;}
};

class not_gate : public gate{
public:
	not_gate(std::string name):gate("not",name){}
	int compute(int);
	bool validate_structural_semantics();
};

class one_gate : public gate{
public:
	one_gate(std::string name):gate("one",name){}
	bool validate_structural_semantics();
	int compute(int);
};

class zero_gate : public gate{
public:
	zero_gate(std::string name):gate("zero",name){}
	bool validate_structural_semantics();
	int compute(int);
};

class input_gate : public gate{
public:
	input_gate(std::string name):gate("input",name){}
	bool validate_structural_semantics();
};

class output_gate : public gate{
public:
	output_gate(std::string name):gate("output",name){}
	bool validate_structural_semantics();
};
#endif
