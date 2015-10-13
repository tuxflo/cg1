TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGLU -lGL -lglut -lGLEW -lglfw -lX11 -lXxf86vm -lXrandr -lpthread -lXi

SOURCES += main.cpp \
    LoadShaders.cpp \
    controls.cpp

OTHER_FILES += \
    vertex.vert \
    fragment.frag

HEADERS += \
    LoadShaders.h \
    controls.h

