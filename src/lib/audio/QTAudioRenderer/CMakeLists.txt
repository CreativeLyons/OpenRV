#
# Copyright (C) 2022  Autodesk, Inc. All Rights Reserved.
#
# SPDX-License-Identifier: Apache-2.0
#

INCLUDE(cxx_defaults)

SET(CMAKE_AUTOUIC
    ON
)
SET(CMAKE_AUTOMOC
    ON
)
SET(CMAKE_AUTORCC
    ON
)

SET(_target
    "QTAudioRenderer"
)

IF(RV_VFX_PLATFORM STRGREATER_EQUAL CY2024)
  FILE(GLOB _sources QTAudioRenderer.cpp QTAudioRenderer/QTAudioRenderer.h)
ELSEIF(RV_VFX_PLATFORM STREQUAL CY2023)
  FILE(GLOB _sources QT5AudioRenderer.cpp QTAudioRenderer/QT5AudioRenderer.h)
ENDIF()

ADD_LIBRARY(
  ${_target} STATIC
  ${_sources}
)

# Try to find Multimedia, but make it optional if not available
FIND_PACKAGE(
  ${RV_QT_PACKAGE_NAME}
  COMPONENTS Core
  REQUIRED
)
FIND_PACKAGE(
  ${RV_QT_PACKAGE_NAME}
  COMPONENTS Multimedia
  QUIET
)
IF(NOT ${RV_QT_PACKAGE_NAME}Multimedia_FOUND)
  MESSAGE(WARNING "Qt6Multimedia not found - audio features may be unavailable")
  SET(QT_MULTIMEDIA_AVAILABLE
      FALSE
  )
ELSE()
  SET(QT_MULTIMEDIA_AVAILABLE
      TRUE
  )
ENDIF()

IF(QT_MULTIMEDIA_AVAILABLE)
  TARGET_COMPILE_DEFINITIONS(
    ${_target}
    PUBLIC QT_MULTIMEDIA_AVAILABLE
  )
ENDIF()

# Note that the audio plugins are opened by the app with dlopen.  We need the includes to compile from some libs to compile, but we don't want to link with them
# to avoid duplicating symbols when the dlopen occurs
TARGET_INCLUDE_DIRECTORIES(
  ${_target}
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} "$<TARGET_PROPERTY:RvApp,INTERFACE_INCLUDE_DIRECTORIES>"
)

# Link libraries - conditionally include Multimedia
SET(_qtaudio_link_libs
    TwkMath TwkUtil Qt::Core
)
IF(QT_MULTIMEDIA_AVAILABLE)
  LIST(APPEND _qtaudio_link_libs Qt::Multimedia)
ENDIF()

TARGET_LINK_LIBRARIES(
  ${_target}
  PUBLIC ${_qtaudio_link_libs}
  PRIVATE TwkAudio
)

RV_STAGE(TYPE "LIBRARY" TARGET ${_target})
