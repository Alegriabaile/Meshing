cmake_minimum_required(VERSION 3.12)
project(Meshing)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)


# deformable alignment template
add_executable(poly2triangle poly2triangle.cpp)
target_link_libraries(poly2triangle
        ${OpenCV_LIBS}
        /usr/lib/x86_64-linux-gnu/libGL.so
        /usr/lib/x86_64-linux-gnu/libGLU.so
        /usr/lib/x86_64-linux-gnu/libglut.so)


# RamerDouglasPeucker: points to polygon
add_executable(pts2poly RamerDouglasPeucker.cpp)
target_link_libraries(pts2poly
        ${OpenCV_LIBS}
        /usr/lib/x86_64-linux-gnu/libGL.so
        /usr/lib/x86_64-linux-gnu/libGLU.so
        /usr/lib/x86_64-linux-gnu/libglut.so)

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



# tiny debug / leetcode
add_executable(main main.cpp TriangulateSimplePolygon.cpp)
target_link_libraries(main ${OpenCV_LIBS})