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


/** \defgroup ColorProcessing 
	\ingroup ImageProcessing
	Functions related to color image processing
**/

/*@{*/

Image <U8> hueCanal(Image <RGB> &im)
{
	Image <U8> colorV(im.getSize());
	for(int i=0; i<colorV.getBufSize(); i++) 
		{
		colorV(i)=std::max(std::max(im(i)[0],im(i)[1]),im(i)[2]);
		}
	return colorV;
}


Image <U8> saturationCanal(Image <RGB> &im)
{
	Image <U8> colorS(im.getSize());
	for(int i=0; i<colorS.getBufSize(); i++) 
		{
		U8 max=std::max(std::max(im(i)[0],im(i)[1]),im(i)[2]);
		U8 min=std::min(std::min(im(i)[0],im(i)[1]),im(i)[2]);
		if(max!=0)
			colorS(i)=255*(max-min)/max;
		else colorS(i)=0;
		}
	return colorS;
}

/*@}*/

}//end namespace
