include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
LDFLAGS		+= -shared
LLIB		= -lrt -ldb_cxx

OBJ_LIB		= EPerfDevice.o EPerfKernelConf.o EPerfKernel.o \
			  EPerfData.o EPerfClock.o EPerfContainer.o EPerf.o

ifeq ($(WITH_CAPI), yes)
    OBJ_CAPI = EPerfC.o EPerfCallbacks.o
else
    OBJ_CAPI =
endif

ifeq ($(WITH_FAPI), yes)
    OBJ_FAPI = EPerfF.o
else
    OBJ_FAPI =
endif

INC = $(wildcard ../include/EPerf/*.h)

%.o : %.cpp ${INC}
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c ${INC}
	$(CC) $(CXXFLAGS) -c $<

%.o : %.F90
	$(FC) $(FFLAGS) -c $<

all: $(OBJ_LIB) $(OBJ_CAPI) $(OBJ_FAPI)
	$(CXX) $(LDFLAGS) $(OBJ_LIB) $(OBJ_CAPI) $(OBJ_FAPI) -o lib$(TARGET).so $(LLIB)

clean:
	rm -f lib$(TARGET).so $(OBJ_LIB) $(OBJ_CAPI) $(OBJ_FAPI) ../include/eperf/m_eperf.mod
