/*
 * This file is part of libTIM.
 *
 * Copyright (©) 2005-20013  Benoit Naegel
 * Copyright (©) 20013 Theo de Carpentier
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

//Meyer's algorithm for watershed computation
//This algorithm is based on the topographical distance and is very simple
//Image src should be a gradient image
//Image marker contains the markers or seeds of the algorithm (each marker being uniquely labeled, 0=foreground)


//Version with image borders: we should define the addborders method in the GImage class
//This way we could avoid the multiple copies of the image

// template <class T, class T2>
// void WatershedMeyer(GImage <T> &src, GImage <T2> &marker, GFlatSE &se)
// {
// 	GOrderedQueue <long> oq;
// 	
// 	int dx=src.getSizeX();
// 	int dy=src.getSizeY();
// 	int dz=src.getSizeZ();
// 	
// 	int *negativeOffsets = se.getNegativeOffsets();
// 	int *positiveOffsets = se.getPositiveOffsets();
// 
// 	
// 	GImage<T> srcBorder = addBorders(src, negativeOffsets, positiveOffsets, maxValue<T>(0));
// 	GImage<T2> markerBorder = addBorders(marker, negativeOffsets, positiveOffsets, maxValue<T2>(0));
// 	
// 	se.setContext(srcBorder.getSize());
// 
// 	//Initialization: put all markers in the queue
// 	for(int i=0; i<dx; i++ )
// 		for(int j=0; j<dy; j++ )
// 			for(int k=0; k<dz; k++)
// 				{
// 				int offset = markerBorder.getOffset(i + negativeOffsets[0],
// 					                            j + negativeOffsets[1],
// 					                            k + negativeOffsets[2]);
// 
// 				if(markerBorder(offset)!=T2(0))
// 					oq.put(src(offset),offset);
// 				}
// 
// 	while(!oq.empty())
// 		{
// 		long int p=oq.get();
// 		
// 		for(int i=0; i<se.getNbPoints(); i++)
// 					{
// 					long int q = p + se.getOffset(i);
// 					
// 					if(markerBorder(q)==T(0))
// 						{
// 						markerBorder(q)=markerBorder(p);
// 						oq.put(srcBorder(q),q);
// 						}
// 					}			
// 		}
// 	
// 	//TODO: Here remove the borders
// 	
// }

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


//  template <class T, class T2>
// int computePriority(GPoint &q, GImage <T> &src,  GImage <T2> &coverMap, GFlatSE &se)
// {
// 	int dx=src.getSizeX();
// 	int dy=src.getSizeY();
// 	int dz=src.getSizeZ();
// 	
// 	double priority=0;
// 	int numberPixels=0;
// 	
// 	for(int i=0; i< se.getNbPoints(); i++)
// 		{
// 		GPoint r = q + se.getPoint(i);
// 		if(src.isPosValid(r))
// 			if(coverMap(r)==T2(0))
// 				{
// 				priority+=src(r);
// 				numberPixels++;
// 				}
// 		}
// 	
// 	return (int)(priority/numberPixels);
// }
// 
// ///This is an alternative version of Richard Beare's algorithm: priority is computed 
// ///on a constant neighborhood (independant of the cover map)
// template <class T, class T2>
// void ConstrainedWatershed(GImage <T> &src, GImage <T2> &marker, GFlatSE &se, bool observe=false)
// {
// 	GOrderedQueue < std::pair<GPoint, TLabel> > oq;
// 	GFlatSE nghb;
// 	nghb.make2DN8();
// 	
// 	int dx=src.getSizeX();
// 	int dy=src.getSizeY();
// 	int dz=src.getSizeZ();
// 	
// 	GImage <T2> coverMap=marker;
// 	GImage <bool> isActive(src.getSize());
// 	isActive.fill(true);
// 	
// 	//Initialization: put all markers in the queue
// 	//We could insert according to the priority function
// 	for(int i=0; i<dx; i++ )
// 		for(int j=0; j<dy; j++ )
// 			for(int k=0; k<dz; k++)
// 				{
// 				if(marker(i,j,k)!=T(0))
// 					{
// 					std::pair <GPoint, TLabel> el;
// 					el.first=GPoint(i,j,k);
// 					el.second=marker(i,j,k);
// 					
// 					oq.put(src(i,j,k),el);
// 					}
// 				}
// 	int iter=0;
// 	int nbFrame=0;
// 	
// 	GImage <TLabel> test(coverMap);
// 	test.fill(0);
// 	
// 	while(!oq.empty())
// 		{
// 		iter++;
// 		std::pair <GPoint, int> el=oq.get();
// 		GPoint p=el.first;
// 		marker(p)=el.second;
// 		if(coverMap(p)==T(0))
// 			coverMap(p)=el.second;
// 		for(int j=0; j<se.getNbPoints(); j++)
// 							{
// 							GPoint q = p + se.getPoint(j);
// 							if(coverMap.isPosValid(q) && coverMap(q)==T2(0))
// 								coverMap(q)=coverMap(p);
// 							}
// 		//p.print();
// 		
// 		//If observe, generate animation
// 		if(observe)
// 			if(iter%(dx*dy*dz/100) == 0) {nbFrame++; 
// 			char name[256]; sprintf(name,"Anims/coverMap_anim_%i.pgm",nbFrame);
// 			std::cout << "Writing: " << name << "\n";
// 			GImage <unsigned char> tmp(src.getSize());
// 			for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)coverMap(i);
// 			tmp.save(name);
// 			 sprintf(name,"Anims/marker_anim_%i.pgm",nbFrame);
// 			std::cout << "Writing: " << name << "\n";
// 			
// 			for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)marker(i);
// 			tmp.save(name);
// 			}
// 		///
// 		
// 		for(int i=0; i<nghb.getNbPoints(); i++)
// 					{
// 					GPoint q = p + nghb.getPoint(i);
// 					
// 					if(marker.isPosValid(q))
// 						if(marker(q)==T(0) && isActive(q)==true)
// 							{
// 							std::pair <GPoint, TLabel > elNew;
// 							elNew.first=q;
// 							elNew.second=marker(p);
// 							
// 							int qPriority=computePriority(q, src,  test, se);
// 							//std::cout << "Priority: " << qPriority << "\n";
// 							oq.put(qPriority,elNew);
// 							isActive(q)=false;
// 							// //dilation of coverMap: labelize all neighbors of q
// // 							for(int j=0; j<se.getNbPoints(); j++)
// // 								{
// // 								GPoint r = q + se.getPoint(j);
// // 								if(coverMap.isPosValid(r))
// // 									coverMap(r)=coverMap(q);
// // 								}
// 							}
// 					}			
// 		}
// 	//marker.save("testCW.pgm");
// 	marker=coverMap;
// }
// 
// 
// ///This is an implementation of the algorithm of Richard Beare described in ISMM'05 page 217
// 
// template <class T, class T2>
// void ConstrainedWatershed2(GImage <T> &src, GImage <T2> &marker, GFlatSE &se, bool observe=false)
// {
// 	GOrderedQueue < std::pair<GPoint, TLabel> > oq;
// 	GFlatSE nghb;
// 	nghb.make2DN8();
// 	
// 	int dx=src.getSizeX();
// 	int dy=src.getSizeY();
// 	int dz=src.getSizeZ();
// 	
// 	GImage <T2> coverMap=marker;
// 	GImage <bool> isActive(src.getSize());
// 	isActive.fill(true);
// 	
// 	//Initialization: put all markers in the queue
// 	//We could insert according to the priority function
// 	for(int i=0; i<dx; i++ )
// 		for(int j=0; j<dy; j++ )
// 			for(int k=0; k<dz; k++)
// 				{
// 				if(marker(i,j,k)!=T(0))
// 					{
// 					std::pair <GPoint, TLabel> el;
// 					el.first=GPoint(i,j,k);
// 					el.second=marker(i,j,k);
// 					
// 					oq.put(src(i,j,k),el);
// 					}
// 				}
// 	int iter=0;
// 	int nbFrame=0;
// 	
// 	GImage <TLabel> test(coverMap);
// 	test.fill(0);
// 	
// 	while(!oq.empty())
// 		{
// 		iter++;
// 		std::pair <GPoint, int> el=oq.get();
// 		GPoint p=el.first;
// 		marker(p)=el.second;
// 		if(coverMap(p)==T(0))
// 			coverMap(p)=el.second;
// 		for(int j=0; j<se.getNbPoints(); j++)
// 							{
// 							GPoint q = p + se.getPoint(j);
// 							if(coverMap.isPosValid(q) && coverMap(q)==T2(0))
// 								coverMap(q)=coverMap(p);
// 							}
// 		//p.print();
// 		
// 		//If observe, generate animation
// 		if(observe)
// 			if(iter%(dx*dy*dz/100) == 0) {nbFrame++; 
// 			char name[256]; sprintf(name,"Anims/coverMap_anim_%i.pgm",nbFrame);
// 			std::cout << "Writing: " << name << "\n";
// 			GImage <unsigned char> tmp(src.getSize());
// 			for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)coverMap(i);
// 			tmp.save(name);
// 			 sprintf(name,"Anims/marker_anim_%i.pgm",nbFrame);
// 			std::cout << "Writing: " << name << "\n";
// 			
// 			for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)marker(i);
// 			tmp.save(name);
// 			}
// 		///
// 		
// 		for(int i=0; i<nghb.getNbPoints(); i++)
// 					{
// 					GPoint q = p + nghb.getPoint(i);
// 					
// 					if(marker.isPosValid(q))
// 						if(marker(q)==T(0) && isActive(q)==true)
// 							{
// 							std::pair <GPoint, TLabel > elNew;
// 							elNew.first=q;
// 							elNew.second=marker(p);
// 							
// 							int qPriority=computePriority(q, src,  coverMap, se);
// 							//std::cout << "Priority: " << qPriority << "\n";
// 							oq.put(qPriority,elNew);
// 							isActive(q)=false;
// 							// //dilation of coverMap: labelize all neighbors of q
// // 							for(int j=0; j<se.getNbPoints(); j++)
// // 								{
// // 								GPoint r = q + se.getPoint(j);
// // 								if(coverMap.isPosValid(r))
// // 									coverMap(r)=coverMap(q);
// // 								}
// 							}
// 					}			
// 		}
// 	//marker.save("testCW.pgm");
// 	marker=coverMap;
// }

// template <class T, class T2>
// void ConstrainedWatershedObserver(GImage <T> &src, GImage <T2> &marker, GFlatSE &se)
// {
// 	GOrderedQueue < std::pair<GPoint, TLabel> > oq;
// 	GFlatSE nghb;
// 	nghb.make2DN4();
// 	
// 	int dx=src.getSizeX();
// 	int dy=src.getSizeY();
// 	int dz=src.getSizeZ();
// 	
// 	GImage <T2> coverMap=marker;
// 	GImage <bool> isActive(src.getSize());
// 	isActive.fill(true);
// 	
// 	//Initialization: put all markers in the queue
// 	//We could insert according to the priority function
// 	for(int i=0; i<dx; i++ )
// 		for(int j=0; j<dy; j++ )
// 			for(int k=0; k<dz; k++)
// 				{
// 				if(marker(i,j,k)!=T(0))
// 					{
// 					std::pair <GPoint, TLabel> el;
// 					el.first=GPoint(i,j,k);
// 					el.second=marker(i,j,k);
// 					
// 					oq.put(src(i,j,k),el);
// 				
// 					}
// 				}
// 
// 	
// 	
// 	int iter=0;
// 	int nbFrame=1;
// 	
// 	GImage<unsigned char> tmp(coverMap.getSize());
// 	while(!oq.empty())
// 		{
// 		std::pair <GPoint, int> el=oq.get();
// 		GPoint p=el.first;
// 		marker(p)=el.second;
// 		if(coverMap(p)==0) coverMap(p)=el.second;
// 		for(int j=0; j<se.getNbPoints(); j++)
// 							{
// 							GPoint q = p + se.getPoint(j);
// 							if(coverMap.isPosValid(q) && coverMap(q)==0)
// 								coverMap(q)=coverMap(p);
// 							}
// 		//p.print();
// 		
// 		if(iter%2000 == 0) {nbFrame++; 
// 			char name[256]; sprintf(name,"Anims/anim_%i.pgm",nbFrame);
// 			std::cout << name << "\n";
// 			for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)coverMap(i);
// 			tmp.save(name);
// 			}
// 		iter++;
// 		for(int i=0; i<nghb.getNbPoints(); i++)
// 					{
// 					GPoint q = p + nghb.getPoint(i);
// 					
// 					if(marker.isPosValid(q))
// 						if(marker(q)==T(0) && isActive(q)==true)
// 							{
// 							std::pair <GPoint, TLabel > elNew;
// 							elNew.first=q;
// 							elNew.second=marker(p);
// 							int qPriority=computePriority(q, src,  coverMap, se);
// 							//std::cout << "Priority: " << qPriority << "\n";
// 							oq.put(qPriority,elNew);
// 							isActive(q)=false;
// 							// //dilation of coverMap: labelize all neighbors of q
// // 							for(int j=0; j<se.getNbPoints(); j++)
// // 								{
// // 								GPoint r = q + se.getPoint(j);
// // 								if(coverMap.isPosValid(r))
// // 									coverMap(r)=coverMap(q);
// // 								}
// 							}
// 					}			
// 		}
// 	//marker.save("testCW.pgm");
// 	marker=coverMap;
// }

/*@}*/
}
