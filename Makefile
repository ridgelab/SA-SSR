# MAKEFILE

# ------ VARIABLES -------------------
CXX?=g++
CXXFLAGS+=-g -Wall -Wextra -std=c++11
LIBS?=

# ------ TARGETS ---------------------
all: prep bin/findSSRs permissions

clean:
	@rm -rf bin/* || true
	@rm -rf obj/* || true

realclean:
	@rm -rf bin obj || true

prep:
	@mkdir -p obj bin || true

obj/FindSSRsArgs.o: src/FindSSRsArgs.cpp include/FindSSRsArgs.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/SingleResult.o: src/SingleResult.cpp include/SingleResult.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/Results.o: src/Results.cpp include/Results.h include/SingleResult.h include/FindSSRsArgs.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/FindSSRs.o: src/FindSSRs.cpp include/FindSSRs.h include/FindSSRsArgs.h include/Results.h include/SingleResult.h lib/sais-lite-lcp/sais.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/FindSSRsMain.o: src/FindSSRsMain.cpp include/FindSSRs.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/findSSRs: obj/FindSSRsMain.o obj/FindSSRs.o obj/Results.o obj/SingleResult.o obj/FindSSRsArgs.o
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $@

permissions:
	@chmod 750 bin bin/findSSRs || true
	@chmod 750 include src obj || true
	@chmod 640 include/* src/* obj/* || true
