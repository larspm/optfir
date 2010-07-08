CXXFLAGS += $$(xml2-config --cflags) -Inlopt/include

all: src/filters.o src/main.o src/optfir.o src/specparser.o
	g++ $$(xml2-config --libs) -Lnlopt/lib -o optfir src/*.o -lnlopt
clean:
	rm -f src/*.o optfir
