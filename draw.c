#include "setimage.h"
#include <unistd.h>
#include<stdio.h>
#include<math.h>
#include "image.c"
#include "configfile.c"
int maxwidth;
int maxheight;
int coeffw;
int coeffh;

Raster* base;
Raster* screenarea;
Raster* background;

Raster* eye;
int eyeimageheight;
int eyeimagewidth;

int eye1x=884;
int eye1y=515;
int eye2x=1320;
int eye2y=515;
int eye1w=180;
int eye1h=30;
int eye2w=180;
int eye2h=30;
int replacergb=0;

int closeness=500;
void drawSquareIf(int x, int y,int w,int h,int val){
   register Raster* imd;
   register Raster* eyeimage=eye;
   unsigned long long pos;
   register Raster* B,*Bf;
   register Raster* G,*Gf;
   register Raster* R,*Rf;
   register int i;
   register int i2;
   x=x-(w>>1);
   y=y-(h>>1);
   for(i=0;i<h;i++){
     pos=getRasterLocation(x,y+i);
     imd=base+pos;
     
     for(i2=0;i2<w;i2++){
        Bf=eyeimage++;
        B=imd++;
        Gf=eyeimage++;
        G=imd++;
        Rf=eyeimage++;
        R=imd++;
        eyeimage++;
        imd++;
        if(*B==val&&*G==val&&*R==val){
          *B=*Bf;
          *G=*Gf;
          *R=*Rf;
        }
    }
   }
}

void drawEyeArea(int cx1,int cx2,int cy1,int cy2,XMotionEvent mouse){
  register int cw1=(eye1w>>1);
  register int ch1=(eye1h>>1);
  register int cw2=(eye2w>>1);
  register int ch2=(eye2h>>1);
  register int mx=mouse.x;
  register int my=mouse.y;
  register double diffx1=mx-cx1;
  register double diffx2=mx-cx2;
  register double diffy1=my-cy1;
  register double diffy2=my-cy2;
  register double aarg1=diffy1/diffx1;
  register double aarg2=diffy2/diffx2;
  register double angle1=atan(aarg1);
  register double angle2=atan(aarg2);
  register double proximity=sqrt(diffx1*diffx1+diffy1*diffy1)/closeness;
  register double proximity2=sqrt(diffx2*diffx2+diffy2*diffy2)/closeness;
  if(proximity>1)
    proximity=1;
  if(proximity2>1)
    proximity2=1;
  double nx1=cos(angle1)*(cw1*proximity);
  double ny1=sin(angle1)*(ch1*proximity);
  double nx2=cos(angle2)*(cw2*proximity2);
  double ny2=sin(angle2)*(ch2*proximity2);
  if(diffx1<0){
    nx1=-nx1;
    ny1=-ny1;
  }
  ny1=ny1+cy1;
  nx1=nx1+cx1;
  if(diffx2<0){
    nx2=-nx2;
    ny2=-ny2;
  }
  ny2=ny2+cy2;
  nx2=nx2+cx2;
  drawSquareIf(nx1,ny1,eyeimagewidth,eyeimageheight,replacergb);
  drawSquareIf(nx2,ny2,eyeimagewidth,eyeimageheight,replacergb);

}
void onMouse(XEvent ev){
  XMotionEvent mv=ev.xmotion;
  drawImage(background);
  drawEyeArea(eye1x,eye2x,eye1y,eye2y,mv);
  usleep(40000);
  setImage();
  flushX();
}

int read_config(){
  char read_buffer[4096];
  if(read_string("mainimage",read_buffer,4096)==-1){
    printf("error: your config must contain field 'mainimage'\n");
    return -1;
  }
  background=rescaled_read(read_buffer,maxwidth,maxheight);
  if(read_string("eyeimage",read_buffer,4096)==-1){
    printf("error: your config must contain field 'eyeimage'\n");
    return -1;
  }
  eye=rescaled_read(read_buffer,-1,-1);
  if(read_string("eye1x",read_buffer,4096)==-1){
    printf("note: eye1x not found\n");
    eye1x=884;}
  else 
    eye1x=atoi(read_buffer);
  if(read_string("eye2x",read_buffer,4096)==-1){
    printf("note: eye2x not found\n");
    eye2x=1320;}
  else 
    eye2x=atoi(read_buffer);

  if(read_string("eye1y",read_buffer,4096)==-1){
    printf("note: eye1y not found\n");
    eye1y=515;}
  else 
    eye1y=atoi(read_buffer);
  if(read_string("eye2y",read_buffer,4096)==-1){
    printf("note: eye2y not found\n");
    eye2y=515;}
  else 
    eye2y=atoi(read_buffer);
  if(read_string("eye1w",read_buffer,4096)==-1){
    printf("note: eye1w not found\n");
    eye1w=180;}
  else 
    eye1w=atoi(read_buffer);
  if(read_string("eye1h",read_buffer,4096)==-1){
    printf("note: eye1h not found\n");
    eye1h=30;}
  else 
    eye1h=atoi(read_buffer);
  if(read_string("eye2w",read_buffer,4096)==-1){
    printf("note: eye2w not found\n");
    eye2w=180;}
  else 
    eye2w=atoi(read_buffer);
  if(read_string("eye2h",read_buffer,4096)==-1){
    printf("note: eye2h not found\n");
    eye2h=30;}
  else 
    eye2h=atoi(read_buffer);
  if(read_string("replacergb",read_buffer,4096)==-1){
    printf("note: replacergb not found\n");
    replacergb=0;}
  else 
    replacergb=atoi(read_buffer);
  if(read_string("mouseproximity",read_buffer,4096)==-1){
    printf("note: mouseproximity not found\n");
    closeness=500;}
  else 
    closeness=atoi(read_buffer);

  return 1;
}

int main(int argn,char* argv[]){
  if(argn!=3){
    if(argn==2){
        initX11(argv[1]);
        maxwidth=getWindowWidth();
        maxheight=getWindowHeight();
        printf("window width: %d\nwindow height: %d\n",maxwidth,maxheight);
        closeX11();
        return 0;
    }
    printf("arguments: <display(ex: \":0\")> <the config file for the model>\n");
    printf("you ideally want to resize your main image to fit your monitor exactly\n");
    printf("you can get the size of the monitor by running this program with only one argument\n");
    printf("------\n\n");
    printf("model file format:\n");
    printf("mainimage=/path/to/your/image.png\n");
    printf("eyeimage=/path/to/your/eyeimage.png\n");
    printf("mouseproximity=500\n");
    printf("eye1x=884\n");
    printf("eye2x=1320\n");
    printf("eye1y=515\n");
    printf("eye2y=515\n");
    printf("eye1w=180\n");
    printf("eye2w=180\n");
    printf("eye1h=30\n");
    printf("eye2h=30\n");
    printf("replacergb=0\n");
    printf("----\n\n");
    printf("explanation:\n");
    printf("mainimage-the image with your person or waifu\n");
    printf("you must set the area with the eyes to have the exact same rgb values as your replacergb value. all 3 colors must be equal\n");
    printf("eyeimage - the image for the eye, ideally sized so that it covers up the eye at any position\n");
    printf("mouseproximity - this value determine how 'close' the mouse is, the large this value, the closer you get to the eyes, the less they will look at your mouse\n");
    printf("eye dimensions, the position on the screen, if you resized the waifu image on gimp, you can put your mouse in the center of the eye and these will be your x and y\n");
    printf("the eye width and height is not the actuall with and height of your eye image but they are the width and height of your eye area or where the eye will be moving\n");
    printf("order does not matter but your file must have all the parameters or else they will be replaced by default values shown here(not the image files)\n");
    return 0;
  }
  initX11(argv[1]);
  maxwidth=getWindowWidth();
  maxheight=getWindowHeight();
  printf("window width: %d\nwindow height: %d\n",maxwidth,maxheight);
  base=getRaster();
  screenarea=base+getScreenArea();
  coeffw=(maxwidth/255)+1;
  coeffh=(maxheight/255)+1;
  if(setup_config_reader(argv[2])<0){
    printf("failed reading config file");
    return 0;
  }
  if(read_config()<0){
    return 0;
  } 
  close_config_reader();
  eyeimagewidth=PWIDTH;
  eyeimageheight=PHEIGHT;
  printf("eye width:%d eye height: %d\n",eyeimagewidth,eyeimageheight);
  setRGB(0,0,0);
  XeventListener(onMouse);
  closeX11();
  return 0;
}
