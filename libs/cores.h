#ifndef CORES_H_RQIK52MY
#define CORES_H_RQIK52MY

#include <stdio.h>

// VERY FAST and VERY accurate, especially since cached on initial call
//
// LINUX
// * core count (physical) : 2
// * total time elapsed : 0.0009058720s                                  
// * average time elapsed : 0.0000000385s  
//
// MACOS
// * core count (physical) : 6
// * total time elapsed : 0.0010840080s
// * average time elapsed : 0.0000000413s
//
// WINDOWS
// * core count (physical) : 4
// * total time elapsed : 0.0017932280s
// * average time elapsed : 0.0000000945s
#ifdef __linux__
inline int cores() {
  static int corecount = 0;

  if (corecount == 0) {
    FILE *fp;
    char var[5] = {0};

    fp = popen("nproc --all", "r");
    while (fgets(var,sizeof(var),fp) != NULL) sscanf(var,"%d",&corecount);
    pclose(fp);
  }

  return corecount;
}
#elif __APPLE__
inline int cores() {
  static int corecount = 0;

  if (corecount == 0) {
    FILE *fp;
    char var[5] = {0};

    fp = popen("sysctl -n hw.physicalcpu", "r");
    while (fgets(var,sizeof(var),fp) != NULL) sscanf(var,"%d",&corecount);
    pclose(fp);
  }
  
  return corecount;
}
#elif defined(_WIN32) || defined(WIN32)
inline int cores() {
  static int corecount = 0;

  if (corecount == 0) {
    FILE *fp;
    char var[5] = {0};
    
    fp = _popen("wmic cpu get NumberOfCores", "r");
    while (fgets(var,sizeof(var),fp) != NULL) sscanf(var,"%d",&corecount);
    _pclose(fp);
  }
  
  return corecount;
}
#endif

#endif