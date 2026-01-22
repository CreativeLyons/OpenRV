#
# Copyright (C) 2022  Autodesk, Inc. All Rights Reserved.
#
# SPDX-License-Identifier: Apache-2.0
#

INCLUDE(cxx_defaults)

SET(_target
    "py-interp"
)

FIND_PACKAGE(
  ${RV_QT_PACKAGE_NAME}
  COMPONENTS Core Gui Widgets
  REQUIRED
)

FILE(GLOB _sources main.cpp)

ADD_EXECUTABLE(
  ${_target}
  ${_sources}
)

TARGET_INCLUDE_DIRECTORIES(
  ${_target}
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
)

# The main.cpp source use its own definitions
TARGET_COMPILE_OPTIONS(
  ${_target}
  PRIVATE -DBUNDLE_MAJOR_VERSION=${RV_MAJOR_VERSION} -DBUNDLE_MINOR_VERSION=${RV_MINOR_VERSION} -DBUNDLE_REVISION_NUMBER=${RV_REVISION_NUMBER}
)

TARGET_LINK_LIBRARIES(
  ${_target}
  PRIVATE Python::Python Boost::filesystem Boost::system Qt::Core Qt::Gui Qt::Widgets
)

IF(RV_TARGET_DARWIN)
  TARGET_LINK_LIBRARIES(
    ${_target}
    PRIVATE DarwinBundle
  )
  # On macOS, handle missing AGL framework (deprecated, removed in macOS 10.14+) Qt::Gui/Qt::Widgets may pull in Qt::OpenGL which adds -framework AGL
  TARGET_LINK_OPTIONS(${_target} PRIVATE "-Wl,-undefined,dynamic_lookup")
ELSE()
  TARGET_LINK_LIBRARIES(
    ${_target}
    PRIVATE QTBundle
  )
ENDIF()

RV_STAGE(TYPE "EXECUTABLE_WITH_PLUGINS" TARGET ${_target})

# On macOS, ensure dummy AGL framework is available at runtime
IF(RV_TARGET_DARWIN)
  SET(_dummy_agl_framework_path
      "${CMAKE_BINARY_DIR}/frameworks/AGL.framework"
  )
  IF(EXISTS "${_dummy_agl_framework_path}")
    ADD_CUSTOM_COMMAND(
      TARGET ${_target}
      POST_BUILD
      COMMENT "Copying dummy AGL framework to staging area"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${RV_STAGE_FRAMEWORKS_DIR}
      COMMAND ${CMAKE_COMMAND} -E copy_directory ${_dummy_agl_framework_path} ${RV_STAGE_FRAMEWORKS_DIR}/AGL.framework
    )
  ENDIF()
ENDIF()

ADD_CUSTOM_COMMAND(
  COMMENT "Generating .pyc for ${RV_STAGE_PLUGINS_PYTHON_DIR}"
  OUTPUT ${RV_STAGE_PLUGINS_PYTHON_DIR}/.compiled
  COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=minimal $<TARGET_FILE:${_target}> -m compileall -f ${RV_STAGE_PLUGINS_PYTHON_DIR}
  COMMAND ${CMAKE_COMMAND} -E touch ${RV_STAGE_PLUGINS_PYTHON_DIR}/.compiled
  DEPENDS ${_target} python_source_modules installed_packages
)

ADD_CUSTOM_TARGET(
  pyc_generation ALL
  DEPENDS ${RV_STAGE_PLUGINS_PYTHON_DIR}/.compiled
)

ADD_DEPENDENCIES(compiled_python_source_modules pyc_generation)

IF(RV_BUILD_TESTS)
  ADD_TEST(
    NAME "py-interp ${python_test_file_name}"
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND} -E env QT_QPA_PLATFORM=minimal "$<TARGET_FILE:${_target}>" -m unittest --verbose
  )
ENDIF()
