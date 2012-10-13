.PHONY: all clean test

APPNAME = unilint
OBJECTS_PATH = objects
SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJECTS_PATH)/, $(SOURCES:.cpp=.o))

export CPATH = $(CURDIR)
CXXFLAGS += -Wall -Wextra -Wno-unused -Wno-switch -Wno-parentheses

all: $(APPNAME)
	./$(APPNAME) example.cpp

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
	$(CXX) -o $@ $^
