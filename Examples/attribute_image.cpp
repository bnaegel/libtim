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
  Image<double> res_ = im;
  double min_attr = std::numeric_limits<double>::max();
  double max_attr = std::numeric_limits<double>::min();

  std::vector<Node *> nodes = tree.indexedNodes();
  for (std::size_t i = 0; i < res_.getSizeX(); i++)
    for (std::size_t j = 0; j < res_.getSizeY(); j++)
      for (std::size_t k = 0; k < res_.getSizeZ(); k++) {
        Node *n = tree.indexedCoordToNode(i, j, k, nodes);
        // remplacer n->attribut par l'attribut souhaité
        // double attr = (double)(n->area);
        // double attr = (double)(n->contrast);
        // double attr = (double)(n->compacity);
        double attr = (double)(n->mser);

        // si l'on cherche la maximum dans la branche parent
        /*
        // sauvegarde valeur initiale
        double attr_init = attr;
        // parcours de l'arbre
        while(n->father != tree.m_root) {
            n = n->father;
            attr = std::max(attr, (double)(n->mser));
        }
        attr = std::max(attr, (double)(tree.m_root->mser));
        */

        min_attr = std::min(min_attr, attr);
        max_attr = std::max(max_attr, attr);

        res_(i, j, k) = attr;
      }

  std::cout << "[INFO] attribute " << min_attr << " " << max_attr << std::endl;

  // normalization [?, ?] -> [0, 255]
  Image<U8> res = res_;

  for (std::size_t i = 0; i < res_.getSizeX(); i++)
    for (std::size_t j = 0; j < res_.getSizeY(); j++)
      for (std::size_t k = 0; k < res_.getSizeZ(); k++) {
        res(i, j, k) =
            255 * (((res_(i, j, k) - min_attr)) / (max_attr - min_attr));
      }

  // Création d'une image contenant les residue de la transformation
  Image<U8> residue = ori - res;
  residue.save("attr_residue.pgm");

  // Ecriture de l'image originale et de l'image filtrée
  ori.save("attr_in.pgm");
  res.save("attr_out.pgm");
}

// non utilisé, pour chercher la valeur maximum dans les fils
double rec_max(Node *n) {
  double attr = n->mser;

  Node::ContainerChilds::iterator jt;
  for (jt = n->childs.begin(); jt != n->childs.end(); ++jt) {
    double tmp = rec_max(*jt);
    if (tmp > attr) attr = tmp;
  }

  return attr;
}
