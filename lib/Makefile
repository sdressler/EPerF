include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
LDFLAGS		+= -shared -g
LLIB 		 = -lrt

SERDIR = Serialization

OBJ = EPerf.o \
	  EPerfC.o EPerfCallbacks.o \
	  EPerfF.o \
	  $(SERDIR)/JSON.o

INC = $(wildcard ../include/eperf/*.h)

%.o : %.cpp ${INC}
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c ${INC}
	$(CC) $(CXXFLAGS) -c $<

%.o : %.F90
	$(FC) $(FFLAGS) -c $<

$(SERDIR)/%.o : $(SERDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o lib$(TARGET).so $(LLIB)

clean:
	rm -f lib$(TARGET).so $(OBJ) ../include/eperf/m_eperf.mod
