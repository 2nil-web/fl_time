
UNAME=$(shell uname)

SRCS=fl_time_test_ui.cpp fl_time_test.cpp utils.cpp Fl_Time_Input.cpp

ifeq ($(findstring NT-, $(UNAME)),)
#FLTK_DIR=/opt/fltk-1.5
FLTK_DIR=/opt/fltk-1.4.4
EXEXT=
TARGET_DIR=build/gcc/linux
BUILD_SYS=gcc
#CXXFLAGS += $(shell pkg-config --cflags gtk4)
#LDFLAGS += $(shell pkg-config --libs gtk4)
else
MSBUILD=/c/Program\ Files/Microsoft\ Visual\ Studio/18/Community/MSBuild/Current/Bin/amd64/MSBuild.exe	
MAGICK=magick
EXEXT=.exe
FLTK_DIR=/c/fltk-1.4.4
TARGET_DIR=build/msvc/win
BUILD_SYS=msvc
endif

ifeq ($(MAKECMDGOALS),gcc)
undefine MSBUILD
MAGICK=magick
EXEXT=.exe
FLTK_DIR=/ucrt64
TARGET_DIR=build/gcc/win
BUILD_SYS=gcc
ifeq ($(MSYSTEM),CLANG64)
CPPFLAGS += -I /clang64/include/c++
CXX=clang++
endif
LDFLAGS += -static
#LDFLAGS += -static-libgcc -static-libstdc++
CPPFLAGS += -DUNICODE -D_UNICODE 
endif

FLTK_CONFIG=${FLTK_DIR}/bin/fltk-config
FLUID=${FLTK_DIR}/bin/fluid
PATH:=${FLTK_DIR}/bin:${PATH}
TARGET=${TARGET_DIR}/fl_time_test${EXEXT}

all : ${TARGET}

gcc : ${TARGET}

# FLUID file rules
fl_time_test_ui.h fl_time_test_ui.cpp : fl_time_test_ui.fl
	@echo "Fluid Gen"
	${FLUID} -c -o .cpp $<

fl_time_test.cpp file_features.cpp edit_features.cpp : fl_time_test_ui.h fl_time_test_icon.h fl_time_test.ico

#	@fold -w 253 fl_time_test.svg | sed -e 's/"/\\"/g;s/\(.*\)/"\1" \\/' >>fl_time_test_icon.h
fl_time_test_icon.h : fl_time_test.svg
	@echo -n "const char *svg_data=" >$@
	@sed -e 's/"/\\"/g;s/\(.*\)/"\1" \\/' $< >>$@
	@echo ";" >>$@

fl_time_test.ico : fl_time_test.svg
	${MAGICK} -density 256x256 -background none $< -define icon:auto-resize=128,96,64,48,32,16 -colors 256 $@
#	${MAGICK} $< -density 300 -define icon:auto-resize=128,96,64,48,32,16 -background none $@


ifeq ($(BUILD_SYS),msvc)
OBJS=$(SRCS:.cpp=.obj)
OBJS:=$(addprefix  ${TARGET_DIR}/,${OBJS})
MSVC_SLN=fl_time_test.slnx
${TARGET} : ${SRCS}
	@${MSBUILD} ${MSVC_SLN} -p:Configuration=Release
	@echo "${TARGET} OK"
else
OBJS=$(SRCS:.cpp=.o)
OBJS:=$(addprefix  ${TARGET_DIR}/,${OBJS})
CC  = $(shell ${FLTK_CONFIG} --cc)
CXX = $(shell ${FLTK_CONFIG} --cxx)
CXXFLAGS += -std=c++23
CXXFLAGS += -Wall -pedantic -Wextra # Utiliser ces 2 dernières options de temps en temps peut-être utile ...
CXXFLAGS += $(shell ${FLTK_CONFIG} --use-gl --use-images --cxxflags )
# Optim
#CXXFLAGS += -Oz
#LDFLAGS += -fno-rtti
#LDLIBS = $(shell ${FLTK_CONFIG} --use-gl --use-images --ldflags )
LDLIBS += $(shell ${FLTK_CONFIG} --use-gl --use-images --ldstaticflags | sed 's/.dll//g' )
LINK     = $(CXX)
${TARGET} : ${OBJS}
	$(LINK.cc) ${OBJS} $(LOADLIBES) $(LDLIBS) -o $@
endif

ALL_SRCS=$(wildcard *.cpp) $(wildcard *.h)
format :
	@echo "Formatting with clang, the following files: ${ALL_SRCS}"
	@clang-format -style="{ BasedOnStyle: Microsoft, ColumnLimit: 256, IndentWidth: 2, TabWidth: 2, UseTab: Never }" --sort-includes -i ${ALL_SRCS}

cfg : 
	@echo "Building TARGET [${TARGET}] for system [${UNAME}] with built tool [${BUILD_SYS}]"
	@echo "fltk tools come from ${FLTK_DIR}"
	@which ${FLTK_CONFIG} ${FLUID}
	@echo "CPPFLAGS: ${CPPFLAGS}"
	@echo ${OBJS}
	@echo "LINK.cc: ${LINK.cc}"
	@echo "OBJS: ${OBJS}"
	@echo "LOADLIBES: ${LOADLIBES}"
	@echo "LDLIBS: ${LDLIBS}"

clean :
	 rm -rf build/msvc build/gcc
	 rm -f $(OBJS) $(SRCS:.cpp=.d) fl_time_test_ui.h fl_time_test_ui.cpp fl_time_test_icon.h *~

ifneq ($(MAKECMDGOALS),clean)
# Implicit rule for building dep file from .c
%.d: %.cpp
	@echo "Building "$@" from "$<
	@${CXX} ${CPPFLAGS} -MM $< -o $@
#	@$(COMPILE.c) -isystem /usr/include -MM $< > $@

${TARGET_DIR}/%.o: %.cpp
	@mkdir -p ${TARGET_DIR}
	$(COMPILE.cc) $(OUTPUT_OPTION) $<

# Inclusion of the dependency files '.d'
ifdef SRCS
-include $(SRCS:.cpp=.d)
endif
endif

