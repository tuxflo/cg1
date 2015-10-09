TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lGLU -lGL -lglut -lGLEW

SOURCES += main.cpp \
    LoadShaders.cpp

OTHER_FILES += \
    vertex.vert \
    fragment.frag

