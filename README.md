# STRAIN
digital image correlation function
--------------
* based on the algorithm of :
Sutton, Ma, Wj Wolters, Wh Peters, Wf Ranson, et Sr McNeill. 
« Determination of Displacements Using an Improved Digital Correlation Method ». 
Image and Vision Computing 1, nᵒ 3 (août 1983): 133‑39. https://doi.org/10.1016/0262-8856(83)90064-1.
* header file library 
* compare two grey bitmaps image in a particular point to found the displacement function
* main function :
* deplacement strainCalcul(const char* fileOrg, const char* fileDeformed, position P, int largeur, deplacement deplMin,
	deplacement deplMax, double errorPixMax, int iterMax);
* wrote using Visual Studio 2022
* use Bitmap library by Arash Partow - 2002 
* two examples application are furnished in mainStrain.cpp
