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

#ifndef ComponentTree_h
#define ComponentTree_h

#include "Morphology.h"
#include "Common/tinyxml/tinyxml.h"

namespace LibTIM {

using std::vector;

/** \defgroup ccTree Component-Tree Based Algorithms
	\ingroup Morpho
**/

/*@{*/

/** @brief Component tree representation of an image
 *	The component tree (or max-tree) is a particular image structure. Nodes represents flat zones.
 *	Leafs represents regional maxima (max-tree) or minima (min-tree). Father-son relation represents
 *	an inclusion relation (with respect to level sets representation of image).
 *	This structure is efficient for computing attribute openings (area, contrast, ...)
**/
const int localMax=std::numeric_limits<int>::max();
const int localMin=std::numeric_limits<int>::min();

//TODO: a generic structure that contains Node attributes
struct Node {
    Node()
    : label(-1),xmin(localMax),ymin(localMax),
    xmax(localMin),ymax(localMin),area(0),
    area_derivative_areaN_h(std::numeric_limits<long double>::max()),
    area_derivative_areaN_h_derivative(std::numeric_limits<long double>::max()),
    area_derivative_h(std::numeric_limits<long double>::max()),
    area_derivative_areaN(std::numeric_limits<long double>::max()),
    mser(std::numeric_limits<long double>::max()),
    area_derivative_delta_h(std::numeric_limits<long double>::max()),
    area_derivative_delta_areaF(std::numeric_limits<long double>::max()),
    sum(0), sum_square(0), mean(0), variance(0),
    area_nghb(0), sum_nghb(0), sum_square_nghb(0), mean_nghb(0), variance_nghb(0),
    contrast(0), volume(0),  contourLength(0),
    complexity(0), subNodes(0),status(true),
    m01(0),m10(0),m20(0),m02(0),
    father(0), active(true),debug(0)
    {
        pixels.reserve(7); childs.reserve(5);
    }
    int label;
    int ori_h;
    int h;
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    int zmin;
    int zmax;
    int64_t area;
    // father correspond au noeud père
    long double area_derivative_areaN_h;
    long double area_derivative_areaN_h_derivative;
    // (aire(father) - aire(noeud) / (h(noeud) - h(father))
    long double area_derivative_h;
    // (aire(father) - aire(noeud)) / aire(noeud)
    long double area_derivative_areaN;
    // father_d correspond au noeud dans la branche parent tel que  (h(noeud) - h(father_d)) >= delta
    // (aire(father_d) - aire(noeud)) / aire(noeud)
    long double mser;
    // (aire(father_d) - aire(noeud)) / (h(noeud) - h(father_d))
    long double area_derivative_delta_h;
    // (aire(father_d) - aire(noeud)) / aire(father_d)
    long double area_derivative_delta_areaF;
    // otsu
    int64_t sum;
    int64_t sum_square;
    long double mean;
    long double variance;
    int64_t area_nghb;
    int64_t sum_nghb;
    int64_t sum_square_nghb;
    long double mean_nghb;
    long double variance_nghb;
    long double otsu;

    int contrast;
    int volume;
    long double mean_gradient_border;
    int contourLength;
    int complexity;
    int compacity;
    
    int debug;
    
    int64_t subNodes;
    
    //experimental... moments
    long double m01;
    long double m10;
    long double m20;
    long double m02;
    //moment d'inertie (first Hu's invariant moment)
    double I;
    
    bool status;
    bool active;
    //Distance measure between the node and a reference vector
    //(and eventually a covariance matrix for Mahalanobis distance)
    double dist;
    
    //Common to all type of nodes:
    Node *father;
    typedef std::vector<TOffset> ContainerPixels;
    ContainerPixels pixels;
    ContainerPixels pixels_border;
    typedef  std::vector<Node *> ContainerChilds;
    ContainerChilds childs;
    ContainerPixels contour;
};


typedef std::vector<std::vector<Node *> > IndexType;

enum ComputedAttributes {
    AREA                = 0b00000001,
    AREA_DERIVATIVES    = 0b00000010,
    OTSU                = 0b00000100,
    CONTRAST            = 0b00001000,
    VOLUME              = 0b00010000,
    BORDER_GRADIENT     = 0b00100000,
    COMP_LEXITY_ACITY   = 0b01000000,
    BOUNDING_BOX        = 0b10000000,
    SUB_NODES           = 0b0100000000,
    INERTIA_MOMENT      = 0b1000000000,
};

template <class T>
class ComponentTreeStrategy;

template <class T>
class SalembierRecursiveImplementation;

template <class T>
class ComponentTree {
	public:
		ComponentTree() {};
        ComponentTree(Image <T> &img);
        ComponentTree(Image <T> &img, FlatSE &connexity);
        ComponentTree(Image <T> &img, FlatSE &connexity, unsigned int delta);
        ComponentTree(Image <T> &img, FlatSE &connexity, ComputedAttributes ca, unsigned int delta);
		~ComponentTree();

        int computeNeighborhoodAttributes(int r);

		enum ConstructionDecision {MIN,MAX,DIRECT};
		Image <T> constructImage(ConstructionDecision decision=MIN);
        Image <T> &constructImageOptimized();

        enum Attribute {
          H,
          AREA,
          AREA_D_AREAN_H,
          AREA_D_AREAN_H_D,
          AREA_D_H,
          AREA_D_AREAN,
          MSER,
          AREA_D_DELTA_H,
          AREA_D_DELTA_AREAF,
          MEAN,
          VARIANCE,
          MEAN_NGHB,
          VARIANCE_NGHB,
          OTSU,
          CONTRAST,
          VOLUME,
          MGB,
          CONTOUR_LENGTH,
          COMPLEXITY,
          COMPACITY
        };
        template<class TVal, class TSel>
        Image <TVal> constructImageAttribute(Attribute value_attribute, Attribute selection_attribute=MSER, ConstructionDecision selection_rule=DIRECT);

        template<class TVal, class TSel, class TLimit>
        Image <TVal> constructImageAttribute(Attribute value_attribute, Attribute selection_attribute=MSER, ConstructionDecision selection_rule=DIRECT,
                                             Attribute limit_attribute=AREA, TLimit limit_min=0, TLimit limit_max=std::numeric_limits<TLimit>::max());


		/**
		  * @brief Print tree on standard output
		**/
		void print();

		/**
		  * @brief Print size of tree (in kilobytes)
		**/
		void printSize();

		/**
		  * @brief Print tree in a .dot file to generate clean drawing
		**/

		int writeDot(const char *filename);

		/**
		  * @brief Print tree in a .XML file
		**/
		void writeXml_rec(Node *node, TiXmlElement * nodeXML);
		int writeXml(const char *filename);

		/**
		  * @brief Restore original tree (i.e. clear all filtering)
		**/

		int restore();

		/**
		  * @brief Area filtering
		**/

        int areaFiltering(int64_t tMin, int64_t tMax=std::numeric_limits<int64_t>::max());

		/**
		  * @brief Contrast filtering
		**/

		//Keep only cc having certain contrast
		int contrastFiltering(int tMin, int tMax=std::numeric_limits<int>::max());


		/**
		  * @brief Volumic filtering
		**/

		int volumicFiltering(int tMin, int tMax=std::numeric_limits<int>::max());

		/**
		  * @brief Connected opening  (experimental))
		**/

		int connectedOpening(FlatSE &se);
		int connectedOpening2(FlatSE &se);

		/**
		  * @brief Shape filtering
		**/


		int complexityFiltering(int tMin, int tMax);
        int compacityFiltering(int tMin, int tMax);
		int intensityFiltering(int tMin, int tMax);

		int boundingBoxFiltering(int min, int max);

		//Experimental 16/07/07
		//N most brightest leafs
		vector<Node *> intensityPruning(int N);


		void setFalse();

		/**
		  * @brief Return the signature of a branch at point (x,y)
		  * (i.e. the value of all attributes along the path from root to the node containing (x,y))
		**/

		typedef std::map<int, Node *> SignatureType;
		SignatureType branchSignature(TCoord x, TCoord y);
		SignatureType branchSignature(Node *node);
		void activeBranch(Node *leaf);
		void activeBranch(SignatureType &signature);
		int writeSignature(SignatureType &signature, const char *file);

	//private:

		void erase_tree();

		//Helper functions for filtering
		std::vector <TOffset > merge_pixels(Node *tree);
		std::vector <TOffset > merge_pixelsFalseNodes(Node *tree);
		void merge_pixels(Node *tree, std::vector <TOffset > &res);

		bool isInclude(FlatSE &se, Node::ContainerPixels &pixels);

		Node * coordToNode(TCoord x, TCoord y);
		Node * coordToNode(TCoord x, TCoord y, TCoord z);

                Node * indexedCoordToNode(TCoord x, TCoord y, TCoord z, std::vector<Node *> &nodes);
                std::vector <Node *> indexedNodes();

		Node * offsetToNode(TOffset offset);

		void constructImageMin(Image <T> &res);
		void constructImageMax(Image<T> &res);
		void constructImageDirect(Image<T> &res);
		void constructImageDirectExpe(Image<T> &res);

        template<class TVal>
        TVal getAttribute(Node *n, Attribute attribute_id);
        template<class TVal, class TSel>
        void constructImageAttributeMin(Image<TVal> &res, Attribute value_attribute, Attribute selection_attribute);
        template<class TVal, class TSel>
        void constructImageAttributeMax(Image<TVal> &res, Attribute value_attribute, Attribute selection_attribute);
        template<class TVal>
        void constructImageAttributeDirect(Image<TVal> &res, Attribute value_attribute);

        template<class TVal, class TSel, class TLimit>
        void constructImageAttributeMin(Image<TVal> &res, Attribute value_attribute, Attribute selection_attribute, Attribute limit_attribute, TLimit limit_min, TLimit limit_max);
        template<class TVal, class TSel, class TLimit>
        void constructImageAttributeMax(Image<TVal> &res, Attribute value_attribute, Attribute selection_attribute, Attribute limit_attribute, TLimit limit_min, TLimit limit_max);
        template<class TVal, class TLimit>
        void constructImageAttributeDirect(Image<TVal> &res, Attribute value_attribute, Attribute limit_attribute, TLimit limit_min, TLimit limit_max);


		void constructNode(Image <T> &res, Node *node);
		void constructNodeDirect(Image <T> &res, Node *node);

		//Experimental 16/07/07
		void constructBranch(Image <T> &res,Node *leaf);

		// Internal structure
		// root node

		Node *m_root;
		//TSize *m_size;

		//original data
		Image<T> m_img;

		//STATUS image
		Image <int> STATUS;

		//max-tree index
        IndexType index;
    
        //hmin
        int hMin;
        int hToIndex(int h)  {return h-hMin;}
        int indexToH(int h)  {return h+hMin;}
};

/** @brief Abstract class for strategy to compute component tree
  *	Abstract class encapsulating the various strategies to compute Max-tree
  *
  *
**/
template <class T>
class ComponentTreeStrategy {
	public:

	ComponentTreeStrategy() {};
	virtual ~ComponentTreeStrategy() {};

	virtual Node *computeTree()=0;
	virtual void computeAttributes(Node *tree)=0;



};

/** @brief Salembier recursive implementation
 **/

template <class T>
class SalembierRecursiveImplementation: public ComponentTreeStrategy <T> {
	public:
	SalembierRecursiveImplementation(ComponentTree <T> *parent, FlatSE &connexity)
	:m_parent(parent)
		{
		this->totalNodes=0;
		this->init(m_parent->m_img, connexity);
		}
	~SalembierRecursiveImplementation()
		{
		delete[] hq;
		}

    Node *computeTree();
    void computeAttributes(Node *tree);
    void computeAttributes(Node *tree, unsigned int delta);
    void computeAttributes(Node *tree, ComputedAttributes ca, unsigned int delta);

    int64_t computeArea(Node *tree);
    void computeAreaDerivative(Node *tree);
    void computeAreaDerivative2(Node *tree);
    void computeMSER(Node *tree, unsigned int delta);

    int64_t computeSum(Node *tree);
    int64_t computeSumSquare(Node *tree);
    void computeMean(Node *tree);
    void computeVariance(Node *tree);
    void computeOtsu(Node *tree);

	int computeContrast(Node *tree);
    int computeVolume(Node *tree);
    void computeBorderGradient(Node *tree);
    int64_t computeSubNodes(Node *tree);

	long double computeM01(Node *tree);
	long double computeM10(Node *tree);
	long double computeM02(Node *tree);
	long double computeM20(Node *tree);
	void computeInertiaMoment(Node *tree);

	//Shape-based attributes
    int computeContour(bool save_pixels=false);
	int computeComplexityAndCompacity(Node *tree);

	int computeBoundingBox(Node *tree);
    
    int hToIndex(int h)  {return h-hMin;}
    int indexToH(int h)  {return h+hMin;}

	private:
		//Helper functions
		inline void update_attributes(Node *n, TOffset &imBorderOffset);
		inline int flood(int m) ;
		void link_node(Node *tree, Node *child) ;
		Node *new_node(int h, int n)  ;

		void init(Image <T> &img, FlatSE &connexity) ;

		

        //members
        Image <T> imBorder;
        Image <T> imGradient;
		FlatSE se;
		TSize oriSize[3];

		static const T BORDER=T(0);
		static const int NOT_ACTIVE=-1;
		static const int ACTIVE=-2;
		static const int BORDER_STATUS=-3;
		TCoord back[3];
		TCoord front[3];
		/** @brief Hierarchical queue
		**/
		typedef std::queue<TOffset> * HierarchicalQueue;

		 HierarchicalQueue hq;

		 T hMin;
		 T hMax;
		// size of different containers
		 int numberOfLevels;

		  int totalNodes;

		 Image <int> STATUS;
		 vector <int> number_nodes;
		 vector <bool> node_at_level;
		// For now, container for accessing nodes by level and cc number
		//typedef std::map <T, std::map<TLabel,  Node *> > IndexType;
		//typedef Node *** IndexType;
    public:
		IndexType index;

		ComponentTree<T> *m_parent;
};

/** @brief Union-Find implementation (Najman-Couprie)
 **/


template <class T>
class UnionFindImplementation: public ComponentTreeStrategy <T> {




};




/*@}*/

}//end namespace

#include "ComponentTree.hxx"
#endif
