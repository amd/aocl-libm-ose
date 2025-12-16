#
# Copyright (C) 2025, Advanced Micro Devices. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice,
#    this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
#    this list of conditions and the following disclaimer in the documentation
#    and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors
#    may be used to endorse or promote products derived from this software
# without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#

if (LIBM_BUILD_DOCS)

  find_package(Doxygen)
  if (NOT DOXYGEN_FOUND)
    message (FATAL_ERROR
             "\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
             "$$$$$ Doxygen is not found on build machine !!!!!           $$$$$\n"
             "$$$$$ Doxygen must be installed to build LibM Documentation $$$$$\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
    )
  endif (NOT DOXYGEN_FOUND)

  find_package (Sphinx)
  if (NOT SPHINX_FOUND)
    message (FATAL_ERROR
             "\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
             "$$$$$ Sphinx is required to build LibM Documentation !!!!! $$$$\n"
             "$$$$$ Please install python packages listed below,         $$$$\n"
             "$$$$$  $pipx install sphinx                                $$$$\n"
             "$$$$$  $pipx install breathe                               $$$$\n"
             "$$$$$  $pipx install rocm-docs-core                        $$$$\n"
             "$$$$$  $pipx inject sphinx breathe rocm-docs-core          $$$$\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
    )
  endif (NOT SPHINX_FOUND)

  find_package (Breathe)
  if (NOT BREATHE_FOUND)
    message (FATAL_ERROR
             "\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
             "$$$$$ Breathe is required to build LibM Documentation !!!!! $$$$\n"
             "$$$$$ Please install python packages listed below,          $$$$\n"
             "$$$$$  $pipx install breathe                                $$$$\n"
             "$$$$$  $pipx install rocm-docs-core                         $$$$\n"
             "$$$$$  $pipx inject sphinx breathe rocm-docs-core           $$$$\n"
             "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"
    )
  endif (NOT BREATHE_FOUND)


  ## configuration
  set (LIBM_DOC_VER ${${PROJECT_PREFIX}_VERSION})
  string(TIMESTAMP COPYRIGHT_YEAR "%Y")
  set (HTML_INDEX_FILE  ${CMAKE_CURRENT_BINARY_DIR}/html/index.html)
  set (DOXY_INPUT_FILES "${PROJECT_SOURCE_DIR}/include/external")


  ##List of files used to generate documentation
  set (SPHINX_INPUT_FILES_HEADERS
       ${PROJECT_SOURCE_DIR}/include/external/amdlibm.h
       ${PROJECT_SOURCE_DIR}/include/external/amdlibm_vec.h
  )

  set (SPHINX_INPUT_FILES_APIGUIDE
       ${CMAKE_CURRENT_SOURCE_DIR}/index.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/arithmetic_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/error_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/euclidean_distance_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/exponential_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/fp_manipulation_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/hyperbolic_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/invhyperbolic_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/invtrigonometric_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/logarithmic_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/nearest_integer_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/power_root_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/remainder_quotient.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/trigonometric_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/complex_api.rst
       ${CMAKE_CURRENT_SOURCE_DIR}/ag/min_max_diff.rst
  )

  set (SPHINX_INPUT_FILES
       ${SPHINX_INPUT_FILES_HEADERS}
       ${SPHINX_INPUT_FILES_APIGUIDE}
  )

  ## Doxygen config file
  set (DOXYCONF_IN  ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
  set (DOXYCONF_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  configure_file (${DOXYCONF_IN} ${DOXYCONF_OUT} @ONLY)

  ## Adding doxygen command
  list (APPEND COMMAND_GENERATE_LIBM_DOCS
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYCONF_OUT}
  )

  ## Sphinx config file
  set (SPHINXCONF_IN  ${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in)
  set (SPHINXCONF_OUT ${CMAKE_CURRENT_BINARY_DIR}/conf.py)
  configure_file (${SPHINXCONF_IN} ${SPHINXCONF_OUT} @ONLY)

  set (SPHINX_SRC "${CMAKE_CURRENT_SOURCE_DIR}/")
  set (SPHINX_IN  "${CMAKE_CURRENT_BINARY_DIR}/")
  set (SPHINX_OUT "${CMAKE_CURRENT_BINARY_DIR}/")

  ## Adding sphinx command
  list (APPEND COMMAND_GENERATE_LIBM_DOCS
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPHINX_SRC} ${SPHINX_IN}
        COMMAND ${SPHINX_EXECUTABLE} -M html ${SPHINX_IN} ${SPHINX_OUT}
  )

  list (APPEND LIBM_DOCS_DEPENDS_ON_FILES
               ${SPHINX_INPUT_FILES}
               ${DOXYCONF_IN}
               ${SPHINXCONF_IN}
  )


  ## Keep generated documents under <build>/aocl_docs folder
  list (APPEND COMMAND_LIBM_DOC_COPY
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${CMAKE_CURRENT_BINARY_DIR}/html
                ${PROJECT_BINARY_DIR}/aocl_docs/html ||
        echo "===== copy_directory ----- FAILED ====="
  )


  add_custom_command (OUTPUT ${HTML_INDEX_FILE}
                      ${COMMAND_GENERATE_LIBM_DOCS}
                      DEPENDS           ${LIBM_DOCS_DEPENDS_ON_FILES}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Doc build for AOCL-LibM Library"
                      VERBATIM
  )

  add_custom_target (libmdoc
                     ${COMMAND_LIBM_DOC_COPY}
                     DEPENDS           ${HTML_INDEX_FILE}
                     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                     COMMENT "Pseudo-target to build LibM Documents"
                     VERBATIM
  )

endif (LIBM_BUILD_DOCS)
