# MAKEFILE

# ------ VARIABLES -------------------
CXX?=g++
CXXFLAGS+=-g -Wall -Wextra -std=c++11 -pthread
LIBS?=
PREFIX=/usr/local/bin

# ------ TARGETS ---------------------
all: prep bin/sa-ssr permissions

clean:
	@rm -rf bin/* || true
	@if [ -e $(PREFIX)/sa-ssr -a -r $(PREFIX)/sa-ssr -a -w $(PREFIX)/sa-ssr ] ; then rm -f $(PREFIX)/sa-ssr || true; fi

realclean: clean
	@rm -rf bin obj || true

prep:
	@mkdir -p obj bin || true

obj/FindSSRsArgs.o: src/FindSSRsArgs.cpp include/FindSSRsArgs.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/OutputFile.o: src/OutputFile.cpp include/OutputFile.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/FastaSequences.o: src/FastaSequences.cpp include/FastaSequences.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/SingleResult.o: src/SingleResult.cpp include/SingleResult.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/Results.o: src/Results.cpp include/Results.h include/SingleResult.h include/FindSSRsArgs.h include/OutputFile.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/FindSSRs.o: src/FindSSRs.cpp include/FindSSRs.h include/FindSSRsArgs.h include/Results.h include/SingleResult.h include/OutputFile.h include/FastaSequences.h lib/sais-lite-lcp/sais.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/FindSSRsMain.o: src/FindSSRsMain.cpp include/FindSSRs.h include/FindSSRsArgs.h include/Results.h include/SingleResult.h include/OutputFile.h include/FastaSequences.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/ProgressMeter.o: src/ProgressMeter.cpp include/ProgressMeter.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

bin/sa-ssr: obj/FindSSRsMain.o obj/OutputFile.o obj/FastaSequences.o obj/FindSSRs.o obj/Results.o obj/SingleResult.o obj/FindSSRsArgs.o obj/ProgressMeter.o
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o $@

permissions:
	@chmod 750 bin bin/sa-ssr || true
	@chmod 750 include src obj || true
	@chmod 640 include/* src/* obj/* example/* || true

install:
	@if [ -e bin/sa-ssr ]; then cp bin/sa-ssr $(PREFIX)/sa-ssr || true; else echo "ERROR: \`bin/sa-ssr' does not exist. Did you forget to run \`make' first?"; fi
