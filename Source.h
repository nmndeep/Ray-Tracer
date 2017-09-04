#ifndef _SOURCE_H
#define _SOURCE_H

class Source {
	public:

	Source();

	virtual Vect get_Light_Pos() {return Vect(0, 0, 0);}
	virtual Color get_Light_Color() {return Color(1, 1, 1, 0);}

};

Source::Source() {}

////////////////     LIGHTING ////

class Light : public Source {
	Vect position;
	Color color;

	public:

	Light ();

	Light (Vect, Color);

	// method functions
	virtual Vect get_Light_Pos () { return position; }
	virtual Color get_Light_Color () { return color; }

};

Light::Light () {
	position = Vect(0,0,0);
	color = Color(1,1,1, 0);
}

Light::Light (Vect p, Color c) {
	position = p;
	color = c;
}
#endif
