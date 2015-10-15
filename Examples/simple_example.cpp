#include <iostream>

// headers pour les classes de base Image et FlatSE 
#include "Common/Image.h"

// déclaration du namespace std pour utilisation de la STL (permet d'écrire: vector<int> ...; au lieu de: std::vector<int>...)
using namespace std;
// déclaration du namespace LibTIM pour utilisation de la lib de traitement d'images (permet d'écrire: Image<int> ...; au lieu de: LibTIM::Image<int>...)
using namespace LibTIM;

int main(int argc, char *argv[])
{
	if(argc !=3)
		{
		std::cout << "Usage: " << argv[0] << " <image.pgm> <s>\n";
		std::cout << "Seuille l'image <image.pgm> à la valeur <s>\n";
		exit(1);
		}
	// Image est une classe générique paramétrée par le type des points contenus dans l'image
	// note: argv[1] désigne la chaîne de caractères correspondant au premier paramètre du programme
	Image <unsigned char> im;
  	if(Image<unsigned char>::load(argv[1],im))
		std::cout <<"Great, image is loaded\n";
	else return 1;
	
	// on récupère le paramètre s (du seuil)
	// note: argv[2] désigne la chaîne de caractères correspondant au second paramètre du programme
	// on le convertit en int grâce à la fonction atoi(...)
	int s=atoi(argv[2]);
	
	// copie de l'image im dans l'image imTmp
	// note: ici imTmp n'a pas d'autre utilité que pédagogique...
	Image <unsigned char> imTmp=im;
	
	// exemple de parcours d'image (par offset)
	for(int i=0; i< imTmp.getBufSize(); i++)
		{
		// initialise les points de l'image à la valeur 0
		imTmp(i)=0;
		}
	// stocke les dimensions de l'image dans dx et dy
	int dx=imTmp.getSizeX();
	int dy=imTmp.getSizeY();
	
	// exemple de parcours d'image (par coordonnées)
	for(int y=0; y<dy; y++)
		for(int x=0; x<dx; x++)
			{
			// initialise chaque pixel de coordonnée (x,y) à la valeur x+y
			imTmp(x,y)=x+y;
			}
			
	// boucle principale pour seuiller l'image en fonction du paramètre s donné en ligne de commande
	// sauvegarde le résultat
	for(int y=0; y<dy; y++)
		for(int x=0; x<dx; x++)
			{
			// seuillage
			if(im(x,y)>=s) im(x,y)=255;
			else im(x,y)=0;
			}
			
	imTmp.save("imTmp.pgm");
	im.save("result.pgm");
}
