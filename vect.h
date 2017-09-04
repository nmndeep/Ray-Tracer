#ifndef _VECT_H
#define _VECT_H

#include "math.h"

class Vect {
	double x, y, z;
	public:

	Vect ();

	Vect (double, double, double);

	// method functions
	double get_Vect_X() { return x; }
	double get_Vect_Y() { return y; }
	double get_Vect_Z() { return z; }

	double magnitude () {
		return sqrt((x*x) + (y*y) + (z*z));
	}

	Vect normalize () {
		double magnitude = sqrt((x*x) + (y*y) + (z*z));
		return Vect (x/magnitude, y/magnitude, z/magnitude);
	}

	Vect negative () {
		return Vect (-x, -y, -z);
	}

	double dotProduct(Vect v) {
		return x*v.get_Vect_X() + y*v.get_Vect_Y() + z*v.get_Vect_Z();
	}

	Vect crossProduct(Vect v) {
		return Vect (y*v.get_Vect_Z() - z*v.get_Vect_Y(), z*v.get_Vect_X() - x*v.get_Vect_Z(), x*v.get_Vect_Y() - y*v.get_Vect_X());
	}

	Vect vectAdd (Vect v) {
		return Vect (x + v.get_Vect_X(), y + v.get_Vect_Y(), z + v.get_Vect_Z());
	}

	Vect vectMult (double scalar) {
		return Vect (x*scalar, y*scalar, z*scalar);
	}
};

Vect::Vect () {
	x = 0;
	y = 0;
	z = 0;
}

Vect::Vect (double i, double j, double k) {
	x = i;
	y = j;
	z = k;
}

#endif
