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
