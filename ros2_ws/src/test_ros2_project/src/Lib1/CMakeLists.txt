add_library(Lib1 Lib1Class.cpp)
set(LibSubPath1 "Lib1Sub/")
add_subdirectory(${LibSubPath1})
target_link_libraries(Lib1 Lib1Sub)
target_include_directories(Lib1 PUBLIC ${LibSubPath1})