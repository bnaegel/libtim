#include <iostream>
#include <ctime>

#include "Common/Image.h"
#include "Common/FlatSE.h"
#include "Algorithms/ComponentTree.h"

using namespace LibTIM;

std::string getfname(char* argv1, const char* fname)
{
    return (std::string(argv1)+ std::string("_") + std::string(fname) + std::string(".pgm"));
}

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
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
    // Image résultat
    Image <U8> res;
    // Arbre des composantes
    ComponentTree<U8> *tree;
    FlatSE connexity;
    connexity.make2DN8();

    // attributes images
    /*
    ComputedAttributes ca = ComputedAttributes::AREA;
    ca = (ComputedAttributes)(ca | ComputedAttributes::AREA_DERIVATIVES);
    ca = (ComputedAttributes)(ca | ComputedAttributes::CONTRAST);
    ca = (ComputedAttributes)(ca | ComputedAttributes::VOLUME);
    ca = (ComputedAttributes)(ca | ComputedAttributes::BORDER_GRADIENT);
    ca = (ComputedAttributes)(ca | ComputedAttributes::COMP_LEXITY_ACITY);
    ca = (ComputedAttributes)(ca | ComputedAttributes::OTSU);

    tree = new ComponentTree<U8>(im, connexity, ca, 8);
    */


    // attributes signature images
    /*
    tree = ComponentTree<U8>(im, connexity);
    */

    // area filtering
    int64_t area_min, area_max;

    tree = new ComponentTree<U8>(im, connexity, 1);
    area_min = 256;
    area_max = 131072;
    tree->areaFiltering(area_min, area_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    res.save(getfname(argv[1], "area_256_131072").c_str());

    return 0;
}
