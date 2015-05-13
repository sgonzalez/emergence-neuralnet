/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include <math.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <algorithm>

/// returns a random integer between x and y
inline int randInt(int x,int y) { return rand() % (y-x+1) + x; }

/// returns a random float between zero and 1
inline double randFloat() { return (rand())/(RAND_MAX+1.0); }

/// returns a random float between -1 and 1
inline double randomClamped() { return randFloat() - randFloat(); }

/// returns a random bool
inline bool randBool() {
	if (randInt(0,1)) return true;
	else return false;
}


/// clamps a variable between two values
inline void clamp(double &arg, double min, double max) {
    if (arg < min) arg = min;
	if (arg > max) arg = max;
}


/// splits a std::string into a std::vector given a delimiter character
inline std::vector<std::string> string_split(std::string s, const char delimiter) {
    size_t start = 0;
    size_t end = s.find_first_of(delimiter);
    
    std::vector<std::string> output;
    
    while (end <= std::string::npos) {
	    output.emplace_back(s.substr(start, end - start));

	    if (end == std::string::npos) break;

    	start = end + 1;
    	end = s.find_first_of(delimiter, start);
    }
    
    return output;
}