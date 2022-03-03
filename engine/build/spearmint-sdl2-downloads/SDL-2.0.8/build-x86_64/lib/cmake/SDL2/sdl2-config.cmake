# sdl2 cmake project-config input for ./configure scripts

set(prefix "/srv/SDL-2.0.8-build-x86_64") 
set(exec_prefix "${prefix}")
set(libdir "${exec_prefix}/lib")
set(SDL2_PREFIX "/srv/SDL-2.0.8-build-x86_64")
set(SDL2_EXEC_PREFIX "/srv/SDL-2.0.8-build-x86_64")
set(SDL2_LIBDIR "${exec_prefix}/lib")
set(SDL2_INCLUDE_DIRS "${prefix}/include/SDL2")
set(SDL2_LIBRARIES "-L${SDL2_LIBDIR}  -lSDL2 ")
string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)
