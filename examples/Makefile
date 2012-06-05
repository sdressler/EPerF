include ../Makefile.inc

TARGET_CPP	= cpp_example
TARGET_MDEV	= multidevice
TARGET_C	= c_example
TARGET_F90	= f_example
LLIB		= -L../lib -leperf

FFLAGS		+= -I../include/eperf

OBJ_CPP		= cpp_example.o
OBJ_MDEV	= multidevice.o
OBJ_C		= c_example.o
OBJ_F90		= f_example.o

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c
	$(CC) $(CXXFLAGS) -c $<

%.o : %.F90
	$(FC) $(FFLAGS) -c $<

all: $(TARGET_CPP) $(TARGET_C) $(TARGET_F90) $(TARGET_MDEV)

only-c: $(TARGET_C)

only-cpp: $(TARGET_CPP)

only-mdev: $(TARGET_MDEV)

only-f90: $(TARGET_F90)

$(TARGET_CPP) : $(OBJ_CPP)
	$(CXX) $(LDFLAGS) $(OBJ_CPP) -o $(TARGET_CPP) $(LLIB) 

$(TARGET_C) : $(OBJ_C)
	$(CC) $(LDFLAGS) $(OBJ_C) -o $(TARGET_C) $(LLIB)

$(TARGET_MDEV) : $(OBJ_MDEV)
	$(CXX) $(LDFALGS) $(OBJ_MDEV) -o $(TARGET_MDEV) $(LLIB)

$(TARGET_F90) : $(OBJ_F90)
	$(FC) $(FFLAGS) $(OBJ_F90) -o $(TARGET_F90) $(LLIB)

run: $(TARGET_CPP) $(TARGET_C)
	@echo "==========="
	@echo "CPP Example"
	@echo "===========\n"
	LD_LIBRARY_PATH=../lib ./$(TARGET_CPP)
	@echo "========="
	@echo "C Example"
	@echo "=========\n"
	LD_LIBRARY_PATH=../lib ./$(TARGET_C)
	@echo "==============="
	@echo "FORTRAN Example"
	@echo "===============\n"
	LD_LIBRARY_PATH=../lib ./$(TARGET_F90)

clean:
	rm -f $(TARGET_CPP) $(TARGET_C) $(OBJ_CPP) $(OBJ_C) $(TARGET_F90) $(OBJ_F90) $(TARGET_MDEV) $(OBJ_MDEV)
