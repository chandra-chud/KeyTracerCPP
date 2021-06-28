#ifndef CUSTTIME_H

#define CUSTTIME_H

#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;


// namespace to reformat date time types
namespace CustTime {
	template <class T>
	string ToString(const T &);

	struct DateTime{
		int D, m, y, M, H, S;

		DateTime(){
			time_t ms;
			time(&ms);

			struct tm* info = localtime(&ms);

			// format data from info
			D = info->tm_mday;
			m = info->tm_mon + 1; // need to add 1 since january is represented 0
			y = 1900 + info->tm_year; // reference year since C came in the 70s and locatime returns time from that particular day until present day
			M = info->tm_min;
			H = info->tm_hour;
			S = info->tm_sec;
		}

		DateTime(int D, int m, int y, int M, int H, int S) : D(D), m(m), y(y), M(M), H(H), S(S) {}
		DateTime(int D, int m, int y) : D(D), m(m), y(y), M(0), H(0), S(0) {}

		DateTime Now() const{
			return DateTime(); // return current date time
		}

		// Format the current date in string
		string GetDateString() const{
			return string(D < 10 ? "0" : "") + ToString(D) +
				string(m < 10 ? ".0" : ".") + ToString(m) + "." + ToString(y);
		}
		
		// Format the current time in string
		string GetTimeString(const string &sep = ":") const{
			return string(H < 10 ? "0" : "") + ToString(H) + sep +
				string(M < 10 ? "0" : "") + ToString(M) + sep +
				string(S < 10 ? "0" : "") + ToString(S);
		}

		string GetDateTimeString(const string &sep = ":") const{
			return GetDateString() + " " + GetTimeString(sep);
		}
	};

	template <class T>
	
	string ToString(const T &e) // only able types that supports the insertion operator
	{
		ostringstream s;
		s << e;
		return s.str();
	}

	// OPTIONAL FUNCTION FOR DEBUGGING PURPOSES

	void WriteAppLog(const string &s) // reference to const string we wish to log
	{
		ofstream file("AppLog.txt", ios::app); // app stands for append file
		file << "[" << CustTime::DateTime().GetDateTimeString() << "]" << "\n" << s << endl << "\n";
		file.close();
	}
}

#endif // CUSTTIME_H