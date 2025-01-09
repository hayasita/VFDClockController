#ifndef filesys_h
#define filesys_h

#ifdef GLOBAL_VAL_DEF
#define GLOBAL
#else
#define GLOBAL extern
#endif

#define USE_LittleFS

#include <FS.h>
#ifdef USE_LittleFS
  #define SPIFFS LittleFS
  #include <LittleFS.h> 
#else
  #include <SPIFFS.h>
#endif 


#undef GLOBAL
#endif