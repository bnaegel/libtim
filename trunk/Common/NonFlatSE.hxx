
namespace LibTIM {

template <>
inline void NonFlatSE<U8>::makeChamfer2D()
{
	this->clear();
	
	Point<TCoord>  N(0,-1);
	Point<TCoord>  S(0,1);
	Point<TCoord>  W(-1,0);
	Point<TCoord>  E(1,0);
	
	Point<TCoord>  NW(-1,-1);
	Point<TCoord>  NE(1,-1);
	Point<TCoord>  SW(-1,1);
	Point<TCoord>  SE(1,1);
	
	Point<TCoord>  O(0,0);
	
	this->addPoint(N,1);
	this->addPoint(S,1);
	this->addPoint(W,1);
	this->addPoint(E,1);
	this->addPoint(NW,1);
	this->addPoint(NE,1);
	this->addPoint(SW,1);
	this->addPoint(SE,1);
	this->addPoint(O,1);
}

template <class T>
NonFlatSE<T> NonFlatSE<T>::rasterScan()
{	
	NonFlatSE<T> res;
	for(int i=0; i< getNbPoints(); i++)
		{
		Point<TCoord> tmp=this->getPoint(i);
		if(tmp.z<0 || (tmp.z==0 && tmp.y<0) || 
		(tmp.z==0 && tmp.y==0 && tmp.x < 0) )
			res.addPoint(tmp,getValue(i));
		}
	
	return res;
}

template <class T>
NonFlatSE<T> NonFlatSE<T>::antiRasterScan()
{
	NonFlatSE<T> res;
	for(int i=0; i< getNbPoints(); i++)
		{
		Point<TCoord> tmp=getPoint(i);
		if(tmp.z>0 || (tmp.z==0 && tmp.y>0) || 
			(tmp.z==0 && tmp.y==0 && tmp.x > 0) )
			res.addPoint(tmp,getValue(i));
		}
	
	return res;

}

template <class T>
double NonFlatSE<T>::getNorm() const
{	
	int res=0;
	for(int i=0; i<getNbPoints(); i++)
		{
		res+=this->getValue(i)*this->getValue(i);
		}
		
	return sqrt((double)res);
}

}
