#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Source.h"
#include "Object.h"


using namespace std;

struct RGBType {
	double r;
	double g;
	double b;
};

void savebmp (const char *filename, int w, int h, int dpi, RGBType *data) {
	FILE *f;
	int k = w*h;
	int s = 4*k;
	int filesize = 54 + s;

	double factor = 39.375;
	int m = static_cast<int>(factor);

	int ppm = dpi*m;

	unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0,0,0, 54,0,0,0};
	unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0};

	bmpfileheader[ 2] = (unsigned char)(filesize);
	bmpfileheader[ 3] = (unsigned char)(filesize>>8);
	bmpfileheader[ 4] = (unsigned char)(filesize>>16);
	bmpfileheader[ 5] = (unsigned char)(filesize>>24);

	bmpinfoheader[ 4] = (unsigned char)(w);
	bmpinfoheader[ 5] = (unsigned char)(w>>8);
	bmpinfoheader[ 6] = (unsigned char)(w>>16);
	bmpinfoheader[ 7] = (unsigned char)(w>>24);

	bmpinfoheader[ 8] = (unsigned char)(h);
	bmpinfoheader[ 9] = (unsigned char)(h>>8);
	bmpinfoheader[10] = (unsigned char)(h>>16);
	bmpinfoheader[11] = (unsigned char)(h>>24);

	bmpinfoheader[21] = (unsigned char)(s);
	bmpinfoheader[22] = (unsigned char)(s>>8);
	bmpinfoheader[23] = (unsigned char)(s>>16);
	bmpinfoheader[24] = (unsigned char)(s>>24);

	bmpinfoheader[25] = (unsigned char)(ppm);
	bmpinfoheader[26] = (unsigned char)(ppm>>8);
	bmpinfoheader[27] = (unsigned char)(ppm>>16);
	bmpinfoheader[28] = (unsigned char)(ppm>>24);

	bmpinfoheader[29] = (unsigned char)(ppm);
	bmpinfoheader[30] = (unsigned char)(ppm>>8);
	bmpinfoheader[31] = (unsigned char)(ppm>>16);
	bmpinfoheader[32] = (unsigned char)(ppm>>24);

	f = fopen(filename,"wb");

	fwrite(bmpfileheader,1,14,f);
	fwrite(bmpinfoheader,1,40,f);

	for (int i = 0; i < k; i++) {
		RGBType rgb = data[i];

		double red = (data[i].r)*255;
		double green = (data[i].g)*255;
		double blue = (data[i].b)*255;

		unsigned char color[3] = {(int)floor(blue),(int)floor(green),(int)floor(red)};

		fwrite(color,1,3,f);
	}

	fclose(f);
}


int winningObjectIndex(vector<double> object_intersections) {
	// return the index of the winning intersection
	int index_of_minimum_value;

	// prevent unnessary calculations
	if (object_intersections.size() == 0) {
		// if there are no intersections
		return -1;
	}
	else if (object_intersections.size() == 1) {
		if (object_intersections.at(0) > 0) {
			// if that intersection is greater than zero then its our index of minimum value
			return 0;
		}
		else {
			// otherwise the only intersection value is negative
			return -1;
		}
	}
	else {
		// otherwise there is more than one intersection
		// first find the maximum value

		double max = 0;
		for (int i = 0; i < object_intersections.size(); i++) {
			if (max < object_intersections.at(i)) {
				max = object_intersections.at(i);
			}
		}

		// then starting from the maximum value find the minimum positive value
		if (max > 0) {
			// we only want positive intersections
			for (int index = 0; index < object_intersections.size(); index++) {
				if (object_intersections.at(index) > 0 && object_intersections.at(index) <= max) {
					max = object_intersections.at(index);
					index_of_minimum_value = index;
				}
			}

			return index_of_minimum_value;
		}
		else {
			// all the intersections were negative
			return -1;
		}
	}
}

Color getColorAt(Vect intersection_position, Vect intersecting_ray_direction, vector<Object*> scene_objects, int index_of_winning_object, vector<Source*> light_sources, double accuracy, double ambientlight) {

	Color winning_object_color = scene_objects.at(index_of_winning_object)->getColor();
	Vect winning_object_normal = scene_objects.at(index_of_winning_object)->getNormalAt(intersection_position);

	if (winning_object_color.getColorSpecial() == 2) {
		// checkered/tile floor pattern

		int square = (int)floor(intersection_position.get_Vect_X()) + (int)floor(intersection_position.get_Vect_Z());

		if ((square % 2) == 0) {
			// black tile
			winning_object_color.setColorRed(0);
			winning_object_color.setColorGreen(0);
			winning_object_color.setColorBlue(0);
		}
		else {
			// white tile
			winning_object_color.setColorRed(1);
			winning_object_color.setColorGreen(1);
			winning_object_color.setColorRed(1);
		}
	}

	Color final_color = winning_object_color.colorScalar(ambientlight);

	if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
		// reflection from objects with specular intensity
		double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
		Vect scalar1 = winning_object_normal.vectMult(dot1);
		Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
		Vect scalar2 = add1.vectMult(2);
		Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
		Vect reflection_direction = add2.normalize();

		Ray reflection_ray (intersection_position, reflection_direction);

		// determine what the ray intersects with first
		vector<double> reflection_intersections;

		for (int reflection_index = 0; reflection_index < scene_objects.size(); reflection_index++) {
			reflection_intersections.push_back(scene_objects.at(reflection_index)->findIntersection(reflection_ray));
		}

		int index_of_winning_object_with_reflection = winningObjectIndex(reflection_intersections);

		if (index_of_winning_object_with_reflection != -1) {
			// reflection ray missed everthing else
			if (reflection_intersections.at(index_of_winning_object_with_reflection) > accuracy) {
				// determine the position and direction at the point of intersection with the reflection ray
				// the ray only affects the color if it reflected off something

				Vect reflection_intersection_position = intersection_position.vectAdd(reflection_direction.vectMult(reflection_intersections.at(index_of_winning_object_with_reflection)));
				Vect reflection_intersection_ray_direction = reflection_direction;

				Color reflection_intersection_color = getColorAt(reflection_intersection_position, reflection_intersection_ray_direction, scene_objects, index_of_winning_object_with_reflection, light_sources, accuracy, ambientlight);

				final_color = final_color.colorAdd(reflection_intersection_color.colorScalar(winning_object_color.getColorSpecial()));
			}
		}
	}

	for (int light_index = 0; light_index < light_sources.size(); light_index++) {
		Vect light_direction = light_sources.at(light_index)->get_Light_Pos().vectAdd(intersection_position.negative()).normalize();

		float cosine_angle = winning_object_normal.dotProduct(light_direction);

		if (cosine_angle > 0) {
			// test for shadows
			bool shadowed = false;

			Vect distance_to_light = light_sources.at(light_index)->get_Light_Pos().vectAdd(intersection_position.negative()).normalize();
			float distance_to_light_magnitude = distance_to_light.magnitude();

			Ray shadow_ray (intersection_position, light_sources.at(light_index)->get_Light_Pos().vectAdd(intersection_position.negative()).normalize());

			vector<double> secondary_intersections;

			for (int object_index = 0; object_index < scene_objects.size() && shadowed == false; object_index++) {
				secondary_intersections.push_back(scene_objects.at(object_index)->findIntersection(shadow_ray));
			}

			for (int c = 0; c < secondary_intersections.size(); c++) {
				if (secondary_intersections.at(c) > accuracy) {
					if (secondary_intersections.at(c) <= distance_to_light_magnitude) {
						shadowed = true;
					}
					break;
				}
			}

			if (shadowed == false) {
				final_color = final_color.colorAdd(winning_object_color.colorMultiply(light_sources.at(light_index)->get_Light_Color()).colorScalar(cosine_angle));

				if (winning_object_color.getColorSpecial() > 0 && winning_object_color.getColorSpecial() <= 1) {
					// special [0-1]
					double dot1 = winning_object_normal.dotProduct(intersecting_ray_direction.negative());
					Vect scalar1 = winning_object_normal.vectMult(dot1);
					Vect add1 = scalar1.vectAdd(intersecting_ray_direction);
					Vect scalar2 = add1.vectMult(2);
					Vect add2 = intersecting_ray_direction.negative().vectAdd(scalar2);
					Vect reflection_direction = add2.normalize();

					double specular = reflection_direction.dotProduct(light_direction);
					if (specular > 0) {
						specular = pow(specular, 10);
						final_color = final_color.colorAdd(light_sources.at(light_index)->get_Light_Color().colorScalar(specular*winning_object_color.getColorSpecial()));
					}
				}

			}

		}
	}

	return final_color.clip();
}
	vector<Object*> scene_objects;

void create_Cube(Vect corner1, Vect corner2, Color color){
double c1x=corner1.get_Vect_X();
double c1y=corner1.get_Vect_Y();
double c1z=corner1.get_Vect_Z();
double c2x=corner2.get_Vect_X();
double c2y=corner2.get_Vect_Y();
double c2z=corner2.get_Vect_Z();
Vect A(c2x,c1y,c1z);
Vect B(c2x,c1y,c2z);
Vect C(c1x,c1y,c2z);
Vect D(c2x,c2y,c1z);
Vect E(c1x,c2y,c1z);
Vect F(c1x,c2y,c2z);


scene_objects.push_back(new Triangle(D,A,corner1,color));
scene_objects.push_back(new Triangle(corner1,E,D,color));
scene_objects.push_back(new Triangle(corner2,B,A,color));
scene_objects.push_back(new Triangle(A,D,corner2,color));

scene_objects.push_back(new Triangle(F,C,B,color));
scene_objects.push_back(new Triangle(B,corner2,F,color));
scene_objects.push_back(new Triangle(E,corner1,C,color));
scene_objects.push_back(new Triangle(C,F,E,color));

scene_objects.push_back(new Triangle(D,E,F,color));
scene_objects.push_back(new Triangle(F,corner2,D,color));

scene_objects.push_back(new Triangle(corner1,A,B,color));
scene_objects.push_back(new Triangle(B,C,corner1,color));

}
int thisone;

int main () {
	cout << "Tracing ..." << endl;


	int dpi = 72;
	int width = 1080;
	int height = 720;
	int n = width*height;
	RGBType *pixels = new RGBType[n];

	int depth_a = 1;
	double threshold_a = 0.1;
	double aspectratio = (double)width/(double)height;
	double ambientlight = 0.2;
	double accuracy = 0.0000001;

	Vect O (0,0,0);
	Vect X (1,0,0);
	Vect Y (0,1,0);
	Vect Z (0,0,1);

	Vect new_sphere_location (-3, 0, -2);
    Vect new_sphere_location1 (-7, 0, -4);
    Vect new_sphere_location2 (2, 0, -1);
    Vect new_sphere_location3 (-1, 0, -1);



	Vect campos (0,1,-17);

	Vect look_at (0, 0, 0);
	Vect diff_btw (campos.get_Vect_X() - look_at.get_Vect_X(), campos.get_Vect_Y() - look_at.get_Vect_Y(), campos.get_Vect_Z() - look_at.get_Vect_Z());

	Vect camdir = diff_btw.negative().normalize();
	Vect camright = Y.crossProduct(camdir).normalize();
	Vect camdown = camright.crossProduct(camdir);
	Camera scene_cam (campos, camdir, camright, camdown);

	Color white_light (1.0, 1.0, 1.0, 0);
	Color pretty_green (0.0, 1.0, 0.0, 0.3);
	Color maroon (1.0, 0.0, 0.2, 0.4);
	Color tile_floor (0.8, 0.8, 1.0, 2);
	Color blue (0.0, 0.2, 1.0,0.2);
	Color yellow (1.0, 1.0, 0.0, 0.3);
	Color magenta(1.0,0.1,0.6,0.0);
	Color gray(0.5,1.0,1.0,0.6);
	Color green(0.2,1.0,0.0,0.4);
	Vect light_position (-2,8,-2);
    Vect light_position1(5,7,1);

	Light scene_light (light_position, white_light);
    Light scene_light1(light_position1, white_light);

	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&scene_light));
		light_sources.push_back(dynamic_cast<Source*>(&scene_light1));


	// scene objects
	Sphere scene_sphere (new_sphere_location, 1, yellow);
	Sphere scene_sphere2 (new_sphere_location2, 0.7, maroon);
    Sphere scene_sphere3 (new_sphere_location1, 1, gray);
    Sphere scene_sphere5 (new_sphere_location3, 0.6, pretty_green);
  //  create_Cube(Vect(-25,0,5),Vect(25,2,5),white_light);
    //           WALLS    /////
    Triangle scene_triangle1 (Vect(-28,-1,-20),Vect(5,24,-20),Vect(28,-1,-20), blue);
    Triangle scene_triangle (Vect(-28,-1,5),Vect(5,24,5),Vect(28,-1,5), gray);
    create_Cube(Vect(-20,10,4),Vect(-2,-1,5),magenta);

    //     SKirting on WALL /////

    create_Cube(Vect(-20,-0.3,4),Vect(-2,-1,4.6),tile_floor);
    create_Cube(Vect(-2,-0.3,4.7),Vect(25,-1,5),tile_floor);


    create_Cube(Vect(6,1,1),Vect(8,-1,-1),green);
	Plane scene_plane (Y, -1, tile_floor);
	scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere2));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere3));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_triangle));
    scene_objects.push_back(dynamic_cast<Object*>(&scene_sphere5));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_triangle1));
	scene_objects.push_back(dynamic_cast<Object*>(&scene_plane));

	int thisone, index_a;
	double amnt_x, yamnt, red_temporary, green_temporary, blue_temporary;

	for (int x = 0; x < width; x++) {
		for (int y = 0; y < height; y++) {
			thisone = y*width + x;

			double red_temporary[depth_a*depth_a];
			double green_temporary[depth_a*depth_a];
			double blue_temporary[depth_a*depth_a];

			for (int aax = 0; aax < depth_a; aax++) {
				for (int aay = 0; aay < depth_a; aay++) {

					index_a = aay*depth_a + aax;


					if (depth_a == 1) {

						if (width > height) {
							amnt_x = ((x+0.5)/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + 0.5)/height;
						}
						else if (height > width) {
							amnt_x = (x + 0.5)/ width;
							yamnt = (((height - y) + 0.5)/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							amnt_x = (x + 0.5)/width;
							yamnt = ((height - y) + 0.5)/height;
						}
					}
					else {
						if (width > height) {
							amnt_x = ((x + (double)aax/((double)depth_a - 1))/width)*aspectratio - (((width-height)/(double)height)/2);
							yamnt = ((height - y) + (double)aax/((double)depth_a - 1))/height;
						}
						else if (height > width) {
							amnt_x = (x + (double)aax/((double)depth_a - 1))/ width;
							yamnt = (((height - y) + (double)aax/((double)depth_a - 1))/height)/aspectratio - (((height - width)/(double)width)/2);
						}
						else {
							amnt_x = (x + (double)aax/((double)depth_a - 1))/width;
							yamnt = ((height - y) + (double)aax/((double)depth_a - 1))/height;
						}
					}

					Vect cam_ray_origin = scene_cam.get_Cam_Pos();
					Vect cam_ray_direction = camdir.vectAdd(camright.vectMult(amnt_x - 0.5).vectAdd(camdown.vectMult(yamnt - 0.5))).normalize();

					Ray cam_ray (cam_ray_origin, cam_ray_direction);

					vector<double> intersections;

					for (int index = 0; index < scene_objects.size(); index++) {
						intersections.push_back(scene_objects.at(index)->findIntersection(cam_ray));
					}

					int index_of_winning_object = winningObjectIndex(intersections);

					if (index_of_winning_object == -1) {
						red_temporary[index_a] = 0;
						green_temporary[index_a] = 0;
						blue_temporary[index_a] = 0;
					}
					else{
						if (intersections.at(index_of_winning_object) > accuracy) {

							Vect intersection_position = cam_ray_origin.vectAdd(cam_ray_direction.vectMult(intersections.at(index_of_winning_object)));
							Vect intersecting_ray_direction = cam_ray_direction;

							Color intersection_color = getColorAt(intersection_position, intersecting_ray_direction, scene_objects, index_of_winning_object, light_sources, accuracy, ambientlight);

							red_temporary[index_a] = intersection_color.getColorRed();
							green_temporary[index_a] = intersection_color.getColorGreen();
							blue_temporary[index_a] = intersection_color.getColorBlue();
						}
					}
				}
			}

			double totalRed = 0;
			double totalGreen = 0;
			double totalBlue = 0;

			for (int iRed = 0; iRed < depth_a*depth_a; iRed++) {
				totalRed = totalRed + red_temporary[iRed];
			}
			for (int iGreen = 0; iGreen < depth_a*depth_a; iGreen++) {
				totalGreen = totalGreen + green_temporary[iGreen];
			}
			for (int iBlue = 0; iBlue < depth_a*depth_a; iBlue++) {
				totalBlue = totalBlue + blue_temporary[iBlue];
			}

			double avgRed = totalRed/(depth_a*depth_a);
			double avgGreen = totalGreen/(depth_a*depth_a);
			double avgBlue = totalBlue/(depth_a*depth_a);

			pixels[thisone].r = avgRed;
			pixels[thisone].g = avgGreen;
			pixels[thisone].b = avgBlue;
		}
	}

	savebmp("ray_traced_scene.bmp",width,height,dpi,pixels);

	delete pixels, red_temporary, green_temporary, blue_temporary;



	return 0;
}

