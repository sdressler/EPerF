include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
<<<<<<< HEAD
LDFLAGS		+= -shared -g
LLIB 		 = -lrt -lcrypto

SERDIR = Serialization

OBJ = EPerf.o \
	  EPerfC.o EPerfCallbacks.o \
	  EPerfF.o \
	  $(SERDIR)/JSON.o

INC = $(wildcard ../include/eperf/*.h)
=======
LDFLAGS		+= -shared
#LLIB 		 = -Wl,-Bstatic -lrt -ldb_cxx -Wl,-Bdynamic
LLIB		= -lrt -ldb_cxx

#SERDIR = Serialization

OBJ = EPerfDevice.o EPerfKernel.o EPerfData.o \
	  EPerfClock.o EPerfContainer.o EPerf.o \
	  EPerfC.o EPerfCallbacks.o \
	  EPerfF.o
#	  $(SERDIR)/JSON.o

INC = $(wildcard ../include/EPerf/*.h)
>>>>>>> db

%.o : %.cpp ${INC}
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c ${INC}
	$(CC) $(CXXFLAGS) -c $<

%.o : %.F90
	$(FC) $(FFLAGS) -c $<

<<<<<<< HEAD
$(SERDIR)/%.o : $(SERDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
=======
#$(SERDIR)/%.o : $(SERDIR)/%.cpp
#	$(CXX) $(CXXFLAGS) -c -o $@ $<
>>>>>>> db

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o lib$(TARGET).so $(LLIB)

clean:
	rm -f lib$(TARGET).so $(OBJ) ../include/eperf/m_eperf.mod
