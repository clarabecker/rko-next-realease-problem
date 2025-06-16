# Detect the operating system
OS := $(shell uname -s)
ifeq ($(OS), Windows_NT)
    RM := del /Q /F
    EXE := .exe
else
    RM := rm -f
    EXE :=
endif

LDFLAGS=-L/opt/homebrew/opt/libomp/lib
CPPFLAGS=-I/opt/homebrew/opt/libomp/include

CCC = g++-15
TARGETDIR = .

CCFLAGS = -fopenmp -g -O3 -Wall -std=c++2a -Wno-unused-but-set-variable
ifeq ($(OS), Darwin)
	CCFLAGS = -Xpreprocessor -fopenmp -I/opt/homebrew/opt/libomp/include -L/opt/homebrew/opt/libomp/lib -lomp -g -O3 -Wall -std=c++2a -Wno-unused-but-set-variable
endif

TARGET = $(TARGETDIR)/runTest$(EXE)

debug: CCFLAGS += -g
release: CCFLAGS += -march=native

OBJS = $(TARGETDIR)/Main/main.o

# Rule to generate the final binary
$(TARGET): $(OBJS)
	$(CCC) $(CCFLAGS) -o $@ $(OBJS)

# Compilation of each object file
$(TARGETDIR)/Main/main.o: $(TARGETDIR)/Main/main.cpp
	$(CCC) $(CCFLAGS) -c $< -o $@

# Rule for running tests
test: $(TARGET)
	./$(TARGET)

# Cleaning rule
clean:
	$(RM) $(OBJS) $(TARGET)

# Rule for compiling in debug mode
debug: clean
	$(MAKE) $(TARGET) CCC=$(CCC)

# Rule to recompile
rebuild: 
	$(MAKE) debug
