#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <ctime>

#include "bitmap_image.hpp"
#include "Strain.hpp"


/**
 * @brief test en comparant deux images, la seconde générée avec un champ de déplacement imposé par rapport à la premiere
 * @return 
*/
int test1CalculStrain()
{
	// création de deux images aléatoires l'une étant déformée par rapport à l'autre 
	bitmap_image image1 = bitmap_image(200, 200);
	bitmap_image image2= bitmap_image(200, 200);

	// champ de déplacement imposé
	deplacement s{ -11.3, 2.5, 0.06, -0.02, -0.02, 0.03 };

	position P1, P1Prime;
	position P2;
	deplacement sFound;

	decalage d;
	time_t tmm=		std::time(NULL);
	int seed = (int)tmm;
	srand(seed);

	image_drawer d1(image1);
	image_drawer d2(image2);
	// construction image 1
	//for (int i = 0; i < 100; i++) {
	//	P1.x1 =  (double)(rand() %200);
	//	P1.x2 =  (double)(rand() %200);
	//	P2.x1 =  (double)(rand() %200);
	//	P2.x2 = (double)(rand() %200);
	//	unsigned char color = rand() % 255;
	//	d1.pen_color(color, color, color); 
	//	d1.line_segment((int)P1.x1, (int)P1.x2, (int)P2.x1, (int)P2.x2);
	//}
	// 
		P1.x1 =  90;
		P1.x2 =  0;
		P2.x1 =  90;
		P2.x2 = 200;
		unsigned char color = 50;
		d1.pen_color(color, color, color); 
		d1.line_segment((int)P1.x1, (int)P1.x2, (int)P2.x1, (int)P2.x2);
		P1.x1 = 0;
		P1.x2 = 95;
		P2.x1 = 200;
		P2.x2 = 95;
		color = 125;
		d1.pen_color(color, color, color);
		d1.line_segment((int)P1.x1, (int)P1.x2, (int)P2.x1, (int)P2.x2);
		P1.x1 = 0;
		P1.x2 = 0;
		P2.x1 = 200;
		P2.x2 = 200;
		 color = 200;
		d1.pen_color(color, color, color);
		d1.line_segment((int)P1.x1, (int)P1.x2, (int)P2.x1, (int)P2.x2);
		P1.x1 = 0;
		P1.x2 = 110;
		P2.x1 = 200;
		P2.x2 = 95;
		color = 75;
		d1.pen_color(color, color, color);
		d1.line_segment((int)P1.x1, (int)P1.x2, (int)P2.x1, (int)P2.x2);
	// construction image 2
	rgb_t color1, color2;
	
	for (int i = -50; i <= 50; i++) {
		for (int j = -50; j <= 50; j++) {
			d.dx1 = i; d.dx2 = j;
			P1.x1 = 100.0 + i; P1.x2 = 100.0 + j;
			P1Prime = positionPrime(P1, s, d);
			color1.blue = image1.get_pixel_BW(P1.x1, P1.x2);
			color1.red = color1.blue; color1.green = color1.blue;
			/*image2.set_pixel(P1Prime.x1, P1Prime.x2, color1);*/
			double P1PrimeX = floor(P1Prime.x1); double P1PrimeX_ = P1Prime.x1-P1PrimeX;
			double P1PrimeY = floor(P1Prime.x2); double P1PrimeY_ = P1Prime.x2 - P1PrimeY;

			//
			image2.get_pixel(P1PrimeX, P1PrimeY, color2);
			color2.blue = color2.blue+ (double)color1.blue * (1.0 - P1PrimeX_) * (1.0 - P1PrimeY_);
			color2.red = color2.red+(double)color1.red * (1.0 - P1PrimeX_) * (1.0 - P1PrimeY_);
			color2.green = color2.green+(double)color1.red * (1.0 - P1PrimeX_) * (1.0 - P1PrimeY_);
			image2.set_pixel(P1PrimeX, P1PrimeY, color2);

			//
			image2.get_pixel(P1PrimeX+1, P1PrimeY, color2);
			color2.blue = color2.blue+(double)color1.blue * (P1PrimeX_);
			color2.red = color2.red+(double)color1.red * ( P1PrimeX_) ;
			color2.green = color2.green+(double)color1.red * (P1PrimeX_) ;
			image2.set_pixel(P1PrimeX+1, P1PrimeY, color2);

			//
			image2.get_pixel(P1PrimeX , P1PrimeY+1, color2);
			color2.blue = color2.blue+(double)color1.blue * ( P1PrimeY_);
			color2.red = color2.red+(double)color1.red * ( P1PrimeY_);
			color2.green = color2.green+(double)color1.red * ( P1PrimeY_);
			image2.set_pixel(P1PrimeX, P1PrimeY+1, color2);
			//
			image2.get_pixel(P1PrimeX+1, P1PrimeY + 1, color2);
			color2.blue = color2.blue+(double)color1.blue * ( P1PrimeX_) * ( P1PrimeY_);
			color2.red = color2.red+(double)color1.red * ( P1PrimeX_) * ( P1PrimeY_);
			color2.green = color2.green+(double)color1.red * (P1PrimeX_) * ( P1PrimeY_);
			image2.set_pixel(P1PrimeX+1, P1PrimeY+1, color2);

		}
	}

	image1.save_image("image1.bmp");
	image2.save_image("image2.bmp");
	

	// Mini et maxi pour le départ du calcul du champ de déplacement 
	position P{ 100, 100 };	// pixel central analysé
	int largeur = 50;		// largeur de la "fenetre" d'analyse 
	deplacement sMin{ -12.0,-2.0,-0.05,-0.05,-0.05,-0.05 };
	deplacement sMax{ 3.0,5.0,0.05,0.05,0.05,0.05 };
	sFound=strainCalcul("image1.bmp", "image2.bmp", P, largeur, sMin, sMax, 0.001, 10);

	return 0;
}

/**
 * @brief test réel sur deux images généré par le logiciel élement fini 
 * @param fileimage1 : première image bmp non déformée, 
 * @param fileimage2 : seconde image bmp déformée 
 * @return 
*/
int test2CalculStrainLattice(const char * fileimage1,const char* fileimage2)
{
	// position des points centraux a analyser
	//position P1{ 670,1670 };
	//position P2{ 670,1334 };
	//position P3{ 1006,1334 };
	//position P4{ 1006,1670 };
	position P1{ 673,1337 };
	position P2{ 673,1004 };
	position P3{ 1006,1004 };
	position P4{ 1006,1337 };

	// largeur de la fenêtre d'analyse 
	int largeur = 35;
	deplacement sFound1, sFound2, sFound3, sFound4;

	// déplacements mini et maxi de départ 
	deplacement sMin{ -5.0,-20.0,-0.05,-0.05,-0.05,-0.05 };
	deplacement sMax{ 5.0,0.0,0.05,0.05,0.05,0.05 };
	// Analyse
	std::cout << "\nAnalyse P1" << "\n";
	sFound1= strainCalcul(fileimage1, fileimage2, P1, largeur, sMin, sMax, 0.001, 10);
	std::cout << "\nAnalyse P2" << "\n";
	sFound2 = strainCalcul(fileimage1, fileimage2, P2, largeur, sMin, sMax, 0.001, 10);
	std::cout << "\nAnalyse P3" << "\n";
	sFound3 = strainCalcul(fileimage1, fileimage2, P3, largeur, sMin, sMax, 0.001, 10);
	std::cout << "\nAnalyse P4" << "\n";
	sFound4 = strainCalcul(fileimage1, fileimage2, P4, largeur, sMin, sMax, 0.001, 10);

	return 0;
}


int main(int argc, char* argv[]) {

	test1CalculStrain();
	test2CalculStrainLattice("lattice1.bmp", "lattice2.bmp");

}