all:
	rm -r build 
	mkdir build
	gcc -o build/creator Creator.c -lrt
	gcc -o build/Reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0` -pthread
reconstructor:
	clear
	gcc -o build/reconstructor Reconstructor.c `pkg-config --cflags --libs gtk+-3.0` -pthread
reconstructordb:
	clear
	gcc -o a.out Reconstructor.c `pkg-config --cflags --libs gtk+-3.0` -pthread -g
clean:
	rm -r build 
	mkdir build
run:

	build/reconstructor 100 outputFile.txt Automatic
