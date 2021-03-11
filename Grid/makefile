COMP = g++-10

GOOGLETEST_LIB = /usr/src/googletest/googletest/gcc-10-build/lib/
GOOGLETEST_INCLUDE = /usr/src/googletest/googletest/include/

#put all your object files here
OBJS = tests.o main.o 
#The executabel filename DON'T CHANGE
EXEC = ex4
CPP_COMP_FLAG = -std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG -I$(GOOGLETEST_INCLUDE) 
CPP_LINK_FLAG = -L$(GOOGLETEST_LIB) -lgtest -lgtest_main -pthread

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

#a rule for building a simple c++ source file
#use g++ -MM main.cpp to see dependencies
tests.o: tests.cpp Grid.h
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $*.cpp

clean:
	rm -f $(OBJS) $(EXEC)
