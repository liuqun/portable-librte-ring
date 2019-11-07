TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += thread

HEADERS += \
        error.h \
        cpu.h

SOURCES += \
        error.c \
        linux_cpu.c \
        main.c

unix: LIBS += -L$$OUT_PWD/../librte_ring/ -lrte_ring

INCLUDEPATH += $$PWD/../librte_ring
DEPENDPATH += $$PWD/../librte_ring

unix: PRE_TARGETDEPS += $$OUT_PWD/../librte_ring/librte_ring.a

unix {
    LIBS += -latomic
}
