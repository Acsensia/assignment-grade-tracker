#pragma once
#include <string.h>
#include <iostream>

namespace ASD {
	class String {
		char* str = 0;
	public:
		String(String & s) { str = 0; add(s.str); }
		String(const char *s = 0) { str = 0; add(s); }
		String(char c, size_t len);
		~String() { empty(); }
		const char * get() { return (str) ? str : ""; }
		size_t length() { return strlen(get()); }
		String & empty() { delete str; str = 0; return *this; }
		
		String & add(const char *s);
		String & add(String &s) { return add(s.str); }

		String & operator = (String &s) { empty();  return add(s.str); }
		String & operator = (const char *s) { empty();  return add(s); }

		String & operator += (String &s) { return add(s.str); }
		String & operator += (const char *s) { return add(s); }

		String & operator << (String &s) { return add(s.str); }
		String & operator << (const char *s) { return add(s); }

		char &operator [](size_t i) { return ((char *)get())[i]; }
		operator const char * () { return get(); }
		
		int compare(const char * s1) { return strcmp(get(), s1);  }

		bool operator ==(String &s1) { return compare(s1.get()) == 0;  }
		bool operator ==(const char * s1) { return compare(s1) == 0; }
		bool operator !=(String &s1) { return compare(s1.get()) != 0; }
		bool operator !=(const char * s1) { return compare(s1) != 0; }

		bool operator !() { return length() == 0; }

		bool operator >(String &s1) { return compare(s1.get()) > 0; }
		bool operator >(const char * s1) { return compare(s1) > 0; }
		bool operator <(String &s1) { return compare(s1.get()) < 0; }
		bool operator <(const char * s1) { return compare(s1) < 0; }


		String operator + (const char *s) { return String(get()).add(s); }		
		//global operator
		friend String operator + (const char *s1, String &s2);
		friend std::ostream & operator << (std::ostream &c, ASD::String &s);
		//
		String & operator += (char s);
		String & removeAt(size_t pos);
		String & remove(size_t pos, size_t count);
		String & insertAt(size_t pos, char);
		String & insert(size_t pos, const char *);
		size_t find(const char *);
	};		
	
};