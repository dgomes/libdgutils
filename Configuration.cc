#include "Configuration.h"

configuration::configuration() {
	path = "";
	if(configfile!="") configuration(CONFIG_FILE);
};

configuration::configuration(const string file_) : configfile(file_) {
	path = "";
	try {
		read_conf();
	} catch (...) {
		cerr << "Problems trying to read " << CONFIG_FILE << endl;
	}
}

unsigned configuration::foreach(foreach_function_t *f) {
	unsigned count = 1; 
	for(map<string,configuration>::iterator i=conf.begin(); i!=conf.end(); i++, count++) {
		std::stringstream mod;
                mod << count;
		f((*this)[mod.str()].value("_NO_VALUE_ASSIGNED_"));
	}
	return count;
}

configuration &configuration::operator[](string s) {
	if(path=="") 
		conf[s].path = s;
	else 
		conf[s].path=path+"."+s;
	return conf[s];
}

const string configuration::value(const string default_value) throw(configuration_exception) {
	if(data.size()==0 && default_value!="") {
		cerr << FG_MAGENTA << BOLD << "VARIABLE " << path << " IS NOT SET, default: " << RESET << endl;
		cerr << default_value << endl;
		return default_value;
	}
	return data;
}

const bool configuration::isSet() throw(configuration_exception) {
	if(data.size()==0) { 
		cerr << FG_MAGENTA << BOLD << "VARIABLE "<< path << " IS NOT SET" << RESET << endl;
		return false;
	}
	if(data==ON1 || data==ON2)
		return true;
	return false;
}

void configuration::read_conf() {
	std::ifstream conf_file(configfile.c_str());
	string temp;
	while(!conf_file.eof()) {
		configuration *c = this; 
		getline(conf_file,temp);
		try {
			temp = temp.substr(0,temp.find(COMMENT));
			if(temp.length()==0) break;
			size_t pos=0;
			unsigned lpos=temp.find(OPERATOR);
			while(pos!=string::npos) {
				if(temp.find(SEPARATOR) > lpos) break;
				pos = temp.find(SEPARATOR);
				string prop = temp.substr(0,pos);
				if(pos!=string::npos) {
					c = &(c->conf[prop]);
				}
				temp = temp.substr(pos+1,string::npos);
			}
			string prop = temp.substr(0,temp.find(OPERATOR)); 
			string val = temp.substr(temp.find(OPERATOR)+1,string::npos);
			c->conf[prop].data=val;
		} catch (std::exception e)  {
			break;
		} catch (...) {
			cerr << "Reading Conf file <"<< configfile;
			cerr << "Error reading line: " << temp  << endl;
		}
	}
	conf_file.close();

};

configuration::~configuration() {
};
