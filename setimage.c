#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include "setimage.h"

int Bw=1000; 
int Bh=1000;

XImage* pix;
unsigned char* image;
Display* dpy;
Window root;

int STOP=0;
unsigned long long SCREEN_AREA_RGB;


XImage *CreatePixmapImage(Display *display, Visual *visual,  void** data, int width, int height) {
	static XShmSegmentInfo shminfo;
	XImage *image = XShmCreateImage(display, visual, 24, ZPixmap, 0, &shminfo, width, height); 
	
	shminfo.shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height,IPC_CREAT|0777);
	shminfo.shmaddr = image->data = *data = shmat(shminfo.shmid, 0, 0);
	shminfo.readOnly = False;

	XShmAttach(display, &shminfo);
	return image;
}


void generateImage(Raster* imagedata){
  unsigned char* cp;
  Raster* end=image+(Bw*Bh)*4;
  for(cp=image;cp<end;){
      *(cp++)=*(imagedata++); //B
      *(cp++)=*(imagedata++); //G 
      *(cp++)=*(imagedata++);  //R
      cp++;

  }
}
void setRGB(int R, int G, int B){
  Raster* cp;
  Raster* end=image+(Bw*Bh)*4;
  for(cp=image;cp<end;){
    *(cp++)=B;
    *(cp++)=G;
    *(cp++)=R;
    cp++;
  }
  
}
void drawImage(Raster* imagein){
  Raster* targ;
  Raster* im=imagein;
  Raster* end=image+(Bw*Bh)*4;
  for(targ=image;targ<end;){
    *(targ++)=*(im++);
  }
}
unsigned long long getRasterLocation(int x,int y){
  return (y*Bw)*4+x*4;
}

void setImage(){
  XPutImage(dpy, root, DefaultGC(dpy, 0), pix, 0, 0, 0, 0, Bw, Bh);
}
void flushX(){
  XFlush(dpy);
}
void initX11(char* display){
  dpy=XOpenDisplay(display);
  unsigned int scr=DefaultScreen(dpy);
  root=RootWindow(dpy,scr);
  XWindowAttributes watr;
  XGetWindowAttributes(dpy,root,&watr);
  Bw=watr.width;
  Bh=watr.height;
  SCREEN_AREA_RGB=(Bw*Bh)*3;
  pix=CreatePixmapImage(dpy,DefaultVisual(dpy, 0),(void**)&image,Bw,Bh);


}
void closeX11(){
  XCloseDisplay(dpy);
}
int getWindowWidth(){
  return Bw;
}
int getWindowHeight(){
  return Bh;
}
void XeventListener(void (*func)(XEvent)){
  XEvent ev;
  XSelectInput(dpy,root,PointerMotionMask);
  STOP=0;
  int i;
  int ec;
  while(STOP!=1){
    ec=XEventsQueued(dpy,PointerMotionMask);
    for(i=0;i<ec;i++){
      XNextEvent(dpy,&ev);
    }
    XNextEvent(dpy,&ev);
    func(ev);
  }
}

Raster* getRaster(){
  return image;
}
unsigned long long getScreenArea(){
  return SCREEN_AREA_RGB;
}
void stopXEvent(){
  STOP=1;
}

