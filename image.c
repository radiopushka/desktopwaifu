#include <png.h>
#include <stdlib.h>
#include <stdio.h>
int PWIDTH;
int PHEIGHT;

unsigned char* rescaled_read(char* image,int width,int height){
  FILE* im=fopen(image,"rb");
  if(!im)
    return NULL;
  png_structp png=png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png)
    return NULL;
  png_infop info = png_create_info_struct(png);
  if(!info)
    return NULL;
  if(setjmp(png_jmpbuf(png)))
    return NULL;
  png_init_io(png, im);
  png_read_info(png, info);
  int iwidth=png_get_image_width(png, info);
  PWIDTH=iwidth;
  int iheight=png_get_image_height(png, info);
  PHEIGHT=iheight;
  double scaley=0;
  double scalex=0;
  int descaley=0;
  int descalex=0;
  if(width==-1||height==-1){
    width=iwidth;
    height=iheight;
  }else{
  if(iwidth<width){
    descalex=(width/iwidth)+1;
  }else if(width<iwidth){
    scalex=iwidth/width;
    scalex=0;
  }
  if(iheight<height){
    descaley=(height/iheight)+1;
  }else if(iheight>height){
    scaley=iheight/height;
    scaley=0;
  }
  }
  int color_type = png_get_color_type(png, info);
  if(color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(png);
  if(color_type == PNG_COLOR_TYPE_GRAY ||
     color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb(png);

  png_read_update_info(png, info);
  png_bytep* rows=malloc(sizeof(png_bytep)*iheight);
  for(int i=0;i<iheight;i++){
    rows[i]=(png_byte*)malloc(png_get_rowbytes(png,info));
  }
  
  png_read_image(png, rows);
  fclose(im);
  png_destroy_read_struct(&png, &info, NULL);
  unsigned long long area=width*height*4;
  unsigned long long yend=height;
  unsigned long long xend=width*4;
  unsigned char* output=malloc(sizeof(char)*area);
  unsigned long long i1;
  unsigned long long i2;
  unsigned long long i3=0;
  unsigned long long i4;
  int mult=3;
  if(color_type==PNG_COLOR_TYPE_RGB_ALPHA ){
    mult=4;
  }
  int yscale=0;
  int xscale=0;
  for(i1=0;i1<yend;i1++){
    i4=0;
    xscale=0;
    if(i3>=iheight-1){
      i3=iheight-1;
    }
    for(i2=i1*xend;i2<(i1*xend+xend);){
      output[i2++]=rows[i3][i4+2];
      output[i2++]=rows[i3][i4+1];
      output[i2++]=rows[i3][i4];
      if(mult==4)
        output[i2++]=rows[i3][i4+3];
      else
        output[i2++]=255;
      if(scalex>0){
        i4=i4+scalex*mult;
      }else if(descalex>0){
        xscale++;
        if(xscale%descalex==0){
          i4=i4+mult;
          //xscale=0;
        }
      }else{
        i4=i4+mult;
      }
      
    }

    if(scaley>0){
      i3=i3+scaley;
    }else if(descaley>0){
      yscale++;
      if(yscale%descaley==0){
        i3=i3+1;
      }
    }else{
      i3=i3+1;
    }
  }
  for(int i=0;i<iheight;i++){
    free(rows[i]);
  }
  free(rows);
  return output;
}
