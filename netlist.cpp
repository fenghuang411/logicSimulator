#include "netlist.h"
#include "syntactic.h"
#include "simulate.h"
#define debug 0
// class netlist
bool input_to_tokens(std::string &in_file, evl_tokens &tokens)
{
		std::ifstream input_file(in_file.c_str());
	if(!input_file)
		{
			std::cerr<<"file unavailable "<<in_file<<std::endl;
			return false;
		}

	tokens.clear();
	std::string line;	
	for(int line_no = 1; std::getline(input_file, line); ++line_no)
		{
				for (size_t i = 0; i < line.size();)
						{
							// comments
							if (line[i] == '/') {
								++i;
								if ((i == line.size()) || (line[i] != '/')) {
									std::cerr << "LINE " << line_no
										<< ": a single / is not allowed" << std::endl;
									return false;
								}
								break; // skip the rest of the line by exiting the loop
							}

							// spaces
							if (isspace(line[i]))
							{
								++i; // skip this space character
								continue; // skip the rest of the iteration
							}
							
							// NUMBER
							if (isdigit(line[i])||((line[i]>='a')&&(line[i]<='f')))       // 0-9 a-f
							{
								size_t number_begin = i;
								for (++i; i < line.size(); ++i)
								{
									if (isspace(line[i]))
									{
										break; // [number_begin, i) is the range for the token
									}
								}
								evl_token token;
								token.type = evl_token::NUMBER;
								token.str = line.substr(number_begin, i-number_begin);
								tokens.push_back(token);
								continue;
							}
								else
							{
								std::cerr << " -> LINE " << line_no
									<< ": Decoding Failure, unknown character '" <<line[i]<<"'"<< std::endl;
								return false;
							}
						}
		}
					return true;
}
bool netlist::simulate_with_input(std::string &evl_file,std::list<gate*>::iterator it_in,std::list<gate*>::iterator it_out, std::ofstream &fout, int t)
{
		std::string in_file = evl_file +"."+ (*it_in)->name;
		//read input title
		evl_tokens tokens;
		evl_tokens spare;
		if (!input_to_tokens(in_file,tokens))//open read stream inside
			{
				std::cerr<<"simulate()-> input_to_tokens failed"<<std::endl;
				return false;
			}
		int pin_no = atoi(tokens.front().str.c_str());
		if (!(pin_no == (*it_in)->pins_.size()))
			{
				std::cerr<<"error->simulate(): input file pin no. not match"<<std::endl;
				return false;
			}
		tokens.pop_front();
		for (int i = 0; i < pin_no; ++i)
			{
				if ((*it_in)->pins_[i]->nets_.size() == atoi(tokens.front().str.c_str()) )
					tokens.pop_front();
				else 
					{
						std::cerr<<"error->simulate(): pin's width not match : "<<(*it_in)->pins_[i]->pin_index_<<"th pin of "<<(*it_in)->pins_[i]->gate_->name<<std::endl;
						return false;
					}
			}
		//begin simulate
				evl_tokens::iterator iit;
				std::vector<int> bits;
	for (int cycle = 1; cycle <= t;)
	{	
#if debug					
		std::cout<<"\nat cycle : "<<cycle<<std::endl; 
#endif
		for (int tran_no = atoi(tokens.front().str.c_str()) ; tran_no > 0; --tran_no,++cycle )
			{
				 iit = tokens.begin();	
				 ++iit;//skip first token (# of transitions)
#if debug					
				std::cout<<"\ntotal cycles remain in this line : "<<tran_no<<std::endl;
#endif
								//one cycle here
				for (std::map<std::string,net*>::iterator it = nets_.begin(); it != nets_.end(); ++it)//all nets set ?
						{
							it->second->value_ = -1;
#if debug
							std::cout<<"assign on net to ?"<<std::endl;
#endif
						}
				for (int i = 0; i <pin_no; ++i)//analysing each input pin
					{
#if debug					
							std::cout<<"convert a token to binary"<<std::endl;
#endif
							bits = conv_input(*iit,(*it_in)->pins_[i]->nets_.size());	
							assert(bits.size() >= (*it_in)->pins_[i]->nets_.size());
						for (int j = 0, k = bits.size()-1; j < (*it_in)->pins_[i]->nets_.size() ; ++j,--k)//each bit of a pin
							{
#if debug							
								std::cout<<"assign bits["<<k<<"] : "<<bits[k]<<" to net"<<std::endl;
#endif
								(*it_in)->pins_[i]->nets_[j]->value_ = bits[k];									//assign each bit
							}
						++iit; //process next token (pin)
					}
				for ( std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)//set dff's output
					{
						if ((*it)->type == "dff")
							{
#if debug							
								std::cout<<"find a dff gate"<<std::endl;
#endif
								(*it)->pins_[0]->nets_[0]->value_ = (*it)->get_next_state(); // dff drive net 
#if debug							
								std::cout<<"get dff's next state value "<< (*it)->get_next_state()<<std::endl;
#endif
							}
					}
				for (int i = 0; i < (*it_out)->pins_.size(); ++i)//compute output
					{	
#if debug					
						std::cout<<"now i="<<i<<std::endl;
#endif
						std::vector<int> dout;
						for (int j = 0; j < (*it_out)->pins_[i]->nets_.size(); ++j )
							{
#if debug							
								std::cout<<"now i="<<i<<" j="<<j<<std::endl;
#endif
								dout.insert(dout.begin(),(*it_out)->pins_[i]->nets_[j]->retrieve_logic_value());
#if debug								
								std::cout<<"get pin value of output gate pin["<<i<<"] net["<<j<<"] = "<< (*it_out)->pins_[i]->nets_[j]->retrieve_logic_value()<<std::endl;	
#endif
							}
							fout<<conv_output(dout)<<" ";
					}
					fout<<std::endl;
#if debug				
					std::cout<<"now gates_ size : "<<gates_.size()<<std::endl;
#endif
				for ( std::list<gate*>::iterator itd = gates_.begin(); itd != gates_.end(); ++itd)//set dff's output
					{
						if ((*itd)->type == "dff")
							{
#if debug							
								std::cout<<"find a dff gate"<<std::endl;
#endif
								(*itd)->compute(1); // save next state for next cycle
#if debug							
								std::cout<<"store dff's next state = "<< (*itd)->compute()<<std::endl;
#endif
							}
					}
			}
		 	iit = tokens.begin();	
			spare.clear();
		for (int i = 0; i < pin_no +1; ++i)//discard this line (pop # tokens)
			{
					spare.push_back(tokens.front());	//save the last line of tokens		
					tokens.pop_front();
			}
		if (tokens.size() == 0)
		{
			tokens = spare;	
		}
	}
	fout.close();
	return true;
}

bool netlist::simulate_without_input(std::list<gate*>::iterator it_out, std::ofstream &fout, int t)
{
		//begin simulate
	for (int cycle = 1; cycle <= t;)
	{	
#if debug					
		std::cout<<"\nat cycle : "<<cycle<<std::endl; 
#endif
								//one cycle here
				for (std::map<std::string,net*>::iterator it = nets_.begin(); it != nets_.end(); ++it)//all nets set ?
						{
							it->second->value_ = -1;
#if debug
							std::cout<<"assign on net to ?"<<std::endl;
#endif
						}
				for ( std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)//set dff's output
					{
						if ((*it)->type == "dff")
							{
#if debug							
								std::cout<<"find a dff gate"<<std::endl;
#endif
								(*it)->pins_[0]->nets_[0]->value_ = (*it)->get_next_state(); // dff drive net 
#if debug							
								std::cout<<"get dff's next state value "<< (*it)->get_next_state()<<std::endl;
#endif
							}
					}
				for (int i = 0; i < (*it_out)->pins_.size(); ++i)//compute output
					{	
#if debug					
						std::cout<<"now i="<<i<<std::endl;
#endif
						std::vector<int> dout;
						for (int j = 0; j < (*it_out)->pins_[i]->nets_.size(); ++j )
							{
#if debug							
								std::cout<<"now i="<<i<<" j="<<j<<std::endl;
#endif
								dout.insert(dout.begin(),(*it_out)->pins_[i]->nets_[j]->retrieve_logic_value());
#if debug								
								std::cout<<"get pin value of output gate pin["<<i<<"] net["<<j<<"] = "<< (*it_out)->pins_[i]->nets_[j]->retrieve_logic_value()<<std::endl;	
#endif
							}
							fout<<conv_output(dout)<<" ";
					}
					fout<<std::endl;
#if debug				
					std::cout<<"now gates_ size : "<<gates_.size()<<std::endl;
#endif
				for ( std::list<gate*>::iterator itd = gates_.begin(); itd != gates_.end(); ++itd)//set dff's output
					{
						if ((*itd)->type == "dff")
							{
#if debug							
								std::cout<<"find a dff gate"<<std::endl;
#endif
								(*itd)->compute(1); // save next state for next cycle
#if debug							
								std::cout<<"store dff's next state = "<< (*itd)->compute()<<std::endl;
#endif
							}
					}
		cycle++;
	}
	fout.close();
	return true;

}

bool  netlist::simulate(int t, std::string &evl_file)
{
	
		std::list<gate*>::iterator it_out;
	for ( it_out = gates_.begin(); it_out != gates_.end(); ++it_out)
		{
			if ((*it_out)->type == "output") break;
		}	//find output gate
	if (it_out == gates_.end())
		{
			std::cerr<<"not output gate in module"<<std::endl;
			return false;
		}
	std::string out_file = evl_file +"."+ (*it_out)->name;
	std::ofstream fout(out_file.c_str());
	fout<<(*it_out)->pins_.size()<<std::endl;
	for (int i = 0; i < (*it_out)->pins_.size(); ++i)
		{
			fout<<"pin "<<(*it_out)->pins_[i]->nets_.size();
			for (int j = 0; j < (*it_out)->pins_[i]->nets_.size(); ++j)
				{
					fout<<" "<<(*it_out)->pins_[i]->nets_[j]->name;
				}
			fout<<std::endl;
		}
		std::list<gate*>::iterator it_in;
		for ( it_in = gates_.begin(); it_in != gates_.end(); ++it_in)
			{
				if ((*it_in)->type == "input") break;
			}	//find inpput gate
		if (it_in == gates_.end())
			return simulate_without_input(it_out,fout,t);
		else 
			return simulate_with_input(evl_file, it_in, it_out,fout, t);
}

std::string netlist::conv_output(std::vector<int>& dout)
{
	while(dout.size()%4 != 0)//adding leading 0
		dout.insert(dout.begin(),0);
	std::string out;
	for (int i = 0, sum=0 ; i < dout.size(); i+=4)
		{
			sum = dout[i]*8 + dout[i+1]*4 + dout[i+2]*2 + dout[i+3]*1;
			switch (sum)
				{
					case 0: out+="0"; break;
					case 1:	out+="1"; break;
					case 2:out+="2"; break;
					case 3:out+="3"; break;
					case 4:out+="4"; break;
					case 5:out+="5"; break;
					case 6:out+="6"; break;
					case 7:out+="7"; break;
					case 8:out+="8"; break;
					case 9:out+="9"; break;
					case 10:out+="A"; break;
					case 11:out+="B"; break;
					case 12:out+="C"; break;
					case 13:out+="D"; break;
					case 14:out+="E"; break;
					case 15:out+="F"; break;
					default : out +="?"; break;
				} 
		}
	return out;
}


std::vector<int> conv_input(evl_token &token, size_t t)
{
	int i = token.str.size();
	std::vector<int> binary;
	for ( ; i < (t/4); ++i)//resize the string, fill unplaced 0
		{
			token.str.insert(token.str.begin(),'0');
		}
	
	for (int i = 0; i < token.str.size(); ++i)
		{
			switch(token.str[i])
			{
				case '0' :binary.push_back(0);binary.push_back(0);binary.push_back(0);binary.push_back(0);
					break;
				case '1' :binary.push_back(0);binary.push_back(0);binary.push_back(0);binary.push_back(1);
					break;
				case '2' :binary.push_back(0);binary.push_back(0);binary.push_back(1);binary.push_back(0);
					break;
				case '3' :binary.push_back(0);binary.push_back(0);binary.push_back(1);binary.push_back(1);
					break;
				case '4' :binary.push_back(0);binary.push_back(1);binary.push_back(0);binary.push_back(0);
					break;
				case '5' :binary.push_back(0);binary.push_back(1);binary.push_back(0);binary.push_back(1);
					break;
				case '6' :binary.push_back(0);binary.push_back(1);binary.push_back(1);binary.push_back(0);
					break;
				case '7' :binary.push_back(0);binary.push_back(1);binary.push_back(1);binary.push_back(1);
					break;
				case '8' :binary.push_back(1);binary.push_back(0);binary.push_back(0);binary.push_back(0);
					break;
				case '9' :binary.push_back(1);binary.push_back(0);binary.push_back(0);binary.push_back(1);
					break;
				case 'a' :binary.push_back(1);binary.push_back(0);binary.push_back(1);binary.push_back(0);
					break;
				case 'b' :binary.push_back(1);binary.push_back(0);binary.push_back(1);binary.push_back(1);
					break;
				case 'c' :binary.push_back(1);binary.push_back(1);binary.push_back(0);binary.push_back(0);
					break;
				case 'd' :binary.push_back(1);binary.push_back(1);binary.push_back(0);binary.push_back(1);
					break;
				case 'e' :binary.push_back(1);binary.push_back(1);binary.push_back(1);binary.push_back(0);
					break;
				case 'f' :binary.push_back(1);binary.push_back(1);binary.push_back(1);binary.push_back(1);
					break;
				default  : std::cerr<<"unresovled number : "<<token.str<<std::endl;
						break;	
			}
		}
	return binary;
}


netlist::~netlist()
{
	for (std::list<gate *>::iterator it_g = gates_.begin(); it_g != gates_.end(); ++it_g)
		delete *it_g;
	for (std::map<std::string,net *>::iterator it_n = nets_.begin(); it_n != nets_.end(); ++it_n)
		delete it_n->second;
}

bool netlist::create(const evl_wires &wires, const evl_components &comps)
{
	return create_nets(wires) && create_gates(comps);
}

bool netlist::create_nets(const evl_wires &wires)
{
	for (evl_wires::const_iterator it = wires.begin(); it != wires.end(); ++it)
	{
		if (it->second == 1)
			{
				if(!create_net(it->first))
					{
						std::cerr<<"create_net "<<it->first<<" error"<<std::endl;
						return false;
					}
			}
		else 
			{
				for (int i = 0; i < it->second; ++i)
					{
						if(!create_net(make_net_name(it->first,i)))
							{
								std::cerr<<"create_net "<<it->first<<" error"<<std::endl;
								return false;
							}
					}
			}
	}
	return true;
}

bool netlist::create_net(std::string net_name)
{
	assert(nets_.find(net_name) == nets_.end());
	nets_[net_name] = new net(net_name);//may fail!!!
	return true;
}

std::string netlist::make_net_name(std::string wire_name, int i)
{
	assert(i >= 0); // why "=" ?
	std::ostringstream oss;
	oss << wire_name <<"["<<i<<"]";
	return oss.str();
}

bool netlist::create_gates(const evl_components &comps)
{
		for (evl_components::const_iterator it = comps.begin(); it != comps.end(); ++it)
			{
				if(!create_gate(*it)) 
					{
						std::cerr<<"create_gate "<<it->name<<" error"<<std::endl;
						return false;
					}
			}
		return true;
}

bool netlist::create_gate(const evl_component &comp)
{
	if (comp.type == "dff"){
		gates_.push_back(new flip_flop(comp.name));
	}
	else if (comp.type == "and"){
		gates_.push_back(new and_gate(comp.name));
	}
	else if (comp.type == "or"){
		gates_.push_back(new or_gate(comp.name));
	}
	else if (comp.type == "xor"){
		gates_.push_back(new xor_gate(comp.name));
	}
	else if (comp.type == "not"){
		gates_.push_back(new not_gate(comp.name));
	}
	else if (comp.type == "buf"){
		gates_.push_back(new buf_gate(comp.name));
	}
	else if (comp.type == "one"){
		gates_.push_back(new one_gate(comp.name));
	}
	else if (comp.type == "zero"){
		gates_.push_back(new zero_gate(comp.name));
	}
	else if (comp.type == "input"){
		gates_.push_back(new input_gate(comp.name));
	}
	else if (comp.type == "output"){
		gates_.push_back(new output_gate(comp.name));
	}
	else if (comp.type == "tris"){
		gates_.push_back(new tris_gate(comp.name));
	}
	else if (comp.type == "lut"){
		gates_.push_back(new lut_gate(comp.name,evl_name));
	}
#if debug
	std::cout<<"go to gate.create()"<<std::endl;
#endif
	return gates_.back()->create(comp,nets_);
}

void netlist::save(std::string &netlist_file)
{
	std::ofstream fout(netlist_file.c_str());
	fout<<nets_.size()<<" "<<gates_.size()<<std::endl;

	for (std::map<std::string,net*>::iterator it = nets_.begin(); it != nets_.end(); ++it)
		{
			fout<<"net "<<it->first<<" "<<it->second->connections_.size()<<std::endl;
			for(std::list<pin*>::iterator it_p = it->second->connections_.begin(); it_p != it->second->connections_.end(); ++it_p )
				{
					fout<<"pin "<<(*(*it_p)).gate_->type<<" "<<(*(*it_p)).gate_->name<<" "<<(*(*it_p)).pin_index_<<std::endl;
				}
		}

  for (std::list<gate*>::iterator it = gates_.begin(); it != gates_.end(); ++it)//it points to a gate*
		{
			fout<<"gate "<<(*it)->type<<" "<<(*(*it)).name<<" "<<(*(*it)).pins_.size()<<std::endl;//two ways of derefenence
			for(int it_p = 0; it_p < (*it)->pins_.size(); ++it_p )//it_p ----one pin* in the vector
				{
					fout<<"pin "<<(*(*(*it)).pins_[it_p]).nets_.size()<<" ";
					for (int it_q = 0; it_q <(*(*(*it)).pins_[it_p]).nets_.size(); ++it_q)
						{
							fout<<(*(*(*(*it)).pins_[it_p]).nets_[it_q]).name<<" ";
						}
					fout<<std::endl;
				}
		}	
	fout.close();
}

// class gate
gate::~gate()
{
	for (size_t i = 0; i < pins_.size(); ++i)
		delete pins_[i];
}

bool gate::create(const evl_component &comp, const std::map<std::string, net *> &netlist_nets)
{
	size_t pin_index = 0;
	for(evl_pins::const_iterator it = comp.pins.begin(); it != comp.pins.end(); ++it)
		{
			create_pin(*it,pin_index,netlist_nets);
			++pin_index;
#if debug		
			std::cout<<"add 1 pin"<<std::endl;
#endif
		}
	return validate_structural_semantics();
}


bool gate::create_pin(const evl_pin &p, size_t pin_index, const std::map<std::string,net *> &netlist_nets)
{
	pins_.push_back(new pin);
	return pins_.back()->create(this,pin_index,p,netlist_nets);
}

std::string gate::get_type() const
{
	return type;
}

std::string gate::get_name() const
{
	return name;
}
// class pin
bool pin::create(gate *g, size_t pin_index, const evl_pin &p, const std::map<std::string,net *> &netlist_nets)
{
	gate_ = g;
	pin_index_ = pin_index;
	if (p.bus_msb == -1)//1-bit
		{
			std::map<std::string,net *>::const_iterator it = netlist_nets.find(p.name);
			if(it == netlist_nets.end())
				{
					std::cerr<<"netlist_nets.find() for "<<p.name<<" error"<<std::endl;
					return false;
				}
			else
				{
					nets_.push_back(it->second);
					it->second->append_pin(this);
				}
		}
	else//bus
		{
			for (size_t i = p.bus_lsb ; i <= p.bus_msb ; ++i)
				{
						std::map<std::string,net *>::const_iterator it = netlist_nets.find(make_net_name(p.name,i));
						if(it == netlist_nets.end())
							{
								std::cerr<<"netlist_nets.find() for "<<p.name<<" ["<<i<<"] error"<<std::endl;
								return false;
							}
						else
							{
								nets_.push_back(it->second);
								it->second->append_pin(this);
							}

				}
		}
	return true;
}

net* pin::get_net(int i)
{
	return nets_[i];
}

int pin::get_value(int p)//problem : bus
{
	if (direction == 1)// this pin is input, ask net for value
		{
#if debut
			std::cout<<"this is a input pin, find value from net"<<std::endl;
#endif
			return nets_[0]->retrieve_logic_value();
		}	
	else // this pin is output , request value by net
		{
#if debut		
			std::cout<<"this is a output pin, compute gate for value"<<std::endl;
#endif
			return gate_->compute(p);
		}
}
std::string pin::make_net_name(std::string wire_name, int i)
{
	assert(i >= 0); // why "=" ?
	std::ostringstream oss;
	oss << wire_name <<"["<<i<<"]";
	return oss.str();
}


//class net

void net::append_pin(pin *p)
{
	connections_.push_back(p);
}

int net::retrieve_logic_value()
{
	if (value_ == -1)		//value_ is ?
	{
		std::list<pin*>::iterator it;
		int ret;
		for ( it = connections_.begin(); it != connections_.end(); ++it)
		{
			if ((*it)->direction == 0)//find output pin, check solo?
				{
					//find which index of pin
					int j;
					for (j = 0; j < (*it)->nets_.size(); ++j)
						{
							if ((*it)->get_net(j) == this)
							break;
						}
					ret = (*it)->get_value(j);
					if (ret != -2)
						break;
				}
		}
#if debug
			std::cout<<"find output pin in gate "<<(*it)->gate_->type<<" of index "<<(*it)->pin_index_<< ", direction : "<<(*it)->direction<<std::endl; 
#endif
		return ret;//let pin::get_value() to comopute
		
	}
	else if (value_ == 0) return false;
	else return true;
}


