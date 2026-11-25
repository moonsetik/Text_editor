cmake_minimum_required(VERSION 3.21)

project(TextEditor VERSION 0.1 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets)

qt_add_executable(TextEditor WIN32 main.cpp)
target_compile_features(TextEditor PRIVATE cxx_std_17)
target_link_libraries(TextEditor PRIVATE Qt6::Widgets)

if(MSVC)
    target_compile_options(TextEditor PRIVATE /utf-8)
endif()

if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${QT_BIN_DIR}" NO_DEFAULT_PATH REQUIRED)

    add_custom_command(TARGET TextEditor POST_BUILD
        COMMAND "${WINDEPLOYQT_EXECUTABLE}" --no-translations --no-compiler-runtime
            "$<TARGET_FILE:TextEditor>"
        VERBATIM
    )
endif()
