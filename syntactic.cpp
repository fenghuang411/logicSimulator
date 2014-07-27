#include "syntactic.h"
bool group_tokens_into_statements(evl_statements &statements, evl_tokens &tokens, evl_wires &wires, evl_components &components, std::ofstream &syntax_out)
{
	assert(statements.empty());
	int num_comp =0;
	std::string module_name;
	for(;!tokens.empty();)
		{
			evl_token token = tokens.front();

			if(token.type != evl_token::NAME)
				{
					std::cerr<<"Need a NAME token but found '"<<token.str<<"' on line "<<token.line_no<<std::endl;
					return false;
				}
			
			if (token.str == "module")
				{
					evl_tokens::const_iterator it = tokens.begin();
					it++;
					module_name = it->str;
					evl_statement module;
					module.type = evl_statement::MODULE;
					if(!move_tokens_to_statement(module.tokens,tokens))
						{
							std::cerr<<"move tokens to statment error "<<token.str<<std::endl;
							return false;
						}
					statements.push_back(module);
				}
			else if (token.str == "endmodule")
				{
					evl_statement endmodule;
					endmodule.type = evl_statement::ENDMODULE;
					endmodule.tokens.push_back(token);
					tokens.pop_front();//didn't call move_tokens_to_statements, so need to be pop out manually
					statements.push_back(endmodule);
				}
			else if (token.str == "wire")
				{
					evl_statement wire;
					wire.type = evl_statement::WIRE;
					if(!move_tokens_to_statement(wire.tokens,tokens))
						{
							std::cerr<<"move tokens to statment error "<<token.str<<std::endl;
							return false;
						}
					statements.push_back(wire);
					if(!process_wire_statement(wires,wire))
						{
							std::cerr<<"processing wires error"<<std::endl;
							return false;
						}
				}
			else if (token.str == "assign")
				{
					evl_statement assign;
					assign.type = evl_statement::ASSIGN;
					if(!move_tokens_to_statement(assign.tokens,tokens))
						{
							std::cerr<<"move tokens to statment error "<<token.str<<std::endl;
							return false;
						}
					statements.push_back(assign);
/*
					if(!process_assign_statement(assign,components))
						{
							std::cerr<<"processing assign error"<<std::endl;
							return false;
						}
*/
				}
			else
				{
					evl_statement component;
					component.type = evl_statement::COMPONENT;
					if(!move_tokens_to_statement(component.tokens,tokens))
						{
							std::cerr<<"move tokens to statment error "<<token.str<<std::endl;
							return false;
						}
					statements.push_back(component);
					num_comp++;
				}
		}

			for (evl_statements::iterator it = statements.begin(); it != statements.end(); ++it)
			{
				if (it->type == evl_statement::ASSIGN)
				{
					if(!process_assign_statement((*it),components,wires))//add component into components
						{
							std::cerr<<"processing components error"<<std::endl;
							return false;
						}
					
				}
			}

			for (evl_statements::iterator it = statements.begin(); it != statements.end(); ++it)
			{
				if (it->type == evl_statement::COMPONENT)
				{
					if(!process_component_statement(components,(*it),wires))//add component into componnets
						{
							std::cerr<<"processing components error"<<std::endl;
							return false;
						}
					
				}
			}

		syntax_out<<"module "<<module_name<<" "<<wires.size()<<" "<<num_comp<<std::endl;
		display_wires(wires,syntax_out);
		display_components(components,syntax_out);
	return true;
}
bool move_tokens_to_statement(evl_tokens &statement_tokens, evl_tokens &tokens)
{
				assert(statement_tokens.empty());
				assert(!tokens.empty());
					for(;!tokens.empty();)
						{
							statement_tokens.push_back(tokens.front());
							tokens.pop_front();
							if(statement_tokens.back().str == ";") break;
						}
					if (statement_tokens.back().str != ";")
						{
							std::cerr<<"Look for  ';' but reach the end of file"<<std::endl;
						return false;
						}
				return true;
}
void display_statements(std::ofstream &out,  evl_statements &statements)
{
	evl_statements::iterator p;
	evl_tokens::iterator q;
	for(p = statements.begin(); p != statements.end(); ++p)
	{
		if(p->type == evl_statement::MODULE)
			{
				out<<"MODULE ";
				for(q = p->tokens.begin(); q != p->tokens.end();++q)
					{
							out<<q->str<<" ";
					}
				out<<std::endl;

			}
		else	if(p->type == evl_statement::ENDMODULE)
			{
				out<<"ENDMODULE ";
				for(q = p->tokens.begin(); q != p->tokens.end();++q)
					{
							out<<q->str<<" ";
					}
				out<<std::endl;

			}
		else if(p->type == evl_statement::WIRE)
			{
				out<<"WIRE ";
				for(q = p->tokens.begin(); q != p->tokens.end();++q)
					{
							out<<q->str<<" ";
					}
				out<<std::endl;
				
			}
		else
			{
				out<<"COMPONENT ";
				for(q = p->tokens.begin(); q != p->tokens.end();++q)
				{
								out<<q->str<<" ";
					}
				out<<std::endl;

			}
	}
}
bool process_wire_statement(evl_wires &wires, evl_statement &s)
{	
	assert(s.type == evl_statement::WIRE);

	enum state_type {INIT, WIRE, DONE, WIRES, WIRE_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE};
	int width = 1;
	state_type state = INIT;
	for(;!s.tokens.empty() && (state != DONE); s.tokens.pop_front())//consumming the list
		{
			evl_token t = s.tokens.front();
			if(state == INIT)
				{
					if(t.str == "wire")
						{
							state = WIRE;
						}	
					else
						{
							std::cerr<<"need 'wire' but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if (state == WIRE)
				{
					if(t.type == evl_token::NAME)
						{
							wires.insert(std::make_pair(t.str,width));
							state = WIRE_NAME;
						}
					else if (t.str == "[")
						{
							state = BUS;
						}
					else
						{
							std::cerr<<"need NAME  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if(state == BUS)
				{
					if(t.type == evl_token::NUMBER)
						{
							width = atoi(t.str.c_str())+1;
							state=BUS_MSB;
						}
					else
						{
							std::cerr<<"need NUMBER but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if(state == BUS_MSB)
				{
					if(t.str == ":")
						{
							state = BUS_COLON;
						}
					else
						{
							std::cerr<<"need ':'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}

				}
			else if (state == BUS_COLON)
				{
					if(t.str == "0")
						{
							state = BUS_LSB;
						}
					else
						{
							std::cerr<<"need '0'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if (state == BUS_LSB)
				{
					if(t.str == "]")
						{
							state = BUS_DONE;
						}
					else
						{
							std::cerr<<"need ']'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if (state == BUS_DONE)
				{
					if (t.type == evl_token::NAME)
						{
							wires.insert(std::make_pair(t.str,width));
							state = WIRE_NAME;
						}
					else
						{
							std::cerr<<"need NAME  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == WIRES)
				{
					if(t.type == evl_token::NAME)
						{
							evl_wires::iterator p = wires.find(t.str);
							if(p != wires.end())
								{
									std::cerr<<"wire '"<<t.str<<"' on line "<<t.line_no<<"is already defined"<<std::endl;
									return false;
								}
							wires[t.str] = width;
							state = WIRE_NAME;
						}
					else
						{
							std::cerr<<"need NAME  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}
			else if (state == WIRE_NAME)
				{
					if(t.str == ",")
						{
							state = WIRES;
						}
					else if(t.str == ";")
						{
							state = DONE;
						}
					else
						{
							std::cerr<<"need ',' or ';'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
					}
			else 
				{
					assert(false);
				}
		}
	if(!s.tokens.empty() ||(state != DONE))
		{
			std::cerr << "statement was illegally ended\n"<<std::endl;
			return false;
		}
		return true;
}	
void display_wires(const evl_wires &wires, std::ofstream &out)//"display" means to store stream to file
{
	evl_wires::const_iterator p;
	for(p = wires.begin(); p != wires.end();++p)
		{
			out<< "wire "<<p->first <<" "<<p->second<<std::endl;
		}
}

void count_tokens_by_types(const evl_tokens &tokens)
{
	typedef std::map<evl_token::token_type,int> token_type_table;
	token_type_table type_counts;

	for(evl_tokens::const_iterator it = tokens.begin(); it != tokens.end();++it)
		{
			token_type_table::iterator map_p = type_counts.find(it->type);
			if(map_p == type_counts.end())	type_counts.insert(std::make_pair(it->type,1));
			else ++type_counts[it->type];
		}
		for(token_type_table::iterator map_it = type_counts.begin(); map_it != type_counts.end(); ++map_it)
			{
				std::string type_name;
				type_name = (map_it->first == 0) ? "NAME" :((map_it->first == 1)? "NUMBER" :"SINGLE");//tricky
				std::cout<<"count of "<<type_name<<"S is "<<map_it->second<<std::endl;
			}
}

bool process_assign_statement(evl_statement &s, evl_components &comps, evl_wires& wires)
{
	assert(s.type == evl_statement::ASSIGN);
	enum state_type {INIT,LHS,LHS_NAME,LHS_BUS,LHS_MSB,LHS_COLON,LHS_LSB,LHS_DONE,RHS,RHS_NAME,RHS_BUS,RHS_COLON,RHS_MSB,RHS_LSB,RHS_DONE,DONE};
	state_type state = INIT;
	evl_pin lhs,rhs;
	
	for(;!s.tokens.empty() && (state != DONE); s.tokens.pop_front())//consumming the list
	{
		evl_token t = s.tokens.front();

		if (state == INIT)
			{
				if(t.type == evl_token::NAME)
					{
						if (t.str == "assign")
						state = LHS;
					}
			}
		else if (state == LHS)
			{
				if (t.type == evl_token::NAME)
					{
						lhs.name = t.str;
						lhs.bus_msb = -1;
						lhs.bus_lsb = -1;
						state = LHS_NAME;
					}
			}
		else if (state == LHS_NAME)
			{
				if (t.str == "[")
					state = LHS_BUS;
				else if (t.str == "=")
					state = RHS;
			}
		else if (state == LHS_BUS)
			{
				if (t.type == evl_token::NUMBER)
					{
						lhs.bus_msb = atoi(t.str.c_str());
						state = LHS_MSB;
					}
			}
		else if (state == LHS_MSB)
			{
				if (t.str == "]")
					state = LHS_DONE;
				else if (t.str == ":")
					state = LHS_COLON;
			}
		else if (state == LHS_COLON)
			{
				if (t.type == evl_token::NUMBER)
					lhs.bus_lsb = atoi(t.str.c_str());
					state = LHS_LSB;
			}
		else if (state == LHS_LSB)
			{
				if (t.str == "]")
					state = LHS_DONE;
			}
		else if (state == LHS_DONE)
			{
				if (t.str == "=")
					state = RHS;
			}
		else if (state == RHS)
			{
				if (t.type == evl_token::NAME)
				{
					rhs.name = t.str;
					rhs.bus_msb = -1;
					rhs.bus_lsb = -1;
					state = RHS_NAME;
				}
			}
		else if (state == RHS_NAME)
			{
					if (t.str == "[")
						state = RHS_BUS;
					else if (t.str == ";")
						state = DONE;
			}
		else if (state == RHS_BUS)
			{
				if (t.type == evl_token::NUMBER)
					{
						rhs.bus_msb = atoi(t.str.c_str());
						state = RHS_MSB;
					}
			}
		else if (state == RHS_MSB )
			{
				if (t.str == "]")
				state = RHS_DONE;
				else if (t.str == ":")
				state = RHS_COLON;
			}
		else if (state == RHS_COLON )
			{
				if (t.type == evl_token::NUMBER)
					{
						rhs.bus_lsb = atoi(t.str.c_str());
						state = RHS_LSB;
					}
			}
		else if (state == RHS_LSB)
			{
				if (t.str == "]")
					state = RHS_DONE;
			}
		else if (state == RHS_DONE)
			{
				if (t.str == ";")
				state = DONE;
			}
		else 
			assert(false);
	}
	if (!s.tokens.empty() || (state != DONE))
		{
			std::cerr<<"assign statement was illegally ended"<<std::endl;
			return false;
		}
	if(!pin_check(wires,lhs))	
		{
			std::cerr<<"process_assign_statement()->pin name "<<lhs.name<<" pin_check failed"<<std::endl;
			return false;
		}
	if(!pin_check(wires,rhs))	
		{
			std::cerr<<"process_assign_statement()->pin name "<<rhs.name<<" pin_check failed"<<std::endl;
			return false;
		}
	if((lhs.bus_msb-lhs.bus_lsb)!=(rhs.bus_msb-rhs.bus_lsb))
		{
			std::cerr<<"unmatched pin width of assign statement"<<std::endl;
			return false;
		}
	//	std::cout<<lhs.name<<"="<<lhs.bus_msb<<" "<<lhs.bus_lsb<<"--"<<rhs.name<<"="<<rhs.bus_msb<<" "<<rhs.bus_lsb<<std::endl;
	for (int i = lhs.bus_lsb,j = rhs.bus_lsb; i <= lhs.bus_msb; ++i,++j)
		{
				evl_component buf;
				buf.type = "buf";
				buf.name = "ASS("+lhs.name+","+rhs.name+")";
				evl_pin pl,pr;
				pl.bus_msb = -1;
				pl.bus_lsb = -1;
				pr.bus_msb = -1;
				pr.bus_lsb = -1;
				pl.name = make_net_name(lhs.name,i);
				pr.name = make_net_name(rhs.name,j);
				buf.pins.push_back(pl);
				buf.pins.push_back(pr);
				comps.push_back(buf);
		}
	
	return true;
}

std::string make_net_name(std::string wire_name, int i)
{
	assert(i >= 0); // why "=" ?
	std::ostringstream oss;
	oss << wire_name <<"["<<i<<"]";
	return oss.str();
}


bool process_component_statement(evl_components &comps, evl_statement &s, evl_wires &wires)
{	
	assert(s.type == evl_statement::COMPONENT);

	enum state_type {INIT, TYPE, NAME, PINS, PIN_NAME, BUS, BUS_MSB, BUS_COLON, BUS_LSB, BUS_DONE,PINS_DONE,DONE};
	state_type state = INIT;
	evl_component comp;
	evl_pin pin;//should be outside th for loop, otherwise compiling error
	for(;!s.tokens.empty() && (state != DONE); s.tokens.pop_front())//consumming the list
		{
			evl_token t = s.tokens.front();

			if(state == INIT)
				{
					if(t.type == evl_token::NAME)
						{
							comp.type = t.str;
							comp.name = "NONE";
							state = TYPE;
						}	
					else
						{
							std::cerr<<"need NAME type but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == TYPE)
				{
					if(t.type == evl_token::NAME)
						{
							comp.name = t.str;
							state = NAME;
						}
					else if (t.str == "(")
						{
							state = PINS;
						}
					else
						{
							std::cerr<<"need NAME or (  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == NAME)
				{
					if (t.str == "(")
						{
							state = PINS;
						}
					else
						{
							std::cerr<<"need '('  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if(state == BUS)
				{
					if(t.type == evl_token::NUMBER)
						{
							pin.bus_msb = atoi(t.str.c_str());
							state=BUS_MSB;
						}
					else
						{
							std::cerr<<"need NUMBER but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if(state == BUS_MSB)
				{
					if(t.str == ":")
						{
							state = BUS_COLON;
						}
					else if(t.str == "]")
						{
							state = BUS_DONE;
						}
					else
						{
							std::cerr<<"need ':' or ']' but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == BUS_COLON)
				{
					if(t.type == evl_token::NUMBER)
						{
							pin.bus_lsb = atoi(t.str.c_str());
							state = BUS_LSB;
						}
					else
						{
							std::cerr<<"need '0'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == BUS_LSB)
				{
					if(t.str == "]")
						{
							state = BUS_DONE;
						}
					else
						{
							std::cerr<<"need ']'  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == BUS_DONE)
				{
					if (t.str == ")")
						{
						if(!pin_check(wires,pin))	
							{
								std::cerr<<"pin name "<<pin.name<<" pin_check failed"<<std::endl;
								return false;
							}
							comp.pins.push_back(pin);
							state = PINS_DONE;
						}
					else if(t.str == ",")
						{
						if(!pin_check(wires,pin))	
							{
								std::cerr<<"pin name "<<pin.name<<" pin_check failed"<<std::endl;
								return false;
							}
							comp.pins.push_back(pin);
							state = PINS;
						}
					else
						{
							std::cerr<<"need ')' or ','  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == PINS)
				{
					if(t.type == evl_token::NAME)
						{
							pin.name = t.str;
							pin.bus_msb = -1;
							pin.bus_lsb = -1;
							state = PIN_NAME;
						}
					else
						{
							std::cerr<<"need NAME  but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
				}

			else if (state == PIN_NAME)
				{
					if(t.str == ",")
						{	
						if(!pin_check(wires,pin))	
							{
								std::cerr<<"pin name "<<pin.name<<" pin_check failed"<<std::endl;
								return false;
							}
							comp.pins.push_back(pin);
							state = PINS;
						}
					else if(t.str == ")")
						{
						if(!pin_check(wires,pin))	
							{
								std::cerr<<"pin name "<<pin.name<<" pin_check failed"<<std::endl;
								return false;
							}
							comp.pins.push_back(pin);
							state = PINS_DONE;
						}
					else if(t.str == "[")
						{
							state = BUS;
						}
					else
						{
							std::cerr<<"need ',' or ')' or '[' but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
							return false;
						}
					}

				else if (state == PINS_DONE)
					{
						if (t.str == ";")
							{
							state = DONE;
							comps.push_back(comp);
							}
						else
							{
								std::cerr<<"need ';' but found '"<<t.str<<"' on line "<<t.line_no<<std::endl;
								return false;
							}
					}
			else 
				{
					assert(false);
				}
		}
	if(!s.tokens.empty() ||(state != DONE))
		{
			std::cerr << "statement was illegally ended\n"<<std::endl;
			return false;
		}
		return true;
}

bool pin_check(evl_wires &wires,evl_pin &pin)
{
	
	evl_wires::const_iterator it = wires.find(pin.name);
	if (it == wires.end())
		{
			std::cerr<<"pin name "<<pin.name<<" doesn't exists in wire table"<<std::endl;
			return false;
		}
	if (it->second >1)//when is a bus
	{// every if should be exclusive with another
		if ((pin.bus_msb == -1) && (pin.bus_lsb == -1))
		{
			pin.bus_msb = it->second -1;
			pin.bus_lsb = 0;
		}
		else if (pin.bus_msb != -1 && pin.bus_lsb != -1)
		{ 
			if(!((it->second > pin.bus_msb) && (pin.bus_msb >= pin.bus_lsb)&&(pin.bus_lsb>=0)))
			{
				std::cerr<<"width > msb >= lsb >= 0 not satisfied about "<<pin.name<<std::endl;
				return false;
			}
		}
		else if((pin.bus_msb != -1)&&(pin.bus_lsb = -1))
		{
			if (!(it->second > pin.bus_msb)&&(pin.bus_msb >= 0))
			{
				std::cerr<<"width > msb >= 0 not satisfied about "<<pin.name<<std::endl;
				return false;
			}
			pin.bus_lsb = pin.bus_msb;
		}
	}
	return true;
}
	
void display_components(const evl_components &comps, std::ofstream &out)
{
	evl_components::const_iterator p;
	evl_pins::const_iterator pp;
	for(p = comps.begin(); p != comps.end();++p)
		{
			out<< "component "<<p->type<<" "<<p->name<<" "<<p->pins.size()<<std::endl;
			for (pp = p->pins.begin();pp != p->pins.end();++pp)
				{
					out<<"pin "<<pp->name<<" "<<pp->bus_msb<<" "<<pp->bus_lsb<<std::endl;
				}
		}
}


