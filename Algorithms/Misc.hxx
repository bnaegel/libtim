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
#include "Common/FlatSE.h"

namespace LibTIM {

/** \defgroup misc Misc Functions
	\ingroup ImageProcessing
**/

/*@{*/

/// Scale image intensity according to the linear relation:
/// x \in [a,b], f(x) \in [A,B]
///f(x)=A+(x-a)(B-A)/(b-a)
///In this version a=im.getMin(), b=im.getMax(), A=typeMin(), B=typeMax 


template <class T>
void adjustContrast(Image <T> &im)
{
	T b=im.getMax();
	T a=im.getMin();
	T B=std::numeric_limits<T>::max();
	T A=std::numeric_limits<T>::min();
	
	if( (b-a)!=0)
		{	
		double ratio=(double)(B-A)/(double)(b-a);
	
		for(int i=0; i<im.getBufSize(); i++)
			im(i)=(T)(A+(im(i)-a)*ratio);
		}
}

///Same thing but with A and B given in parameters

template <class T>
void adjustContrast(Image <T> &im, T A, T B)
{
	T b=im.getMax();
	T a=im.getMin();
	
	if( (b-a)!=0)
		{	
		double ratio=(double)(B-A)/(b-a);
		for(int i=0; i<im.getBufSize(); i++)
			im(i)=(T)(A+(im(i)-a)*ratio);
		}
}

///Same thing but with A, B, a and b given in parameters

template <class T>
void adjustContrast(Image <T> &im, T A, T B, T a, T b)
{
	if( (b-a)!=0)
		{	
		double ratio=(double)(B-A)/(b-a);
		for(int i=0; i<im.getBufSize(); i++)
			im(i)=(T)(A+(im(i)-a)*ratio);
		}
}

///For each marker compute the mean of the points on original image
template <class T, class T2>
Image <T> computeMarkerMean(Image <T> &src, Image <T2> &marker)
{
	Image<T> res(src.getSize());
	res.fill(T(0));
	
	T2 max=marker.getMax();
	typedef std::vector <double> tVectorD;
	typedef std::vector <int>  tVectorI;
	tVectorD sumGreyLevel(max+1);
	tVectorI nbPoints(max+1);
	
	for(tVectorD::iterator it=sumGreyLevel.begin(); it!=sumGreyLevel.end(); ++it) *it=0;
	for(tVectorI::iterator it=nbPoints.begin(); it!=nbPoints.end(); ++it) *it=0;
	
	for(int i=0; i< src.getBufSize(); i++)
		{
		if(marker(i)!=T2(0)) 
			{
			sumGreyLevel[marker(i)]+=src(i);
			nbPoints[marker(i)]++;
			}
		}
	
	for(int i=0; i<res.getBufSize(); i++)
		{
		if(marker(i)!=T2(0))
			res(i)=(T)(sumGreyLevel[ marker(i)  ] / nbPoints[ marker(i) ]);	 
		
		}
	return res;
}

///For each marker compute the mean of the points on original image
template <class T, class T2>
Image <T> computeMarkerMeanFast(Image <T> &src, Image <T2> &marker)
{
	T2 max=marker.getMax();
	typedef std::vector <double> VectorD;
	typedef std::vector <int>  VectorI;
	VectorD sumGreyLevel(max+1);
	VectorI nbPoints(max+1);
	
	for(VectorD::iterator it=sumGreyLevel.begin(); it!=sumGreyLevel.end(); it++) *it=0;
	for(VectorI::iterator it=nbPoints.begin(); it!=nbPoints.end(); it++) *it=0;
	
	Image<T> res=marker;
	res.fill(T(0));
	
	for(int i=0; i< src.getBufSize(); i++)
		{
		if(marker(i)!=T2(0)) 
			{
			nbPoints[marker(i)]++;
			double N=(double)nbPoints[marker(i)];
			sumGreyLevel[marker(i)]*=(N-1)/(N);
			sumGreyLevel[marker(i)]+=(double)(src(i)/N);
			}
		}
	
	for(int i=0; i<marker.getBufSize(); i++)
		if(marker(i)!=T2(0))
			res(i)=(T)sumGreyLevel[marker(i)];	 
		
	return res;
}

///For each marker compute the mean of the points on original image

template <class T2>
Image <RGB> computeMarkerMean(Image <RGB> &src, Image <T2> &marker)
{
	Image<RGB> res(src.getSize());
	
	T2 max=marker.getMax();
	typedef std::vector <Table <double,3> > containerValue;
	typedef std::vector <int>  containerArea;
	containerValue values(max+1);
	containerArea area(max+1);
	
	for(containerValue::iterator it=values.begin(); it!=values.end(); ++it) 
		{
		(*it)[0]=0;
		(*it)[1]=0;
		(*it)[2]=0;
		}
	for(containerArea::iterator it=area.begin(); it!=area.end(); ++it) 
		*it=0;
	
	for(int i=0; i< src.getBufSize(); i++)
		{
		if(marker(i)!=T2(0)) 
			{
			values[marker(i)][0]+=src(i)[0];
			values[marker(i)][1]+=src(i)[1];
			values[marker(i)][2]+=src(i)[2];
			area[marker(i)]++;
			}
		}
	
	for(int i=0; i<res.getBufSize(); i++)
		{
		if(marker(i)!=T2(0))
			{
			res(i)[0]=(U8)(values[ marker(i)  ][0] / area[ marker(i) ]);	 
			res(i)[1]=(U8)(values[ marker(i)  ][1] / area[ marker(i) ]);	 
			res(i)[2]=(U8)(values[ marker(i)  ][2] / area[ marker(i) ]);	 
			
			}
		
		}
	return res;
}

///Image decimation by imposing a regular grid -> useful for simplifying structuring elements

template <class T>
void decimateTemplate(Image <T> &im, int nx=1, int ny=1, int nz=1)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	int px=1;
	int py=1;
	int pz=1;
	for(int z=0; z<dz; z++)
		{
		for(int y=0; y<dy; y++)
			{
			for(int x=0; x<dx; x++)
				{
				if(px>=nx && py>=ny && pz>=nz)
					{
					px=0;
					}
				else im(x,y,z)=T(0);
				px++;
				}
			if(py>=ny) py=0;
			py++;
			}
		if(pz>=nz) pz=0;
		pz++;
		}
}

///compute the centroids of labelled objects (first moments) in 2D images
inline std::map<TLabel,Point<double> > centroids(Image <TLabel> &im)
{
	assert(im.getSizeZ()==1);
	
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	
	TLabel maxValue=im.getMax();
	std::map<TLabel, Point<double> > res;
	std::map<TLabel, int> sum;
	
	for(int x=0; x<dx; x++)
		for(int y=0; y<dy; y++)
			{
			if(im(x,y)!=0)
				{
				res[im(x,y)].x+=x;
				res[im(x,y)].y+=y;
				sum[im(x,y)]++;
				}
			}
	
	for(std::map<TLabel,Point<double> >::iterator it=res.begin(); it!=res.end(); ++it)
			{
			it->second.x/=sum[it->first];
			it->second.y/=sum[it->first];
			}
	return res;
}

template <class T>
void drawContour(Image <T> &im, const Image <U8> &mask, const T val)
{
	assert(	im.getSizeX()==mask.getSizeX() && 
			im.getSizeY()==mask.getSizeY() &&
			im.getSizeZ()==mask.getSizeZ() );
	
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	FlatSE nghb;
	nghb.make2DN8();
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(mask(x,y,z) != 0)
					for(int i=0; i<nghb.getNbPoints(); i++)
						{
						Point <TCoord> p(x,y,z);
						Point <TCoord> q=p+nghb.getPoint(i);
						if(im.isPosValid(q))
							if(mask(q)==0)
								im(x,y,z)=val; 	
					   	}
				} 
}

template <class T>
void drawContourN4(Image <T> &im, const Image <U8> &mask, const T val)
{
	assert(	im.getSizeX()==mask.getSizeX() && 
			im.getSizeY()==mask.getSizeY() &&
			im.getSizeZ()==mask.getSizeZ() );
	
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	FlatSE nghb;
	nghb.make2DN4();
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(mask(x,y,z) != 0)
					for(int i=0; i<nghb.getNbPoints(); i++)
						{
						Point <TCoord> p(x,y,z);
						Point <TCoord> q=p+nghb.getPoint(i);
						if(im.isPosValid(q))
							if(mask(q)==0)
								im(x,y,z)=val; 	
					   	}
				} 
}


template <class T>
void contourBinaryObjectN4(Image <T> &im)
{
	
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image<U8> tmp=im;
	
	im.fill(T(0));
	
	FlatSE nghb;
	nghb.make2DN4();
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(tmp(x,y,z) != 0)
					for(int i=0; i<nghb.getNbPoints(); i++)
						{
						Point <TCoord> p(x,y,z);
						Point <TCoord> q=p+nghb.getPoint(i);
						if(tmp.isPosValid(q))
							if(tmp(q)==0)
								im(x,y,z)=T(255); 	
					   	}
				} 
}


template <class T>
void contourBinaryObjectN8(Image <T> &im)
{
	
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image<U8> tmp=im;
	
	im.fill(0);
	
	FlatSE nghb;
	nghb.make2DN8();
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(tmp(x,y,z) != 0)
					for(int i=0; i<nghb.getNbPoints(); i++)
						{
						Point <TCoord> p(x,y,z);
						Point <TCoord> q=p+nghb.getPoint(i);
						if(tmp.isPosValid(q))
							if(tmp(q)==0)
								im(x,y,z)=255; 	
					   	}
				} 
}
/*@}*/

} //namespace
