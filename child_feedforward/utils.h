/////////////////////////
/// Santiago Gonzalez ///
/////////////////////////

#include <math.h>
#include <stdlib.h>

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