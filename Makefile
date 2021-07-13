dvdmake: dvd.c
	gcc -o dvd.bin dvd.c -lSDL2 -lSDL2_image
