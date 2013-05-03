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

#ifndef Types_h
#define Types_h

namespace LibTIM 
{
//Machine dependant typedefs

typedef unsigned char U8;
typedef signed char S8;

typedef unsigned short U16;
typedef signed short S16;

typedef unsigned long  U32;
typedef signed long    S32;


//Table 
template <class T, int N>
struct Table{
	Table(){};
	Table(const Table &v)
		{
		for(int i=0; i<N; i++) el[i]=v.el[i];
		}
	Table(int p) {for(int i=0; i<N; i++) el[i]=p;}
	Table(int *vect) {for(int i=0; i<N; i++) el[i]=vect[i];}
	
	T el[N]; 
	T &operator[](int i)
	{return el[i];}
	};

//Type of RGB point
typedef Table<U8,3> RGB;

//Type of image size 
typedef unsigned short TSize;

//Type of point spacing
typedef double TSpacing;

//Type of points coordinates
typedef int TCoord;

//Type of label
typedef unsigned long TLabel;

//Type of offset
typedef  long TOffset;

const float FLOAT_EPSILON=0.0000000001f;
}

#endif
