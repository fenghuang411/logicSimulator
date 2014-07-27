#include "lex.h"
bool extract_tokens_from_file(std::string file_name, evl_tokens &tokens)
{
	std::ifstream input_file(file_name.c_str());
	if(!input_file)
		{
			std::cerr<<"file unavailable "<<file_name<<"."<<std::endl;
			return false;
		}

	tokens.clear();
	std::string line;	
	for(int line_no = 1; std::getline(input_file, line); ++line_no)
		{
			if(!extract_tokens_from_line(line, line_no, tokens))
				{
					return false;
				}
		}
		return true;
}


bool extract_tokens_from_line(std::string line, int line_no,evl_tokens &tokens)
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
			// SINGLE
			if ((line[i] == '(')||(line[i] == ')')||(line[i] == '[')||(line[i] == ']')||(line[i] == ';')||(line[i] == ':')||(line[i] == ',')||(line[i]== '@')||(line[i]== '!')||(line[i]== '='))
			{
				evl_token token;
				token.type = evl_token::SINGLE;
				token.str = std::string(1,line[i]);
				tokens.push_back(token);
				++i; // we consumed this character
				continue; // skip the rest of the iteration
			}

			// NAME
			if ((isalpha(line[i]))||(line[i] == '_')||(line[i] == '\\')||(line[i] == '.'))
			{
				size_t name_begin = i;
				for (++i; i < line.size(); ++i)
				{
					if (!((isalpha(line[i]))||(isdigit(line[i]))||(line[i] == '_')||(line[i] == '\\')||(line[i] == '.')))
					{
						break; // [name_begin, i) is the range for the token
					}
				}
				evl_token token;
				token.type = evl_token::NAME;
				token.str = line.substr(name_begin, i-name_begin);
				tokens.push_back(token);
				continue;
			}
			
			// NUMBER
			if (isdigit(line[i]))       // 0-9
			{
				size_t number_begin = i;
				for (++i; i < line.size(); ++i)
				{
					if (!((line[i] >= '0') && (line[i] <= '9')))
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
	return true;
}

/*void display_tokens(std::ostream &out, const evl_tokens &tokens)
{
	for(size_t i = 0; i < tokens.size(); ++i)
	{
		if(tokens[i].type == evl_token::SINGLE)
			{
				out<< "SINGLE "<<tokens[i].str << std::endl;
			}
		else	if(tokens[i].type == evl_token::NAME)
			{
				out<< "NAME "<<tokens[i].str << std::endl;
			}
		else //(tokens[i].type == evl_token::NUMBER)
			{
				out<< "NUMVER "<<tokens[i].str << std::endl;
			}
	}
}*/
void display_tokens(std::ofstream &out,  evl_tokens &tokens)
{
	evl_tokens::iterator p;
	for(p = tokens.begin(); p != tokens.end(); ++p)
	{
		if(p->type == evl_token::SINGLE)
			{
				out<< "SINGLE "<<p->str << std::endl;
			}
		else	if(p->type == evl_token::NAME)
			{
				out<< "NAME "<<p->str << std::endl;
			}
		else //(tokens[i].type == evl_token::NUMBER)
			{
				out<< "NUMBER "<<p->str << std::endl;
			}
	}
}
