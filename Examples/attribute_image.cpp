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
    return (int32_t)std::max(std::min(a, (int64_t)std::numeric_limits<int32_t>::max()), (int64_t)std::numeric_limits<int32_t>::min());
}

template <class T>
T getAttribute(Node *n, AttributeID attribute_id) {
  switch (attribute_id) {
    case AREA:
      return n->area;
    case MSER:
      return n->mser;
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

template <class Tval, class Tsel>
Image<Tval> attributeImage(Image<U8> &im, AttributeID value_attribute, AttributeID selection_attribute, unsigned int delta, AttributeSelectionRule selection_rule=NODE)
{
    // Construction du component-tree par la méthode de Salembier
    FlatSE connexity;
    connexity.make2DN8();
    ComponentTree<U8> tree(im, connexity, delta);

    // Valeurs d'attribut pour chaque pixel
    Image<Tval> res = im;

    std::vector<Node *> nodes = tree.indexedNodes();
    for (TSize i = 0; i < res.getSizeX(); i++)
      for (TSize j = 0; j < res.getSizeY(); j++)
        for (TSize k = 0; k < res.getSizeZ(); k++) {
          Node *n = tree.indexedCoordToNode(i, j, k, nodes);
          // noeud selectionné
          Node *n_s = n;
          if(selection_rule != NODE)
          {
              Tsel attr = getAttribute<Tsel>(n, selection_attribute);
              // maximum / minimum, dans la branche parent
              Tsel attr_father;
              // parcours de l'arbre
              while(n->father != tree.m_root) {
                  n = n->father;
                  attr_father = getAttribute<Tsel>(n, selection_attribute);

                  if(attr == attr_father) {}
                  // maximise attribute (be carefule to infinity)
                  else if(selection_rule == MAX && attr_father > attr &&
                          attr_father < std::numeric_limits<Tsel>::max())
                  {
                      n_s = n;
                      attr = attr_father;
                  }
                  // minimize attribute
                  else if(selection_rule == MIN && attr_father < attr &&
                          attr_father > 0)
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

          res(i, j, k) = getAttribute<Tval>(n_s, value_attribute);
        }
    return res;
}

template <class T>
int normalizeAndSave(Image<U8> &ori, Image<T> &res_, std::string name, int limit=std::numeric_limits<int>::max())
{
    T min_attr, max_attr;
    min_attr = res_.getMin();
    max_attr = res_.getMax();
    std::cout << "[INFO] attribute [" << min_attr << ", " << max_attr << "]" << std::endl;

    // limit [?, ?] -> [0, limit]
    for (TSize i = 0; i < res_.getSizeX(); i++)
      for (TSize j = 0; j < res_.getSizeY(); j++)
        for (TSize k = 0; k < res_.getSizeZ(); k++) {
            res_(i, j, k) = std::min((int)res_(i, j, k), limit);
        }

    min_attr = res_.getMin();
    max_attr = res_.getMax();
    std::cout << "[INFO] limit     [" << min_attr << ", " << max_attr << "]" << std::endl;

    Image<U8> res(res_.getSize());

    // normalization [?, ?] -> [0, 255]
    for (TSize i = 0; i < res_.getSizeX(); i++)
      for (TSize j = 0; j < res_.getSizeY(); j++)
        for (TSize k = 0; k < res_.getSizeZ(); k++) {
          res(i, j, k) =
              (U8)(255.0 * (((long double)(res_(i, j, k) - min_attr)) / (long double)(max_attr - min_attr)));
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
    for (TSize i = 0; i < res.getSizeX(); i++)
      for (TSize j = 0; j < res.getSizeY(); j++)
        for (TSize k = 0; k < res.getSizeZ(); k++) {
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
  int limit = 4411800; // 256 4096 16384 65536 262144

  /*
  FlatSE connexity;
  connexity.make2DN8();
  ComponentTree<U8> tree(im, connexity, delta);

  Image<int64_t> res = tree.constructImageAttribute<int64_t, long double>
          (ComponentTree<U8>::AREA, ComponentTree<U8>::MSER, ComponentTree<U8>::MAX);
  normalizeAndSave(ori, res, "attr", limit);
  return 0;
  */

  AttributeSelectionRule rule = MIN;
  // compute image
  Image<int> res_ = attributeImage<int64_t, long double>(im, AREA, MSER, delta, rule);
  // save result as U8 img
  normalizeAndSave(ori, res_, "attr", limit);

  // Dual
  bool dual = true;
  if(dual)
  {
      Image<U8> im_dual = im;
      for (TSize i = 0; i < im.getSizeX(); i++)
        for (TSize j = 0; j < im.getSizeY(); j++)
          for (TSize k = 0; k < im.getSizeZ(); k++) {
            im_dual(i, j, k) = 255 - im_dual(i, j, k);
          }
      res_ = attributeImage<int64_t, long double>(im_dual, AREA, MSER, delta, rule);
      normalizeAndSave(ori, res_, "attr_dual", limit);
  }

  return 0;
}

// non utilisé, pour chercher la valeur maximum dans les fils
/*
double rec_max(Node *n) {
  double attr = n->mser;

  Node::ContainerChilds::iterator jt;
  for (jt = n->childs.begin(); jt != n->childs.end(); ++jt) {
    double tmp = rec_max(*jt);
    if (tmp > attr) attr = tmp;
  }

  return attr;
}
*/
