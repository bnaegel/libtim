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
