/*
 * This file is part of libTIM.
 *
 * Copyright (©) 2005-2013  Benoit Naegel
 * Copyright (©) 2013 Theo de Carpentier
 *
 * libTIM is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libTIM is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <http://www.gnu.org/licenses/gpl>.
 */

#include <gzstream.h>


namespace LibTIM {

struct CImageHeader
{
    CImageHeader() {};
    CImageHeader(CImageHeader const & src);
    ~CImageHeader() {}
    
    CImageHeader & operator=(CImageHeader const & src);
        
    bool operator==(CImageHeader const & rhs) const;    
    bool operator!=(CImageHeader const & rhs) const;
    
    /// Dimensions of the image.
    TSize size[3];
    
    /// Size of the voxels.
    TSpacing spacing[3];
    
    /// Origin of the bounding box.
    //Point bbOrigin;
    
    /// Size of the bounding box.
    //TSize  bbSize[3];
    
    /// Image identifier.
   // unsigned long identifier;
    
    /// Image name.
    //std::string name;        
        
}; // class CImageHeader


/**
 *  Load or save a header from an InrImage file.
 *  \sa CHeaderIO.
 */
class CHeaderIO_InrImage 
{
    public:
    /**
     *  Initializes from a istream.
     */
    CHeaderIO_InrImage(std::istream * stream)
    :istream_(stream), ostream_(static_cast<std::ostream*>(0)) {}
    
    /**
     *  Initializes from a ostream.
     */
    CHeaderIO_InrImage(std::ostream * stream)
    : istream_(static_cast<std::istream*>(0)), ostream_(stream)
    {
        istream_ = static_cast<std::istream*>(0);
        ostream_ = stream;
    }
    
    /**
     *  Initializes from a iostream.
     */
    CHeaderIO_InrImage(std::iostream * stream)
    :istream_(stream), ostream_(stream)
    {
    }
    
    /**
     *  Destructor, does nothing (parent's called implicitely).
     */
    ~CHeaderIO_InrImage() {};
    
     CHeaderIO_InrImage(CHeaderIO_InrImage const & src)
    {
        istream_ = src.istream_;
        ostream_ = src.ostream_;
    }
    
    /**
     *  Used to determine the type of the image.
     *  @return true if the type of the template matches the type of the buffer,
     *  false otherwise.
     */
    bool isOfType(std::type_info const & ti);
    
    const std::type_info & getType();
    
    /**
     *  Load header (parse InrImage header)
     */
    CImageHeader load();
    
    /**
     *  Save header in InrImage format
     */
    void save(CImageHeader const & header, std::type_info const & ti);
    
    /**
     *  Informations about some type-dependant fields in the header.
     *  Has to be specialized for each known type.
     */
    template<typename T>
    struct HeaderInfo
    {
        static bool const knownType = false;
        static std::string vdim()    { return std::string(""); }
        static std::string pixsize() { return std::string(""); }
        static std::string type()    { return std::string(""); }
    };
    
    protected :
    /**
     *  Header parsed in the form <field, value>
     */
    std::map<std::string, std::string> parsedHeader_;
    
    std::istream * istream_;
    std::ostream * ostream_;
    
}; // class CHeaderIO_InrImage


inline bool CHeaderIO_InrImage::isOfType(std::type_info const & ti)
{
    if(parsedHeader_.count("PIXSIZE")==0 || parsedHeader_.count("TYPE")==0) this->load();
    
    std::string baseType = parsedHeader_["TYPE"];
    std::string nbBytes = parsedHeader_["PIXSIZE"];
    
    if(parsedHeader_["VDIM"]=="1")
    {
        if(baseType=="unsigned fixed" && nbBytes=="8 bits" &&  ti==typeid(U8)) return true;
        else if(baseType=="unsigned fixed" && nbBytes=="16 bits" && ti==typeid(U16)) return true;
        else if(baseType=="unsigned fixed" && nbBytes=="32 bits" && ti==typeid(U32)) return true;
        else if(baseType=="signed fixed"   && nbBytes=="8 bits"  && ti==typeid(S8)) return true;
        else if(baseType=="signed fixed"   && nbBytes=="16 bits" && ti==typeid(S16)) return true;
        else if(baseType=="signed fixed"   && nbBytes=="32 bits" && ti==typeid(S32)) return true;
      
        else return false;
    }
    else if(parsedHeader_["VDIM"]=="3") 
    {
        if(baseType=="unsigned fixed" && nbBytes=="8 bits" && ti==typeid(RGB)) return true;
        else return false;
    }
    else return false;
}
/******************************************************************************/
inline const std::type_info & CHeaderIO_InrImage::getType()
{
    if(parsedHeader_.count("PIXSIZE")==0 || parsedHeader_.count("TYPE")==0) this->load();
    
    std::string baseType = parsedHeader_["TYPE"];
    std::string nbBytes = parsedHeader_["PIXSIZE"];
    
    if(parsedHeader_["VDIM"]=="1")
    {
        if(baseType=="unsigned fixed" && nbBytes=="8 bits") return typeid(U8);
        else if(baseType=="unsigned fixed" && nbBytes=="16 bits") return typeid(U16);
        else if(baseType=="unsigned fixed" && nbBytes=="32 bits") return typeid(U32);
        else if(baseType=="signed fixed"   && nbBytes=="8 bits" ) return typeid(S8);
        else if(baseType=="signed fixed"   && nbBytes=="16 bits") return typeid(S16);
        else if(baseType=="signed fixed"   && nbBytes=="32 bits") return typeid(S32);

        else return typeid(void);
    }
    else if(parsedHeader_["VDIM"]=="3") 
    {
        if(baseType=="unsigned fixed" && nbBytes=="8 bits") return typeid(RGB);
        else return typeid(void);
    }
    else return typeid(void);
}

inline CImageHeader CHeaderIO_InrImage::load()
{
    assert(istream_!=static_cast<std::istream*>(0) && istream_->good());
    CImageHeader h;
    
    /* According to the only web page I've found describing the InrImage format,
     * the header should be 256 bytes long. However, there are some images that
	 * have a 1024 bytes header or whatever. So instead of reading 256 bytes and
	 * check that it ends in "##}\n", we look for the first occurence of "##}\n"
	 * in the file.
     */
    
    
    // Header is 256 bytes, read them
    char buffer[257];
    istream_->read(buffer, 256);
    buffer[256]='\0';
    // If we read less than 256 bytes, error    
    assert(!istream_->bad());
    std::string headerString(buffer);
    // If we encounter a '\0' before the end, then error
    assert(headerString.length()==256);
    
    // Length is ok, let's do some parsing!
    /* The header of an InrImage is as follows (BNF-form) :
     * header -> openingLine lines closingLine
     * openingLine -> "#INRIMAGE-4#{\n"
     * closingLine -> "##}\n"
     * lines -> line
     *       -> line lines
     * line -> comment'\n'
     *      -> blank'\n'
     *      -> pair'\n'
     * comment -> #string
     * blank -> whitespace
     * pair -> field=value
     * field -> string
     * value -> string
     */
    std::string const openingLine("#INRIMAGE-4#{\n");
    std::string const closingLine("##}\n");
    
    // The string should begin with the openingLine ...
    assert(headerString.find(openingLine)==0);
    // ... and finish with the closing line
    assert(headerString.rfind(closingLine) == headerString.length()-closingLine.length());        
    
    // Read line by line
    int currentIndex = 0;
    std::string::size_type lineEnd=0;
    std::string currentLine;
    while(lineEnd<=headerString.length())
    {
        lineEnd = headerString.find('\n', currentIndex);
        currentLine = headerString.substr(currentIndex, lineEnd-currentIndex);                        
        if(currentLine[0]=='#') ; // skip comments
        else
        {
            if(currentLine.find_first_not_of(" \t")==std::string::npos) ; // skip blank lines
            else
            {
                // Field left of equal
                int fieldBegin = currentLine.find_first_not_of(" \t");
                int fieldEnd   = currentLine.find_first_of(" \t=", fieldBegin); // first character after the end of the field
                std::string field = currentLine.substr(fieldBegin, fieldEnd);
                // Find equal
                int equalIndex = currentLine.find('=');
                // Value right of equal
                int valueBegin = currentLine.find_first_not_of(std::string(" \t"), equalIndex+1);
                //int valueEnd   = currentLine.find_last_not_of(std::string(" \t"), valueBegin+1);
                // FIXME : consider lines with trailing whitespaces
                int valueEnd   = currentLine.length();
                std::string value = currentLine.substr(valueBegin, valueEnd);
                parsedHeader_[field]=value;
            }
        }
        currentIndex = lineEnd+1;
    }
    
    /* Following fields HAVE to be there for a header to be OK. Other can be set to default values : 
     * XDIM, YDIM, ZDIM, VDIM
     * CPU
     * PIXSIZE, TYPE
     */
    assert(parsedHeader_.count("XDIM")==1 && parsedHeader_.count("YDIM")==1 && parsedHeader_.count("ZDIM")==1 /*&& parsedHeader_.count("VDIM")*/);
    assert(parsedHeader_.count("CPU")==1 && parsedHeader_.count("PIXSIZE")==1 && parsedHeader_.count("TYPE")==1); 
    assert(parsedHeader_["PIXSIZE"]=="8 bits"  || parsedHeader_["PIXSIZE"]=="16 bits" || 
           parsedHeader_["PIXSIZE"]=="32 bits" || parsedHeader_["PIXSIZE"]=="64 bits");
    
    // Header is valid in the file, generate the CImage header.
    /* Image size
     * VDIM is technically used for data types that are not of dimension one
     * (vector, color, ...). We'll assume that an InrImage cannot store 
     * time series, and keep the VDIM for what it's designed for.
     */
	h.size[0] = atol(parsedHeader_["XDIM"].c_str());
	h.size[1] = atol(parsedHeader_["YDIM"].c_str());
	h.size[2] = atol(parsedHeader_["ZDIM"].c_str());
	
    // Voxel size, default to 1.0
	h.spacing[0] = h.spacing[1] = h.spacing[2] = 1.0;    
	if(parsedHeader_.count("VX")==1) h.spacing[0] = atof(parsedHeader_["VX"].c_str());
	if(parsedHeader_.count("VY")==1) h.spacing[1] = atof(parsedHeader_["VY"].c_str());
	if(parsedHeader_.count("VZ")==1) h.spacing[2] = atof(parsedHeader_["VZ"].c_str());
	
    return h;
}
/******************************************************************************/
inline void CHeaderIO_InrImage::save(CImageHeader const & header, std::type_info const & ti)
{
    assert(ostream_!=NULL);
    
    std::stringstream headerString;
    headerString << "#INRIMAGE-4#{\n";
 
    headerString << "XDIM=" << header.size[0] << "\n";
    headerString << "YDIM=" << header.size[1] << "\n";
    headerString << "ZDIM=" << header.size[2] << "\n";
    if(ti!=typeid(RGB)) headerString << "VDIM=" << 1 << "\n";
    else headerString << "VDIM=" << 3 << "\n";

    headerString << "VX=" << header.spacing[0] << "\n";
    headerString << "VY=" << header.spacing[1] << "\n";
    headerString << "VZ=" << header.spacing[2] << "\n";
    headerString << "SCALE=2**0\n";
    headerString << "CPU=" << "pc" << "\n";
    
    if(ti==typeid(S8)) headerString << "PIXSIZE=8 bits\nTYPE=signed fixed\n";
    else if(ti==typeid(U8)) headerString << "PIXSIZE=8 bits\nTYPE=unsigned fixed\n";
    else if(ti==typeid(S16)) headerString << "PIXSIZE=16 bits\nTYPE=signed fixed\n";
    else if(ti==typeid(U16)) headerString << "PIXSIZE=16 bits\nTYPE=unsigned fixed\n";
    else if(ti==typeid(S32)) headerString << "PIXSIZE=32 bits\nTYPE=signed fixed\n";
    else if(ti==typeid(U32)) headerString << "PIXSIZE=32 bits\nTYPE=unsigned fixed\n";
    else if(ti==typeid(char)) // find if it is signed or unsigned, and behave appropriately
    {
        if(std::numeric_limits<char>::is_signed) headerString << "PIXSIZE=8 bits\nTYPE=signed fixed\n";
        else headerString << "PIXSIZE=8 bits\nTYPE=unsigned fixed\n";
    }
    else if(ti==typeid(int)) // find if it is short or long, or something else
    {
        if(std::numeric_limits<int>::min()==std::numeric_limits<S16>::min() &&
           std::numeric_limits<int>::max()==std::numeric_limits<S16>::max())
        {
            headerString << "PIXSIZE=16 bits\nTYPE=signed fixed\n";
        }
        else if(std::numeric_limits<int>::min()==std::numeric_limits<S32>::min() &&
           std::numeric_limits<int>::max()==std::numeric_limits<S32>::max())
        {
            headerString << "PIXSIZE=32 bits\nTYPE=signed fixed\n";
        }
        else assert(false); // int neither long nor short.
    }
    else assert(false); // Unknown type
    
    std::string const closingLine("##}\n");
    for(std::string::size_type i=headerString.str().length(); i<256-closingLine.length(); ++i)
        headerString << "\n";
    
    headerString << closingLine;
    
    (*ostream_) << headerString.str();
}

/**
 *  Load or save an image buffer.
 *  This class is similar to CHeaderIO, except it deals with CImage<T> buffer I/O.
 *  \sa CHeaderIO
 *
 *  Load example, using the CBufferIO_InrImage :
 *  \code
 *  std::ifstream stream("image.inr");
 *  CBufferIO_InrImage<U8> bLoader(&stream);
 *  U8* buffer = bLoader.load(image.getBufSize());
 *  \endcode
 *
 *  Save example, using the CBufferIO_InrImage : 
 *  \code
 *  U8* buffer = new U8[512*512*150];
 *  // ...
 *  std::ofstream stream("image.inr.gz");
 *  CBufferIO_InrImage<U8> bSaver(&stream);
 *  bSaver.save(buffer, 512*512*150);
 *  \endcode
 */

/**
 *  Load or save a buffer from an InrImage file.
 *  \sa CBufferIO.
 */
template<typename T>
class CBufferIO_InrImage 
{
    public:
    /**
     *  Initializes from a istream.
     */
    CBufferIO_InrImage(std::istream * stream)
    :istream_(stream), ostream_(static_cast<std::ostream*>(0)) {};
    
    /**
     *  Initializes from a ostream.
     */
    CBufferIO_InrImage(std::ostream * stream)
    : istream_(static_cast<std::istream*>(0)), ostream_(stream)
    	{
        istream_ = static_cast<std::istream*>(0);
        ostream_ = stream;
   		}
    
    /**
     *  Initializes from a iostream.
     */
    CBufferIO_InrImage(std::iostream * stream)
    :istream_(stream), ostream_(stream)
    {
    }
    
    
    /**
     *  Destructor, does nothing (parent's called implicitely).
     */
    ~CBufferIO_InrImage() {};
       
    /**
     *  Load header (parse InrImage header)
     */
    T* load(TOffset const size);
    
    /**
     *  Save header in InrImage format
     */
    void save(T* buffer, TOffset size);   
    
    std::istream * istream_;
    std::ostream * ostream_;
}; 


template<typename T>
T* CBufferIO_InrImage<T>::load(TOffset const size)
{
    // Skip 256 bytes of the header
    //istream_->ignore(256);
    T* buffer = new T[size];
    istream_->read(reinterpret_cast<char*>(buffer), static_cast<std::streamsize>(size*sizeof(T)));
    assert(istream_->gcount()==static_cast<std::streamsize>(size*sizeof(T)));
    
    return buffer;
}
/******************************************************************************/
template<typename T>
void CBufferIO_InrImage<T>::save(T* buffer, TOffset size)
{
    ostream_->write( reinterpret_cast<char*>(buffer), size*sizeof(T));
    assert(ostream_->good());
}



template<typename T>
int Image<T>::loadInrGz(const char *fileName, Image<T> &image)
{
    igzstream stream(fileName);
    // This is brutal, but what else ?
    assert(!stream.bad() && !stream.fail());       
    
    CHeaderIO_InrImage hLoader(&stream);        
    
    // Load header
    CImageHeader header = hLoader.load();
    // Check if the types of the file and the image match.
    assert(hLoader.isOfType(typeid(T)));
    image.size[0]    = header.size[0];
    image.size[1]      = header.size[1];
    image.size[2]      = header.size[2];
    
    image.setSize(image.size);
   
    image.spacing[0]       = header.spacing[0];
    image.spacing[1]         = header.spacing[1];
    image.spacing[2]         = header.spacing[2];
    
    // Header is loaded, now let's take care of the buffer
    CBufferIO_InrImage<T> bLoader(&stream);
    if(image.data != static_cast<T*>(0)) delete[] image.data;
    image.data = bLoader.load(image.getBufSize());
    
    stream.close();

    return 0;

}
/***********************************************************/
template<typename T>
/*static*/ int Image<T>::saveInrGz(const char *fileName)
{
    std::ostream * stream = NULL;

    stream = new ogzstream(fileName);
    
    // Brutality once again...
    assert(!stream->bad() && !stream->fail());
    
    // Switch on si.fileType
    CHeaderIO_InrImage* hSaver=NULL;
    CBufferIO_InrImage<T>* bSaver=NULL;
    
    hSaver = new CHeaderIO_InrImage(stream);
    bSaver = new CBufferIO_InrImage<T>(stream);    
    
    // Save header
    CImageHeader h;
    h.size[0]=this->getSizeX(); 
    h.size[1]=this->getSizeY(); 
    h.size[2]=this->getSizeZ(); 
    h.spacing[0]=this->getSpacingX(); 
    h.spacing[1]=this->getSpacingY();
    h.spacing[2]=this->getSpacingZ();
    
    // FIXME : as usual, do something with VT
    hSaver->save(h, typeid(T));
    
    // And now the buffer
    bSaver->save(this->data, this->getBufSize());
    
    delete stream;
    delete hSaver;
    delete bSaver;
    return 0;
}

}	//End namespace
