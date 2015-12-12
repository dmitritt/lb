CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -lpthread -static-libstdc++
CC_FLAGS := -std=c++14 -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -Iboost/asio/include -Iboost/system/include -Iboost/predef/include -O2

bin/qlb: $(OBJ_FILES)
	mkdir -p bin
	g++ $(LD_FLAGS) -o $@ $^

build/%.o: src/%.cpp
	mkdir -p build
	rm -f "$@.d"	
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf build bin
