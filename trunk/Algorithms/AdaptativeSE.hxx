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

namespace LibTIM {

template <class T>
void dynamicSeNormL2(Image <T> &img, const Point <TCoord> &p, const FlatSE &B, int param,  FlatSE &se)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	se.clear();
	
	//Scan entire image to compute the similarity measure between p and each q
	//If similarity measure is lesser or equal to param, we include the point in the se
	
		for(int y=-10; y<10; y++)
			for(int x=-10; x<10; x++)
				{
				double sim=0;
				Point <TCoord> q(x+p.x,y+p.y,0);
				//q.print();
				if(img.isPosValid(q))
				{
				for(int i=0; i<B.getNbPoints(); i++)
					{
					Point <TCoord> pn=p+B.getPoint(i);
					Point <TCoord> qn=q+B.getPoint(i);
					if(img.isPosValid(pn) && img.isPosValid(qn)  )
						{
						double value=img(qn)-img(pn);
						value*=value;
						sim+=value;
						}
					}
				sim=sqrt((double)sim/B.getNbPoints());
				//std::cout << "Similarity: " << sim << "\n";
				if(sim<=param)
					{
					se.addPoint(q-p);
					}
				}
				}
}

template <class T>
void dynamicSeNormL2Rand(Image <T> &img, const Point <TCoord> &p, const FlatSE &B, int param, 
FlatSE &se, int nbPoints)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	se.clear();
	
	//Scan entire image to compute the similarity measure between p and each q
	//If similarity measure is lesser or equal to param, we include the point in the se
	
	//Generates nbPoints random points around p
	vector <Point <TCoord> > nghb;
	for(int i=0; i<nbPoints; i++)
		{
		int x=rand()%dx;
		int y=rand()%dy;
	
		Point <TCoord> q(x,y,0);
		nghb.push_back(q);
		}
	
	for(int i=0; i<nghb.size(); i++)
				{
				double sim=0;
				Point <TCoord> q(nghb[i].x,nghb[i].y,0);
				
				for(int i=0; i<B.getNbPoints(); i++)
					{
					Point <TCoord> pn=p+B.getPoint(i);
					Point <TCoord> qn=q+B.getPoint(i);
					if(img.isPosValid(pn) && img.isPosValid(qn)  )
						{
						double value=img(qn)-img(pn);
						value*=value;
						sim+=value;
						}
					}
				sim=sqrt((double)sim/B.getNbPoints());

				if(sim<=param)
					{
					se.addPoint(q-p);
					}
				
				}
}

template <class T>
void dynamicSeNormL2NPoints(Image <T> &img, const Point <TCoord> &p, const FlatSE &B, int NPoints, FlatSE &se)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	se.clear();
	
	//Scan entire image to compute the similarity measure between p and each q
	//Here we define the number of points we want to include in the structuring element
	//So we scan the neighborhood B in order to find the NPoints most similar points
	
	int NbIncludedPoints=0;
	map <double, Point <TCoord> > vectorPoints;
	
	//Scan a neighborhood of p
	for(int y=-10; y<=10; y++)
		for(int x=-10; x<=10;x++)
			{
			
			Point <TCoord> q(p.x+x,p.y+y,0);
			
			//If the neighbor q is in the domain
			if(img.isPosValid(q))
			{
			double sim=0;
			//For each point of the ball B sum the similarity value
			for(int i=0; i<B.getNbPoints(); i++)
				{
				Point <TCoord> pn=p+B.getPoint(i);
				Point <TCoord> qn=q+B.getPoint(i);
				if(img.isPosValid(pn) && img.isPosValid(qn)  )
					{
					double value=img(qn)-img(pn);
					value*=value;
					sim+=value;
					}
				}
			//Does not require normalization as we keep the smallest values
			vectorPoints[sim]=q;
			}
			}
	//Now keep the NbPoints most similar points
			for(std::map<double,Point <TCoord> >::iterator it=vectorPoints.begin(); it!= vectorPoints.end(); it++)
				{
				Point <TCoord> q=it->second;
				se.addPoint(q-p);
				if(NbIncludedPoints==NPoints)
					break;
				NbIncludedPoints++;
				}						
			
}


///Special function to compute the context given 
///Compute an image giving on each point a uchar value resuming the context of
///the point (configuration of neighborhood) based on the order of the neighbors
template <class T>
Image <unsigned char> computeNeighborhoodS1(const Image <T> &im)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <unsigned char> res(im.getSize() );
	unsigned char context;
	
	for(int z=0; z<dz; z++)	
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				Point <TCoord> p(x,y,z);
				T value=im(p);
				context=0;
				
				Point <TCoord> N(0,-1);
				Point <TCoord> S(0,1);
				Point <TCoord> W(-1,0);
				Point <TCoord> E(1,0);
	
				Point <TCoord> NW(-1,-1);
				Point <TCoord> NE(1,-1);
				Point <TCoord> SW(-1,1);
				Point <TCoord> SE(1,1);
				if(im.isPosValid(im(N+p)))
				if(im(N+p)<value)
					{
					context+=1;
					}
				if(im.isPosValid(im(S+p)))
				if(im(S+p)<value)
					{
					context+=2;
					}
				if(im.isPosValid(im(W+p)))
				if(im(W+p)<value)
					{
					context+=4;
					}
				if(im.isPosValid(im(E+p)))
				if(im(E+p)<value)
					{
					context+=8;
					}
				if(im.isPosValid(im(NW+p)))
				if(im(NW+p)<value)
					{
					context+=16;
					}
				if(im.isPosValid(im(NE+p)))
				if(im(NE+p)<value)
					{
					context+=32;
					}
				if(im.isPosValid(im(SW+p)))
				if(im(SW+p)<value)
					{
					context+=64;
					}
				if(im.isPosValid(im(SE+p)))
				if(im(SE+p)<value)
					{
					context+=128;
					}
					
				res(p)=context;
				}
				
				
	return res;
}


///The latter method seems to perform badly...
///Same thing as before but with vectors

template <class T>
Image <vector <bool> > computeNeighborhoodS1v2(const Image <T> &im)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <vector <bool> > res(im.getSize() );
	
	for(int i=0; i<res.getBufSize(); i++)
		{
		res(i).resize(8,0);
		}
	
	for(int z=0; z<dz; z++)	
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				Point <TCoord> p(x,y,z);
				T value=im(p);
				unsigned char context=0;
				
				Point <TCoord> N(0,-1);
				Point <TCoord> S(0,1);
				Point <TCoord> W(-1,0);
				Point <TCoord> E(1,0);
	
				Point <TCoord> NW(-1,-1);
				Point <TCoord> NE(1,-1);
				Point <TCoord> SW(-1,1);
				Point <TCoord> SE(1,1);
				if(im.isPosValid(im(N+p)))
				if(im(N+p)<value)
					{
					res(p)[0]=1;
					}
				if(im.isPosValid(im(S+p)))
				if(im(S+p)<value)
					{
					res(p)[1]=1;
					}
				if(im.isPosValid(im(W+p)))
				if(im(W+p)<value)
					{
					res(p)[2]=1;
					}
				if(im.isPosValid(im(E+p)))
				if(im(E+p)<value)
					{
					res(p)[3]=1;
					}
				if(im.isPosValid(im(NW+p)))
				if(im(NW+p)<value)
					{
					res(p)[4]=1;
					}
				if(im.isPosValid(im(NE+p)))
				if(im(NE+p)<value)
					{
					res(p)[5]=1;
					}
				if(im.isPosValid(im(SW+p)))
				if(im(SW+p)<value)
					{
					res(p)[6]=1;
					}
				if(im.isPosValid(im(SE+p)))
				if(im(SE+p)<value)
					{
					res(p)[7]=1;
					}
				}
				
				
	return res;
}

template <class T>
void dynamicSeS1(Image <T> &img, const Point <TCoord> &p,  int param, FlatSE &se)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	se.clear();
	
	//Scan entire image to compute the similarity measure between p and each q
	//If similarity measure is lesser or equal to param, we include the point in the se
	//Here similarity is based on the 8-neighborhood of p
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				
				Point <TCoord> q(x,y,z);
				//q.print();
				if(img.isPosValid(q))
				{
				if(img(p)==img(q))
					{
					se.addPoint(q-p);
					}
				}
				}
}

inline void dynamicSeS1v2(Image <vector <bool> > &img, const Point <TCoord> &p,  int param, FlatSE &se)
{
	
	se.clear();
	
	//Scan entire image to compute the similarity measure between p and each q
	//If similarity measure is lesser or equal to param, we include the point in the se
	//Here similarity is based on the 8-neighborhood of p
	vector <bool> pContext=img(p);
	
		for(int y=-10; y<=10; y++)
			for(int x=-10; x<=10; x++)
				{
				Point <TCoord> q(x+p.x,y+p.y,0);
				//q.print();
				if(img.isPosValid(q))
				{
				vector <bool> qContext=img(q);
				double sim=0;
				for(int i=0; i<8; i++)
					{
					sim+=abs(pContext[i]-qContext[i]);
					}
				
				if(sim<param)
					{
					se.addPoint(q-p);
					}
				}
				}
}

template <class T>
Image <T> printSeAtPoint(Image <T> &img, FlatSE &se, Point <TCoord> &p)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	Image <T> res=img;
	
	T maxValue=std::numeric_limits<T>::max();
	
	for(int i=0; i<se.getNbPoints(); i++)
		{
		Point <TCoord> q=p+se.getPoint(i);
		if(img.isPosValid(q))
			{
			res(q)=maxValue;
			}
		}
		
	return res;
}

}
