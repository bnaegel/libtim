#include <iostream>
#include <string>

#include "Common/Image.h"
#include "Common/FlatSE.h"
#include "Algorithms/ComponentTree.h"

using namespace std;
using namespace LibTIM;


int main(int argc, char *argv[])
{

    if( argc < 3 )
    {
        std::cerr << "Usage: " << argv[0] << " <imgSrc> <areaMin> \n";
        exit(-1);
    }

    string filename=argv[1];
    int areaMin=atoi(argv[2]);

    Image<U16> image;

    Image<U16>::load(filename.c_str(),image);

    std::cout << "SizeX: "  << image.getSizeX() << " SizeY: " << image.getSizeY() << "\n";
    std::cout << "Min value: " << image.getMin() << " Max value: " << image.getMax() << "\n";

    FlatSE connexity;
	connexity.make2DN8();
	
	FlatSE se;
	se.make2DN9();

    ComponentTree<U16> tree(image);

    tree.areaFiltering(areaMin);

    Image <U16> image_filtered=tree.constructImage(ComponentTree<U16>::DIRECT);

    image_filtered.save("image_filtered.pgm");
}
