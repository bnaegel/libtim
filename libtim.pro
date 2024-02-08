#-------------------------------------------------
#
# Project created by QtCreator 2022
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = libtim
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        GUILibTIM/main.cpp \
        GUILibTIM/guilibtim.cpp \
        GUILibTIM/qgraphicsviewclick.cpp \
    Algorithms/random-singleton.cpp \
    Common/tinyxml/tinystr.cpp \
    Common/tinyxml/tinyxml.cpp \
    Common/tinyxml/tinyxmlerror.cpp \
    Common/tinyxml/tinyxmlparser.cpp


HEADERS += \
        GUILibTIM/guilibtim.h \
        GUILibTIM/qgraphicsviewclick.h \
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

FORMS += \
        GUILibTIM/guilibtim.ui
