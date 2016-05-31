#include <iostream>
#include <ctime>

#include "Common/Image.h"
#include "Common/FlatSE.h"
#include "Algorithms/ComponentTree.h"
#include "Algorithms/ConnectedComponents.h"
#include "Algorithms/Thresholding.h"

using namespace std;
using namespace LibTIM;

int main(int argc, char *argv[])
{
	if(argc<=2)
		{
		cout << "Usage: " << argv[0] << " <imgSrc> <areaMin> [<areaMax>]\n";
		exit(-1);
		}
	
	
  	Image <U8> im;
  	if(Image<U8>::load(argv[1],im))
		std::cout <<"OK, image is loaded\n";
	FlatSE connexity;
	connexity.make2DN8();
	
	FlatSE se;
	se.make2DN9();
	
	//  const U8 data[]={0, 0, 0, 0, 0, 0, 0, 0, 0,
// 					0, 1, 1, 1, 0, 0, 0, 0, 0, 
// 					0, 1, 2, 1, 0, 0, 3, 3, 0,
// 					0, 1, 1, 1, 1, 1, 1, 1, 1,
// 					0, 0, 1, 0, 0, 0, 0, 0, 0,
// 					0, 0, 4, 0, 4, 4, 4, 4, 4,
// 					0, 0, 4, 4, 4, 3, 5, 2, 1,
// 					0, 0, 2, 2, 4, 2, 2, 2, 0,
// 					0, 2, 2, 2, 0, 0, 0, 0, 0,
// 					0, 0, 3, 0, 0, 0, 0, 2, 0};
// 	TSize size[]={9,10,1};
//   	TSpacing spacing[]={1.0,1.0,1.0};
// 	
//  	 const U8 data[]={0,4,4,0,0,
//  					 0,4,3,4,1,
//   					 0,4,4,4,1,
//  					 0,0,0,0,0,
// 					 0,1,1,1,0};
//  	TSize size[]={5,5,1};
	
 	//Image <U8> imBasic(size,spacing,data);
 	//imBasic.save("imBasic2.pgm");
	
	// Copie de l'image originale
	Image <U8> ori=im;
	
	// Construction du component-tree par la méthode de Salembier
	ComponentTree<U8> tree(im);
	
	
	// Ecriture de l'arbre au format .dot: attention, dans le cas d'images "naturelles" le fichier peut-être gros ...
   	if(!tree.writeDot("testBefore.dot"))
   		std::cerr << "Error writing dot file\n";
//  	
 	
 	// Elagage de l'arbre en utilisant un paramï¿½tre relatif ï¿½ la taille de la composante
	if(argc==3)	
		tree.areaFiltering(atoi(argv[2]));
	else
  		tree.areaFiltering(atoi(argv[2]),atoi(argv[3]));
	//tree.contrastFilteringReverse(atoi(argv[2]));
	//tree.boundingBoxFiltering(atoi(argv[2]),atoi(argv[3]));
	//tree.complexityFiltering(atoi(argv[2]), atoi(argv[3]));
	//ComponentTree<U8>::SignatureType signature=tree.branchSignature(atoi(argv[2]),atoi(argv[3]));
	//tree.writeSignature(signature,"signature.dat");
	//tree.compacityFiltering(atoi(argv[2]),atoi(argv[3]));
 	
    // Reconstruction d'une image à partir de l'arbre élagué
 	Image <U8> res=tree.constructImage(ComponentTree<U8>::DIRECT);
 	
    // Création d'une image contenant les rï¿½sidus de la transformation
	Image<U8> residue=ori-res;
 	residue.save("residue.pgm");
	
    // Ecriture de l'image originale et de l'image filtrée
 	ori.save("treeIn.pgm");
 	res.save("treeOut.pgm");
	
    // Ecriture de l'arbre élagué au format .dot
   	if(!tree.writeDot("testAfter.dot"))
		std::cerr << "Error writing dot file\n";

}
