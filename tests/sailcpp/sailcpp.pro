include(../tests.pri)
TARGET = tst_sailcpp

SOURCES += \
    ../../src/geocpp/matrix.cpp \
    ../../src/geocpp/matrix4x4.cpp \
    ../../src/geocpp/rect.cpp \
    ../../src/geocpp/subspace.cpp \
    ../../src/geocpp/vector.cpp \
    ../../src/sailcpp/panel.cpp \
    ../../src/sailcpp/sailcalc.cpp \
    ../../src/sailcpp/saildef.cpp \
    ../../src/sailcpp/sailworker.cpp \
    tst_sailcpp.cpp
