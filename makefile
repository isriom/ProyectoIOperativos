all:
	rm -r build 
	mkdir build
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0`
reconstructor:
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0`
	build/Reconstructor
	
Client:
	gcc -o build/Client Client.c -lrt -pthread `pkg-config --cflags --libs gtk+-3.0`
	build/Client
