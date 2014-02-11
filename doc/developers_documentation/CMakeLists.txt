find_package( Inkscape REQUIRED )
find_package( LATEX REQUIRED )
include( UseLATEX )

set( DEV_DOC_INKSCAPE_IMAGES
  GUI/images/YUV_view_window.svg
  implementation/images/entries_in_plane.svg
)

foreach( file ${DEV_DOC_INKSCAPE_IMAGES} )
  get_filename_component( file_we ${file} NAME_WE )
  get_filename_component( file_path ${file} PATH )
  set( file_path_we ${file_path}/${file_we} )
  add_custom_command(
    OUTPUT
      ${CMAKE_CURRENT_BINARY_DIR}/${file_path_we}.pdf_tex
      ${CMAKE_CURRENT_BINARY_DIR}/${file_path_we}.pdf
    DEPENDS
      ${CMAKE_CURRENT_SOURCE_DIR}/${file_path_we}.svg
    COMMAND
      ${CMAKE_COMMAND}
    ARGS
      -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/${file_path}
    COMMAND
      ${INKSCAPE_EXECUTABLE}
    ARGS
      -D
      -z
      --file=${CMAKE_CURRENT_SOURCE_DIR}/${file_path_we}.svg
      --export-pdf=${CMAKE_CURRENT_BINARY_DIR}/${file_path_we}.pdf
      --export-latex
  )
  set( DEV_DOC_INKSCAPE_IMAGES_PDF
    ${DEV_DOC_INKSCAPE_IMAGES_PDF}
    ${CMAKE_CURRENT_BINARY_DIR}/${file_path_we}.pdf
    ${CMAKE_CURRENT_BINARY_DIR}/${file_path_we}.pdf_tex
  )
endforeach()

add_latex_document(
    YUVtool_developers_documentation.tex
  INPUTS
    Introduction.tex
    GUI/GUI.tex
    implementation/implementation.tex
  DEPENDS
    ${DEV_DOC_INKSCAPE_IMAGES_PDF}
  DEFAULT_PDF
)
