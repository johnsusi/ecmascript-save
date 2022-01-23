set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_BUILTIN_STL_SUPPORT YES)

doxygen_add_docs(docs ecmascript.hpp "${CMAKE_CURRENT_SOURCE_DIR}/index.md"
                 WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/libes")
