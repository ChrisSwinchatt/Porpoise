override CXX     := g++
override CXFLAGS := -std=c++17 -Wall -Wextra -I../include/shared -I../include/shared/lib -ggdb -O0 $(DEFINES)

OBJECTS := ../obj/shared/assert.o\
           ../obj/shared/io/logging/log.o ../obj/shared/io/logging/manipulators.o\
		   ../obj/shared/sync/spinlock.o ../obj/shared/sync/semaphore.o ../obj/shared/sync/lock-guard.o\
		   ../obj/shared/time/timespan.o\
		   ../obj/test-util.o

ifeq ($(VALGRIND),1)
RUNTEST=valgrind --track-origins=yes
else
RUNTEST=
endif

ifeq ($(BUILD_ONLY),1)
RUNTEST=true
endif

../obj/%.o: %.cpp
	@echo "<<< `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -c -o $@ $<

../obj/%.o: ../src/%.cpp
	@echo "<<< `basename $^`"
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -c -o $@ $<

all: test-array test-algorithm test-bitarray test-vector

test-array: ../bin/test/lib/test-array
../bin/test/lib/test-array: ../obj/lib/test-array.o $(OBJECTS)
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -o $@ $^
	@$(RUNTEST) $@

test-algorithm: ../bin/test/lib/test-algorithm
../bin/test/lib/test-algorithm: ../obj/lib/test-algorithm.o $(OBJECTS)
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -o $@ $^
	@$(RUNTEST) $@

test-bitarray: ../bin/test/lib/test-bitarray
../bin/test/lib/test-bitarray: ../obj/lib/test-bitarray.o $(OBJECTS)
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -o $@ $^
	@$(RUNTEST) $@

test-vector: ../bin/test/lib/test-vector
../bin/test/lib/test-vector: ../obj/lib/test-vector.o $(OBJECTS)
	@mkdir -p `dirname $@`
	@$(CXX) $(CXFLAGS) -o $@ $^
	@$(RUNTEST) $@

.PHONY: ../bin/test/lib/test-array ../bin/test/lib/test-algorithm ../bin/test/lib/test-bitarray ../bin/test/lib/test-vector
