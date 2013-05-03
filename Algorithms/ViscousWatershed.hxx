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

#include <cmath>
#include <map>


namespace LibTIM {

template <class T>
int functionR0(double r0, T t)
{

return (int)(r0*exp(-t*0.02));
}

/** \defgroup constrainedWatershed Constrained Watershed Algorithms
	\ingroup Morpho
**/

/*@{*/

///Viscous closing according to Vachier's definition.
///This function defines the mercury viscous closing on the gradient image src


template <class T>
void viscousClosingMercuryBasic(Image <T> &src, double r0)
{
	T maxVal=std::numeric_limits<T>::min();
	
	for(int i=0; i<src.getBufSize(); i++)
		{
		if(src(i)>maxVal) maxVal=src(i);
		}
	
	T minVal=std::numeric_limits<T>::max();
	for(int i=0; i<src.getBufSize(); i++)
		{
		if(src(i)<minVal) minVal=src(i);
		}	
	Image <int> res=src;
	
	Image <int> tmp(src.getSize() );
	for(int i=0; i<tmp.getBufSize(); i++)
		tmp(i)=(int)src(i);
	
	///First closing with maximal disk
	FlatSE se;
	se.makeBallEuclidian2D(res,r0);
	
	closing(res, se);
	int r=r0;
	for(T t=minVal+1; r>=1; t++)
		{
		
		for(int i=0; i<tmp.getBufSize(); i++)
			tmp(i)=(int)src(i)+t;
		r=functionR0(r0,t);
		std::cout << "T= " << (int)t << "Tmax= " << (int)maxVal << "r= " << r << "\n";

		if(r>=1)
		{
		se.makeBallEuclidian2D(res, r);
		closing(tmp,se);res&=tmp;
		}
		
		}
	
	for(int i=0; i<src.getBufSize(); i++)
		src(i)=(T)res(i);
	
}

///Version two: we try to optimize a little

template <class T>
void viscousClosingMercury(Image <T> &src, double r0)
{
	T maxVal=std::numeric_limits<T>::min();
	
	for(int i=0; i<src.getBufSize(); i++)
		{
		if(src(i)>maxVal) maxVal=src(i);
		}
	
	T minVal=std::numeric_limits<T>::max();
	for(int i=0; i<src.getBufSize(); i++)
		{
		if(src(i)<minVal) minVal=src(i);
		}
	
	///First we close the image src with all possible structuring elements
	///We put each closing into a map referenced by the parameter r of the structuring element
	std::map<int, Image<T> > imageOfClosings;
 	int r;
 	int t=minVal;
 		do
		{
		r=functionR0(r0,t);
		if(imageOfClosings.count(r)==0)
			{
			///element r is not yet in the map
			std::cout << "r: " << r << "size of map: " << imageOfClosings.size() << "\n";
			Image <T> tmp=src;
			if(r>=1)
				{
				FlatSE se;
				se.makeBallEuclidian2D(src,r);
				closing(tmp, se);
				}
			imageOfClosings[r]=tmp;
			}
		t++;
		}while(r>0 && t<=maxVal);
	r=functionR0(r0,minVal);	
	Image <int> res(imageOfClosings[(int)r]);
	
	for(int t=minVal+1; t<=maxVal; t++)
		{
		r=functionR0(r0,t);
		Image <int> tmp(imageOfClosings[r]);
		tmp=tmp+(int)t;
		res&=tmp;
		}
src=res;
	
}

/*@}*/

} //end namespace
