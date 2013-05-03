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
