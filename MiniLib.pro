TEMPLATE = app
CONFIG += console
CONFIG -= qt

LIBS += \
	-lSDL \
	-lSDLmain

SOURCES += main.cpp \
    MGL/mglTransform.cpp \
    MGL/mglTexture.cpp \
    MGL/mglRasterizer.cpp \
    MGL/mglPlane.cpp \
    MGL/mglObject.cpp \
    MGL/mglModel.cpp \
    MGL/mglEngine.cpp \
    MGL/mglCollider.cpp \
    MGL/mglCamera.cpp \
    MTL/mtlString.cpp \
    MTL/mtlParser.cpp \
    MTL/mtlDirectory.cpp \
    SDL_Engine.cpp

HEADERS += \
    MGL/mglTransform.h \
    MGL/mglTexture.h \
    MGL/mglRay.h \
    MGL/mglRasterizer.h \
    MGL/mglPlane.h \
    MGL/mglObject.h \
    MGL/mglModel.h \
    MGL/mglFramebuffer.h \
    MGL/mglEngine.h \
    MGL/mglColor.h \
    MGL/mglCollider.h \
    MGL/mglCamera.h \
    MML/mmlVector.h \
    MML/mmlQuaternion.h \
    MML/mmlMatrix.h \
    MML/mmlMath.h \
    MML/mmlFixed.h \
    MTL/mtlVersion.h \
    MTL/mtlString.h \
    MTL/mtlParser.h \
    MTL/mtlList.h \
    MTL/mtlDuplex.h \
    MTL/mtlDirectory.h \
    MTL/mtlAsset.h \
    MTL/mtlArray.h \
    SDL_Engine.h \
    MGL/mglInput.h

