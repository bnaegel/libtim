TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    Algorithms/random-singleton.cpp \
    Common/tinyxml/tinystr.cpp \
    Common/tinyxml/tinyxml.cpp \
    Common/tinyxml/tinyxmlerror.cpp \
    Common/tinyxml/tinyxmlparser.cpp \
    Examples/preprocess_nenist.cpp

HEADERS += \
    Algorithms/AdaptativeSE.h \
    Algorithms/AdaptativeSE.hxx \
    Algorithms/ColorProcessing.h \
    Algorithms/ColorProcessing.hxx \
    Algorithms/ComponentTree.h \
    Algorithms/ComponentTree.hxx \
    Algorithms/ConnectedComponents.h \
    Algorithms/ConnectedComponents.hxx \
    Algorithms/DistanceTransform.h \
    Algorithms/DistanceTransform.hxx \
    Algorithms/KMeans.h \
    Algorithms/KMeans.hxx \
    Algorithms/Misc.h \
    Algorithms/Misc.hxx \
    Algorithms/Morphology.h \
    Algorithms/Morphology.hxx \
    Algorithms/random-singleton.h \
    Algorithms/RegionGrowing.h \
    Algorithms/RegionGrowing.hxx \
    Algorithms/Tarjan.h \
    Algorithms/Tarjan.hxx \
    Algorithms/TemplateMatching.h \
    Algorithms/TemplateMatching.hxx \
    Algorithms/Thresholding.h \
    Algorithms/Thresholding.hxx \
    Algorithms/ViscousWatershed.h \
    Algorithms/ViscousWatershed.hxx \
    Algorithms/Watershed.h \
    Algorithms/Watershed.hxx \
    Common/tinyxml/tinystr.h \
    Common/tinyxml/tinyxml.h \
    Common/FlatSE.h \
    Common/FlatSE.hxx \
    Common/Histogram.h \
    Common/Histogram.hxx \
    Common/Image.h \
    Common/Image.hxx \
    Common/ImageIO.hxx \
    Common/ImageIO_InrGz.hxx \
    Common/ImageIterators.h \
    Common/NonFlatSE.h \
    Common/NonFlatSE.hxx \
    Common/OrderedQueue.h \
    Common/Point.h \
    Common/Types.h
