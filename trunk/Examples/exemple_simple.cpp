#include <iostream>

// headers pour les classes de base Image et FlatSE 
#include "Common/Image.h"

// d�claration du namespace std pour utilisation de la STL (permet d'�crire: vector<int> ...; au lieu de: std::vector<int>...)
using namespace std;
// d�claration du namespace LibTIM pour utilisation de la lib de traitement d'images (permet d'�crire: Image<int> ...; au lieu de: LibTIM::Image<int>...)
using namespace LibTIM;

int main(int argc, char *argv[])
{
	if(argc !=3)
		{
		std::cout << "Usage: " << argv[0] << " <image.pgm> <s>\n";
		std::cout << "Seuille l'image <image.pgm> � la valeur <s>\n";
		exit(1);
		}
	// Image est une classe g�n�rique param�tr�e par le type des points contenus dans l'image
	// note: argv[1] d�signe la cha�ne de caract�res correspondant au premier param�tre du programme
	Image <unsigned char> im;
  	if(Image<unsigned char>::load(argv[1],im))
		std::cout <<"Great, image is loaded\n";
	else return 1;
	
	// on r�cup�re le param�tre s (du seuil)
	// note: argv[2] d�signe la cha�ne de caract�res correspondant au second param�tre du programme
	// on le convertit en int gr�ce � la fonction atoi(...)
	int s=atoi(argv[2]);
	
	// copie de l'image im dans l'image imTmp
	// note: ici imTmp n'a pas d'autre utilit� que p�dagogique...
	Image <unsigned char> imTmp=im;
	
	// exemple de parcours d'image (par offset)
	for(int i=0; i< imTmp.getBufSize(); i++)
		{
		// initialise les points de l'image � la valeur 0
		imTmp(i)=0;
		}
	// stocke les dimensions de l'image dans dx et dy
	int dx=imTmp.getSizeX();
	int dy=imTmp.getSizeY();
	
	// exemple de parcours d'image (par coordonn�es)
	for(int y=0; y<dy; y++)
		for(int x=0; x<dx; x++)
			{
			// initialise chaque pixel de coordonn�e (x,y) � la valeur x+y
			imTmp(x,y)=x+y;
			}
			
	// boucle principale pour seuiller l'image en fonction du param�tre s donn� en ligne de commande
	// sauvegarde le r�sultat
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
