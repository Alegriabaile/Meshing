cmake_minimum_required(VERSION 3.12)
project(Meshing)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(Assimp REQUIRED)
find_package(glfw3 3.2 REQUIRED)
find_package(OpenGL 3.3 REQUIRED)

include_directories(./)

# debug flood fill algorithm and find contours algorithm
add_executable(debug_flood_fill debug_flood_fill.cpp TriangulateSimplePolygon.cpp)
target_link_libraries(debug_flood_fill ${OpenCV_LIBS})

# debug flood fill algorithm and find contours algorithm
add_executable(debug_triangulation debug_triangulation.cpp
        FloodfillWithoutHoles.cpp
        SimplePolygonContours.cpp
        TriangulateSimplePolygon.cpp)
target_link_libraries(debug_triangulation ${OpenCV_LIBS})

# debug generate triangles
add_executable(debug_generateTriangles debug_generateTriangles.cpp
        FloodfillWithoutHoles.cpp
        SimplePolygonContours.cpp
        TriangulateSimplePolygon.cpp)
target_link_libraries(debug_generateTriangles ${OpenCV_LIBS})

# debug simple .obj file writer
add_executable(debug_simpleObjWriter debug_simpleObjWriter.cpp
        FloodfillWithoutHoles.cpp
        SimplePolygonContours.cpp
        TriangulateSimplePolygon.cpp
        SimpleObjWriter.cpp)
target_link_libraries(debug_simpleObjWriter ${OpenCV_LIBS} ${ASSIMP_LIBRARIES})

# debug simple mesh viewer from learnopengl.com
add_executable(debug_simpleViewer debug_simpleViewer.cpp
        FloodfillWithoutHoles.cpp
        SimplePolygonContours.cpp
        TriangulateSimplePolygon.cpp
        glad/glad.c)
target_link_libraries(debug_simpleViewer ${OpenCV_LIBS} ${ASSIMP_LIBRARIES} glfw libGL.so)

# debug simple mesh viewer from learnopengl.com
add_executable(debug_simpleCustomizedViewer
        debug_simpleCustomizedViewer.cpp
        glad/glad.c)
target_link_libraries(debug_simpleCustomizedViewer ${OpenCV_LIBS} ${ASSIMP_LIBRARIES} glfw libGL.so)

# tiny debug / leetcode
add_executable(main main.cpp TriangulateSimplePolygon.cpp)
target_link_libraries(main ${OpenCV_LIBS})