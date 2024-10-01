CXX=/opt/homebrew/opt/llvm/bin/clang++ -Wno-undefined-var-template
CPPFLAGS := -MMD -MP
#CXXFLAGS=--std=c++2b -O0 -g -fopenmp -I/opt/homebrew/opt/llvm/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers/
CXXFLAGS=--std=c++2b -O3 -fopenmp -I/opt/homebrew/opt/llvm/include -I/Library/Developer/CommandLineTools/SDKs/MacOSX14.4.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers/

LDFLAGS=-framework Accelerate -L/opt/homebrew/opt/llvm/lib -lomp
SOURCES := $(wildcard *.cpp)

all: test

-include $(SOURCES:.cpp=.d)

test: matrix.o matmul.o test.o 
	clang++ $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f test *.o *.d

