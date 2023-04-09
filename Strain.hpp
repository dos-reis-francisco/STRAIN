/**
* @file strain.hpp
* @author Francisco Dos Reis
* @version 0.0
* @date 09.04.2023
* @brief calcul de D.I.C. (Digital Image Correlation)
* @basée sur l'algorithme de Sutton, Ma, Wj Wolters, Wh Peters, Wf Ranson, et Sr McNeill. 
* « Determination of Displacements Using an Improved Digital Correlation Method ». 
* Image and Vision Computing 1, nᵒ 3 (août 1983): 133‑39. https://doi.org/10.1016/0262-8856(83)90064-1.
*/
#pragma once

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#include "bitmap_image.hpp"

/**
 * @brief classe champ de déplacement 
*/
class deplacement {
public:
	double u1, u2;				// coeffs translation 
	double du1dx1, du2dx2;		// coeffs dilatation
	double du1dx2, du2dx1;		// coeffs cisaillement
	deplacement();
	deplacement(double u1, double u2, double du1dx1, double du2dx2, double du1dx2, double du2dx1) :
		u1(u1), u2(u2), du1dx1(du1dx1), du2dx2(du2dx2), du1dx2(du1dx2), du2dx1(du2dx1)
	{}
};

/**
 * @brief position d'un point en pixels
*/
struct position {
	double x1, x2;
};

/**
 * @brief decalage en pixels
*/
struct decalage {
	double dx1, dx2;
};

/**
 * @brief fonction de calcul de la correlation 
 * @param P : point central en pixels de la fenêtre
 * @param s : champ de déplacement considéré 
 * @param largeur : largeur de la fenêtre d'analyse en pixel
 * @param image1 : image non déformée bmp en gris
 * @param image2 : image déformée bmp en gris
 * @return : valeur de la correlation (la plus grande est la meilleure)
*/
double crossCorrelation(position P, deplacement s, int largeur, bitmap_image* image1, bitmap_image* image2);

/**
 * @brief extraction de la couleur d'un pixel 
 * @param P : position du pixel (la partie après la virgule est extraite a partir des pixels voisins par bilinéarité)
 * @param image image bmp en gris de départ
 * @return couleur du pixel
*/
double valuePixel(position P, bitmap_image* image);

/**
 * @brief calcul d'une nouvelle position à partir d'un champ de déplacement donné
 * @param x position initiale
 * @param s champ de déplacement 
 * @param d décalage par rapport à la position centrale du champ de déplacement 
 * @return position après déformation
*/
position positionPrime(position x, deplacement s, decalage d);

/**
 * @brief détermine si une des valeur du tableau de correlation C est supérieure à Cmax
 * @param maxX coordonnée ligne du tableau C de la valeur max trouvée
 * @param maxY coordonnées colonne du tableau C de la valeur max trouvée
 * @param C tableau contenant les valeur de correlation
 * @param Cmax valeur maxi actualisée du maximum
 * @return true si il y a eu modification de Cmax, false sinon (dans ce cas maxX et maxY ne sont pas utiles)
*/
bool maxXY(int& maxX, int& maxY, double* C, double& bestCorrelation, int npasX, int npasY);

/**
 * @brief algorithme principal de détermination du champ de déplacement 
 * @param fileOrg image non déformée bmp grise
 * @param fileDeformed image déformée bmp grise
 * @param P position en pixel du point central d'analyse
 * @param largeur largeur de la fenêtre d'analyse en pixels
 * @param deplMin déplacement mini de départ
 * @param deplMax déplacement maxi de départ
 * @param errorPixMax erreur maxi admissible
 * @param iterMax nombre d'itérations maxi avec error<errorPixMax
 * @return déplacement trouvé
*/
deplacement strainCalcul(const char* fileOrg, const char* fileDeformed, position P, int largeur, deplacement deplMin,
	deplacement deplMax, double errorPixMax, int iterMax);


/**
 * @brief constructeur 
*/
deplacement::deplacement() {
	u1 = 0.0; u2 = 0.0;
	du1dx1 = 0.0; du2dx2 = 0.0;
	du1dx2 = 0.0, du2dx1 = 0.0;
}

double crossCorrelation(position P, deplacement s, int largeur, bitmap_image* image1, bitmap_image* image2) {
	double decalageXmin = -(double)largeur / 2.0;
	double decalageXmax = (double)largeur / 2.0;

	decalage d;

	double decalageYmin = -(double)largeur / 2.0;
	double decalageYmax = (double)largeur / 2.0;

	position Pxy, PxyPrime;
	double fxy, fxyPrime;
	double intfxyfxyPrime = 0.0;
	double intfxyfxy = 0.0;
	double intfxyPrimefxyPrime = 0.0;
	double C;

	for (d.dx1 = decalageXmin; d.dx1 < decalageXmax; d.dx1 += 1.0) {
		for (d.dx2 = decalageYmin; d.dx2 < decalageYmax; d.dx2 += 1.0) {
			Pxy.x1 = P.x1 + d.dx1;	 
			Pxy.x2 = P.x2 + d.dx2;
			fxy = valuePixel(Pxy, image1);
			PxyPrime = positionPrime(Pxy, s, d);
			fxyPrime = valuePixel(PxyPrime, image2);
			intfxyfxyPrime += fxy * fxyPrime;
			intfxyfxy += fxy * fxy;
			intfxyPrimefxyPrime += fxyPrime * fxyPrime;
		}
	}
	if (intfxyfxy * intfxyPrimefxyPrime != 0) {
		C = sqrt(intfxyfxyPrime / (intfxyfxy * intfxyPrimefxyPrime));
		return C;
	}
	else
		return 0.0;

}

double valuePixel(position P, bitmap_image* image)
{
	double X = P.x1;
	double Y = P.x2;

	if ((X < 0) || (X > image->width())) {
		std::cout << "analyse hors limites de l'image";
		return 0;
	}

	if ((Y < 0) || (Y > image->height())) {
		std::cout << "analyse hors limites de l'image";
		return 0;
	}

	int ix = floor(X); int iy = floor(Y);
	double dx = X - (double)ix; double dy = Y - (double)iy;

	double v00 = (double)image->get_pixel_BW(ix, iy);
	double v01 = (double)image->get_pixel_BW(ix, iy + 1);
	double v10 = (double)image->get_pixel_BW(ix + 1, iy);
	double v11 = (double)image->get_pixel_BW(ix + 1, iy + 1);
	// interpolation linéaire
	double dfx = v10 - v00;
	double dfy = v01 - v00;
	double dfxy = v00 + v11 - v01 - v10;
	double v = v00 + dfx * dx + dfy * dy + dfxy * dx * dy;
	return v;
}

position positionPrime(position x, deplacement s, decalage d) {
	position xp;
	xp.x1 = x.x1 + s.u1 + s.du1dx1 * d.dx1 + s.du1dx2 * d.dx2;
	xp.x2 = x.x2 + s.u2 + s.du2dx1 * d.dx1 + s.du2dx2 * d.dx2;

	return xp;
}

bool maxXY(int& maxX, int& maxY, double* C, double& bestCorrelation, int npasX, int npasY) {
	bool f = false;
	for (int x = 0; x < npasX; x++) {
		for (int y = 0; y < npasY; y++) {
			if (C[y+x*npasY] > bestCorrelation) {
				bestCorrelation = C[y + x * npasY];
				maxX = x; maxY = y;
				f = true;
			}
		}
	}
	return f;
}

deplacement strainCalcul(const char* fileOrg, const char* fileDeformed, position P, int largeur, deplacement deplMin,
	deplacement deplMax, double errorPixMax, int iterMax) {

	std::string file_name1(fileOrg);
	bitmap_image image1(file_name1);
	image1.convert_to_grayscale();

	std::string file_name2(fileDeformed);
	bitmap_image image2(file_name2);
	image2.convert_to_grayscale();
	int iter = 0;
	double errorPix = 10.0;
	//
	deplacement s;
	deplacement sn;
	deplacement sn1;
	deplacement step;

	
	sn.u1 = (deplMin.u1+deplMax.u1)/2.0;
	sn.u2 = (deplMin.u2 + deplMax.u2) / 2.0;
	sn.du1dx1 = (deplMin.du1dx1 + deplMax.du1dx1) / 2.0;
	sn.du2dx2 = (deplMin.du2dx2+ deplMax.du2dx2) / 2.0;
	sn.du1dx2 = (deplMin.du1dx2 + deplMax.du1dx2) / 2.0;
	sn.du2dx1 = (deplMin.du2dx1 + deplMax.du2dx1) / 2.0;
	sn1 = sn;

	//
	int npasX = (deplMax.u1 - deplMin.u1);
	int npasY = (deplMax.u2 - deplMin.u2);
	step.u1 = 1 ;
	step.u2 = 1 ;
	step.du1dx1 = (deplMax.du1dx1 - deplMin.du1dx1) / npasX;
	step.du2dx2 = (deplMax.du2dx2 - deplMin.du2dx2) / npasY;
	step.du1dx2 = (deplMax.du1dx2 - deplMin.du1dx2) / npasX;
	step.du2dx1 = (deplMax.du2dx1 - deplMin.du2dx1) / npasY;
	int maxX, maxY;
	double bestCorrelation = 0.0;
	double *C=(double*) _aligned_malloc(npasX*npasY*sizeof(double),64);
	memset(C, 0, npasX * npasY * sizeof(double));
	int debX=-npasX/2, finX=debX+npasX;
	int debY=-npasY/2, finY=debY+npasY;
	//
	while ((iter < iterMax) ) {
		s = sn;
		// mise a jour u1, u2
		double corel;
		for (int x = debX; x < finX; x++) {
			for (int y = debY; y < finY; y++) {
				s.u1 = sn.u1 + (double)x * step.u1; s.u2 = sn.u2 + (double)y * step.u2;
				int idx = x - debX; int idy = y - debY;
				corel= crossCorrelation(P, s, largeur, &image1, &image2);
				C[idy + idx * npasY] = corel;
			}
		}
		if (maxXY(maxX, maxY, C, bestCorrelation, npasX, npasY)) {
			sn1.u1 = sn.u1 + (double)(maxX + debX) * step.u1; sn1.u2 = sn.u2 + (double)(maxY + debY) * step.u2;
		}

		// mise a jour du1dx1, du2dx2
		s = sn1;
		for (int x = debX; x < finX; x++) {
			for (int y = debY; y < finY; y++) {
				s.du1dx1 = sn.du1dx1 + (double)x * step.du1dx1; s.du2dx2 = sn1.du2dx2 + (double)y * step.du2dx2;
				int idx = x - debX; int idy = y - debY;
				C[idy + idx * npasY] = crossCorrelation(P, s, largeur, &image1, &image2);
			}
		}
		if (maxXY(maxX, maxY, C, bestCorrelation, npasX, npasY)) {
			sn1.du1dx1 = sn.du1dx1 + (double)(maxX +debX) * step.du1dx1; sn1.du2dx2 = sn.du2dx2 + (double)(maxY +debY) * step.du2dx2;
		}
		// mise a jour du1dx2, du2dx1
		//s.du1dx1= sn.du1dx1; s.du2dx2 = sn.du2dx2;
		s = sn1;
		for (int x = debX; x < finX; x++) {
			for (int y = debY; y < finY; y++) {
				s.du1dx2 = sn.du1dx2 + (double)x * step.du1dx2; s.du2dx1 = sn1.du2dx1 + (double)y * step.du2dx1;
				int idx = x - debX; int idy = y - debY;
				C[idy + idx * npasY] = crossCorrelation(P, s, largeur, &image1, &image2);
			}
		}
		if (maxXY(maxX, maxY, C, bestCorrelation, npasX, npasY)) {
			sn1.du1dx2 = sn.du1dx2 + (double)(maxX +debX) * step.du1dx2; sn1.du2dx1 = sn.du2dx1 + (double)(maxY +debY) * step.du2dx1;
		}
		//Calcul d'erreur
		double errorx = sn1.u1 - sn.u1, errory = sn1.u2 - sn.u2;
		double errordx1 = sn1.du1dx1 - sn.du1dx1, errordy1 = sn1.du2dx2 - sn.du2dx2;
		double errordx2 = sn1.du1dx2 - sn.du1dx2, errordy2 = sn1.du2dx1 - sn.du2dx1;
		errorPix = sqrt(errorx * errorx + errory * errory + errordx1 * errordx1 +
			errordy1 * errordy1 + errordx2 * errordx2 + errordy2 * errordy2);
		// maj limites 
		//deplMin.u1 = sn1.u1 - 3.5 * step.u1;
		//deplMin.u2 = sn1.u2 - 3.5 * step.u2;
		//deplMin.du1dx1 = sn1.du1dx1 - 3.5 * step.du1dx1;
		//deplMin.du2dx2 = sn1.du2dx2 - 3.5 * step.du2dx2;
		//deplMin.du1dx2 = sn1.du1dx2 - 3.5 * step.du1dx2;
		//deplMin.du2dx1 = sn1.du2dx1 - 3.5 * step.du2dx1;
		// maj step
		step.u1 /= 2.0;
		step.u2 /= 2.0;
		step.du1dx1 /= 2.0;
		step.du2dx2 /= 2.0;
		step.du1dx2 /= 2.0;
		step.du2dx1 /= 2.0;

		//fin de boucle
		if (errorPix < errorPixMax)
			iter++;
		else
			iter = 0;
		sn = sn1;

	}
	std::cout << "Correlation - champ de deplacement trouve :\n";
	std::cout << "Translation	    u1= " << sn1.u1 << ", " << "u2 = " << sn1.u2 << "\n";
	std::cout << "Expansion    du1/dx1= " << sn1.du1dx1 << ", " << "du2/dx2 = " << sn1.du2dx2 << "\n";
	std::cout << "Cisaillement du1/dx2= " << sn1.du1dx2 << ", " << "du2/dx1 = " << sn1.du2dx1 << "\n";
	_aligned_free(C);
	return sn1;
}

