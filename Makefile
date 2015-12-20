CPP_FILES := $(shell find src -name '*.cpp')
OBJ_FILES := $(subst src,build,$(CPP_FILES:.cpp=.o))
#OBJ_FILES := $(addprefix build/,$(notdir $(CPP_FILES:.cpp=.o)))
LD_FLAGS := -lpthread -static-libstdc++
BOOST_FLAGS := -DBOOST_ERROR_CODE_HEADER_ONLY -DBOOST_SYSTEM_NO_DEPRECATED -DBOOST_ASIO_DISABLE_BOOST_DATE_TIME
BOOST_LIB_FLAGS := -Iboost/core/include -Iboost/system/include -Iboost/predef/include -Iboost/asio/include -Iboost/endian/include	
CC_FLAGS := -std=c++14 -O2 $(BOOST_FLAGS) $(BOOST_LIB_FLAGS)

bin/qlb: $(OBJ_FILES)
	mkdir -p bin
	g++ $(LD_FLAGS) -o $@ $^

build/%.o: src/%.cpp
	mkdir -p `dirname $@`
	g++ $(CC_FLAGS) -c -o $@ $<

clean:
	rm -rf build bin