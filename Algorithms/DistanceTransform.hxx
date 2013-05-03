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

#include "Common/Image.h"
#include "Common/NonFlatSE.h"

namespace LibTIM {

/** \defgroup DistanceTransform Distance Transform
	\ingroup ImageProcessing **/
	
/*@{*/

///Distance transform
///Compute distance transform from non-zero pixels of im from the chamfer mask mask

template <class T, class T2>
Image <U16> chamferDistanceTransform(Image <T> &im, NonFlatSE <T2> &mask)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <unsigned int> res(im.getSize());
	
	U16 minValue=std::numeric_limits<U16>::min(); 
	U16 maxValue=std::numeric_limits<U16>::max(); 
	
	NonFlatSE <T2> maskRasterScan=mask.rasterScan();
	NonFlatSE <T2> maskAntiRasterScan=mask.antiRasterScan();
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	typename Image<unsigned int>::iterator itRes=res.begin();
	
	
	for(it=im.begin(); it!=end; ++it,++itRes) 
		if(*it > T(0)) *itRes=maxValue;
		else *itRes=minValue;
		
	///Raster scan
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				unsigned int currentValue=res(x,y,z);
				Point<TCoord> currentPoint(x,y,z);
				for(int i=0; i<maskRasterScan.getNbPoints(); i++)
					{
					if(res.isPosValid(currentPoint+maskRasterScan.getPoint(i) ))
						currentValue=std::min(currentValue, 
						res(currentPoint+maskRasterScan.getPoint(i))+maskRasterScan.getValue(i));
					}
				res(x,y,z)=currentValue;
				}
	
	
	///Anti-raster scan
	for(int z=dz-1; z>=0; z--)
		for(int y=dy-1; y>=0; y--)
			for(int x=dx-1; x>=0; x--)
				{
				unsigned int currentValue=res(x,y,z);
				Point<TCoord> currentPoint(x,y,z);
				for(int i=0; i<maskAntiRasterScan.getNbPoints(); i++)
					{
					if(res.isPosValid(currentPoint+maskAntiRasterScan.getPoint(i) ))
						currentValue=std::min(currentValue, 
						res(currentPoint+maskAntiRasterScan.getPoint(i))+maskAntiRasterScan.getValue(i));
					}
				res(x,y,z)=currentValue;
				}
				
	return res;
}

/*@]*/

} //namespace
