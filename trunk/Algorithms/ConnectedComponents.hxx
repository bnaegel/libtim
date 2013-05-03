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

#include <queue>
#include <map>
#include "Algorithms/Morphology.h"



namespace LibTIM{

/** \defgroup ccLabelling Connected Components Labelling
/** \ingroup Morpho 
**/
/*@{*/

///Labelisation of connected components 
///img is considered as a binary image with two values: foreground >0 and background = 0

template <class T>
Image <TLabel> labelConnectedComponents(Image <T> &img, FlatSE &se)
{
	std::queue<TOffset> fifo;
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	TLabel BORDER=-1;
	
	Image<T> imBorder=img;
	Image<TLabel> resBorder(img.getSize() );
	resBorder.fill(0);
		
	addBorders(imBorder,back,front,T(0));
	addBorders(resBorder,back,front,BORDER);
	
	se.setContext(imBorder.getSize());
	
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=imBorder.end();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	TLabel currentLabel=1;
	
	TOffset curOffset=0;
	for(it=imBorder.begin(); it!=end; ++it,curOffset++)
		{
		if(*it>0 && resBorder(curOffset) ==0)
			{
			fifo.push(curOffset);
			
			while(!fifo.empty())
				{
				TOffset p=fifo.front();
				fifo.pop();
		
				resBorder(p)=currentLabel;

				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p+ *itSe;
					
					if(imBorder(q)>0 && resBorder(q)==0)
						{
						resBorder(q)=currentLabel;
						fifo.push(q);
						}
					}
				}
			
			currentLabel++;

			}
 		}
		
	Image<TLabel> result(img.getSize());
	typename Image<TLabel>::iteratorXYZ itLabelXYZ;
	typename Image<TLabel>::iteratorXYZ endRes=result.end();
	for(itLabelXYZ=result.begin(); itLabelXYZ!=endRes; ++itLabelXYZ)
		{
		*itLabelXYZ=resBorder(itLabelXYZ.x+back[0], itLabelXYZ.y+back[1], itLabelXYZ.z+back[2]);
		}
		
	return result;
}

///Sort the connected components by their size and keep only the iest largest one(s)
/// Largest one = 1 (not 0)
 
inline void keepIestLargestComponent(Image <TLabel> &img, FlatSE &se, int Iest)
{
	///Map CC number to its size
	typedef std::map <TLabel, int> mapCCType;
	mapCCType connectedComponentsMap;
	
	///Map CC size to its corresponding label
	///Multiple CC can have the same size so we use multimap
	
	typedef std::multimap<int, TLabel> sortedMapCCType;
	sortedMapCCType sortedCC;
	
	//Labels to keep in img
	std::vector <TLabel> keepLabel;
	
	Image<TLabel>::iterator it;
	Image<TLabel>::iterator end=img.end();
	
	for(it=img.begin(); it!=end; ++it)
		connectedComponentsMap[*it]++;
	
	mapCCType::iterator itMap;
	mapCCType::iterator endMap=connectedComponentsMap.end();
	
	for(itMap=connectedComponentsMap.begin(); itMap!=endMap; ++itMap)
		{
		sortedCC.insert(std::make_pair(itMap->second,itMap->first));
		}
	
	//Find the iest largest CC (0=background)
	int currentIest=0;
	int oldSize=sortedCC.rbegin()->first;
	int currentSize;
	
	sortedMapCCType::reverse_iterator rIt;
	sortedMapCCType::reverse_iterator endR=sortedCC.rend();
	
	for( rIt=sortedCC.rbegin(); rIt!=endR; ++rIt)
		{
		currentSize=rIt->first;
		if(currentSize!=oldSize) currentIest++;
		
		if(currentIest==Iest)
		 	{
			 break;
			}
		oldSize=currentSize;
		}
	if(rIt!=sortedCC.rend())
		{
	
		//Find if there is multiple label for that size (very rare...)
		//If yes we keep all the corresponding label
	
		typedef sortedMapCCType::iterator ITM;
	
		std::pair<ITM,ITM> p=sortedCC.equal_range(currentSize);
	
	for(ITM itM=p.first; itM!=p.second; ++itM)
		{
		keepLabel.push_back(itM->second);
		}
	
	std::vector<TLabel>::iterator itV;
	std::vector<TLabel>::iterator endV=keepLabel.end();
		
	for(it=img.begin(); it!=end; ++it)	
		{
		bool keepCC=false;
		for(itV=keepLabel.begin(); itV!=endV; ++itV)
			if(*it==*itV) {keepCC=true; break;}
		if(keepCC!=true) *it=TLabel(0);
		}
	}
}

/** Keep the CC pointed by the point x. Equivalent to the \gamma_x opening of Serra
 *
**/
inline Image <TLabel> pointConnectedOpening(Image <TLabel> &img, Point <TCoord> &x, FlatSE &se)
{
	Image <TLabel> res=img;
	res.fill(TLabel(0));
	
	std::queue<Point<TCoord> > fifo;
	
	fifo.push(x);
	res(x)=img(x);
	
	FlatSE::iterator_point it;
	FlatSE::iterator_point end=se.end_point();
	
	while(!fifo.empty() )
		{
		Point <TCoord> p=fifo.front();
		fifo.pop();
		for(it=se.begin_point(); it!=end; ++it)
			{
			Point <TCoord> q=p+*it;
			if(img.isPosValid(q))
				if(img(q)==img(p) && res(q)==TLabel(0) )
					{
					fifo.push(q);
					res(q)=img(q);
					}
			}
		}
	return res;
}
	
/*}@*/
}

