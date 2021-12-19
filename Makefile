CC = g++
CXXFLAGS = -Wall -Wextra -O2 -std=c++20

test: test.o
test.o: test.cc virus_genealogy.h

ok: ok.o
ok.o: ok.cc virus_genealogy.h

clean:
	rm -f *.o test ok