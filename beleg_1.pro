TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGLU -lGL -lglut -lGLEW

SOURCES += main.cpp \
    LoadShaders.cpp \
    camera.cpp

OTHER_FILES += \
    vertex.vert \
    fragment.frag

HEADERS += \
    LoadShaders.h \
    camera.h

