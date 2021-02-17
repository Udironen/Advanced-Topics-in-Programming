COMP = g++-10

SOURCE_FILES := $(notdir $(shell find . -name '*.cpp'))
OBJS_FILES := $(SOURCE_FILES:%.cpp=%.o)
BIN_DIR = bin
OBJS := $(addprefix $(BIN_DIR)/, $(OBJS_FILES))
SHARED_OBJ = Navigation_313452542.so
OUTPUT_DIR := ../../OUTPUT/Navigation
OUTPUT_SHARED_OBJ := $(OUTPUT_DIR)/$(SHARED_OBJ)

CPP_COMP_FLAG = -std=c++20 -Werror -Wall -Wextra -pedantic-errors -DNDEBUG -fPIC -fvisibility=hidden -fvisibility-inlines-hidden
CPP_LINK_FLAG = -shared 

all: $(BIN_DIR) $(OUTPUT_DIR) $(OUTPUT_SHARED_OBJ)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

$(OUTPUT_SHARED_OBJ): $(OBJS)
	$(COMP) $(CPP_LINK_FLAG) -o $@ $^

# Make sure to have all your dependencies for each .o file creation rule
$(BIN_DIR)/Navigation.o:   Navigation.cpp ../Common/NavigationRegistration.h Routes.h ../Common/NavigationGIS.h ../UserCommon/Hashers.h ../Common/AbstractNavigation.h ../Common/AbstractRoutes.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Route.o: Route.cpp Node.h ../Common/GISNamedTypes.h ../Common/AbstractWay.h ../Common/CoordinatesMath.h ../Common/AbstractRoute.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@

$(BIN_DIR)/Routes.o: Routes.cpp Route.h ../Common/AbstractRoutes.h
	$(COMP) $(CPP_COMP_FLAG) -c $< -o $@






clean:
	rm -rf $(BIN_DIR) $(OUTPUT_SHARED_OBJ)
