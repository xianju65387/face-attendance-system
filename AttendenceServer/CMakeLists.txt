cmake_minimum_required(VERSION 3.19)
project(AttendenceServer LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network Sql)

# OpenCV
set(OpenCV_DIR "E:/OpenCV-4.5.2/opencv-4.5.2/opencv-4.5.2/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install")
find_package(OpenCV REQUIRED)

# SeetaFace2
set(SeetaNet_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")
set(SeetaFaceDetector_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")
set(SeetaFaceLandmarker_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")
set(SeetaFaceRecognizer_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")
set(SeetaFaceTracker_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")
set(SeetaQualityAssessor_DIR "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/lib/cmake")

find_package(SeetaNet REQUIRED)
find_package(SeetaFaceDetector REQUIRED)
find_package(SeetaFaceLandmarker REQUIRED)
find_package(SeetaFaceRecognizer REQUIRED)

qt_standard_project_setup()

qt_add_executable(AttendenceServer
    WIN32 MACOSX_BUNDLE
    main.cpp
    attendencewin.cpp
    attendencewin.h
    attendencewin.ui
    qfaceobject.h qfaceobject.cpp
    registerwin.h registerwin.cpp registerwin.ui
    selectwin.h selectwin.cpp selectwin.ui
)

target_include_directories(AttendenceServer PRIVATE
    # 提升控件（attendencewin.ui 里的 RegisterWin / SelectWin）由 uic 生成
    # #include <registerwin.h> 这种尖括号包含，必须把源码目录加进包含路径，
    # 否则会报 "registerwin.h: No such file or directory"。
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/include"
    # seeta/FaceEngine.h 和 seeta/Struct_cv.h 只存在于源码的 example/search 目录，
    # 没有被 install 到上面的 install/include，所以必须单独加这一条路径。
    "E:/SeetaFace2-master/SeetaFace2-master/example/search"
)

target_link_libraries(AttendenceServer
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        Qt::Sql
        ${OpenCV_LIBS}
        SeetaFace::SeetaNet
        SeetaFace::SeetaFaceDetector
        SeetaFace::SeetaFaceLandmarker
        SeetaFace::SeetaFaceRecognizer
)

install(TARGETS AttendenceServer
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET AttendenceServer
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
