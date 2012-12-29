set( CPP_SOURCE_FILES
  TYUV_file.cpp
)

set( HEADER_FILES
  TYUV_file.h
)

add_library( yuv ${CPP_SOURCE_FILES} ${HEADER_FILES} )
