include ../Makefile.inc

TARGET_CPP	= cpp_example
TARGET_MDEV	= multidevice

ifeq ($(WITH_CAPI), yes)
TARGET_C	= c_example
else
TARGET_C	=
endif

ifeq ($(WITH_FAPI), yes)
TARGET_F90	= f_example
else
TARGET_F90	=
endif

LLIB		= -L../lib -leperf -lcrypto

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

$(TARGET_CPP) : $(OBJ_CPP)
	$(CXX) $(LDFLAGS) $(OBJ_CPP) -o $(TARGET_CPP) $(LLIB) 

$(TARGET_C) : $(OBJ_C)
	$(CC) $(LDFLAGS) $(OBJ_C) -o $(TARGET_C) $(LLIB)

$(TARGET_MDEV) : $(OBJ_MDEV)
	$(CXX) $(LDFALGS) $(OBJ_MDEV) -o $(TARGET_MDEV) $(LLIB)

$(TARGET_F90) : $(OBJ_F90)
	$(FC) $(FFLAGS) $(OBJ_F90) -o $(TARGET_F90) $(LLIB)

clean:
	rm -f $(TARGET_CPP) $(TARGET_C) $(OBJ_CPP) $(OBJ_C) $(TARGET_F90) $(OBJ_F90) $(TARGET_MDEV) $(OBJ_MDEV)
