TEMPLATE = app
TARGET = ParticlePanic
CONFIG += c++11
CONFIG += opengl
INCLUDEPATH += .
OBJECTS_DIR = obj

SOURCES += src/main.cpp \
    src/Vec3.cpp \
    src/Mat3.cpp \
    src/Particle.cpp \
    src/World.cpp

HEADERS +== \
    include/Particle.h \
    include/Vec3.h \
    include/Mat3.h \
    include/World.h

LIBS += -L/usr/local/lib

linux: {
  LIBS += -lSDL2 -lglut -lGLU -lGL
}

macx: {
  DEFINES+=MAC_OS_X_VERSION_MIN_REQUIRED=1060
  QMAKE_LFLAGS += -F/Library/Frameworks
  LIBS += -framework SDL2
  LIBS += -framework SDL2_image
  INCLUDEPATH += /Library/Frameworks/SLD2_image.framework
  INCLUDEPATH += /Library/Frameworks/SDL2.framework/Headers
  INCLUDEPATH += /usr/local/include

  LIBS+= -framework OpenGL
}
