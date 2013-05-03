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

#include <fstream>
namespace LibTIM {


template <class T>
Histogram <T> ::Histogram(Image <T> &im)
{
	int dx=im.getSizeX();
	int dy=im.getSizeY();
	int dz=im.getSizeZ();
	
	for(int i=0; i< dx*dy*dz; i++)
		data[im(i)]++;
}

///Write histogram into file
template <class T>
int Histogram <T>::write(const char *filename)
{
	std::ofstream outputFile(filename); 	
	if(!outputFile)
 		{
 		std::cerr << "Cannot open " << filename << "\n";
 		return 0;
 		}
	typename HistoType::iterator it;
	
	 for( it=data.begin(); it!=data.end(); it++)
 		outputFile << (int)it->first << " " << it->second << "\n";
 	outputFile.close();
	return 1;
}

}
