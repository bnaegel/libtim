
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

///Same thing but without the border hits condition

// template <class T>
// GImage <int> templateMatchingL2(const GImage <T> &im, const GMask &mask)
// {
// 	int dx=im.getSizeX();
// 	int dy=im.getSizeY();
// 	int dz=im.getSizeZ();
// 	
// 	GImage <int> res(im.getSize());
// 	
// 	int maxValue=std::numeric_limits<int>::max();
// 	
// 	T maxValueImageType=std::numeric_limits<T>::max();
// 	
// 	for(int z=0; z<dz; z++)
// 		for(int y=0; y<dy; y++)
// 			for(int x=0; x<dx; x++)
// 				{
// 				bool hitsBorder=false;
// 				int currentDifference=0;
// 				GPoint p(x,y,z);
// 				for(int i=0; i<mask.getNbPoints(); i++)
// 					{
// 					GPoint q=p+mask.getPoint(i);
// 					if(res.isPosValid(q))
// 						{
// 						int templateValue=mask.getValue(i);
// 						currentDifference+=(templateValue-im(q))*(templateValue-im(q));
// 						}
// 					}
// 				
// 				res(x,y,z)=currentDifference/mask.getNbPoints();
// 				}
// 	return res;
// }	

///Same thing but with two templates: one for foreground (255), the other for background (0)

// template <class T>
// GImage <int> templateMatchingL2(const GImage <T> &im, const GFlatSE &seA, const GFlatSE &seB)
// {
// 	int dx=im.getSizeX();
// 	int dy=im.getSizeY();
// 	int dz=im.getSizeZ();
// 	
// 	GImage <int> res(im.getSize());
// 	
// 	int maxValue=std::numeric_limits<int>::max();
// 	
// 	T maxValueImageType=std::numeric_limits<T>::max();
// 	
// 	T maxImage=im.getMax();
// 	
// 	for(int z=0; z<dz; z++)
// 		for(int y=0; y<dy; y++)
// 			for(int x=0; x<dx; x++)
// 				{
// 				
// 				int currentDifference=0;
// 				GPoint p(x,y,z);
// 				///Background: if a point hits the border it has a 0 contribution
// 				for(int i=0; i<seB.getNbPoints(); i++)
// 					{
// 					GPoint q=p+seB.getPoint(i);
// 					if(res.isPosValid(q))
// 						currentDifference+=(T(0)-im(q))*(T(0)-im(q));
// 					else currentDifference+=(maxValueImageType)*(maxValueImageType);
// 					}
// 				
// 				///Foreground: if a point hits the border we stop
// 				for(int i=0; i<seA.getNbPoints(); i++)
// 					{
// 					GPoint q=p+seA.getPoint(i);
// 					if(res.isPosValid(q))
// 						currentDifference+=(maxImage-im(q))*(maxImage-im(q));
// 					else currentDifference+=(maxValueImageType)*(maxValueImageType);
// 						
// 					}
// 				
// 				res(x,y,z)=currentDifference/(seA.getNbPoints()+seB.getNbPoints() );
// 				}
// 	
// 	return res;
// }

	
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
