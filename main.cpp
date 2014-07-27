#include <iostream>
#include <string>
#include "lex.h"
#include "parse.h"
#include "syntactic.h"
#include "netlist.h"
int
main(int argc, char *argv[])
{
		if(argc < 2)
			{
				std::cerr<<"lack of file"<<std::endl;
				return -1;
			}
		std::string evl_file(argv[1]);
		evl_tokens tokens;
		evl_wires wires;
		evl_components comps;
		evl_statements statements;
    if (! parse_evl_file(evl_file,tokens,wires,statements,comps))
			{
				std::cerr<<"parse_evl_file error"<<std::endl;
				return -1;
			}
		netlist nl(evl_file);
		if (! nl.create(wires,comps))
			{
				std::cerr<<"nl.creat error"<<std::endl;
				return -1;
			}

		std::string nl_file = std::string(argv[1])+".netlist";
		nl.save(nl_file);
		if(!nl.simulate(1000,evl_file))
		{
			std::cerr<<"main()-> simulate failed"<<std::endl;
			return -1;
		}

		return 0;
}
