#ifndef BASE64_H
#define BASE64_H

#include <vector>
#include <string>
#include <algorithm>
using namespace std;
// our encryption will use the base64 algorithm

namespace Base64{
	string EncodeB64(const string &);

	const string &SALT1 = "SVT78:]HUIGGiyyigoOIYGYGSD17293812873";
	const string &SALT2 = "[p..;.'.[p.[[.pdsposafd.csa/fdsfkipads,f";
	const string &SALT3 = "4;.'213.4;67;'.8;78.56;'87.7';6.8";
	const string &SALT4 = "]epr1]230=32,/..;weoqpxsopa{}:";
	const string &BASE64_CODES = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	// Randomly shuffle the combine the salt with the string
	string EncryptB64(string s){
		s = SALT1 + SALT2 + SALT3 + SALT4 + s;
		s = EncodeB64(s);
		reverse(s.begin(), s.end());
		s = EncodeB64(s);
		s += SALT3;
		s = EncodeB64(s);
		s += SALT4;
		s += SALT2;
		s = EncodeB64(s);
		s += SALT1;
		s = EncodeB64(s);

		return s;
	}


	string EncodeB64(const string &s){
		string ret;
		int val = 0; // index we use map to table
		int bits = -6; // used to represent number of bits in a subgroup (of six)
		const unsigned int b63 = 0x3F; // decimal value is 63

		for (const auto &c : s) {
			val = (val << 8) + c; // left binary shift by 8, same as val * 2^8 + c
			bits += 8; // add 8 to numbers of bits when extracting info (octets)

			while (bits >= 0){
				ret.push_back(BASE64_CODES[(val >> bits) & b63]); // does binary right shift + binary AND comparison
				bits -= 6;
			}
		}

		if (bits > -6) // means at least one character has been insert
			ret.push_back(BASE64_CODES[((val << 8) >> (bits + 8)) & b63]);

		while (ret.size() % 4)
			ret.push_back('=');

		return ret;
	}
}

#endif // BASE64_H