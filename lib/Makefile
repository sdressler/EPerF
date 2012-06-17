include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
LDFLAGS		+= -shared
LLIB		= -lrt -ldb_cxx

OBJ = EPerfDevice.o EPerfKernel.o EPerfData.o \
	  EPerfClock.o EPerfContainer.o EPerf.o \
	  EPerfC.o EPerfCallbacks.o \
	  EPerfF.o

INC = $(wildcard ../include/EPerf/*.h)

%.o : %.cpp ${INC}
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c ${INC}
	$(CC) $(CXXFLAGS) -c $<

%.o : %.F90
	$(FC) $(FFLAGS) -c $<

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o lib$(TARGET).so $(LLIB)

clean:
	rm -f lib$(TARGET).so $(OBJ) ../include/eperf/m_eperf.mod
