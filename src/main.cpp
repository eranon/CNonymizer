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

int main(int argc, char **argv)
{
	CNonymizer *application = new CNonymizer(argc,argv);
	if(application->runable())
	{
		application->execute();
	}
	delete application;

	return 0;
}
