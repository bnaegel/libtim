#ifndef NonFlatSE_h
#define NonFlatSE_h

#include <cmath>
#include <limits>

#include "Point.h"
#include "FlatSE.h"

namespace LibTIM {

/** \defgroup NonFlatSE Non-flat structuring elements
	\ingroup DataStructures
**/

/*@{*/
/// Non-flat structuring elements (or ponderated masks)
/** Can be used for convolution, chanfrein masks, or non-flat morphology
**/
template <class T>
class NonFlatSE: public FlatSE {
		//points and offsets inherited from FlatSE
		//std::vector<Point<TCoord> > points;
		//std::vector<TOffset> offsets;
		std::vector<T> attributes;		
	public:
		NonFlatSE() {}
		~NonFlatSE() {
			this->attributes.clear();
			}
		 
		 void makeChamfer2D();
		 NonFlatSE rasterScan();
		 NonFlatSE antiRasterScan();
		 double getNorm() const;
		
		T getValue(int i) const {return attributes[i];}
		
		void addPoint(Point <TCoord> p, T attribute) 
			{
			assert(this->points.size() == this->attributes.size() );
			points.push_back(p);
			attributes.push_back(attribute);
			assert(this->points.size() == this->attributes.size() );
			}
		
		void print() {for(int i=0; i<getNbPoints(); i++) {this->points[i].print(); 
		std::cout << " " << this->attributes[i] << "\n";} }
		void reserve(size_t size) {attributes.reserve(size);}
		void clear() { this->points.clear(); this->offsets.clear(); this->attributes.clear(); }
};

/*@}*/
}

#include "NonFlatSE.hxx"

#endif
