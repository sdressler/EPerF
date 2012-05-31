include ../Makefile.inc

TARGET_CPP	= cpp_example
TARGET_C	= c_example
LLIB		= -L../lib -leperf

OBJ_CPP		= cpp_example.o
OBJ_C		= c_example.o

%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $<

%.o : %.c
	$(CC) $(CXXFLAGS) -c $<

all: $(TARGET_CPP) $(TARGET_C)

only-c: $(TARGET_C)

only-cpp: $(TARGET_CPP)

$(TARGET_CPP) : $(OBJ_CPP)
	$(CXX) $(LDFLAGS) $(OBJ_CPP) -o $(TARGET_CPP) $(LLIB) 

$(TARGET_C) : $(OBJ_C)
	$(CC) $(LDFLAGS) $(OBJ_C) -o $(TARGET_C) $(LLIB)

run: $(TARGET_CPP) $(TARGET_C)
	@echo "==========="
	@echo "CPP Example"
	@echo "===========\n"
	LD_LIBRARY_PATH=../lib ./$(TARGET_CPP)
	@echo "========="
	@echo "C Example"
	@echo "=========\n"
	LD_LIBRARY_PATH=../lib ./$(TARGET_C)

clean:
	rm -f $(TARGET_CPP) $(TARGET_C) $(OBJ_CPP) $(OBJ_C)
