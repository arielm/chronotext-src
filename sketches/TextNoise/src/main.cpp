
#include "Sketch.h"

#include "ios/Application.h"

namespace chr
{
  CrossSketch* createSketch()
  {
    return new Sketch();
  }
}

int main(int argc, char** argv)
{
  @autoreleasepool
  {
    return UIApplicationMain(argc, argv, nil, NSStringFromClass([Application class]));
  }
}

/*
 * Functions that appear to be missing when targeting the Xcode iOS Simulators
 * Reference: http://cknotes.com/ipad-simulator-undefined-symbols-for-architecture-x86_64-_readdirinode64/
 */
 
#include <dirent.h>
#include <fnmatch.h>
 
extern "C" DIR * opendir$INODE64( char * dirName );
DIR * opendir$INODE64( char * dirName )
{
  return opendir( dirName );
}
 
extern "C" struct dirent * readdir$INODE64( DIR * dir );
struct dirent * readdir$INODE64( DIR * dir )
{
  return readdir( dir );
}

extern "C" struct dirent * readdir_r$INODE64( DIR * dir );
struct dirent * readdir_r$INODE64( DIR * dir )
{
  return readdir( dir );
}
