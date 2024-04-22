all:
	rm -r build 
	mkdir build
	gcc -o build/creator Creator.c -lrt
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0`
reconstructor:
	clear
	gcc -o build/reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0` -pthread
	build/reconstructor
reconstructordb:
	clear
	gcc -o a.out Reconstructor.c `pkg-config --cflags --libs gtk+-3.0` -pthread -g
clean:
	rm -r build 
	mkdir build
