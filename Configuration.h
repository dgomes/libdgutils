/* 
 * Configuration.h
 *
 * Copyright (C) 2004 Universidade Aveiro - Instituto de Telecomunicacoes Polo A
veiro
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:     Diogo Gomes, <dgomes@av.it.pt>
 */
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

#define CONFIG_FILE "config.cfg"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <utility>
#include <exception>
#include <string>
#include <map>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <sstream>

#include "color.h"

#define OPERATOR	"="
#define SEPARATOR	"."
#define COMMENT		"#"

#define ON1	"1"
#define ON2	"on"

using namespace std;

class configuration_exception {
	public:
		configuration_exception() {};
		~configuration_exception() {};
};

typedef void foreach_function_t(const string &);

class configuration {
	public:
		configuration();
		configuration(const string file_);
		void read_conf();
		~configuration();
		configuration &operator[](string s);
		const string value(const string default_value) throw(configuration_exception);
		const bool isSet() throw(configuration_exception);
		const unsigned size() {return conf.size(); };
		unsigned foreach(foreach_function_t *f);
	protected:
		map<string,configuration> conf;
		string data;
		string configfile;
		string path;
};

#endif // _CONFIGURATION_H_
