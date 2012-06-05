include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
LDFLAGS		+= -shared -g
LLIB 		 = -lrt

OBJ = EPerf.o EPerfC.o EPerfCallbacks.o EPerfF.o
INC = $(wildcard ../include/eperf/*.h)

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
