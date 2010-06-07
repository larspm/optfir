CXXFLAGS += $$(xml2-config --cflags)

all: src/filters.o src/main.o src/optfir.o src/specparser.o
	g++ $$(xml2-config --libs) -o optfir src/*.o -lnlopt
clean:
	rm src/*.o
