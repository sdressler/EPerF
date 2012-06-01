include ../Makefile.inc

TARGET		 = eperf

CXXFLAGS	+= -fPIC
LDFLAGS		+= -shared -g
LLIB 		 = -lrt

OBJ = $(TARGET).o eperf_callbacks.o ceperf.o
INC = $(wildcard ../include/eperf/*.h)

%.o : %.cpp ${INC}
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c ${INC}
	$(CC) $(CXXFLAGS) -c $<

all: $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o lib$(TARGET).so $(LLIB)
	# FORTRAN Module
	$(CF) $(CFFLAGS) -c modeperf.F90
	mv modeperf.mod ../include/eperf

clean:
	rm -f lib$(TARGET).so $(OBJ) modeperf.o
