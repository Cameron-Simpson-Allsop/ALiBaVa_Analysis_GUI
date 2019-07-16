TEMPLATE = app
TARGET = ALiBaVa_Reader

QT = core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

INCLUDEPATH +=/usr/include/root \
"/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros" \
/home/aliuser/Documents/boost_1_67_0 \
/home/aliuser/Documents/qcustomplot

LIBS += -L"/usr/lib64/root" \
-lCore \
#-lCint \
-lRIO \
-lNet \
-lHist \
-lGraf \
-lGraf3d \
-lGpad \
-lTree \
-lRint \
-lPostscript \
-lMatrix \
-lPhysics \
-lMathCore \
-lThread \
-pthread \
-lm \
-ldl \
-rdynamic \
-lboost_system \
-lboost_filesystem \
-lRooFit \
-lRooStats \
-lRooFitCore \
-lFoam \
-lMinuit \
-lTreePlayer \
-lHistFactory

SOURCES += \
    main.cpp \
    window.cpp \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/DataFileRoot.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/HDFRoot.cc" \
    "/home/aliuser/Documents//ALiBaVaReaderGUI/root_macros/AsciiRoot.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/ChanList.cc"  \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/Tracer.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/utils.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/Hit.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/root_macros/crrc.cc" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/AtlasLabels.C" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/AtlasStyle.C" \
    "/home/aliuser/Documents/ALiBaVaReaderGUI/AtlasUtils.C" \
    /home/aliuser/Documents/qcustomplot/qcustomplot.cpp


HEADERS += \
    window.h \
    /home/aliuser/Documents/qcustomplot/qcustomplot.h
