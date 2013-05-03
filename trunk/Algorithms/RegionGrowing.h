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

#ifndef RegionGrowing_h
#define RegionGrowing_h

#include "Common/OrderedQueue.h"
#include "Common/Image.h"
#include <cmath>
#include <map>


namespace LibTIM {

///Class that encapsulates various informations useful for region growing algorithms
/*
Warning: this class acts directly on the input images
*/

using std::map;

template <class T, class T2>
class ImageRegionsInfos {
	public:
		ImageRegionsInfos(Image <T> &img, Image <T2> &seedRegions):imgSrc(img), seedRegions(seedRegions) {}
		
		///Return the distance between p and q. The return  value can be interpreted as a priority.
		double computeDistance(Point <TCoord> &p, Point <TCoord>  &q)
			{
			double moy=(double)sumIntensityRegions[(seedRegions)(p)]/nbPointsRegions[(seedRegions)(p)];
			double prio=(fabs((imgSrc)(q)-moy));
			return prio;
			}
		
		double computeDistance(TOffset &p, TOffset  &q)
			{
			if(nbPointsRegions[(seedRegions)(p)]==0) 
				{
				return -1;
				}
			double moy=(double)sumIntensityRegions[(seedRegions)(p)]/nbPointsRegions[(seedRegions)(p)];
			double prio=(fabs((imgSrc)(q)-moy));
			
			return prio;
			}
		
		//Fusion model Ri=m(Ri,p)
		void setPoint(Point <TCoord> &p, T2 label) 
			{
			(seedRegions)(p)=label;
			sumIntensityRegions[label]+=(imgSrc)(p);
			nbPointsRegions[label]++; 
			}
		
		//Fusion model Ri=m(Ri,p)
		void setPoint(TOffset &p, T2 label) 
			{
			(seedRegions)(p)=label;
			sumIntensityRegions[label]+=(imgSrc)(p);
			nbPointsRegions[label]++; 
			}
		
		
		void fusion(Point <TCoord> &p, T2 label)
			{
			
			}
		
	private:
		map <T2, double> sumIntensityRegions;
		map <T2, int>    nbPointsRegions;
		Image <T> &imgSrc;
		Image <T2> &seedRegions;
		
		//inclure lecture d'un fichier de contraintes
		//Ex:
		//NDG mean 100
		//Compacity 1.5
		//Area MIN 5% MAX 30%
};

class ImageRegionsInfosRGB {
	public:
		ImageRegionsInfosRGB(Image <RGB> &img, Image <TLabel> &seedRegions):imgSrc(img), seedRegions(seedRegions) {}
		
		///Return the distance between p and q. The return  value can be interpreted as a priority.
		double computeDistance(Point <TCoord> &p, Point <TCoord>  &q)
			{
			Table <double,3> moy;
			moy[0]=(double)sumIntensityRegions[(seedRegions)(p)][0]/nbPointsRegions[(seedRegions)(p)];
			moy[1]=(double)sumIntensityRegions[(seedRegions)(p)][1]/nbPointsRegions[(seedRegions)(p)];
			moy[2]=(double)sumIntensityRegions[(seedRegions)(p)][2]/nbPointsRegions[(seedRegions)(p)];
			
			double diffR=imgSrc(q)[0]-moy[0];
			diffR*=diffR;
			double diffG=imgSrc(q)[1]-moy[1];
			diffG*=diffG;
			double diffB=imgSrc(q)[2]-moy[2];
			diffB*=diffB;
			
			
			double prio=diffR+diffG+diffB;
			return prio;
			}
		
		double computeDistance(TOffset &p, TOffset  &q)
			{
			if(nbPointsRegions[(seedRegions)(p)]==0) 
				{
				return -1;
				}
			Table <double,3> moy;
			moy[0]=(double)sumIntensityRegions[(seedRegions)(p)][0]/nbPointsRegions[(seedRegions)(p)];
			moy[1]=(double)sumIntensityRegions[(seedRegions)(p)][1]/nbPointsRegions[(seedRegions)(p)];
			moy[2]=(double)sumIntensityRegions[(seedRegions)(p)][2]/nbPointsRegions[(seedRegions)(p)];
			
			double diffR=imgSrc(q)[0]-moy[0];
			diffR*=diffR;
			double diffG=imgSrc(q)[1]-moy[1];
			diffG*=diffG;
			double diffB=imgSrc(q)[2]-moy[2];
			diffB*=diffB;
			
			
			double prio=diffR+diffG+diffB;
			return prio;
			}
		
		//Fusion model Ri=m(Ri,p)
		void setPoint(Point <TCoord> &p, TLabel label) 
			{
			(seedRegions)(p)=label;
			sumIntensityRegions[label][0]+=(imgSrc)(p)[0];
			sumIntensityRegions[label][1]+=(imgSrc)(p)[1];
			sumIntensityRegions[label][2]+=(imgSrc)(p)[2];
			nbPointsRegions[label]++; 
			}
		
		//Fusion model Ri=m(Ri,p)
		void setPoint(TOffset &p, TLabel label) 
			{
			(seedRegions)(p)=label;
			sumIntensityRegions[label][0]+=(imgSrc)(p)[0];
			sumIntensityRegions[label][1]+=(imgSrc)(p)[1];
			sumIntensityRegions[label][2]+=(imgSrc)(p)[2];
			nbPointsRegions[label]++; 
			}
		
		
		
	private:
		map <TLabel, RGB> sumIntensityRegions;
		map <TLabel, int>    nbPointsRegions;
		Image <RGB> &imgSrc;
		Image <TLabel> &seedRegions;
		
		//inclure lecture d'un fichier de contraintes
		//Ex:
		//NDG mean 100
		//Compacity 1.5
		//Area MIN 5% MAX 30%
};


}
#include "RegionGrowing.hxx"
#endif 

