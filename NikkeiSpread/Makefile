all : BUILD_OBJ

DATE=$(shell echo `date +'%Y%m%d'`)
TIME=$(shell echo `date +'%H%M%S'`)
VERSION=$(shell cat version.txt)


PLATFORM = AS5_64
C++ = gcc -rdynamic
C++FLAGS= -g $(INC) -Wreturn-type -Wall -O2 -Wno-deprecated

ifeq ($(PLATFORM),AS5)
C++FLAGS += -m32 -fPIC
LDFLAGS = -m32 -shared
endif

ifeq ($(PLATFORM),AS5_64)
C++FLAGS += -fPIC
LDFLAGS = -shared
endif

ifeq ($(PLATFORM),SUN10_64)
C++ = gcc
C++FLAGS += -m64 -fPIC
LDFLAGS += -m64
endif

DEFINE  =       -DFOR_UNIX
DEFINE += -DBUILD_DATE="\"$(DATE)\"" -DBUILD_TIME="\"$(TIME)\""
DEFINE += -DBUILD_VERSION="\"$(VERSION)\""


INC += -I./ -I./$(PLATFORM)  -I/$(FLEXSYS)/omstrats/Common/boost_1_46_1
INC	+= -I./include
INC	+= -I$(PLATFORM)
INC	+= -I$(FLEXAPP)/flex/apinew/include

#RULE	= OnEvent
COMMON	= $(PLATFORM)/RECommon.h
EXPORT	= $(PLATFORM)/REHostExport.h


:
OM = $(FLEXSYS)/flex/bin/OM_flex_new

MAKEDIR:
	-mkdir $(PLATFORM)

$(COMMON): 
	$(OM) RulesEngineCmnInclude > $@

$(EXPORT):
	$(OM) RulesEngineHostInclude > $@

#$(RULE).cpp : $(RULE).rule
#	ln -s $(RULE).rule $(RULE).cpp

OBJ		= NikkeiSpread.o

%.o: %.cpp
	$(C++) -c $(C++FLAGS) $(INC) $(DEFINE) -O2 $<  -o $(PLATFORM)/$@

CAL_SPD.so: MAKEDIR $(COMMON) $(EXPORT) $(OBJ) 
	cd $(PLATFORM);$(C++) $(LDFLAGS) $(OBJ) -o $@

clean:
	$(RM) $(COMMON)
	$(RM) $(EXPORT)
	$(RM) -rf $(PLATFORM)

BUILD_OBJ: clean MAKEDIR $(COMMON) $(EXPORT) $(OBJ)

