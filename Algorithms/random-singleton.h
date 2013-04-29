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
            //Si le numeric_limits<T> ne marche pas/if numeric_limits doesn't work
            //{return static_cast<T>(1)/static_cast<T>(2)==static_cast<T>(0);}

  public:
    //Reinitialise la graine / Inits the seed
    //Attention, 0 et 1 pour la graine donnent la meme suite 
    //Beware that 0 and 1 for the seed give the same sequence
    static void Randomize(long thatSeed=0);

    //Pour un type T, renvoie une valeur uniformement dans [min;max]
    //(min et max exclu pour les types non entiers)
    //For a type T, returns a value uniformly in [min;max]
    //(min and max excluded for non integral types)
    template <typename T>
    static inline T Uniform(T min, T max)
           {return static_cast<T>(min+(max+(typeIsInteger<T>()?1:0)-min)*theRandom());}
    
    //Par defaut : Uniform<double>(0,1) / default : Uniform<double>(0,1)
    static inline double Uniform(void) {return theRandom();}

    //Renvoie un nombre selon la Gaussienne de moyenne et d'ecartype specifies
    //Par defaut, c'est la loi normale centree reduite
    //Returns a double taken on a Gaussian with specified mean and standard dev.
    //By default, it is the Normal law with mean=0, std dev=1
    static double Gaussian(double mean=0, double standardDeviation=1);

    //Exponential
    inline static double Exponential(double lambda)
           {return -std::log(Uniform())/lambda;}
             
    //On ne peut instancier d'objets de cette classe
    //You cannot instanciate objects of this class
  private:
    Random(long seed=0) {iv = new long[NTAB]; Random::Randomize(seed);}
    Random(const Random&) {};
    Random operator=(const Random&) {return *this;}
    ~Random() {if (iv) delete [] iv; iv = 0;}
      
  private:
    //Toutes ces constantes sont definies pour l'algorithme du generateur
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
    //Ces variables sont utilisees pour les calculs du generateur
    //Useful variables
    static long idum;
    static long idum2;
    static long iy;
    static const int NTAB;
    static long* iv;

    //Cette fonction renvoie un double aleatoire uniforme dans ]0;1[
    //C'est le coeur du generateur
    //The kernel of the generator : returns a double uniformly in ]0;1[
    static double theRandom(void);
      
  private:
    static Random Singleton; //Instanciation unique / Single instanciation
};

#endif // __RANDOM_H__
