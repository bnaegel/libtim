#include <iostream>

// headers pour les classes de base Image et FlatSE 
#include "Common/Image.h"
#include "Common/FlatSE.h"
// header pour les algos principaux de morphologie (�ro, dil, ouv, fer, rec,...)
#include "Algorithms/Morphology.h"
// header pour l'algo du Watershed
#include "Algorithms/Watershed.h"
// header pour l'algo de seeded region growing (SRG)
#include "Algorithms/RegionGrowing.h"
// header pour la gestion des composantes connexes
#include "Algorithms/ConnectedComponents.h"


using namespace std;
using namespace LibTIM;

int main(int argc, char *argv[])
{
	if(argc !=3)
		{
		std::cout << "Usage: " << argv[0] << " <image.pgm> <h>\n";
		exit(1);
		}
	// Image est une classe g�n�rique param�tr�e par le type des points contenus dans l'image
	Image <unsigned char> im;
  	if(Image<U8>::load(argv[1],im))
		std::cout <<"Great, image is loaded\n";
	else return 1;
	
	// FlatSE est la classe stockant un �l�ment structurant
	FlatSE connexity;
	// initialisation de l'�l�ment structurant 'connexity' � un 8-voisinage (l'�l�ment ne contient pas l'origine)
	//   X X X
	//   X . X
	//   X X X
	connexity.make2DN8();
	
	FlatSE se;
	// initialisation � un 9-voisinage (l'�l�ment contient l'origine)
	// X X X
	// X X X
	// X X X 
	se.make2DN9();
	
	////////////////////////////////////////////////////
	// exemple �rosion/dilatation                     //
	////////////////////////////////////////////////////
	
	// �rosion de l'image im par l'�l�ment structurant se
	Image <U8> imEro=erosion(im, se);
	
	// dilatation de l'image im par l'�l�ment structurant se
	Image <U8> imDil=dilation(im, se);

	// �criture des images r�sultat au format .pgm
	imEro.save("imEro.pgm");
	imDil.save("imDil.pgm");
	
	////////////////////////////////////////
	// exemple d'utilisation du watershed //
	////////////////////////////////////////
	
	// calcul du gradient morphologique
	Image <U8> grad=morphologicalGradient(im,se);
	grad.save("gradBeforeFiltering.pgm");
	
	// filtre h-min: suppression des minima du gradient non significatifs (�vite la sur-segmentation du watershed)
	
	// param�tre h d�terminant la profondeur maximale des minima �limin�s
	// possibilit� d'obtenir une pyramide d'images pour chaque h
	int h=atoi(argv[2]);
	
	hMinFilter(grad,se,h);
	grad.save("gradAfterFiltering.pgm");
	
	// calcul des minima r�gionaux du gradient filtr�
	Image <U8> minima=regionalMinima(grad, connexity);
	
	// labelisation des minima
	Image <TLabel> minimaLabel1=labelConnectedComponents(minima, connexity);
	Image <TLabel> minimaLabel2=minimaLabel1;
	
	// Algo 1: watershed, algo de Meyer (le r�sultat est stock� dans l'image Label minimaLabel1)
	watershedMeyer(grad,minimaLabel1,connexity);
	
	// Algo 2: croissance de r�gions (avantage: calcul� directement sur l'image originale)
	seededRegionGrowing(im,minimaLabel2,connexity);
	
	// pour chaque r�gion lab�lis�e, calcule la moyenne de la r�gion dans l'image originale
	// cr�ation d'une image mosa�que
	
	Image <U8> result1=computeMarkerMean(im, minimaLabel1);
	Image <U8> result2=computeMarkerMean(im, minimaLabel2);
	
	result1.save("result1.pgm");
	result2.save("result2.pgm");
}
