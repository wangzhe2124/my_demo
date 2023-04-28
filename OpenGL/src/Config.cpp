#include "Config.h"

/* static */
template<class T>
std::string Config::T_as_string(const T& t)
{
	// Convert from a T to a string  
	// Type T must support << operator  
	std::ostringstream ost;
	ost << t;
	return ost.str();
}

std::string Config::T_as_string(const std::vector<float>& t)
{
	std::ostringstream ost;
	ost << "{";
	int cnt = 0;
	for (auto it = t.begin(); it != t.end(); it++) {
		if (it != t.begin()) {
			ost << " ,";
		}
		if (cnt % 10 == 0)
			ost << "\n";
		cnt++;
		ost << *it;

	}
	ost << "\n}";
	return ost.str();
}
/* static */
template<class T>
T Config::string_as_T(const std::string& s)
{
	// Convert from a string to a T  
	// Type T must support >> operator  
	T t;
	std::istringstream ist(s);
	ist >> t;
	return t;
}


/* static */
template<>
inline std::string Config::string_as_T<std::string>(const std::string& s)
{
	// Convert from a string to a string  
	// In other words, do nothing  
	return s;
}


/* static */
template<>
inline bool Config::string_as_T<bool>(const std::string& s)
{
	// Convert from a string to a bool  
	// Interpret "false", "F", "no", "n", "0" as false  
	// Interpret "true", "T", "yes", "y", "1", "-1", or anything else as true  
	bool b = true;
	std::string sup = s;
	for (std::string::iterator p = sup.begin(); p != sup.end(); ++p)
		*p = toupper(*p);  // make string all caps  
	if (sup == std::string("FALSE") || sup == std::string("F") ||
		sup == std::string("NO") || sup == std::string("N") ||
		sup == std::string("0") || sup == std::string("NONE"))
		b = false;
	return b;
}


template<class T>
T Config::Read(const std::string& key) const
{
	// Read the value corresponding to key  
	mapci p = m_Contents.find(key);
	if (p == m_Contents.end()) throw Key_not_found(key);
	return string_as_T<T>(p->second);
}

std::vector<float> Config::ReadVector(const std::string& key) const
{
	// Read the value corresponding to key  
	mapci p = m_Contents.find(key);
	if (p == m_Contents.end()) throw Key_not_found(key);
	std::vector<float> t;
	/*	for (auto it = p->second.begin(); it != p->second.end(); it++)
		{
			if (*it != '\0' && *it != ' ' && *it != '\n')
			{

				std::stringstream ost;
				ost << *it;
				std::string st = ost.str();

				float sb = std::atof(st.c_str());
				std::cout << st.c_str();
				t.push_back(sb);
			}
		}*/
	std::string word = p->second;
	word.erase(0, word.find_first_not_of("{"));
	word.erase(word.find_last_not_of("}") + 1);
	while (word.find(",") != word.npos)
	{
		std::string preword = word.substr(0, word.find(','));
		Trim(preword);
		preword.erase(preword.find_last_not_of("f") + 1);
		t.push_back(std::stof(preword));
		word = word.substr(word.find(',') + 1);
	}
	t.push_back(std::stof(word));
	return t;
}
template<class T>
T Config::Read(const std::string& key, const T& value) const
{
	// Return the value corresponding to key or given default value  
	// if key is not found  
	mapci p = m_Contents.find(key);
	if (p == m_Contents.end()) return value;
	return string_as_T<T>(p->second);
}


template<class T>
bool Config::ReadInto(T& var, const std::string& key) const
{
	// Get the value corresponding to key and store in var  
	// Return true if key is found  
	// Otherwise leave var untouched  
	mapci p = m_Contents.find(key);
	bool found = (p != m_Contents.end());
	if (found) var = string_as_T<T>(p->second);
	return found;
}


template<class T>
bool Config::ReadInto(T& var, const std::string& key, const T& value) const
{
	// Get the value corresponding to key and store in var  
	// Return true if key is found  
	// Otherwise set var to given default  
	mapci p = m_Contents.find(key);
	bool found = (p != m_Contents.end());
	if (found)
		var = string_as_T<T>(p->second);
	else
		var = value;
	return found;
}


template<typename T>
void Config::Add(const std::string& in_key, const T& value)
{
	// Add a key with given value  
	std::string v = T_as_string(value);
	std::string key = in_key;
	Trim(key);
	Trim(v);
	m_Contents[key] = v;
	return;
}

using namespace std;


Config::Config(string filename, string delimiter,
	string comment)
	: m_Delimiter(delimiter), m_Comment(comment)
{
	// Construct a Config, getting keys and values from given file  

	std::ifstream in(filename.c_str());

	if (!in) throw File_not_found(filename);

	in >> (*this);
}


Config::Config()
	: m_Delimiter(string(1, '=')), m_Comment(string(1, '#'))
{
	// Construct a Config without a file; empty  
}



bool Config::KeyExists(const string& key) const
{
	// Indicate whether key is found  
	mapci p = m_Contents.find(key);
	return (p != m_Contents.end());
}


/* static */
void Config::Trim(string& inout_s)
{
	// Remove leading and trailing whitespace  
	static const char whitespace[] = " \n\t\v\r\f";
	inout_s.erase(0, inout_s.find_first_not_of(whitespace));
	inout_s.erase(inout_s.find_last_not_of(whitespace) + 1U);
}


std::ostream& operator<<(std::ostream& os, const Config& cf)
{
	// Save a Config to os  
	for (Config::mapci p = cf.m_Contents.begin();
		p != cf.m_Contents.end();
		++p)
	{
		os << p->first << " " << cf.m_Delimiter << " ";
		os << p->second << std::endl;
	}
	return os;
}

void Config::Remove(const string& key)
{
	// Remove key and its value  
	m_Contents.erase(m_Contents.find(key));
	return;
}

std::istream& operator>>(std::istream& is, Config& cf)
{
	// Load a Config from is  
	// Read in keys and values, keeping internal whitespace  
	typedef string::size_type pos;
	const string& delim = cf.m_Delimiter;  // separator  
	const string& comm = cf.m_Comment;    // comment  
	const pos skip = delim.length();        // length of separator  

	string nextline = "";  // might need to read ahead to see where value ends  

	while (is || nextline.length() > 0)
	{
		// Read an entire line at a time  
		string line;
		if (nextline.length() > 0)
		{
			line = nextline;  // we read ahead; use it now  
			nextline = "";
		}
		else
		{
			std::getline(is, line);
		}

		// Ignore comments  
		line = line.substr(0, line.find(comm));

		// Parse the line if it contains a delimiter  
		pos delimPos = line.find(delim);
		if (delimPos < string::npos)
		{
			// Extract the key  
			string key = line.substr(0, delimPos);
			line.replace(0, delimPos + skip, "");

			// See if value continues on the next line  
			// Stop at blank line, next line with a key, end of stream,  
			// or end of file sentry  
			bool terminate = false;
			while (!terminate && is)
			{
				std::getline(is, nextline);
				terminate = true;

				string nlcopy = nextline;
				Config::Trim(nlcopy);
				if (nlcopy == "***") continue;

				nextline = nextline.substr(0, nextline.find(comm));
				if (nextline.find(delim) != string::npos)
					continue;

				nlcopy = nextline;
				Config::Trim(nlcopy);
				if (nlcopy != "") line += "\n";
				line += nextline;
				terminate = false;
			}

			// Store key and value  
			Config::Trim(key);
			Config::Trim(line);
			cf.m_Contents[key] = line;  // overwrites if key is repeated  
		}
	}

	return is;
}
bool Config::FileExist(std::string filename)
{
	bool exist = false;
	std::ifstream in(filename.c_str());
	if (in)
		exist = true;
	return exist;
}

void Config::ReadFile(string filename, string delimiter,
	string comment)
{
	m_Delimiter = delimiter;
	m_Comment = comment;
	std::ifstream in(filename.c_str());

	if (!in) throw File_not_found(filename);

	in >> (*this);
}