#include<stdlib.h>
#include<string.h>
  FILE* read_object;
int setup_config_reader(char* config){
  read_object=fopen(config,"rb");
  if(!read_object)
    return -1;
  return 1;
}
void close_config_reader(){
  fclose(read_object);
}
int read_string(char* identifier,char* string,int size){
  fseek(read_object,0,SEEK_SET);
  char cread;
  char isreadup=0;
  char* setpointer;
  int strsize=strlen(identifier);
  int it=0;
  char found=0;
  while((cread=fgetc(read_object))!=EOF){
    if(cread==*identifier&&isreadup!=1){
      isreadup=1;
      setpointer=identifier;
      it=0;
    }
    if(isreadup){
      if(it>=strsize){
        if(cread=='='){
          found=1;
          break;
        }else{
          isreadup=0;
          setpointer=identifier;
        }
      }
      if(*setpointer==cread){
        setpointer++;
        it++;
      }else{
        isreadup=0;
      }
    }
  }
  if(found==0)
    return -1;
  char* end=string+size;
  while((cread=fgetc(read_object))!=EOF){
    if(cread=='\n')
      break;
    if(string>end){
      break;
    }
    *string=cread;
    string++;
  }
  if(string<end)
    *string=0;
  return 1;
}
