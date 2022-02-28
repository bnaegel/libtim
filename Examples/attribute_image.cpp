#include <ctime>
#include <iostream>

#include "Algorithms/ComponentTree.h"
#include "Algorithms/ConnectedComponents.h"
#include "Algorithms/Thresholding.h"
#include "Common/FlatSE.h"
#include "Common/Image.h"

using namespace LibTIM;

enum AttributeID {
  AREA,
  MSER,
  CONTRAST,
  VOLUME,
  CONTOUR_LENGTH,
  COMPLEXITY,
  COMPACITY
};

enum AttributeSelectionRule {
  NODE,
  MIN,
  MAX
};

int32_t int64Toint32(int64_t a)
{
    return std::max(std::min(a, (int64_t)std::numeric_limits<int32_t>::max()), (int64_t)std::numeric_limits<int32_t>::min());
}

int getAttribute(Node *n, AttributeID attribute_id) {
  switch (attribute_id) {
    case AREA:
      return int64Toint32(n->area);
    case MSER:
      return int64Toint32(n->mser);
    case CONTRAST:
      return n->contrast;
    case VOLUME:
      return n->volume;
    case CONTOUR_LENGTH:
      return n->contourLength;
    case COMPLEXITY:
      return n->complexity;
    case COMPACITY:
      return n->compacity;
  }
  return 0;
}

Image<int> attributeImage(Image<U8> &im, AttributeID value_attribute, AttributeID selection_attribute, unsigned int delta, AttributeSelectionRule selection_rule=NODE)
{
    // Construction du component-tree par la méthode de Salembier
    FlatSE connexity;
    connexity.make2DN8();
    ComponentTree<U8> tree(im, connexity, delta);

    // Valeurs d'attribut pour chaque pixel
    Image<int> res = im;

    std::vector<Node *> nodes = tree.indexedNodes();
    for (std::size_t i = 0; i < res.getSizeX(); i++)
      for (std::size_t j = 0; j < res.getSizeY(); j++)
        for (std::size_t k = 0; k < res.getSizeZ(); k++) {
          int attr;
          Node *n = tree.indexedCoordToNode(i, j, k, nodes);
          // noeud selectionné
          Node *n_s = n;

          if(selection_rule != NODE)
          {
              // remplacer par l'attribut souhaité
              attr = getAttribute(n, selection_attribute);

              // maximum / minimum, dans la branche parent
              int attr_father;
              // parcours de l'arbre
              while(n->father != tree.m_root) {
                  n = n->father;
                  attr_father = getAttribute(n, selection_attribute);

                  if(attr == attr_father) {}
                  // maximise attribute (be carefule to infinity)
                  else if(attr_father > attr && selection_rule == MAX &&
                          attr_father < std::numeric_limits<int32_t>::max()/1000)
                  {
                      n_s = n;
                      attr = attr_father;
                  }
                  // minimize attribute
                  else if(attr_father < attr && selection_rule == MIN)
                  {
                      n_s = n;
                      attr = attr_father;
                  }
              }
              if(attr < 0)
              {
                  std::cout << "ERROR: NEGATIVE SELECTION ATTRIBUTE" << attr << std::endl;
                  exit(1);
              }
          }

          attr = getAttribute(n_s, value_attribute);

          res(i, j, k) = attr;
        }
    return res;
}

int normalizeAndSave(Image<U8> &ori, Image<int> &res_, std::string name, int limit=std::numeric_limits<int>::max())
{
    int min_attr, max_attr;
    min_attr = res_.getMin();
    max_attr = res_.getMax();
    std::cout << "[INFO] attribute [" << min_attr << ", " << max_attr << "]" << std::endl;

    // limit [?, ?] -> [0, limit]
    for (std::size_t i = 0; i < res_.getSizeX(); i++)
      for (std::size_t j = 0; j < res_.getSizeY(); j++)
        for (std::size_t k = 0; k < res_.getSizeZ(); k++) {
            res_(i, j, k) = std::min(res_(i, j, k), limit);
        }

    min_attr = res_.getMin();
    max_attr = res_.getMax();
    std::cout << "[INFO] limit     [" << min_attr << ", " << max_attr << "]" << std::endl;

    Image<U8> res = res_;

    // normalization [?, ?] -> [0, 255]
    for (std::size_t i = 0; i < res_.getSizeX(); i++)
      for (std::size_t j = 0; j < res_.getSizeY(); j++)
        for (std::size_t k = 0; k < res_.getSizeZ(); k++) {
          res(i, j, k) =
              (U8)(255.0 * (((double)(res_(i, j, k) - min_attr)) / (double)(max_attr - min_attr)));
        }

    min_attr = res.getMin();
    max_attr = res.getMax();
    std::cout << "[INFO] norm      [" << min_attr << ", " << max_attr << "]" << std::endl;

    // Image d'attribut
    res.save((name + std::string(".pgm")).c_str());

    // Image residue
    Image<U8> residue = ori - res;
    residue.save((name + std::string("_residue.pgm")).c_str());

    // Image residue inversé
    for (std::size_t i = 0; i < res.getSizeX(); i++)
      for (std::size_t j = 0; j < res.getSizeY(); j++)
        for (std::size_t k = 0; k < res.getSizeZ(); k++) {
          res(i, j, k) = 255 - res(i, j, k);
        }
    Image<U8> residue_inv = ori - res;
    residue_inv.save((name + std::string("_residue_inv.pgm")).c_str());

    return 0;
}


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
  ori.save("attr_in.pgm");


  unsigned int delta = 5;
  int limit = 16384; // 256 4096 16384 65536 262144
  AttributeSelectionRule rule = MIN;
  // compute image
  Image<int> res_ = attributeImage(im, AREA, MSER, delta, rule);
  // save result as U8 img
  normalizeAndSave(ori, res_, "attr", limit);

  // Dual
  bool dual = true;
  if(dual)
  {
      Image<U8> im_dual = im;
      for (std::size_t i = 0; i < im.getSizeX(); i++)
        for (std::size_t j = 0; j < im.getSizeY(); j++)
          for (std::size_t k = 0; k < im.getSizeZ(); k++) {
            im_dual(i, j, k) = 255 - im_dual(i, j, k);
          }
      res_ = attributeImage(im_dual, AREA, MSER, delta, rule);
      normalizeAndSave(ori, res_, "attr_dual", limit);
  }

  return 0;
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
