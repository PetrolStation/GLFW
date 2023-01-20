set( BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE )
set( GLFW_BUILD_DOCS   OFF CACHE BOOL "GLFW lib only"          FORCE )
set( GLFW_INSTALL      OFF CACHE BOOL "GLFW lib only"          FORCE )

add_subdirectory(glfw)