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

#include "Common/OrderedQueue.h"

namespace LibTIM {

/** \defgroup watershed Watershed-based Algorithms 
	\ingroup Morpho
**/

/*@{*/

///Meyer's watershed algorithm
/**
	Watershed algorithm. (Meyer's definition based on
	the topographic distance).
	@param img Source image (type T)
	@param marker Marker image (type TLabel)
	@param se The connexity used 
	@param observe If you want to trace the result (for this you must create
	a directory "Anims") (facultative) 
**/

template <class T>
void watershedMeyer(Image <T> &img, Image <TLabel> &marker, FlatSE &se, bool observe=false)
{
	OrderedQueue <TOffset> oq;
	
	Image <T> imBorder=img;
	Image <TLabel> markerBorder=marker;
	
	typename Image<TLabel>::iteratorXYZ it;
	typename Image<TLabel>::iteratorXYZ end=marker.end();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T maxValueT=std::numeric_limits<T>::max();
	TLabel maxValueTLabel=std::numeric_limits<TLabel>::max();

	//Now enlarge the images
	addBorders(imBorder,back,front,maxValueT);
	addBorders(markerBorder,back,front, maxValueTLabel);
	TOffset imageSize=imBorder.getBufSize();	
	se.setContext(imBorder.getSize());
	
	//Put the markers in the queue
	//For this, we scan the inner part of marker (without the borders)
	TOffset offset=0;
	for(it=marker.begin(); it!=end; ++it,offset++)
		{
		if(*it!=TLabel(0))
			{
			int value=(int)img(offset);
			//compute the offset in the border image
			TOffset offsetBorder=it.x+back[0]+(it.y+back[1])*markerBorder.getSizeX()+
			(it.z+back[2])*markerBorder.getSizeX()*markerBorder.getSizeY();
			oq.put(value,offsetBorder);
			}
		}
	
	
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	int iter=0;
	int nbFrame=0;
	while(!oq.empty())
		{
		TOffset p=oq.get();
		
		iter++;
		if(observe)
			if(iter%(imageSize/100) == 0) {nbFrame++; 
			char name[256]; sprintf(name,"Anims/watershedMeyer_anim_%i.pgm",nbFrame);
			std::cout << "Writing: " << name << "\n";
			Image <unsigned char> tmp=markerBorder;
			tmp.save(name);
			}
			
		for(itSe=se.begin(); itSe!=endSe; ++itSe)
			{
			TOffset q = p + *itSe;
			
			if(markerBorder(q)==TLabel(0))
				{
				markerBorder(q)=markerBorder(p);
				oq.put((int)imBorder(q),q);
				}
			}			
		}
	//reCrop the marker resulting image
	for(it=marker.begin(); it!=end; ++it)
		*it=markerBorder(it.x+back[0],it.y+back[1],it.z+back[2]);
}

/*@}*/
}
