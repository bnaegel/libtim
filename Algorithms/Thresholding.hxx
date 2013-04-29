
namespace LibTIM {

/** \defgroup ImageProcessing Image Processing Basis Functions
**/

/** \defgroup thresholding Thresholding Functions
	\ingroup ImageProcessing
**/

/*@{*/

///Thresholding

template <class T>
Image <T> threshold(Image <T> &im, T tLow, T tHigh)
{
	Image <T> res(im.getSize());
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	
	typename Image<T>::iterator itRes=res.begin();
	
	for(it=im.begin(); it!=end; ++it,++itRes)
		if(*it<tLow || *it >tHigh)
			*itRes=T(0);
		else *itRes=*it;
	
	return res;
}

///Thresholding (overloaded) 

template <class T>
Image <T> threshold(Image <T> &im, int tLow, int tHigh)
{
	Image <T> res(im.getSize());
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	
	typename Image<T>::iterator itRes=res.begin();
	
	for(it=im.begin(); it!=end; ++it,++itRes)
		if(*it<(T)tLow || *it >(T)tHigh)
			*itRes=T(0);
		else *itRes=*it;
	
	return res;
}

///Binarization: 0->0, !=0 -> 255

template <class T>
Image <U8> binarize(Image <T> &im)
{
	Image <T> res(im.getSize());
	typename Image<T>::iterator it;
	typename Image<T>::iterator end=im.end();
	
	typename Image<T>::iterator itRes=res.begin();
	
	for(it=im.begin(); it!=end; ++it,++itRes)
		if(*it!=T(0))
			*itRes=255;
		else *itRes=0;
	return res;
}

/*@}*/

} //namespace
