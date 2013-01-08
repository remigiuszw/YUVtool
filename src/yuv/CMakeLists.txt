set( CPP_SOURCE_FILES
  TYUV_file.cpp
  TComponents.cpp
)

set( HEADER_FILES
  TYUV_file.h
  TComponents.h
)

add_library( yuv ${CPP_SOURCE_FILES} ${HEADER_FILES} )
