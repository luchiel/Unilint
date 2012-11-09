.PHONY: all clean test

APPNAME = unilint
OBJECTS_PATH = objects
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJECTS_PATH)/, $(SOURCES:.cpp=.o))

export CPATH = $(CURDIR)
CXXFLAGS += -Wall -Wextra -Wno-unused -Wno-switch -Wno-parentheses -std=c++0x -Wno-sign-compare

all: $(APPNAME)
	./$(APPNAME) "tests/c_tests/compulsory_block_braces.c" -r="tests/c_tests/compulsory_block_braces.result" -s="tests/c_tests/compulsory_block_braces.ini"

test: all
	$(APPNAME) --test

clean:
	$(RM) $(APPNAME) $(OBJECTS) $(DEPENDENCIES)

$(OBJECTS): | $(OBJECTS_PATH)

$(OBJECTS_PATH):
	mkdir $@

DEPENDENCIES = $(OBJECTS:.o=.d)
-include $(DEPENDENCIES)

$(OBJECTS_PATH)/%.o: %.cpp
	$(CXX) -c -o $@ -MMD $(CXXFLAGS) $<

$(APPNAME): $(OBJECTS)
	$(CXX) -o $@ $^ -lsource-highlight
