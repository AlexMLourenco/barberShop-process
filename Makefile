# to generate core file execute in bash:
#    ulimit -c unlimited

.PHONY: clean cleanall

OBJS=global.o \
     barber-chair.o washbasin.o tools-pot.o barber-bench.o client-benches.o barber-shop.o \
     service.o client-queue.o barber.o client.o

TARGETS_OBJS=simulation.o

TARGETS := $(TARGETS_OBJS:.o=)

CPPFLAGS=-static --verbose -Wall -ggdb -pthread -I.. -Iinclude -Llib      # if necessary add/remove options
#CPPFLAGS=-Wall -ggdb -rdynamic -pthread -I.. -Iinclude -Llib    # if necessary add/remove options
SYMBOLS=-DEXIT_POLICY            # -DEXCEPTION_POLICY or -DEXIT_POLICY; for ascii output: -DASCII_MODE
LDFLAGS=-lrt -lsoconcur

all: $(TARGETS)

simulation: simulation.o $(OBJS)
	$(CXX) $(SYMBOLS) $(CPPFLAGS) $^ $(LDFLAGS) -o simulation

%.o: %.cpp
	$(CXX) $(SYMBOLS) $(CPPFLAGS) -c $<

clean:
	rm -fv $(OBJS) $(TARGETS_OBJS) core

cleanall: clean
	rm -fv $(TARGETS)

