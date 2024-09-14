all:
	cc draw.c setimage.c -march=native -O2 -lX11 -lXext -lm -lpng -Wall -o deskwaifu 
	 
