#include "parse.h"
bool parse_evl_file(std::string evl_file, evl_tokens &tokens, evl_wires &wires,evl_statements &statements, evl_components &comps)
{

		if(!extract_tokens_from_file(evl_file,tokens))
			{
				return false;	
			}
		std::string tokens_name(evl_file);
		tokens_name += ".tokens";
		std::ofstream tokens_out(tokens_name.c_str());
		if(!tokens_out)
			{
				std::cerr<<"creat tokens output file failed"<<std::endl;
				return false;
			}
		display_tokens(tokens_out,tokens); //store in file
		tokens_out.close();
		
//		count_tokens_by_types(tokens);		//map manipulating

		std::string statements_name(evl_file);
		statements_name += ".statements";
		std::ofstream statements_out(statements_name.c_str());
		if(!statements_out)
			{
				std::cerr<<"creat statements output file failed"<<std::endl;
				return false;
			}
		std::string syntax_name(evl_file);
		syntax_name += ".syntax";
		std::ofstream syntax_out(syntax_name.c_str());
		if(!syntax_out)
			{
				std::cerr<<"creat syntax output file failed"<<std::endl;
				return false;
			}
		
		if(!group_tokens_into_statements(statements,tokens,wires,comps,syntax_out))
			{
				std::cerr<<"group_tokens_into_statements failed"<<std::endl;
				return false;
			}
		display_statements(statements_out,statements);
		statements_out.close();
		syntax_out.close();

		return true;
}
