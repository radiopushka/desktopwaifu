#ifndef X11IMAGE
  #define X11IMAGE
  #include <X11/Xlib.h>
  typedef unsigned char Raster;
  //our raster declares image type in B G R per each pixel
  //one pixel has 3 chars B G and R
  //the Y is 

  void setRGB(int R,int G ,int B);//sets the background RGB
  void drawImage(Raster* im);//sets the background Image
  unsigned long long getRasterLocation(int x,int y);//returns the location on the raster of that pixel
  int getWindowHeight();
  int getWindowWidth();
  unsigned long long getScreenArea();
  void stopXEvent();
  Raster* getRaster();
  void generateImage(Raster* data);

  void XeventListener(void (*func)(XEvent));//calls function when the mouse moves
   

  void initX11(char* display);
  void setImage();//refreshes display
  void flushX();//flush the data
  void closeX11(); 
                  //
#endif // !X11IMAGE
