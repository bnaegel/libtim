
#include <fstream>
namespace LibTIM {


template <class T>
Histogram <T> ::Histogram(Image <T> &im)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	for(int i=0; i< dx*dy*dz; i++)
		data[im(i)]++;
}

///Write histogram into file
template <class T>
void Histogram <T>::write(const char *filename)
{
	std::ofstream outputFile(filename); 	
	if(!outputFile)
 		{
 		std::cerr << "Cannot open " << filename << "\n";
 		exit(1);
 		}
	typename HistoType::iterator it;
	
	 for( it=data.begin(); it!=data.end(); it++)
 		outputFile << (int)it->first << " " << it->second << "\n";
 	outputFile.close();
}

}
