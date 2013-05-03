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

// random-singleton.cpp (Singleton)
// Implementation de la classe Random / Implementation of the class Random
#include "random-singleton.h"

#include <cfloat> //DBL_EPSILON
using namespace std;

inline static double sqr(double x) {return x*x;}

//These consts are required by the algorithm
const int Random::NTAB = 32;

const long Random::IM1 = 2147483563;
const long Random::IM2 = 2147483399;
const long Random::IMM1 = Random::IM1 - 1;
const double Random::AM = 1.0/Random::IM1;
const int Random::IA1 = 40014;
const int Random::IA2 = 40692;
const int Random::IQ1 = 53668;
const int Random::IQ2 = 52774;
const int Random::IR1 = 12211;
const int Random::IR2 = 3791;
const int Random::NDIV = 1 + Random::IMM1/Random::NTAB;
const double Random::EPS = DBL_EPSILON;
const double Random::RNMX = 1.0-Random::EPS;

long  Random::idum   = 0;
long  Random::idum2  = 123456789L;
long  Random::iy     = 0;
long* Random::iv     = 0;

Random Random::Singleton(0);

void Random::Randomize(long seed)
{
  idum = (seed <= 0) ? (seed == 0 ? 1 : -seed) : seed; //be sure to prevent idum=0

  idum2 = idum;
  if (idum2 < 0) idum2 = -idum2;
   
  long k = 0;
  for(int j = NTAB+7 ; j >= 0 ; j--)
  {
    k = idum/IQ1;
    idum = IA1*(idum - k*IQ1) - k*IR1;
    if (idum < 0)
      idum += IM1;

    if (j < NTAB)
      iv[j] = idum;
  }

  iy = iv[0];
}

//This function returns a double, taken uniformly in ]0;1[
//It is the algorithm of L'Ecuyer with a Bays-Durham shuffle
double Random::theRandom(void)
{
  long k = idum/IQ1;
  idum = IA1*(idum - k*IQ1) - k*IR1;
  if (idum < 0)
    idum +=IM1;

  k = idum2/IQ2;
  idum2 = IA2*(idum2 -k*IQ2) - k*IR2;
  if (idum2 < 0)
    idum2 +=IM2;

  long j = iy/NDIV;
  iy = iv[j] - idum2;
  iv[j] = idum;
  if (iy < 1)
    iy += IMM1;
      
  double temp = AM*iy;
  if (temp >= RNMX)
    return RNMX;  //prevents from returning 1
  else
    return temp;
}

//Returns a double taken on a Gaussian with specified mean and standard deviation
double Random::Gaussian(double mean, double standardDeviation)
{
  const int NbTirages = 12; //increase for better precision. 12 works fine.
  double valeur = 0;
  for(int i=0 ; i < NbTirages ; ++i)
    valeur += Random::Uniform();
   
  //centering the sum
  valeur -= NbTirages/2;

  //spread with standard deviation
  //NbTirages = 12
  //the 12 is not related to NbTirages, but it explains why it is often chosen that NbTirages=12
  valeur *= (NbTirages == 12) ? standardDeviation
                              : sqrt(12/static_cast<double>(NbTirages))*standardDeviation;

  //debias
  valeur += mean;

  return valeur;
}


//end random-singleton.cpp
