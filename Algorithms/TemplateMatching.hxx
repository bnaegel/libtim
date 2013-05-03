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

namespace LibTIM {

/** \defgroup templateMatching Template Matching Based Algorithms
	\ingroup ImageProcessing
**/

/*@{*/

///Compute point by point the mean euclidian distance (L2 norm) between the image and the template
///To avoid false detections we set to max the distance when the template hits the image border

template <class T>
Image <int> templateMatchingL2(const Image <T> &im, const NonFlatSE <U8> &mask)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <int> res(im.getSize());
	
	int maxValue=std::numeric_limits<int>::max();
	
	T maxValueImageType=std::numeric_limits<T>::max();
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				bool hitsBorder=false;
				int currentDifference=0;
				Point <TCoord> p(x,y,z);
				for(int i=0; i<mask.getNbPoints(); i++)
					{
					Point <TCoord> q=p+mask.getPoint(i);
					if(!res.isPosValid(q))
						{
						hitsBorder=true;
						break;
						}
					else 
						{
						int templateValue=mask.getValue(i);
						currentDifference+=(templateValue-im(q))*(templateValue-im(q));
						}
					}
				if(!hitsBorder)
					res(x,y,z)=currentDifference/mask.getNbPoints();
				else res(x,y,z)=maxValue;
				}
	
	return res;
}	
	
template <class T, class T2>
Image <T> printBestTemplate(const Image <T2> &resTM, const Image <T> &im, const FlatSE &A, T2 value)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <T> res=im;
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(resTM(x,y,z)==value)
					{
					Point <TCoord> p(x,y,z);
					for(int i=0; i<A.getNbPoints(); i++)
						{
						Point <TCoord> q=p+A.getPoint(i);
						if(res.isPosValid(q))
							if(res(q)>0) res(q)=200;
							else res(q)=225;
						}
					}
				}
	return res;
}	

/** @brief Correlation between template and image point by point
* Correlation score is regularized with respect to the product of the vector norms
* Here, we correlate a template of size L with a subimage of size L
* When template hits border, we set correlation score to 0
* Regularized correlation score is comprised between -1 (anti-correlation) and 1 (correlation).
**/

template <class T>
Image <double> templateMatchingCorrelation(const Image <T> &im, const NonFlatSE <U8> &mask)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <double> res(im.getSize());
	
	///Mask size
	int maskSize=mask.getNbPoints();
	int maskSizeSqr=maskSize*maskSize;
	
	double normMask=mask.getNorm();
	double normSubImage;
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				bool hitsBorder=false;
				double currentScore=0;
				normSubImage=0;
				Point <TCoord> p(x,y,z);
				for(int i=0; i<maskSize; i++)
					{
					Point <TCoord> q=p+mask.getPoint(i);
					if(!res.isPosValid(q))
						{
						hitsBorder=true;
						break;
						}
					else 
						{
						int templateValue=mask.getValue(i);
						currentScore+=templateValue*im(q);
						normSubImage+=im(q)*im(q);
						}
					}
				if(!hitsBorder)
					res(x,y,z)=currentScore/(normMask*sqrt(normSubImage));
				else res(x,y,z)=(double)0;
				}
	
	return res;
}	

/*@}*/

}
