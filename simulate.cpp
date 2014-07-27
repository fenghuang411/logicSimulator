//#include "netlist.h"
#include "simulate.h"

bool lut_gate::validate_structural_semantics()
{//std::cout<<"enter lut v\n";
		if (pins_.size()!=2) { 
			std::cerr<<name<<" structural error (pin number is not 2)"<<std::endl; 
			return false;
		}
					 pins_[0]->set_as_output();
					 pins_[1]->set_as_input();
		evl_file = evl_file +"."+ name;
		if (!read_rom(evl_file,tokens)){
			std::cerr<<"read lut file -"<<evl_file<<"- failed"<<std::endl; 
			return false;
		}
	return true;
}

int  lut_gate::compute(int p)
{
		int addr = 0;
	 for (int i = 0; i < pins_[1]->nets_.size(); ++i)
		{
			addr += (pins_[1]->nets_[i]->retrieve_logic_value()<<i);
		}
		 	return get_data(addr,pins_[0]->nets_.size(),p);

}

bool lut_gate::read_rom(std::string &evl_file, evl_tokens &tokens )
{
	std::ifstream fin(evl_file.c_str());
	if (!input_to_tokens(evl_file,tokens))
		{
			std::cerr<<"input to tokens error in lut gate"<<std::endl;
			return false;
		}
	if (pins_[0]->nets_.size() != atoi(tokens.front().str.c_str()))
		{
			std::cerr<<"output pin number no match in lut gate"<<std::endl;
			return false;
		}
	tokens.pop_front();
	if (pins_[1]->nets_.size() != atoi(tokens.front().str.c_str()))
		{
			std::cerr<<"input pin number no match in lut gate"<<std::endl;
			return false;
		}
	tokens.pop_front();
	return true;
}

int lut_gate::get_data(int addr, int width, int p)
{
	std::vector<int> ret;
	evl_tokens::iterator it = tokens.begin();
	for (int i = 0; i < addr; ++i)
		{
			++it;
		}//it points at addrth line
	ret = conv_input(*it,width);
	return ret[ret.size()-p-1];
}

bool tris_gate::validate_structural_semantics()
{//std::cout<<"enter tris v\n";
		if (pins_.size()!=3) { 
	//		std::cerr<<name<<" structural error (pin number is not 3)"<<std::endl; 
			return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
							std::cerr<<"tris's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
							return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave and v\n";
	return true;
}

int  tris_gate::compute(int p) 
{
	if (pins_[2]->get_value(0) == 1)
		return pins_[1]->get_value(0);
	else 
		return -2; // Z = -2		
}



bool and_gate::validate_structural_semantics()
{//std::cout<<"enter and v\n";
		if (pins_.size()<3) { 
	//		std::cerr<<name<<" structural error (pin number less than 3)"<<std::endl; 
			return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
							std::cerr<<"and's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
							return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave and v\n";
	return true;
}

int  and_gate::compute(int p)
{
	bool output = pins_[1]->get_value(0);
	for (int i = 2; i != pins_.size(); ++i)
		{
			output &= pins_[i]->get_value(0);
		}
	if (output == 1) return true;
	else return false;
}

bool or_gate::validate_structural_semantics()
{//std::cout<<"enter or v\n";
		if (pins_.size()<3) { 
			std::cerr<<name<<" structural error (pin number less than 3)"<<std::endl; 
			return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
							std::cerr<<"or's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
							return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave or v\n";
	return true;
}

int or_gate::compute(int p)
{
	bool output = pins_[1]->get_value(0);
	for (int i = 2; i != pins_.size(); ++i)
		{
			output |= pins_[i]->get_value(0);
		}
	if (output == 1) return true;
	else return false;
}

bool xor_gate::validate_structural_semantics()
{//std::cout<<"enter xor v\n";
		if (pins_.size()<3) { 
			std::cerr<<name<<" structural error (pin number less than 3)"<<std::endl; 
			return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
							std::cerr<<"xor's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
							return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave xor v\n";
	return true;
}

int xor_gate::compute(int p)
{
//	std::cout<<"computing a xor : ";
	bool output = pins_[1]->get_value(0);
//	std::cout<<"first input is "<<output<<std::endl;
	for (int i = 2; i != pins_.size(); ++i)
		{
//			std::cout<<"next input is "<<pins_[i]->get_value()<<std::endl;
			output ^= pins_[i]->get_value(0);
//			std::cout<<"atfer input "<<i<<" ="<<output<<std::endl;
		}
	return output;
}

bool not_gate::validate_structural_semantics()
{//std::cout<<"enter not v\n";
		if (pins_.size() != 2) { 
			std::cerr<<name<<" structural error (pin number isn't 2)"<<std::endl;
		  return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
						std::cerr<<"not's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
						return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave not v\n";
		return true;
}

int not_gate::compute(int p)
{
	if (pins_[1]->get_value(0) == 1)
		return false;
	else	return true; 
}

bool buf_gate::validate_structural_semantics()
{//std::cout<<"enter buf v\n";
		if (pins_.size() != 2) { 
			std::cerr<<name<<" structural error (pin number isn't 2)"<<std::endl;
		  return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
						std::cerr<<"buf's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
						return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave buf v\n";
		return true;
}

int  buf_gate::compute(int p)
{
	if (pins_[1]->get_value(0) == 1)
		return true;
	else return false;
}

bool flip_flop::validate_structural_semantics()
{
//		std::cout<<"enter dff v\n";
			if (pins_.size() != 2) { 
			std::cerr<<name<<" structural error (pin number isn't 2)"<<std::endl;
		  return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					if( (*pins_[i]).nets_.size() != 1) {
						std::cerr<<"dff's validate()->"<<name<<" has pin more than 1 bit"<<std::endl; 
						return false;
					}
					if (i == 0) pins_[i]->set_as_output();
					else pins_[i]->set_as_input();
				}//std::cout<<"leave dff v\n";
		return true;
}

int flip_flop::compute(int p)
{
	next_state_ = pins_[1]->get_value(0);//store
	return next_state_;
}

bool one_gate::validate_structural_semantics()
{//std::cout<<"enter one v\n";
		if (pins_.size() < 1) { 
				std::cerr<<name<<" structural error (pin number less than 1)"<<std::endl; 
				return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					pins_[i]->set_as_output();
					for (int j = 0; j <pins_[i]->nets_.size();++j)
						{
							pins_[i]->nets_[j]->value_ = 1;
						}
				}//std::cout<<"leave one v\n";
	return true;
}

int one_gate::compute(int p)
{
	return true;
}

bool zero_gate::validate_structural_semantics()
{//std::cout<<"enter zero v\n";
		if (pins_.size() < 1) { 
				std::cerr<<name<<" structural error (pin number less than 1)"<<std::endl; 
				return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					pins_[i]->set_as_output();
					for (int j = 0; j <pins_[i]->nets_.size();++j)
						{
							pins_[i]->nets_[j]->value_ = 0;
						}
				}//std::cout<<"leave zero v\n";
	return true;
}

int zero_gate::compute(int p)
{
	return false;
}

bool input_gate::validate_structural_semantics()
{//std::cout<<"enter input v\n";
		if (pins_.size() < 1) { 
				std::cerr<<name<<" structural error (pin number less than 1)"<<std::endl; 
				return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				{
					pins_[i]->set_as_output();
				}//std::cout<<"leave input v\n";
	return true;
}


bool output_gate::validate_structural_semantics()
{//std::cout<<"enter output v\n";
	if (pins_.size() < 1) { 
				std::cerr<<name<<" structural error (pin number less than 1)"<<std::endl; 
				return false;
		}
			for (int i = 0; i < pins_.size(); ++i)
				pins_[i]->set_as_input();
//			std::cout<<"leave output v\n";
	return true;
}

