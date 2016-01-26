CPP_FILES := $(shell find src -name '*.cpp')
OBJ_FILES := $(subst src,build,$(CPP_FILES:.cpp=.o))
LD_FLAGS := -lpthread -static-libstdc++
BOOST_FLAGS := -DBOOST_ASIO_STANDALONE -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_SYSTEM_NO_LIB 
BOOST_LIB_FLAGS := $(shell ls boost/*/include -d | sed -e 's/boost/-Iboost/g' -e 's/include\//include/')
CC_FLAGS := -std=c++14 -O2 $(BOOST_FLAGS) $(BOOST_LIB_FLAGS)

bin/qlb: $(OBJ_FILES)
	mkdir -p bin
	g++ $(LD_FLAGS) -o $@ $^

build/%.o: src/%.cpp
	mkdir -p `dirname $@`
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf build bin
