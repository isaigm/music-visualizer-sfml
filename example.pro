TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        App.cpp \
        Bar.cpp \
        FFTStream.cpp \
        imgui-SFML.cpp \
        imgui.cpp \
        imgui_draw.cpp \
        imgui_tables.cpp \
        imgui_widgets.cpp \
        implot.cpp \
        implot_items.cpp \
        main.cpp

HEADERS += \
    App.h \
    Bar.h \
    FFTStream.h \
    Utils.h \
    imconfig-SFML.h \
    imconfig.h \
    imfilebrowser.h \
    imgui-SFML.h \
    imgui-SFML_export.h \
    imgui.h \
    imgui_internal.h \
    implot.h \
    implot_internal.h \
    imstb_rectpack.h \
    imstb_textedit.h \
    imstb_truetype.h
LIBS +=  -lsfml-window -lsfml-system -lsfml-graphics -lsfml-audio -lfftw3 -lGL
CONFIG += c++17
