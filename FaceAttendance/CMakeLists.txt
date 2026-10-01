cmake_minimum_required(VERSION 3.19)
project(FaceAttendance LANGUAGES CXX)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

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

qt_add_executable(FaceAttendance
    WIN32 MACOSX_BUNDLE
    main.cpp
    faceattendance.cpp
    faceattendance.h
    faceattendance.ui
)

target_include_directories(FaceAttendance PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    "E:/SeetaFace2-master/SeetaFace2-master/build/Desktop_Qt_6_10_3_MinGW_64_bit_Debug/install/include"
)

target_link_libraries(FaceAttendance
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
        ${OpenCV_LIBS}
        SeetaFace::SeetaNet
        SeetaFace::SeetaFaceDetector
        SeetaFace::SeetaFaceLandmarker
        SeetaFace::SeetaFaceRecognizer
)

install(TARGETS FaceAttendance
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
qt_generate_deploy_app_script(
    TARGET FaceAttendance
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
