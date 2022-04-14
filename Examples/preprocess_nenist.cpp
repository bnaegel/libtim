#include <iostream>
#include <ctime>

#include "Common/Image.h"
#include "Common/FlatSE.h"
#include "Algorithms/ComponentTree.h"

using namespace LibTIM;

std::string getfname(char* argv1, std::string fname)
{
    return (std::string(argv1)+ std::string("_") + std::string(fname) + std::string(".pgm"));
}
std::string getfname(char* argv1, const char* fname)
{
    return getfname(argv1, std::string(fname));
}

template<class T>
Image<U8> normalize(Image<T> image, T limit_in = 0)
{
    Image<U8> res(image.getSize());
    T limit = std::max(image.getMax(), (T)1);

    if(limit_in != 0)
    {
        limit = limit_in;
    }

    for (TSize x = 0; x < image.getSizeX(); x++) {
        for (TSize y = 0; y < image.getSizeY(); y++) {
            for (TSize z = 0; z < image.getSizeZ(); z++) {
                long double normalized_val =
                        ((long double)std::min(image(x, y, z), limit) / (long double)limit);
                int c = ((long double)normalized_val*255.0);
                res(x, y, z) = c;
            }
        }
    }
    return res;
}

int main(int argc, char *argv[])
{
    bool debug = true;
    if(argc <= 1)
    {
        std::cout << "usage: " << argv[0] << " <imgSrc>" << std::endl;
        exit(-1);
    }

    // Lecture de l'image
    Image<U8> im;
    if (Image<U8>::load(argv[1], im))
        std::cout << "[INFO] " << argv[1] << " image is loaded" << std::endl;
    else
        std::cout << "[ERRO] " << argv[1] << " impossible to load the input image" << std::endl;

    // Copie de l'image originale
    Image<U8> ori = im;
    // Dual de l'image
    Image<U8> im_dual = im;
    for (TSize x = 0; x < im.getSizeX(); x++) {
        for (TSize y = 0; y < im.getSizeY(); y++) {
            for (TSize z = 0; z < im.getSizeZ(); z++) {
                im_dual(x, y, z) = 255 - im_dual(x, y, z);
            }
        }
    }
    // Image résultat
    Image<U8> res;
    Image<int> res_int;
    Image<long double> res_longdouble;
    // Arbre des composantes
    ComponentTree<U8> *tree;
    FlatSE connexity;
    connexity.make2DN8();

    // attributes images
    ComputedAttributes ca = ComputedAttributes::AREA;
    ca = (ComputedAttributes)(ca | ComputedAttributes::AREA_DERIVATIVES);
    ca = (ComputedAttributes)(ca | ComputedAttributes::CONTRAST);
    ca = (ComputedAttributes)(ca | ComputedAttributes::VOLUME);
    ca = (ComputedAttributes)(ca | ComputedAttributes::BORDER_GRADIENT);
    ca = (ComputedAttributes)(ca | ComputedAttributes::COMP_LEXITY_ACITY);
    // ca = (ComputedAttributes)(ca | ComputedAttributes::OTSU);

    tree = new ComponentTree<U8>(im, connexity, ca, 5);
    /*
    int64_t limit_criterion_min = 0;
    int64_t limit_criterion_max = 1048576;
    ComponentTree<U8>::Attribute attribute;
    ComponentTree<U8>::Attribute criterion;
    ComponentTree<U8>::Attribute limit_criterion = ComponentTree<U8>::AREA;

    // CONTRAST, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576
    attribute = ComponentTree<U8>::CONTRAST;
    criterion = ComponentTree<U8>::AREA_D_AREAN_H_D;
    res_int =
            tree->constructImageAttribute<int, long double, int64_t>
            (attribute, criterion, ComponentTree<U8>::MAX,
             limit_criterion, limit_criterion_min, limit_criterion_max);
    res = normalize<int>(res_int);
    if(debug)
    {
        std::cout << "CONTRAST, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576" << std::endl;
        std::cout << (int)res_int.getMin() << " " << (int)res_int.getMax() << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "CONTRAST-MAX_AREA_D_AREAN_H_D-LIMIT_AREA").c_str());

    // VOLUME, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576
    attribute = ComponentTree<U8>::VOLUME;
    criterion = ComponentTree<U8>::AREA_D_AREAN_H_D;
    res_int =
            tree->constructImageAttribute<int, long double, int64_t>
            (attribute, criterion, ComponentTree<U8>::MAX,
             limit_criterion, limit_criterion_min, limit_criterion_max);
    res = normalize<int>(res_int);
    if(debug)
    {
        std::cout << "VOLUME, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576" << std::endl;
        std::cout << (int)res_int.getMin() << " " << (int)res_int.getMax() << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "VOLUME-MAX_AREA_D_AREAN_H_D-LIMIT_AREA").c_str());

    // COMPLEXITY, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576
    attribute = ComponentTree<U8>::COMPLEXITY;
    criterion = ComponentTree<U8>::AREA_D_AREAN_H_D;
    res_int =
            tree->constructImageAttribute<int, long double, int64_t>
            (attribute, criterion, ComponentTree<U8>::MAX,
             limit_criterion, limit_criterion_min, limit_criterion_max);
    res = normalize<int>(res_int);
    if(debug)
    {
        std::cout << "COMPLEXITY, MAX AREA_D_AREAN_H_D, LIMIT AREA 1048576" << std::endl;
        std::cout << (int)res_int.getMin() << " " << (int)res_int.getMax() << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "COMPLEXITY-MAX_AREA_D_AREAN_H_D-LIMIT_AREA").c_str());

    // MGB, MAX MGB, LIMIT AREA 1048576
    attribute = ComponentTree<U8>::MGB;
    criterion = ComponentTree<U8>::MGB;
    res_longdouble =
            tree->constructImageAttribute<long double, long double, int64_t>
            (attribute, criterion, ComponentTree<U8>::MAX,
             limit_criterion, limit_criterion_min, limit_criterion_max);
    res = normalize<long double>(res_longdouble);
    if(debug)
    {
        std::cout << "MGB, MAX MGB, LIMIT AREA 1048576" << std::endl;
        std::cout << (int)res_int.getMin() << " " << (int)res_int.getMax() << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
        res.save(getfname(argv[1], "MGB-MAX_MGB-LIMIT_AREA").c_str());
    }

    // COMPACITY (MAX = 50), MAX AREA_D_AREAN_H_D
    attribute = ComponentTree<U8>::COMPACITY;
    criterion = ComponentTree<U8>::AREA_D_AREAN_H_D;
    res_int =
            tree->constructImageAttribute<int, long double, int64_t>
            (attribute, criterion, ComponentTree<U8>::MAX);
    res = normalize<int>(res_int, 50);
    if(debug)
    {
        std::cout << "COMPACITY (MAX = 50), MAX AREA_D_AREAN_H_D" << std::endl;
        std::cout << (int)res_int.getMin() << " " << (int)res_int.getMax() << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "COMPACITY_MAX_50-MAX_AREA_D_AREAN_H_D").c_str());
    */

    // attributes signature images
    std::vector<Node *> nodes = tree->indexedNodes();

    std::vector<Image<U8>> res_sign;
    std::vector<bool> set_sign;
    for(int i = 0; i < 26 ; ++i)
    {
        res_sign.push_back(im.getSize());
        set_sign.push_back(true);
        res_sign[i].fill(0);
    }

    // MGB (MAX = 150 (150x2 = 300))
    for (TSize x = 0; x < im.getSizeX(); x++) {
        for (TSize y = 0; y < im.getSizeY(); y++) {
            for (TSize z = 0; z < im.getSizeZ(); z++) {
                Node* node = tree->indexedCoordToNode(x, y, z, nodes);

                while(node != tree->m_root)
                {
                    for(int i = 0; i < 26 ; ++i)
                    {
                        if(set_sign[i] && node->h <= i*10)
                        {
                            std::cout << res_sign[i](x, y, z) << std::endl;
                            res_sign[i](x, y, z) = 2*node->mean_gradient_border;
                            set_sign[i] = false;
                        }
                    }
                    node = node->father;
                }
            }
        }
    }

    for(int i = 0; i < 26 ; ++i)
    {
        if(debug)
        {
            std::cout << "SIGN_MGB_" << i*10 << std::endl;
            std::cout << (int)res_sign[i].getMin() << " " << (int)res_sign[i].getMax() << std::endl;
        }
        normalize<U8>(res_sign[i], 300).save(getfname(argv[1], std::string("SIGN_MGB_") + std::to_string(i*10)).c_str());
    }

    // OTSU (MAX = 20)
    for(int i = 0; i < 26 ; ++i)
    {
        res_sign[i].fill(0);
        set_sign[i] = true;
    }

    // area filtering
    int64_t area_min, area_max;

    delete tree;
    tree = new ComponentTree<U8>(im, connexity, 1);
    area_min = 2048;
    area_max = 1048576;
    tree->areaFiltering(area_min, area_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    if(debug)
    {
        std::cout << "AREA 2048 1048576" << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "AREA_2048_1048576").c_str());

    delete tree;
    tree = new ComponentTree<U8>(im, connexity, 1);
    area_min = 4096;
    area_max = std::numeric_limits<int64_t>::max();
    tree->areaFiltering(area_min, area_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    if(debug)
    {
        std::cout << "AREA 4096" << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "AREA_4096").c_str());

    // area filtering dual
    delete tree;
    tree = new ComponentTree<U8>(im_dual, connexity, 1);
    area_min = 256;
    area_max = 262144;
    tree->areaFiltering(area_min, area_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    if(debug)
    {
        std::cout << "AREA DUAL 256 262144" << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "AREA_DUAL_256_262144").c_str());

    // contrast filtering
    int contrast_min, contrast_max;

    delete tree;
    tree = new ComponentTree<U8>(im, connexity, 1);
    contrast_min = 10;
    contrast_max = 100;
    tree->contrastFiltering(contrast_min, contrast_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    if(debug)
    {
        std::cout << "CONTRAST 10 100" << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "CONTRAST_10_150").c_str());

    // contrast filtering dual
    delete tree;
    tree = new ComponentTree<U8>(im_dual, connexity, 1);
    contrast_min = 10;
    contrast_max = 100;
    tree->contrastFiltering(contrast_min, contrast_max);
    res = tree->constructImage(ComponentTree<U8>::DIRECT);
    if(debug)
    {
        std::cout << "CONTRAST DUAL 10 100" << std::endl;
        std::cout << (int)res.getMin() << " " << (int)res.getMax() << std::endl;
    }
    res.save(getfname(argv[1], "CONTRAST_DUAL_10_150").c_str());

    std::cout << "[INFO] " << argv[1] << " end without error" << std::endl;

    return 0;
}