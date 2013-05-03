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

#include <vector>

namespace LibTIM {

/** \defgroup Tarjan Tarjan's Union Find Algorithms
	\ingroup ImageProcessing 
	Procedures implementing Tarjan's union-find algorithm

	Tarjan's Union Find based routines, and algorithms.
	Mostly research code.
**/

/*@{*/


using namespace std;

typedef vector <int> treeType;

void MakeSet( treeType &tree, const int &offset)
{
	tree[offset]=offset;
	
}

int Find( treeType &tree, const int &offset)
{
	int res=offset;
	while(tree[res]!=res) {res=tree[res];}
	int root=res;
	res=offset;
	while(tree[res]!=res) {int tmp=tree[res]; tree[res]=root; res=tmp;}
	
	return root;
}

int FindSimple( treeType &tree, const int &offset)
{
	int res=offset;
	while(tree[res]!=res) {res=tree[res];}

	return res;
}

int Link( treeType &tree,  int &x,  int &y)
{
	if(x!=y)
		tree[x]=y;
	return y;
}

void MakeSet(int *tree, const int &offset)
{
	tree[offset]=offset;
}

int Find(int *tree, const int &offset)
{
	int res=offset;
	while(tree[res]!=res) {res=tree[res];}
	int root=res;
	res=offset;
	while(tree[res]!=res) {int tmp=tree[res]; tree[res]=root; res=tmp;}
	
	return root;
}

int Link( int *tree,  int &x,  int &y)
{
	if(x!=y)
		tree[x]=y;
	return y;
}

template <class T>
Image <TLabel> labelConnectedComponentsTarjan(const Image <T> &im, const FlatSE &se)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <TLabel> res(im.getSize());
	
	treeType tree(im.getBufSize());
	for(int i=0; i<im.getBufSize(); i++) tree[i]=-1;
	
	for(int i=0; i<im.getBufSize(); i++)
		{
		if(im(i)!=0)
			MakeSet(tree,i);
		}
	
	for(int z=0; z<dz; z++)
		for(int y=0; y<dy; y++)
			for(int x=0; x<dx; x++)
				{
				if(im(x,y,z)!=0)
					{
					Point <TCoord> p(x,y,z);
					int po=z*dx*dy+y*dx+x;
					int compP=Find(tree,po);
					int compQ;
					for(int i=0; i<se.getNbPoints(); i++)
						{
						Point <TCoord> q=p+se.getPoint(i);
					
						if(im.isPosValid(q) && im(q)!=0)
							{
							int qo=q.z*dx*dy+q.y*dx+q.x;
						 	compQ=Find(tree,qo);
						
							if(compP!=compQ)
								{
								compP=Link(tree,compQ,compP);
								}
							}
					
						}
					}
				}

	for(int i=0; i<im.getBufSize(); i++)
		if(im(i)!=0)
			res(i)=Find(tree,i);
		
	return res;
}

///Second version, using implementation described in ISMM'05
///Tests showed that labeling with the method of breadth scan (propagation) is faster (in our implementation)



template <class T>
Image <TLabel> labelConnectedComponentsTarjan2(const Image <T> &im, const FlatSE &se)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	Image <TLabel> res(im.getSize());
	
	Image <bool> isProcessed(im.getSize());
	isProcessed.fill(false);
	
	//treeType tree(im.getBufSize());
	int *tree=new int[im.getBufSize()];
	for(int i=0; i<im.getBufSize(); i++) tree[i]=-1;
	
	//First scan (backward)
	for(int z=dz-1; z>=0; z--)
		for(int y=dy-1; y>=0; y--)
			for(int x=dx-1; x>=0; x--)
				{
				if(im(x,y,z)!=0)
					{
					Point <TCoord> p(x,y,z);
					int po=z*dx*dy+y*dx+x;
					MakeSet(tree,po);
					for(int i=0; i<se.getNbPoints(); i++)
						{
						Point <TCoord> q=p+se.getPoint(i);
						if(im.isPosValid(q))
							if ( im(q)!=0)
							{
							if(isProcessed(q))
								{
								int qo=q.z*dx*dy+q.y*dx+q.x;
								int rootQ=Find(tree,qo);
								Link(tree,rootQ,po);
								}
							}
						}
					isProcessed(p)=true;
					}
				}
	//Second scan: inverse order (here forward)		
	int label=0;
	for(int i=0; i<im.getBufSize(); i++)
		{
		if (im(i)==0) res(i)=0;
		else
			{
			//Check if i is a root
			if(tree[i]==i)
				{
				label++;
				res(i)=label;
				}
			else res(i)=res(tree[i]);
			}
		}
	delete[] tree;
	return res;
}

/*@}*/

} // end namespace
