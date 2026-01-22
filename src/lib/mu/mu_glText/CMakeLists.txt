#
# Copyright (C) 2022  Autodesk, Inc. All Rights Reserved.
#
# SPDX-License-Identifier: Apache-2.0
#

INCLUDE(cxx_defaults)

SET(_target
    "mu_glText"
)

SET(_sources
    init.cpp
)

ADD_LIBRARY(
  ${_target} SHARED
  ${_sources}
)

SET_TARGET_PROPERTIES(
  ${_target}
  PROPERTIES OUTPUT_NAME "glText"
)

TARGET_INCLUDE_DIRECTORIES(
  ${_target}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

TARGET_LINK_LIBRARIES(
  ${_target}
  PRIVATE Mu MuLang MuGLText
)

# On macOS, handle missing AGL framework (deprecated, removed in macOS 10.14+) MuGLText links to TwkGLText which links to TwkGLF which links to Qt::OpenGL
IF(RV_TARGET_DARWIN)
  TARGET_LINK_OPTIONS(${_target} PRIVATE "-Wl,-undefined,dynamic_lookup")
ENDIF()

RV_STAGE(TYPE "MU_PLUGIN" TARGET ${_target})
