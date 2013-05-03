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
#include <vector>

namespace LibTIM {

#define EPSILON 0.0000001


/**  \defgroup kMeans K-Means 
	\ingroup ImageProcessing
**/

/*@{*/
///K-means segmentation
///Take image and a vector containing centroids initialization (size of vector gives number of classes)
///Return classification result 





template <class T>
Image <TLabel> kMeansScalarImage(const Image <T> &img,  std::vector<double> &centroids)
{
	int dx=img.getSizeX();
	int dy=img.getSizeY();
	int dz=img.getSizeZ();
	
	int nClasses=centroids.size();
	
	typename std::vector<T>::iterator it;
	
	Image <TLabel> res(img.getSize());
	res.fill(0);
	
	
	double distanceClass;
	double bestDistance;
	TLabel bestClass;
	double doubleMax=std::numeric_limits<double>::max();
	
	
	//Algorithm
	bool stop=false;
	std::vector <double> sumElements;
	sumElements.resize(nClasses,0);
	std::vector <int> nbElements;
	nbElements.resize(nClasses,0);
	std::vector<double> oldCentroids=centroids;
	
	while(!stop)
		{
		//Update clusters
		for(int i=0; i<img.getBufSize(); i++)
		{
		T value=img(i);
		bestDistance=doubleMax;
		bestClass=0;
		for(int c=0; c<nClasses; c++)
			{
			distanceClass=fabs(value-centroids[c]);
			if(distanceClass<bestDistance) 
				{
				bestDistance=distanceClass;
				bestClass=(TLabel)c;
				}
			}
		res(i)=bestClass;
		}
		
		//Update centroids
		for(int i=0; i<nClasses; i++) sumElements[i]=0;
		for(int i=0; i<nClasses; i++) nbElements[i]=0;
		
		for(int i=0; i<img.getBufSize(); i++)
			{
			sumElements[res(i)]+=(double)img(i);
			nbElements[res(i)]++;
			for(int c=0; c<nClasses; c++)
				{
				centroids[c]=(double)sumElements[c]/nbElements[c];
				}
			}
		
		//Test convergence
		stop=true;
		for(int c=0; c<nClasses; c++)
			{
			if( (centroids[c]-oldCentroids[c])< EPSILON )
				stop=false;
			}
			
		oldCentroids=centroids;
		}
	
	return res;
}

/*@]*/

} //end namespace
