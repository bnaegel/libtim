#include <ctime>
#include <iostream>

#include "Algorithms/ComponentTree.h"
#include "Algorithms/ConnectedComponents.h"
#include "Algorithms/Thresholding.h"
#include "Common/FlatSE.h"
#include "Common/Image.h"

using namespace LibTIM;

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    std::cout << "usage: " << argv[0] << " <imgSrc>" << std::endl;
    exit(-1);
  }

  // Lecture de l'image
  Image<U8> im;
  if (Image<U8>::load(argv[1], im))
    std::cout << "[INFO] image is loaded" << std::endl;
  else
    std::cout << "[ERRO] impossible to load the input image" << std::endl;

  // Copie de l'image originale
  Image<U8> ori = im;

  // Construction du component-tree par la méthode de Salembier
  FlatSE connexity;
  connexity.make2DN8();
  ComponentTree<U8> tree(im, connexity);

  // Valeurs d'attribut pour chaque pixel
  Image<U8> res = im;
  std::vector<Node *> nodes = tree.indexedNodes();

  std::cout << "[INFO] image size" << res.getSizeX() << " " << res.getSizeY()
            << " " << res.getSizeZ() << std::endl;

  int attr_global_max = 0;
  int attr_max = 0;

  for (std::size_t i = 0; i < res.getSizeX(); i++)
    for (std::size_t j = 0; j < res.getSizeY(); j++)
      for (std::size_t k = 0; k < res.getSizeZ(); k++) {
        res(i, j, k) = tree.indexedCoordToNode(i, j, k, nodes)->compacity;
        if (res(i, j, k) > attr_global_max) attr_global_max = res(i, j, k);
      }
  std::cout << attr_global_max << std::endl;

  // Création d'une image contenant les residue de la transformation
  Image<U8> residue = ori - res;
  residue.save("attr_residue.pgm");

  // Ecriture de l'image originale et de l'image filtrée
  ori.save("attr_in.pgm");
  res.save("attr_out.pgm");
}
