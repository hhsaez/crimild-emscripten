CMAKE_MINIMUM_REQUIRED( VERSION 3.1.0 FATAL_ERROR )

PROJECT( crimild-emcc )

SET( CRIMILD_ENABLE_OPENGL ON CACHE BOOL "Enable opengl module for Crimild" )
SET( CRIMILD_ENABLE_GLFW ON CACHE BOOL "Enable glfw module for Crimild" )
SET( CRIMILD_ENABLE_TESTS OFF CACHE BOOL "Disable testing for Crimild" )

ADD_SUBDIRECTORY( crimild )

SET ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CRIMILD_SOURCE_DIR}/CMakeTools" )

SET( CRIMILD_APP_NAME crimild-emcc )
SET( CRIMILD_APP_SOURCE_DIRECTORIES src )
SET( CRIMILD_APP_INCLUDE_DIRECTORIES src )

INCLUDE( ModuleBuildApp )

if ( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" )
    set_target_properties( crimild-emcc PROPERTIES LINK_FLAGS "-s DEMANGLE_SUPPORT=1 -s --preload-file assets" )

    configure_file( ${PROJECT_SOURCE_DIR}/src/index.html "." COPYONLY )

	FILE ( INSTALL ${PROJECT_SOURCE_DIR}/assets DESTINATION . )
endif ()

