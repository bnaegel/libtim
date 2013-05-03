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

template <>
inline void NonFlatSE<U8>::makeChamfer2D()
{
	this->clear();
	
	Point<TCoord>  N(0,-1);
	Point<TCoord>  S(0,1);
	Point<TCoord>  W(-1,0);
	Point<TCoord>  E(1,0);
	
	Point<TCoord>  NW(-1,-1);
	Point<TCoord>  NE(1,-1);
	Point<TCoord>  SW(-1,1);
	Point<TCoord>  SE(1,1);
	
	Point<TCoord>  O(0,0);
	
	this->addPoint(N,1);
	this->addPoint(S,1);
	this->addPoint(W,1);
	this->addPoint(E,1);
	this->addPoint(NW,1);
	this->addPoint(NE,1);
	this->addPoint(SW,1);
	this->addPoint(SE,1);
	this->addPoint(O,1);
}

template <class T>
NonFlatSE<T> NonFlatSE<T>::rasterScan()
{	
	NonFlatSE<T> res;
	for(int i=0; i< getNbPoints(); i++)
		{
		Point<TCoord> tmp=this->getPoint(i);
		if(tmp.z<0 || (tmp.z==0 && tmp.y<0) || 
		(tmp.z==0 && tmp.y==0 && tmp.x < 0) )
			res.addPoint(tmp,getValue(i));
		}
	
	return res;
}

template <class T>
NonFlatSE<T> NonFlatSE<T>::antiRasterScan()
{
	NonFlatSE<T> res;
	for(int i=0; i< getNbPoints(); i++)
		{
		Point<TCoord> tmp=getPoint(i);
		if(tmp.z>0 || (tmp.z==0 && tmp.y>0) || 
			(tmp.z==0 && tmp.y==0 && tmp.x > 0) )
			res.addPoint(tmp,getValue(i));
		}
	
	return res;

}

template <class T>
double NonFlatSE<T>::getNorm() const
{	
	int res=0;
	for(int i=0; i<getNbPoints(); i++)
		{
		res+=this->getValue(i)*this->getValue(i);
		}
		
	return sqrt((double)res);
}

}
