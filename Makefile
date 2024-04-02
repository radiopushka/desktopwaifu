all:
	gcc draw.c setimage.c -lX11 -lXext -lm -lpng -Wall -o deskwaifu 
	 
