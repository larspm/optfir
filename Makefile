CXXFLAGS += $$(xml2-config --cflags)

all: src/filters.o src/main.o src/optfir.o src/specparser.o
	g++ -lnlopt $$(xml2-config --libs) -o optfir src/*.o
clean:
	rm src/*.o