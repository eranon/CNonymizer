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

#include "CNonymizer.h"

CNonymizer::CNonymizer(int argc, char **argv) : run(true)
{
	setup(argc,argv);
	srand(static_cast<unsigned int>(time(NULL)));
}

CNonymizer::~CNonymizer(void)
{
}

void CNonymizer::setup(int argc, char **argv)
{
	po::options_description cmdlineOptions( "CNonymizer command line options" );
	cmdlineOptions.add_options()
	( "version,v", "print version string" )
	( "help,h", "Help Message" )
	( "target,t", po::value<std::string>(), "File or folder (works recursive) to anonymize" )
	( "output,o", po::value<std::string>(), "Folder for the anonymized data" )
	( "config,c", po::value<std::string>(), "File with word replacements" )
	( "devider,d", po::value<char>(), "Char to break long texts in smaller parts (default: \\n)" )
	( "ip,i", "Anonymize IP-Addresses")
	( "mac,m", "Anonymize MAC-Addresses")
	( "name,n", "Anonymize Computernames")
	( "cons,s", "Anonymize Switch Ports")
	( "all,a", "Like -i -m -n -s");

	po::store( po::parse_command_line( argc, argv, cmdlineOptions ), config );
	po::notify( config );

	if(config.count("help"))
	{
		std::cout << cmdlineOptions << std::endl;
		run = false;
	}

	if(config.count("version"))
	{
		std::cout << "---- CNonymizer Version 0.1 ----\n"
					 "Patrick Burkard - BSK-Consulting\n"
					 "--------- November 2012 --------" << std::endl;

		run = false;
	}

	if(config.count("devider"))
	{
		devider = config["devider"].as<char>();
	}
	else
	{
		devider = '\n';
	}

	if(run)
	{
		if((!config.count("target")) || (!config.count("output")))
		{
			std::cout << cmdlineOptions << std::endl;
			run = false;
		}
		else
		{
			if(config.count("config"))
			{
				if(!fs::is_regular_file(config["config"].as<std::string>()))
				{
					std::cout<<"Config File not found"<<std::endl;
					run = false;
				}
			}

			if(!fs::exists(config["target"].as<std::string>()))
			{
				std::cout<<"Target for anonymization not found"<<std::endl;
				run = false;
			}

			if(!fs::is_directory(config["output"].as<std::string>()))
			{
				std::cout<<"Output parameter must be a directory"<<std::endl;
				run = false;
			}
		}
	}
}

void CNonymizer::readWordList()
{
	boost::regex ini( "^([^#=]+)=([^#]*)(#.*)?$" );
	boost::smatch hits;

	std::ifstream in(std::string(confFile.string()).c_str());
	std::string keyvalue;
	while(in.good())
	{
		std::getline(in,keyvalue);

		if(boost::regex_match(keyvalue,hits,ini))
		{
			words.push_back(std::pair<std::string,std::string>(boost::algorithm::trim_copy( hits[1].str() ),
				boost::algorithm::trim_copy( hits[2].str() )));
		}
	}
}

bool CNonymizer::runable()
{
	return run;
}

void CNonymizer::execute()
{
	target = fs::path(config["target"].as<std::string>());
	output = fs::path(config["output"].as<std::string>());

	if(config.count("config"))
	{
		confFile = fs::path(config["config"].as<std::string>());
		readWordList();
	}
	
	createNumberMapping();

	if(fs::is_directory(config["target"].as<std::string>()))
	{
		std::cout<<"DIR"<<std::endl;//Anonymize a complete directory
		fs::path indir(config["target"].as<std::string>());
		processDir(indir);
	}
	else
	{
		std::cout<<"FILE"<<std::endl;
		fs::path infile(config["target"].as<std::string>());
		processFile(infile);
	}
}

void CNonymizer::processDir(fs::path dir)
{
	std::vector< fs::path > rootDir;
	std::copy(fs::directory_iterator(dir),fs::directory_iterator(),std::back_inserter(rootDir));

	for(std::vector< fs::path >::iterator i = rootDir.begin() ; i != rootDir.end() ; ++i)
	{
		if(fs::is_directory(*i))
		{
			fs::path sub = i->relative_path();
			fs::path out = output;
			fs::path::iterator iSub = sub.begin();
			++iSub;

			for( ; iSub != sub.end() ; ++iSub)
			{
				out /= (*iSub);
			}

			fs::create_directory(out);
			processDir(*i);
		}
		else
		{
			std::cout<<"FILE: "<<i->string().c_str()<<std::endl;
			processFile(*i);
		}
	}
}

void CNonymizer::processFile(fs::path infile)
{
	//Get Input and Output file
	fs::path sub = infile.relative_path();
	fs::path out = output;
	fs::path::iterator iSub = sub.begin();

	//Forget the "user given" part
	fs::path t = target.relative_path();
	t = t.remove_filename();
	fs::path::iterator iTarget = t.begin();
	int targetLength = 0;

	for( ; iTarget != t.end() ; ++iTarget)
	{
		++iSub;
	}

	//Append subdir and filename to the outfile for recursive directory processing
	if(iSub != sub.end())
	{
		for( ; iSub != sub.end() ; ++iSub)
		{
			out /= (*iSub);
		}
	}
	else
	{
		out/=sub;
	}

	std::cout<<"OUTPUT: "<<out.string()<<std::endl;
	std::ofstream anonFile(out.c_str());

	if(!anonFile.good())
	{
		std::cout<<"Cannot open outfile for writing!"<<std::endl;
		return;
	}

	std::ifstream file(infile.c_str());
	std::string *in = new std::string;
	Line *anon = 0;

	std::cout<<"INPUT: "<<infile.string()<<std::endl;

	unsigned long count = 0;
	while(file.good())
	{
		std::getline(file,*in,devider);
		//(*in) += devider;
		anon = new Line(*in);

		if(anon == 0)
			break;

		if(config.count("config"))
		{
			anon->mangle(words);
		}

		if(config.count("ip") || config.count("all"))
		{
			anon->mangleIPs(numberMapping);
		}

		if(config.count("mac") || config.count("all"))
		{
			anon->mangleMACs(numberMapping);
		}

		if(config.count("name") || config.count("all"))
		{
			anon->mangleNames(numberMapping);
		}

		if(config.count("cons") || config.count("all"))
		{
			anon->mangleCons(numberMapping);
		}

		
		if(file.good())
			anonFile<<anon->get()<<devider;
		else
			anonFile<<anon->get();

		delete anon;
		++count;

		if(count%1000 == 0)
			std::cout<<"PROC: "<<count<<std::endl;
	}

	file.close();
	anonFile.close();
	std::cout<<"Anonymized "<<count<<" Lines"<<std::endl;
	delete in;
}

void CNonymizer::createNumberMapping()
{
	for(int i=0;i<10;++i)
	{
		numberMapping.push_back(i+48);
	}

	std::random_shuffle(numberMapping.begin(),numberMapping.end());

	for(int i=0;i<6;++i)
	{
		numberMapping.push_back(i+97);
	}

	std::random_shuffle((numberMapping.begin()+10),numberMapping.end());
}
