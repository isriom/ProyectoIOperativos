all:
	rm -r build 
	mkdir build
	gcc -o build/creator creator.c -lrt
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0`
reconstructor:
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0`
	build/Reconstructor
