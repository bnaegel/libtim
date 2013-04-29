
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
