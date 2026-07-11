all:
	make clean && make build && make run

build:
	mkdir -p build && cd build && cmake .. && make

run:
	./build/vacuum_sim

clean:
	rm -rf build
