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

#pragma once

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "Line.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

class CNonymizer
{
public:
	CNonymizer(int,char**);
	~CNonymizer(void);

	void setup(int,char**);
	void execute();
	bool runable();

	void processFile(fs::path);
	void processDir(fs::path);

private:
	void readWordList();
	void createNumberMapping();

	po::variables_map config;
	bool run;

	fs::path output;
	fs::path target;
	fs::path confFile;

	std::vector< std::pair<std::string,std::string> > words;
	std::vector<unsigned int> numberMapping;

	char devider;
};
