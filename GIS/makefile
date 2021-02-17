COMP = g++-10

SOURCE_FILES := $(notdir $(shell find . -name '*.cpp'))
OBJS_FILES := $(SOURCE_FILES:%.cpp=%.o)
BIN_DIR := bin
OBJS := $(addprefix $(BIN_DIR)/, $(OBJS_FILES))
SHARED_OBJ := GIS_313452542.so
OUTPUT_DIR := ../../OUTPUT/GIS
OUTPUT_SHARED_OBJ := $(OUTPUT_DIR)/$(SHARED_OBJ)

CPP_COMP_FLAG := -std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
CPP_LINK_FLAG := -shared 

all: $(BIN_DIR) $(OUTPUT_DIR) $(OUTPUT_SHARED_OBJ)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(OUTPUT_SHARED_OBJ): $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^ #$(addprefix $(BIN_DIR)/, $^)

# Make sure to have all your dependencies for each .o file creation rule

$(BIN_DIR)/GIS.o: GIS.cpp ../Common/GISRegistration.h GridEntity.h GridWay.h IdEntityMap.h ../Common/Restrictions.h ../Common/AbstractGIS.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Way.o: Way.cpp Junction.h ../Common/CoordinatesMath.h ../Common/AbstractWay.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/POI.o: POI.cpp Circle.h 
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Junction.o: Junction.cpp Entity.h #put dependencies here!
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Circle.o: Circle.cpp Entity.h Shape.h 
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/GridEntity.o: GridEntity.cpp BaseGrid.h Junction.h POI.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/GridWay.o: GridWay.cpp BaseGrid.h 	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/IdEntityMap.o: IdEntityMap.cpp Junction.h Way.h POI.h ../UserCommon/Hashers.h	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/BaseGrid.o: BaseGrid.cpp Cell.h Way.h ../Common/CoordinatesMath.h ../UserCommon/Hashers.h	
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(OUTPUT_SHARED_OBJ)
