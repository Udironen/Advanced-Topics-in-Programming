COMP = g++-10

GOOGLE_LIB = /usr/src/googletest/googletest/gcc-10-build/lib/
GOOGLE_TEST_INCLUDE = /usr/src/googletest/googletest/include/

PROJECT_DIR := ../GIS
SOURCE_FILES := $(notdir $(shell find $(PROJECT_DIR) -name '*.cpp')) tests.cpp main.cpp
OBJS_FILES := $(SOURCE_FILES:%.cpp=%.o)
BIN_DIR := bin
OBJS := $(addprefix $(BIN_DIR)/, $(OBJS_FILES))
EXEC := ex1
CPP_COMP_FLAG := -std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG -I$(GOOGLE_TEST_INCLUDE) 
CPP_LINK_FLAG := -L$(GOOGLE_LIB) -lgtest -lgtest_main -pthread

all: $(BIN_DIR) $(EXEC)

$(BIN_DIR):
	mkdir -p $(BIN_DIR) && echo $(OBJS_FILES) 

$(EXEC): $(OBJS)
	$(COMP) $(OBJS) $(CPP_LINK_FLAG) -o $@

$(BIN_DIR)/GIS.o: $(PROJECT_DIR)/GIS.cpp ../Common/GISRegistration.h $(PROJECT_DIR)/GridEntity.h $(PROJECT_DIR)/GridWay.h $(PROJECT_DIR)/IdEntityMap.h ../Common/Restrictions.h ../Common/AbstractGIS.h
	$(COMP) $(CPP_COMP_FLAG) -D EX1_EX2 -c $< -o $@

$(BIN_DIR)/Way.o: $(PROJECT_DIR)/Way.cpp $(PROJECT_DIR)/Junction.h ../Common/CoordinatesMath.h ../Common/AbstractWay.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/POI.o: $(PROJECT_DIR)/POI.cpp $(PROJECT_DIR)/Circle.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Junction.o: $(PROJECT_DIR)/Junction.cpp $(PROJECT_DIR)/Entity.h 
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Circle.o: $(PROJECT_DIR)/Circle.cpp $(PROJECT_DIR)/Shape.h $(PROJECT_DIR)/Entity.h 
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/GridEntity.o: $(PROJECT_DIR)/GridEntity.cpp $(PROJECT_DIR)/BaseGrid.h $(PROJECT_DIR)/Junction.h $(PROJECT_DIR)/POI.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/GridWay.o: $(PROJECT_DIR)/GridWay.cpp $(PROJECT_DIR)/BaseGrid.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/IdEntityMap.o: $(PROJECT_DIR)/IdEntityMap.cpp $(PROJECT_DIR)/Junction.h $(PROJECT_DIR)/Way.h $(PROJECT_DIR)/POI.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/BaseGrid.o: $(PROJECT_DIR)/BaseGrid.cpp $(PROJECT_DIR)/Cell.h $(PROJECT_DIR)/Way.h ../Common/CoordinatesMath.h ../UserCommon/Hashers.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/tests.o: tests.cpp mocks.h $(PROJECT_DIR)/GISProvider.h ../Common/CoordinatesMath.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/main.o: main.cpp
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(OUTPUT_SHARED_OBJ)


