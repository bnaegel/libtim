#ifndef Histogram_h
#define Histogram_h

#include "Image.h"
#include <map>


namespace LibTIM {

/** \defgroup Histogram 
	\ingroup DataStructures
**/

/*@{*/

///Container for histograms
/**
	Structure describing an histogram.
	Histogram can be constructed from an Image
**/

template <class T>
class Histogram {
	
	typedef std::map <T, int, std::less<T> > HistoType;
	
	public:
		///Constructs an histogram from image im
		Histogram(Image <T> &im);
		///Write histogram to disk
		/**
		Histogram is writed in a text file (xmgrace format)
		**/
		void write(const char *filename);
	
	private:
		HistoType data;
};

/*@}*/

}

#include "Histogram.hxx"
#endif
