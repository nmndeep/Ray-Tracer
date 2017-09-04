#ifndef _Camera_H
#define _Camera_H

#include "Vect.h"

class Camera {
	Vect campos, camdir, camright, camdown;

	public:

	Camera ();

	Camera (Vect, Vect, Vect, Vect);

	// method functions
	Vect get_Cam_Pos () { return campos; }
	Vect get_Cam_Dir () { return camdir; }
	Vect get_Cam_Right () { return camright; }
	Vect get_Cam_Down () { return camdown; }

};

Camera::Camera () {
	campos = Vect(0,0,0);
	camdir = Vect(0,0,1);
	camright = Vect(0,0,0);
	camdown = Vect(0,0,0);
}

Camera::Camera (Vect pos, Vect dir, Vect right, Vect down) {
	campos = pos;
	camdir = dir;
	camright = right;
	camdown = down;
}

#endif
