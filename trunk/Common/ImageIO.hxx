#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>

namespace LibTIM {

inline std::string  GImageIO_NextLine(std::ifstream &file)
{
 std::string str;
 file >> str;
 char buf[256];
 char ch;
 while(str[0]=='#'){file.getline(buf,256,'\n'); file>>str;}

 //For the separator after the ASCII string ('\n' or ' ')
 file.get(ch);
 return str;
}

inline void GImageIO_ReadPPMHeader(std::ifstream &file, std::string &format,
unsigned int &width, unsigned int &height, unsigned int &colormax )
{
	  std::string str;

	  str=GImageIO_NextLine(file);

      format=str;

      str=GImageIO_NextLine(file);
      std::stringstream str_stream(str);
      str_stream>> width;

      str=GImageIO_NextLine(file);
      str_stream.clear();
      str_stream.str(str);
      str_stream>> height;


      str=GImageIO_NextLine(file);
      str_stream.clear();
      str_stream.str(str);
      str_stream>> colormax;


     // std::cout << format << "\n" << width << " " << height << "\n" << colormax << "\n";
}

//TODO: load should return a value!!!!

template <>
inline int Image<U8>::load(const char*filename, Image <U8> &im)
{
	std::ifstream file(filename,std::ios_base::in  | std::ios_base::binary);
	if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}
	std::string format;
	unsigned int width,height,colormax;

    GImageIO_ReadPPMHeader(file,format,width,height,colormax);

    if(format!="P5" || colormax >=256)
    	{
    	std::cerr<< "Error: either type mismatch image type or image is in ASCII .ppm format\n";
    	exit(1);
    	}
    else {
    	if(im.data!=0) delete[] im.data;

    	im.size[0] = width;
		im.size[1] = height;
		im.size[2] = 1;
		im.dataSize=width*height*1;

		for (int i = 0; i < 3; i++)
			{
			im.spacing[i] = 1.0;
			}
		im.data = new U8 [im.dataSize];
    	file.read(reinterpret_cast<char *> (im.data),im.dataSize);
 		}
 	file.close();
 	return 1;
}

template <>
inline int Image<U16>::load(const char*filename, Image <U16> &im)
{
	std::ifstream file(filename,std::ios_base::in  | std::ios_base::binary);
	if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}
	std::string format;
	unsigned int width,height,colormax;

    GImageIO_ReadPPMHeader(file,format,width,height,colormax);

    if(format!="P5")
    	{
    	std::cerr<< "Error: either type mismatch image type or image is in ASCII .ppm format\n";
    	exit(1);
    	}
    else {
    	if(im.data!=(U16*)(0)) delete[] im.data;

    	im.size[0] = width;
		im.size[1] = height;
		im.size[2] = 1;
		im.dataSize=width*height*1;
		for (int i = 0; i < 3; i++)
			{
			im.spacing[i] = 1.0;
			}
		im.data = new U16 [im.dataSize];
    	file.read(reinterpret_cast<char *> (im.data),im.dataSize);
 		}
 	file.close();
 	return 1;
}

template <>
inline int Image<RGB>::load(const char*filename, Image <RGB> &im)
{
	std::ifstream file(filename,std::ios_base::in  | std::ios_base::binary);
	if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}
	std::string format;
	unsigned int width,height,colormax;

    GImageIO_ReadPPMHeader(file,format,width,height,colormax);

    if(format!="P6" || colormax>=256)
    	{
    	std::cerr<< "Error: either type mismatch image type or image is in ASCII .ppm format\n";
    	exit(1);
    	}
    else {
    	if(im.data!=(RGB*)(0)) delete[] im.data;

    	im.size[0] = width;
		im.size[1] = height;
		im.size[2] = 1;
		im.dataSize=width*height;
		for (int i = 0; i < 3; i++)
			{
			im.spacing[i] = 1.0;
			}
		im.data = new RGB [im.dataSize];
    	file.read(reinterpret_cast<char *> (im.data),im.dataSize*3);
 		}
 	file.close();
 	return 1;
}



///Pgm writer
template <>
inline int Image <U8>::save( const char *filename) {
      std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
      if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}

      std::string str;
      std::string format;

	  int width=getSizeX();
	  int height=getSizeY();

      int buf_size = width*height;

      int maxVal=(int)(this->getMax());

	  file << "P5\n#CREATOR: GImage \n" << width << " " << height << "\n" << "255" ;
	  file << "\n";

	  file.write(reinterpret_cast<char *> (this->data),buf_size);

	  file << "\n";

	  file.close();
}

///Pgm writer
template <>
inline int Image <U16>::save( const char *filename) {
      std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
      if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}
      std::string str;
      std::string format;

	  int width=getSizeX();
	  int height=getSizeY();

      int buf_size = width*height*sizeof(U16);

      int maxVal=(int)(this->getMax());

	 /* file << "P5\n#CREATOR: GImage \n" << width << " " << height << "\n" << "65535\n" ;
	 */
	  file << "P5\n#CREATOR: GImage \n" << width << " " << height << "\n" << maxVal << "\n" ;

	  file.write(reinterpret_cast<char *> (this->data),buf_size);

	  file << "\n";

	  file.close();
}

template <>
inline int Image <RGB>::save( const char *filename) {
      std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
      if(!file)
      	{
      	std::cerr << "Image file I/O error\n";
      	return 0;
      	}

      std::string str;
      std::string format;

      int width=getSizeX();
	  int height=getSizeY();

      int buf_size = width*height*3;

      file << "P6\n#CREATOR: GImage \n" << width << " " << height << "\n" << "255\n" ;

      U8 *buf=new U8[width*height*3];

      for(int i=0; i<width*height; i++)
      	{
      	buf[i*3]=(*this)(i)[0];
      	buf[i*3+1]=(*this)(i)[1];
      	buf[i*3+2]=(*this)(i)[2];
      	}

	  file.write(reinterpret_cast<char *> (buf),buf_size);

	  file << "\n";

	  file.close();

	  delete[] buf;

	  return 1;
}

/*
template <>
		inline int Image <RGB>::save( const char *filename) {
	std::ofstream file(filename,std::ios_base::trunc  | std::ios_base::binary);
	if(!file)
	{
		std::cerr << "Image file I/O error\n";
		return 0;
	}

	std::string str;
	std::string format;

	int width=getSizeX();
	int height=getSizeY();

	int buf_size = width*height*3;

	file << "P6\n#CREATOR: GImage \n" << width << " " << height << "\n" << "255\n" ;

	U8 *buf=new U8[width*height*3];

	for(int i=0; i<width*height; i++)
	{
		buf[i*3]=(*this)(i)[0];
		buf[i*3+1]=(*this)(i)[1];
		buf[i*3+2]=(*this)(i)[2];
	}

	file.write(reinterpret_cast<char *> (buf),buf_size);

	file << "\n";

	file.close();

	delete[] buf;

	return 1;
		}*/

}
