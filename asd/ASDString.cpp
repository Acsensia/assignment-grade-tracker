#include "ASDString.h"
#include <iostream>

std::ostream & ASD::operator << (std::ostream &c, ASD::String &s) {
	return c << s.get();
}

ASD::String ASD::operator + (const char *s1, ASD::String &s2) {
	return ASD::String(s1).add(s2.get());
}

ASD::String::String(char c, size_t len) {
	str = new char[len + 1];
	for (size_t i = 0; i < len; i++) {
		str[i] = c;		
	}
	str[len] = 0;
}

ASD::String & ASD::String::add(const char s[]) {
	if (!s) return *this;
	//std::cout << "length: " << length() << " strlen: " << strlen(s) << " s = " << s << '\n';
	//strlen(s);
	size_t nlen = length() + strlen(s) + 1;
	char * nstr = new char[nlen];
	nstr[0] = 0;
	if (str) strcpy_s(nstr, nlen, str);
	strcat_s(nstr, nlen, s);
	delete str;
	str = nstr;
	return *this;
}