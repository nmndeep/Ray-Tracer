#ifndef _OBJECT_H
#define _OBJECT_H

#include "Ray.h"
#include "Vect.h"
#include "Color.h"

class Object {
	public:

	Object ();

	// method functions
	virtual Color getColor () { return Color (0.0, 0.0, 0.0, 0); }

	virtual Vect getNormalAt(Vect intersection_position) {
		return Vect (0, 0, 0);
	}

	virtual double findIntersection(Ray ray) {
		return 0;
	}

};

Object::Object () {}



/////    SPHERE   //////

class Sphere : public Object {
	Vect center;
	double radius;
	Color color;

	public:

	Sphere ();

	Sphere (Vect, double, Color);

	// method functions
	Vect getSphereCenter () { return center; }
	double getSphereRadius () { return radius; }
	virtual Color getColor () { return color; }

	virtual Vect getNormalAt(Vect point) {
		// normal always points away from the center of a sphere
		Vect normal_Vect = point.vectAdd(center.negative()).normalize();
		return normal_Vect;
	}

	virtual double findIntersection(Ray ray) {
		Vect ray_origin = ray.getRayOrigin();
		double ray_origin_x = ray_origin.get_Vect_X();
		double ray_origin_y = ray_origin.get_Vect_Y();
		double ray_origin_z = ray_origin.get_Vect_Z();

		Vect ray_direction = ray.getRayDirection();
		double ray_direction_x = ray_direction.get_Vect_X();
		double ray_direction_y = ray_direction.get_Vect_Y();
		double ray_direction_z = ray_direction.get_Vect_Z();

		Vect sphere_center = center;
		double sphere_center_x = sphere_center.get_Vect_X();
		double sphere_center_y = sphere_center.get_Vect_Y();
		double sphere_center_z = sphere_center.get_Vect_Z();

		double a = 1; // normalized
		double b = (2*(ray_origin_x - sphere_center_x)*ray_direction_x) + (2*(ray_origin_y - sphere_center_y)*ray_direction_y) + (2*(ray_origin_z - sphere_center_z)*ray_direction_z);
		double c = pow(ray_origin_x - sphere_center_x, 2) + pow(ray_origin_y - sphere_center_y, 2) + pow(ray_origin_z - sphere_center_z, 2) - (radius*radius);

		double discriminant = b*b - 4*c;

		if (discriminant > 0) {
			/// the ray intersects the sphere

			// the first root
			double root_1 = ((-1*b - sqrt(discriminant))/2) - 0.000001;

			if (root_1 > 0) {
				// the first root is the smallest positive root
				return root_1;
			}
			else {
				// the second root is the smallest positive root
				double root_2 = ((sqrt(discriminant) - b)/2) - 0.000001;
				return root_2;
			}
		}
		else {
			// the ray missed the sphere
			return -1;
		}
	}

};

Sphere::Sphere () {
	center = Vect(0,0,0);
	radius = 1.0;
	color = Color(0.5,0.5,0.5, 0);
}

Sphere::Sphere (Vect centerValue, double radiusValue, Color colorValue) {
	center = centerValue;
	radius = radiusValue;
	color = colorValue;
}

/////// PLANE///////

class Plane : public Object {
	Vect normal;
	double distance;
	Color color;

	public:

	Plane ();

	Plane (Vect, double, Color);

	// method functions
	Vect getPlaneNormal () { return normal; }
	double getPlaneDistance () { return distance; }
	virtual Color getColor () { return color; }

	virtual Vect getNormalAt(Vect point) {
		return normal;
	}

	virtual double findIntersection(Ray ray) {
		Vect ray_direction = ray.getRayDirection();

		double a = ray_direction.dotProduct(normal);

		if (a == 0) {
			// ray is parallel to the plane
			return -1;
		}
		else {
			double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
			return -1*b/a;
		}
	}

};

Plane::Plane () {
	normal = Vect(1,0,0);
	distance = 0;
	color = Color(0.5,0.5,0.5, 0);
}

Plane::Plane (Vect normalValue, double distanceValue, Color colorValue) {
	normal = normalValue;
	distance = distanceValue;
	color = colorValue;
}

/////// TRIANGLE //////////////

class Triangle : public Object {
    Vect A,B,C;
	Vect normal;
	double distance;
	Color color;

	public:

	Triangle ();

	Triangle (Vect, Vect, Vect,Color);

	// method functions
	Vect getTriangleNormal () {
	    Vect CA(C.get_Vect_X()-A.get_Vect_X(),C.get_Vect_Y()-A.get_Vect_Y(),C.get_Vect_Z()-A.get_Vect_Z());
        Vect BA(B.get_Vect_X()-A.get_Vect_X(),B.get_Vect_Y()-A.get_Vect_Y(),B.get_Vect_Z()-A.get_Vect_Z());
        normal=CA.crossProduct(BA).normalize();
	    return normal;
	    }
	double getTriangleDistance () {
        normal=getTriangleNormal();
        distance=normal.dotProduct(A);
	     return distance;
	      }
	virtual Color getColor () { return color; }

	virtual Vect getNormalAt(Vect point) {
	            normal=getTriangleNormal();
		return normal;
	}

	virtual double findIntersection(Ray ray) {
		Vect ray_direction = ray.getRayDirection();
		Vect ray_origin=ray.getRayOrigin();
		normal=getTriangleNormal();
		distance=getTriangleDistance();
		double a = ray_direction.dotProduct(normal);

		if (a == 0) {
			// ray is parallel to the Triangle
			return -1;
		}
		else {
			double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
			double distance2plane=-1*b/a;
			double Qx=ray_direction.vectMult(distance2plane).get_Vect_X()+ray_origin.get_Vect_X();
            double Qy=ray_direction.vectMult(distance2plane).get_Vect_Y()+ray_origin.get_Vect_Y();
            double Qz=ray_direction.vectMult(distance2plane).get_Vect_Z()+ray_origin.get_Vect_Z();
            Vect Q(Qx,Qy,Qz);
            Vect CA(C.get_Vect_X()-A.get_Vect_X(),C.get_Vect_Y()-A.get_Vect_Y(),C.get_Vect_Z()-A.get_Vect_Z());
            Vect QA(Q.get_Vect_X()-A.get_Vect_X(),Q.get_Vect_Y()-A.get_Vect_Y(),Q.get_Vect_Z()-A.get_Vect_Z());
            double test1=(CA.crossProduct(QA)).dotProduct(normal);
            Vect BC(B.get_Vect_X()-C.get_Vect_X(),B.get_Vect_Y()-C.get_Vect_Y(),B.get_Vect_Z()-C.get_Vect_Z());
            Vect QC(Q.get_Vect_X()-C.get_Vect_X(),Q.get_Vect_Y()-C.get_Vect_Y(),Q.get_Vect_Z()-C.get_Vect_Z());
            double test2=(BC.crossProduct(QC)).dotProduct(normal);
            Vect AB(A.get_Vect_X()-B.get_Vect_X(),A.get_Vect_Y()-B.get_Vect_Y(),A.get_Vect_Z()-B.get_Vect_Z());
            Vect QB(Q.get_Vect_X()-B.get_Vect_X(),Q.get_Vect_Y()-B.get_Vect_Y(),Q.get_Vect_Z()-B.get_Vect_Z());
            double test3=(AB.crossProduct(QB)).dotProduct(normal);

			if(test1>=0 &&test2>=0 &&test3>=0){
                    //inside
                    return -1*b/a;
			}
			else
                return -1;
		}
	}

};

Triangle::Triangle () {
	A=Vect(1,0,0);
		B=Vect(0,1,0);
	C=Vect(0,0,1);

	color = Color(0.5,0.5,0.5, 0);
}

Triangle::Triangle (Vect pointA,Vect pointB,Vect pointC, Color colorValue) {
	A=pointA;
	B=pointB;
	C=pointC;
	color = colorValue;
}
#endif
