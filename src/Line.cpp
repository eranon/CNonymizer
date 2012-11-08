/*
	Copyright (C) 2012, Patrick Burkard, <the-dark-eye@web.de>

	This file is part of CNonymizer.

	CNonymizer is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CNonymizer is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CNonymizer. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Line.h"

Line::Line(std::string text) : line(text)
{
}

Line::~Line(void)
{
}

std::string Line::get()
{
	return line;
}

void Line::mangle(const std::vector< std::pair<std::string,std::string> > &words)
{
	if(line.size()>2)
	{
		boost::regex *current = 0;
		for(size_t i=0;i<words.size();++i)
		{
			current = new boost::regex(words[i].first);
			line = boost::regex_replace(line,(*current),words[i].second);
			delete current;
		}
	}
}

void Line::mangleIPs(const std::vector<unsigned int> &numberMapping)
{
	if(line.size()>7)
	{
		std::string x = line;

		boost::sregex_iterator ipm(line.begin(),line.end(),boost::regex("([0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3})"));
		boost::sregex_iterator end;
		
		for( ;ipm!=end;++ipm)
		{
			std::string newip(ipm->str());
			for(size_t i = 0;i<newip.size();++i)
			{
				if(newip[i] != '.')
				{
					newip[i] = static_cast<char>(numberMapping[(static_cast<int>(newip[i])-48)]);
				}
			}

			int ip_parts[4];
			char dots = '.';
			std::stringstream in_addr(newip);
			std::stringstream out_addr;

			in_addr<<newip;
			in_addr>>ip_parts[0];
			dots = in_addr.get();
			in_addr>>ip_parts[1];
			dots = in_addr.get();
			in_addr>>ip_parts[2];
			dots = in_addr.get();
			in_addr>>ip_parts[3];

			for(size_t i=0;i<4;++i)
			{
				if(ip_parts[i] > 255)
				{
					ip_parts[i] = ip_parts[i] % 255;
				}
			}

			out_addr<<ip_parts[0]<<dots<<ip_parts[1]<<dots<<ip_parts[2]<<dots<<ip_parts[3];
			x = boost::regex_replace(x,boost::regex(ipm->str()),out_addr.str());
		}

		line = x;
	}
}

void Line::mangleMACs(const std::vector<unsigned int> &numberMapping)
{
	if(line.size()>16)
	{
		std::string x = line;

		boost::sregex_iterator ipm(line.begin(),line.end(),boost::regex("([0-9a-f]{2}\\-[0-9a-f]{2}\\-[0-9a-f]{2}\\-[0-9a-f]{2}\\-[0-9a-f]{2}\\-[0-9a-f]{2})"));
		boost::sregex_iterator end;
		
		for( ;ipm!=end;++ipm)
		{
			std::string newip(ipm->str());
			for(size_t i = 0;i<newip.size();++i)
			{
				if(newip[i] != '-')
				{
					int current = static_cast<int>(newip[i]);
					if(current > 57)
					{
						newip[i] = static_cast<char>(numberMapping[(current-87)]);
					}
					else
					{
						newip[i] = static_cast<char>(numberMapping[(current-48)]);
					}
				}
			}

			x = boost::regex_replace(x,boost::regex(ipm->str()),newip);
		}

		line = x;
	}
}

void Line::mangleNames(const std::vector<unsigned int> &numberMapping)
{
	if(line.size()>9)
	{
		std::string x = line;

		boost::sregex_iterator ipm(line.begin(),line.end(),boost::regex("([0]{5,7})([0-9]{5})"));
		boost::sregex_iterator end;
		
		for( ;ipm!=end;++ipm)
		{
			boost::match_results<std::string::const_iterator> match(*ipm);
			std::string newip(match[2].str());
			for(size_t i = 0;i<newip.size();++i)
			{
				newip[i] = static_cast<char>(numberMapping[(static_cast<int>(newip[i])-48)]);
			}

			x = boost::regex_replace(x,boost::regex(std::string("[0]{5,7}"+match[2].str())),std::string(match[1].str()+newip));
		}

		line = x;
	}
}

void Line::mangleCons(const std::vector<unsigned int> &numberMapping)
{
	if(line.size()>4)
	{
		std::string x = line;

		boost::sregex_iterator ipm(line.begin(),line.end(),boost::regex("(Fa0\\/)([0-9]+)"));
		boost::sregex_iterator end;
		
		for( ;ipm!=end;++ipm)
		{
			boost::match_results<std::string::const_iterator> match(*ipm);
			std::string newcon(match[2].str());
			for(size_t i = 0;i<newcon.size();++i)
			{
				newcon[i] = static_cast<char>(numberMapping[(static_cast<int>(newcon[i])-48)]);
			}

			x = boost::regex_replace(x,boost::regex(std::string("Fa0\\/"+match[2].str())),(match[1].str()+newcon));
		}

		line = x;
	}
}
