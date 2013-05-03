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

#include "Common/FlatSE.h"
#include "Common/Image.h"
#include "Algorithms/Misc.h"

#include <list>

namespace LibTIM {

using std::pair;

struct Region
	{
	long int *sumIntensity;
	int *nbPoints;
	};

inline int labelToOffset(TLabel label)
{
	return (int)(label-1);
}

inline double computePriority(pair <long int, int> offset, Image <U8> &src, struct Region  &region)
{
	assert(region.nbPoints[labelToOffset(offset.second)]!=0);
	double moy=(double)region.sumIntensity[labelToOffset(offset.second)]/region.nbPoints[labelToOffset(offset.second)];
	double prio=fabs(src(offset.first)-moy);
	return prio;
}

/** \defgroup regionGrowing Region Growing Algorithms
	\ingroup Morpho
**/

/*@{*/

template <class T, class T2>
void RegionGrowingCriterion(Image <T> &src, Image <T2> &marker, FlatSE &se, bool observe=false)
{
	int bufferSize=src.getBufSize();
	TSize *imageSize=src.getSize();
	int seedsNumber=marker.getMax();
	se.setContext(imageSize);
	using std::multimap;
	
	Image <U8> tmp=src;
	
	int dx=src.getSizeX();
	int dy=src.getSizeY();
	int dz=src.getSizeZ();
	
	int max=std::numeric_limits<int>::max();
	int min=std::numeric_limits<int>::min();
	
	bool *processedPoints=new bool [bufferSize];
	bool *inSet=new bool [bufferSize];
	Image <T2> *regionSet=new Image <T2> [seedsNumber]; 
	multimap <long int, int > neighborSet;
	multimap <long int, int >::iterator itSet;
	
	struct Region sRegion;
	sRegion.sumIntensity=new long int[seedsNumber];
	sRegion.nbPoints=new int [seedsNumber];
	
	//Init all the regions 
	for(int i=0; i<seedsNumber; i++)
		{
		regionSet[i].setSize(imageSize);
		sRegion.sumIntensity[i]=0;
		sRegion.nbPoints[i]=0;
		}
		
	for(int i=0; i<bufferSize; i++)
		{
		TLabel label=marker(i);
		for(int j=0; j<seedsNumber; j++)
			{
			regionSet[j](i)=0;
			}
		inSet[i]=false;
		if(label!=T2(0) )
			{
			regionSet[labelToOffset(label)](i)=label;
			processedPoints[i]=true;
			sRegion.sumIntensity[labelToOffset(label)]+=src(i);
			sRegion.nbPoints[labelToOffset(label)]++;
			}
		else
			processedPoints[i]=false;		
		}
	
		std::set <TLabel> nLabel;
		for(int z=0; z<dz; z++)
			for(int y=0; y<dy; y++)
				for(int x=0; x<dx; x++)
					{
					Point <TCoord> p(x,y,z);
					bool neighbor=false;
					if(marker.isPosValid(x,y,z) )
						{
						if(marker(p)==T2(0) )
							{
							for(int i=0; i<se.getNbPoints(); i++)
								{
								Point<TCoord> q=p+se.getPoint(i);
								if(marker(q)!=T2(0) )
									{
									nLabel.insert(marker(q));
									neighbor=true;
									}
								}
							if(neighbor==true)
								{
								int offset=z*dx*dy+y*dx+x;
								//Using set allows the neighbors label to be inserted once in the multimap neighborSet
								for(std::set<TLabel>::iterator it=nLabel.begin(); it!=nLabel.end(); ++it)
									neighborSet.insert(std::make_pair(offset,*it));
								inSet[offset]=true;
								}
							}
						}
					}
	//neighborSet contains the set of neighbors of all regions (seeds)
	bool stop=false;
	int iter=0;
	int nbFrame=0;

	while(!stop)
	{
	iter++;

	//we search for the neighbor having the maximum priority (here least)
	pair <long int, int> bestOffset;
	std::vector <pair <long int ,int> > bestOffsetList;
	double minPrio;
	double currentPrio;

	minPrio=std::numeric_limits<double>::max();
	bestOffsetList.clear();
	for(itSet=neighborSet.begin(); itSet!=neighborSet.end(); ++itSet)
		{
		pair <long int, int> currentOffset=*itSet;
		currentPrio=computePriority(currentOffset, src,sRegion);

		if(fabs(currentPrio-minPrio)<0.0000001)
			{
			bestOffsetList.push_back(currentOffset);
			}
		else if(currentPrio<minPrio) 
			{
			bestOffsetList.clear();
			minPrio=currentPrio;
			bestOffsetList.push_back(currentOffset);
			}
		}
	std::vector <pair <long int ,int> >::iterator itO;
	if(bestOffsetList.size()>1) std::cout <<iter << " Egalit� entre plusieurs points!!!!\n";
	for(itO=bestOffsetList.begin(); itO!=bestOffsetList.end(); ++itO)
		{
		//process the point(s)
		//first erase it from the set of neighbors
		neighborSet.erase(itO->first);
		//update the marker image
		marker(itO->first)=itO->second;
		//update the region attributes
		sRegion.sumIntensity[labelToOffset(itO->second)]+=src(itO->first);
		sRegion.nbPoints[labelToOffset(itO->second)]++;
		
		//mark the point as processed (never process it again)
		processedPoints[itO->first]=true;
		}
	//second pass: add neighbors
	for(itO=bestOffsetList.begin(); itO!=bestOffsetList.end(); ++itO)
		{
		for(int i=0; i<se.getNbPoints(); i++)
			{
			long int qo=itO->first + se.getOffset(i);
			if(src.isPosValid(qo))
				if( processedPoints[qo]==false )
				{
				typedef std::multimap<long int, int>::const_iterator I;
				pair <I,I> nOffset=neighborSet.equal_range(itO->first);
				bool isInserted=false;
				for(I it=nOffset.first; it!=nOffset.second; ++it)
					{
					if(marker(itO->first)==it->second)
						{
						isInserted=true;
						break;
						}
					}
				if(!isInserted)
					neighborSet.insert(std::make_pair(qo,marker(itO->first)));
				inSet[qo]=true;
				}
			}
		}
	
	//If observe, generate animation
	if(observe)
		{nbFrame++; 
		std::stringstream s;
		s.width(3);
		s.fill('0');
		s << nbFrame;
		std::stringstream name;
		name << "Anims/RG_criterion_"<<s.str()<< ".pgm";

		for(int i=0; i<tmp.getBufSize(); i++) tmp(i)=(unsigned char)marker(i);
		adjustContrast(tmp);
		tmp.save(name.str().c_str() );
		}
	if(neighborSet.empty()==true) stop=true;
	}
	
	delete inSet;
	delete sRegion.sumIntensity;
	delete sRegion.nbPoints;
	delete processedPoints;
	delete[] regionSet;
}

/// Seeded region-growing algorithm: non-biased implementation
/**
Seeded region-growing (see works of Adams-Bischhof, Mehnert-Jackway, Salembier,...) is usually
implemented by using hierarchical queues containing points to be processed. Each point is
put in the queue with a priority given by some measure of distance.
In the first implementation (Adams-Bischhof, and Salembier seems to have the same implementation)
a point is put once in the queue with a fixed priority. That is to say that even if in the future
these points are found to have a lesser priority (because a neighbor having a similar grey-level is processed,
 for example), their priority is not updated.
 Mehnert-Jackway pointed out the two bias present in the SRG, and proposed an unbiased region-growing algorithms,  
 (ISRG= improved seeded region growing). However, they don't talk of the bias induced by the use
 of hierarchical queues without the recomputation of priorities.
 The function seededRegionGrowing() implements SRG with the recomputation of priorities (see function
 for details) based on HQ.
 The function implemented here don't use anymore HQ: it is based on the trivial formulation
 of RG. The aim is to see if there is any difference with the RG based on HQ with recomputation of
 priorities.
 That is why the function is called "ExactAlgorithm", because the result should be the reference.
 It is based on this trivial algorithm:
 	\li INIT: each region with the given seeds
 	\li 1) For each region: process sequentially the neighbors, compute the distance and keep the neighbor having
 	the least distance. STOP if there is no avalaible neighbors
 	\li	2) Aggregate the point with the corresponding region and recompute the region characteristics
 	\li 3) GOTO 1)
**/

template <class T>
void seededRegionGrowingExactAlgorithm(Image <T> &im, Image <TLabel> &marker, FlatSE &se, bool observe=false)
{
	Image <T> imBorder=im;
	Image <TLabel> markerBorder=marker;
	
	typename Image<TLabel>::iteratorXYZ it;
	typename Image<TLabel>::iteratorXYZ end=marker.end();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	TLabel BORDER=-1;
	
	addBorders(imBorder, se, T(0));
	addBorders(markerBorder, se, BORDER);
	Image<bool> imageContourBorder(markerBorder.getSize() );
	imageContourBorder.fill(false);
	se.setContext(markerBorder.getSize() );
	
	//Regions are stocked as lists of offsets in a map
	typedef std::list<TOffset> offsetsContainerType;
	typedef std::map<TLabel, offsetsContainerType > regionsType;
	regionsType regions;
	
	FlatSE::iterator_point itSePoint;
	FlatSE::iterator_point endSePoint=se.end_point();
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
	ImageRegionsInfos <T, TLabel> imageRegionsHelper(imBorder,markerBorder);
	
	int toto=0;
	int contour=0;
	//Put the contour of the region seeds in the regions
	for(it=marker.begin(); it!=end; ++it)
		{
		if(*it!=TLabel(0))
			{
			toto++;
			bool isContour=false;
			for(itSePoint=se.begin_point(); itSePoint!=endSePoint; ++itSePoint)
			{
				Point <TCoord> q=Point<TCoord> (it.x,it.y,it.z)+(*itSePoint);
				if(marker.isPosValid(q))
					if(marker(q)==0)
						isContour=true;
			}
			//Compute offset in the markerBorder
			TOffset offset=(it.x+back[0])+(it.y+back[1])*markerBorder.getSizeX()
			+(it.z+back[2])*markerBorder.getSizeX()*markerBorder.getSizeY();
			if(isContour) {	
				contour++;					
				imageContourBorder(offset)=true;
				//Put offset in the corresponding region
				regions[*it].push_back(offset);
			}
			
			imageRegionsHelper.setPoint(offset,*it);
			}
		}
	std::cout << "TOTO= " << toto << "\n";
	std::cout << "CONTOUR= " << contour << "\n";
	
	typename offsetsContainerType::iterator itC;
	
	typename regionsType::iterator itR;
	typename regionsType::iterator endR=regions.end();
	
	TOffset bestOffset;
	double bestDist;
	TLabel bestRegion;
	
	bool stop=false;
	
	int iter=0;
	
	std::queue<TOffset> eraseQueue;
	
	
	
	while(!stop) {
	int nPoints=0;
	bestDist=std::numeric_limits<double>::max();
	
	stop=true;
	// 1) Search for the "best" offset
	for(itR=regions.begin(); itR!=endR; ++itR)
		{
		//browse offsets
		for(itC=itR->second.begin(); itC!=itR->second.end(); ++itC)
			{
			nPoints++;

			TOffset p=*itC;
			if(imageContourBorder(*itC)==true) {
			bool isContour=false;
			for(itSe=se.begin(); itSe!=endSe; ++itSe)
				{
				TOffset q=p+ *itSe;
				if(markerBorder(q)==0)
					{
					stop=false;
					isContour=true;
					//Compute distance
					double dist=imageRegionsHelper.computeDistance( p , q);
					if(dist<bestDist)
						{
						bestDist=dist;
						bestOffset=q;
						bestRegion=markerBorder(p);
						}
					}
				}
			//Test to remove interior points
			// They will not be processed anymore
			if(!isContour)
				{
				
				imageContourBorder(p)=false;
				eraseQueue.push(p);

				}
			}
		}
		//Remove interior points
		while(!eraseQueue.empty() ) 
			{
			TOffset p=eraseQueue.front();
			eraseQueue.pop();
			itR->second.remove(p);
			}
		
	}

	//2) Put the best offset in the corresponding region
	
	if(!stop) 
		{
		if(markerBorder(bestOffset)==0) {
		if(iter%100==0)
		{std::cout << "On assigne un point! iter= " << iter << "\n";
		std::cout << "Number of points: " << nPoints << "\n";	}
		imageRegionsHelper.setPoint(bestOffset,bestRegion);
		iter++;}
		// Put it in the region contour
		// WARNING: we should remove its neighbors that are not anymore contour points (they are now "inside"
		// the region. The complexity may explode. That is why we add the "imageContourBorder" image
		
		regions[bestRegion].push_back(bestOffset);
		imageContourBorder(bestOffset)=true;;
		}
	}
	
	for(it=marker.begin(); it!=end; ++it)
		*it=markerBorder(it.x+back[0],it.y+back[1],it.z+back[2]);
		
}

///Seeded region growing algorithm.
/** Adaptation of the algorithm of Adams and Bischof (and similar algorithm proposed by Salembier) 
	with the following variations:
	Points can be inserted several times in the queue
	Each time a point is inserted, its neighbors are scanned and their priority is recomputed
	and eventually reinserted in the queue if their priority has been lowered
**/ 

template <class T, class T2>
void seededRegionGrowing(Image <T> &img, Image <T2> &marker, FlatSE &se, bool observe=false)
{
	OrderedQueueDouble <TOffset> oq;
	
	Image <T> imBorder=img;
	Image <T2> markerBorder=marker;
	
	double maxDouble=std::numeric_limits<double>::max();
	double BORDER=-1;
	double INIT=-2;
	
	///Image containing the priority of points in the queue. Max if not in the queue
	///BORDER=-1 if border point
	
	Image <double> inFilePrio(marker.getSize() );
	inFilePrio.fill(INIT);
	
	typename Image<T2>::iteratorXYZ it;
	typename Image<T2>::iteratorXYZ end=marker.end();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T maxValueT=std::numeric_limits<T>::max();
	T2 maxValueT2=std::numeric_limits<T2>::max();

	//Now enlarge the images
	addBorders(imBorder,back,front,maxValueT);
	addBorders(markerBorder,back,front, maxValueT2);
	addBorders(inFilePrio,back,front, BORDER);
	se.setContext(imBorder.getSize());
	
	ImageRegionsInfos <T, T2> imageRegions(imBorder,markerBorder);
	
	//Put the markers in the queue
	//For this, we scan the inner part of marker (without the borders)
	for(it=marker.begin(); it!=end; ++it)
		{
		if(*it!=0)
			{
			//Highest priority for the seeds
			double value=0.0; 
			//compute the offset in the border image
			TOffset offsetBorder=it.x+back[0]+(it.y+back[1])*markerBorder.getSizeX()+
			(it.z+back[2])*markerBorder.getSizeX()*markerBorder.getSizeY();
			oq.put(value,offsetBorder);
			
			imageRegions.setPoint(offsetBorder,*it);
			}
		}
		
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
int nbProcess=0;
bool flag=false;
while(!oq.empty())
		{
		TOffset p=oq.get();
	
		//Put neighbors of markers in the queue (the sequel of the initialization part)
		//points that have been inserted several times are not processed 
		//because inFilePrio(p)!=INIT
		
		if(markerBorder(p)!=T2(0) && inFilePrio(p) == INIT)
			{
			for(itSe=se.begin(); itSe!=endSe; ++itSe)
				{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) /*&& inFilePrio(q) == INIT*/)
					{
					double prio=imageRegions.computeDistance( p , q);
					//First approximation
					//To do better, we can update the priority of q (if it is necessary)
					//Hence each point can be inserted several times
					//the most prioritary point are processed first: hence
					//the other similar points are not processed (since markerBorder!=0 && inFilePrio!=INIT)
					// Here prio is inferior only if q is a neighbor of multiple regions
					// Hence q is inserted in the queue with the label of the nearest region
					if(prio < inFilePrio(q) || inFilePrio(q)==INIT);
						{
						oq.put(prio, q); 
						inFilePrio(q)=prio;
						}
					
					}
					
				}			
			}
		else if (markerBorder(p)==T2(0) ) {
			//p is an active points waiting to be labelled
			//First we compute the distance between this point and all the neighboring
			//regions
			double prio=std::numeric_limits<double>::max();
			TOffset bestQ;
			
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)!=T2(0) && inFilePrio(q)!=BORDER /*avoid the border points*/)
						{
						
						double tmpPrio=imageRegions.computeDistance(q,p);
						
						if(tmpPrio<prio )
							{
							prio=tmpPrio;
							bestQ=q;
							}	
						}
					}
				//we label p with the region having the least distance
				imageRegions.setPoint(p,markerBorder(bestQ));	
				
				nbProcess++;
				
				//Scan neighbors, compute disntaces and update (if necessary)
				//the priorities of neighbors
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) && inFilePrio(q)!=BORDER)
						{
						double prio=imageRegions.computeDistance(p,q);
						if(prio < inFilePrio(q) || inFilePrio(q)==INIT )
							{
							oq.put(prio,q);	
							inFilePrio(q)=prio;
							}
						
						}
					}		
		}
	}
				
	for(it=marker.begin(); it!=end; ++it)
		*it=markerBorder(it.x+back[0],it.y+back[1],it.z+back[2]);
}

///Same thing but each point is inserted with a fixed priority in the queue
///This gives slightly altered results 
template <class T, class T2>
void seededRegionGrowing0(Image <T> &img, Image <T2> &marker, FlatSE &se, bool observe=false)
{
	OrderedQueueDouble <TOffset> oq;
	
	Image <T> imBorder=img;
	Image <T2> markerBorder=marker;
	
	typename Image<T2>::iteratorXYZ it;
	typename Image<T2>::iteratorXYZ end=marker.end();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	T maxValueT=std::numeric_limits<T>::max();
	T2 maxValueT2=std::numeric_limits<T2>::max();

	//Now enlarge the images
	addBorders(imBorder,back,front,maxValueT);
	addBorders(markerBorder,back,front, maxValueT2);
	se.setContext(imBorder.getSize());
	
	//imBorder.saveInrGz("borderIm.inr.gz");
	//markerBorder.saveInrGz("borderMarker.inr.gz");
	ImageRegionsInfos <T, T2> imageRegions(imBorder,markerBorder);
	
	//Put the markers in the queue
	//For this, we scan the inner part of marker (without the borders)
	TOffset offset=0;
	for(it=marker.begin(); it!=end; ++it,offset++)
		{
		if(*it!=T2(0))
			{
			//Highest priority for the seeds
			double value=0.0; 
			//compute the offset in the border image
			TOffset offsetBorder=it.x+back[0]+(it.y+back[1])*markerBorder.getSizeX()+
			(it.z+back[2])*markerBorder.getSizeX()*markerBorder.getSizeY();
			oq.put(value,offsetBorder);
			imageRegions.setPoint(offsetBorder,*it);
			}
		}
	
	double INIT=-1;
	
	///Image containing the priority of points in the queue. Max if not in the queue
	Image <double> inFilePrio(markerBorder.getSize() );
	inFilePrio.fill(INIT );
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
int nbProcess=0;
while(!oq.empty())
		{
		TOffset p=oq.get();
	
		//Put neighbors of markers in the queue
		if(markerBorder(p)!=T2(0) )
			{
			for(itSe=se.begin(); itSe!=endSe; ++itSe)
				{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) && inFilePrio(q) == INIT)
					{
					double prio=imageRegions.computeDistance( p , q);
					oq.put(prio, q); 
					inFilePrio(q)=prio;
					}
				}			
			}
		else {
			double prio=std::numeric_limits<double>::max();
			TOffset bestQ=-1;
			bool flag=false;
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)!=T2(0) && markerBorder(q)!=maxValueT2)
						{
						flag=true;
						double tmpPrio=imageRegions.computeDistance(q,p);
						
						if(tmpPrio<prio )
							{
							prio=tmpPrio;
							bestQ=q;
							}	
						}
					}
				
				imageRegions.setPoint(p,markerBorder(bestQ));	
				
				nbProcess++;
				
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) && inFilePrio(q)==INIT )
						{
						double prio=imageRegions.computeDistance(p,q);
						oq.put(prio,q);	
						inFilePrio(q)=prio;
						}
					}		
		}
	}
	
	for(it=marker.begin(); it!=end; ++it)
		*it=markerBorder(it.x+back[0],it.y+back[1],it.z+back[2]);
}

/**Same thing but each point is inserted with a fixed priority in the queue
/* This gives slightly altered results 
 * Color version
**/


template <class T2>
void seededRegionGrowing0(Image <RGB> &img, Image <T2> &marker, FlatSE &se, bool observe=false)
{
	OrderedQueueDouble <TOffset> oq;
	
	Image <RGB> imBorder=img;
	Image <T2> markerBorder=marker;
	
	typename Image<T2>::iteratorXYZ it;
	typename Image<T2>::iteratorXYZ end=marker.end();
	
	const TCoord *back=se.getNegativeOffsets();
	const TCoord *front=se.getPositiveOffsets();
	
	RGB maxValueT=std::numeric_limits<RGB>::max();
	T2 maxValueT2=std::numeric_limits<T2>::max();

	//Now enlarge the images
	addBorders(imBorder,back,front,maxValueT);
	addBorders(markerBorder,back,front, maxValueT2);
	se.setContext(imBorder.getSize());
	
	imBorder.save("borderIm.ppm");
	//markerBorder.save("borderMarker.ppm");
	ImageRegionsInfosRGB imageRegions(imBorder,markerBorder);
	
	//Put the markers in the queue
	//For this, we scan the inner part of marker (without the borders)
	TOffset offset=0;
	for(it=marker.begin(); it!=end; ++it,offset++)
		{
		if(*it!=T2(0))
			{
			//Highest priority for the seeds
			double value=0.0; 
			//compute the offset in the border image
			TOffset offsetBorder=it.x+back[0]+(it.y+back[1])*markerBorder.getSizeX()+
			(it.z+back[2])*markerBorder.getSizeX()*markerBorder.getSizeY();
			oq.put(value,offsetBorder);
			imageRegions.setPoint(offsetBorder,*it);
			}
		}	
	double INIT=-1;
	
	///Image containing the priority of points in the queue. Max if not in the queue
	Image <double> inFilePrio(markerBorder.getSize() );
	inFilePrio.fill(INIT );
	
	FlatSE::iterator itSe;
	FlatSE::iterator endSe=se.end();
	
int nbProcess=0;
while(!oq.empty())
		{
		TOffset p=oq.get();
	
		//Put neighbors of markers in the queue
		if(markerBorder(p)!=T2(0) )
			{
			for(itSe=se.begin(); itSe!=endSe; ++itSe)
				{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) && inFilePrio(q) == INIT)
					{
					double prio=imageRegions.computeDistance( p , q);
					oq.put(prio, q); 
					inFilePrio(q)=prio;
					}
				}			
			}
		else {
			double prio=std::numeric_limits<double>::max();
			TOffset bestQ=-1;
			bool flag=false;
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)!=T2(0) && markerBorder(q)!=maxValueT2)
						{
						flag=true;
						double tmpPrio=imageRegions.computeDistance(q,p);
						
						if(tmpPrio<prio )
							{
							prio=tmpPrio;
							bestQ=q;
							}	
						}
					}
				
				imageRegions.setPoint(p,markerBorder(bestQ));	
				
				nbProcess++;
				
				for(itSe=se.begin(); itSe!=endSe; ++itSe)
					{
					TOffset q=p + *itSe;
					
					if(markerBorder(q)==T2(0) && inFilePrio(q)==INIT )
						{
						double prio=imageRegions.computeDistance(p,q);
						oq.put(prio,q);	
						inFilePrio(q)=prio;
						}
					}		
		}
	}
	
	for(it=marker.begin(); it!=end; ++it)
		*it=markerBorder(it.x+back[0],it.y+back[1],it.z+back[2]);
}
/*@}*/

}
