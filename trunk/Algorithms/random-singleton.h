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

//Random Singleton

//Ce fichier d'en-tete propose un bon generateur aleatoire
//de nombre reels ou entiers.Il s'agit du generateur de L'Ecuyer avec melange
//de Bays-Durham, des Numerical Recipes (http://www.nr.com)
//This header file provides a good random generator. It is the one of L'Ecuyer
//with Bays-Durham shuffle, found in Numerical Recipes(http://www.nr.com)

#ifndef __RANDOM_H__
#define __RANDOM_H__

#include <cmath>
#include <limits>
using namespace std;

class Random //Singleton
{
  private:
    template<typename T>
    static inline bool typeIsInteger(void)
           {return numeric_limits<T>::is_integer;}
            //if numeric_limits<T> doesn't work
            //{return static_cast<T>(1)/static_cast<T>(2)==static_cast<T>(0);}

  public:
    //Inits the seed
    //Beware that 0 and 1 for the seed give the same sequence
    static void Randomize(long thatSeed=0);

    //For a type T, returns a value uniformly in [min;max]
    //(min and max excluded for non integral types)
    template <typename T>
    static inline T Uniform(T min, T max)
           {return static_cast<T>(min+(max+(typeIsInteger<T>()?1:0)-min)*theRandom());}
    
    //default : Uniform<double>(0,1)
    static inline double Uniform(void) {return theRandom();}

    //Returns a double taken on a Gaussian with specified mean and standard dev.
    //By default, it is the Normal law with mean=0, std dev=1
    static double Gaussian(double mean=0, double standardDeviation=1);

    //Exponential
    inline static double Exponential(double lambda)
           {return -std::log(Uniform())/lambda;}
             
    //You cannot instanciate objects of this class
  private:
    Random(long seed=0) {iv = new long[NTAB]; Random::Randomize(seed);}
    Random(const Random&) {};
    Random operator=(const Random&) {return *this;}
    ~Random() {if (iv) delete [] iv; iv = 0;}
      
  private:
    //Useful consts
    static const long int IM1;
    static const long int IM2;
    static const long int IMM1;

    static const double AM;

    static const int IA1;
    static const int IA2;
    static const int IQ1;
    static const int IQ2;
    static const int IR1;
    static const int IR2;

    static const int NDIV;

    static const double EPS;
    static const double RNMX;

  private:
    //Useful variables
    static long idum;
    static long idum2;
    static long iy;
    static const int NTAB;
    static long* iv;

    //The kernel of the generator : returns a double uniformly in ]0;1[
    static double theRandom(void);
      
  private:
    static Random Singleton; //Single instanciation
};

#endif // __RANDOM_H__
