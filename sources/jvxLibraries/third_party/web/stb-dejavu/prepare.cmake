# =====================================================================================
# Third party content for the text output of the plot overlay in jvx-opengl-cpp
# (jvxVideoOverlayText.h). Nothing of it is part of the repository - in the configure
# step it is downloaded and generated into this folder:
#
#   include/stb/stb_truetype.h        stb_truetype (pinned commit), public domain / MIT
#   include/fonts/jvxFontDejaVuSans.h DejaVu Sans 2.37, subset embedded as byte array,
#                                     Bitstream Vera / Arev font license (in the header)
#
# The font subset requires Python 3 with fontTools (pip install fonttools).
#
# After inclusion, JVX_STB_DEJAVU_INCLUDE_DIR holds the include path.
# =====================================================================================

set(JVX_STB_DEJAVU_DIR ${CMAKE_CURRENT_LIST_DIR})
set(JVX_STB_DEJAVU_INCLUDE_DIR ${JVX_STB_DEJAVU_DIR}/include)
set(JVX_STB_DEJAVU_DOWNLOAD_DIR ${JVX_STB_DEJAVU_DIR}/download)

# stb_truetype v1.27 - release commit "update version numbers"
set(JVX_STB_COMMIT 48fb1c9709f973041a7e6992d720ac625266cdd3)
set(JVX_STB_SHA256 94b846320ae50233a25ea4aa451b8cc8150a6654aa3d97b8a422355ce3fdbbc3)
set(JVX_STB_URL https://raw.githubusercontent.com/nothings/stb/${JVX_STB_COMMIT}/stb_truetype.h)

# DejaVu fonts 2.37
set(JVX_DEJAVU_VERSION 2.37)
set(JVX_DEJAVU_SHA256 7576310b219e04159d35ff61dd4a4ec4cdba4f35c00e002a136f00e96a908b0a)
set(JVX_DEJAVU_URL https://github.com/dejavu-fonts/dejavu-fonts/releases/download/version_2_37/dejavu-fonts-ttf-2.37.zip)

function(jvx_stb_dejavu_download url file sha256)
	if(EXISTS ${file})
		file(SHA256 ${file} hash_have)
		if(hash_have STREQUAL sha256)
			return()
		endif()
		file(REMOVE ${file})
	endif()
	message("--> Downloading ${url}")
	file(DOWNLOAD ${url} ${file} EXPECTED_HASH SHA256=${sha256} TLS_VERIFY ON STATUS dl_status)
	list(GET dl_status 0 dl_code)
	if(NOT dl_code EQUAL 0)
		list(GET dl_status 1 dl_msg)
		file(REMOVE ${file})
		message(FATAL_ERROR "Download of <${url}> failed: ${dl_msg}")
	endif()
endfunction()

# 1) stb_truetype
jvx_stb_dejavu_download(${JVX_STB_URL} ${JVX_STB_DEJAVU_INCLUDE_DIR}/stb/stb_truetype.h ${JVX_STB_SHA256})

# 2) DejaVu Sans -> subset -> font header. Regenerate if font, generator or header are changed / missing.
set(JVX_DEJAVU_ZIP ${JVX_STB_DEJAVU_DOWNLOAD_DIR}/dejavu-fonts-ttf-${JVX_DEJAVU_VERSION}.zip)
set(JVX_DEJAVU_EXTRACTED ${JVX_STB_DEJAVU_DOWNLOAD_DIR}/dejavu-fonts-ttf-${JVX_DEJAVU_VERSION})
set(JVX_DEJAVU_HEADER ${JVX_STB_DEJAVU_INCLUDE_DIR}/fonts/jvxFontDejaVuSans.h)
set(JVX_DEJAVU_STAMP ${JVX_STB_DEJAVU_DOWNLOAD_DIR}/font_header.stamp)

file(SHA256 ${JVX_STB_DEJAVU_DIR}/gen_font_header.py gen_hash)
set(stamp_expected "${JVX_DEJAVU_SHA256}-${gen_hash}")
set(stamp_have "")
if(EXISTS ${JVX_DEJAVU_STAMP})
	file(READ ${JVX_DEJAVU_STAMP} stamp_have)
endif()

if(NOT EXISTS ${JVX_DEJAVU_HEADER} OR NOT stamp_have STREQUAL stamp_expected)

	jvx_stb_dejavu_download(${JVX_DEJAVU_URL} ${JVX_DEJAVU_ZIP} ${JVX_DEJAVU_SHA256})

	file(REMOVE_RECURSE ${JVX_DEJAVU_EXTRACTED})
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar xf ${JVX_DEJAVU_ZIP}
		WORKING_DIRECTORY ${JVX_STB_DEJAVU_DOWNLOAD_DIR}
		RESULT_VARIABLE res)
	if(NOT res EQUAL 0 OR NOT EXISTS ${JVX_DEJAVU_EXTRACTED}/ttf/DejaVuSans.ttf)
		message(FATAL_ERROR "Extracting <${JVX_DEJAVU_ZIP}> failed.")
	endif()

	find_package(Python3 COMPONENTS Interpreter QUIET)
	if(NOT Python3_Interpreter_FOUND)
		message(FATAL_ERROR "Python 3 is required to generate the overlay font (jvx-opengl-cpp).")
	endif()
	execute_process(
		COMMAND ${Python3_EXECUTABLE} -c "import fontTools.subset"
		RESULT_VARIABLE res OUTPUT_QUIET ERROR_QUIET)
	if(NOT res EQUAL 0)
		message(FATAL_ERROR "Python module fontTools is required to generate the overlay font (jvx-opengl-cpp). "
			"Install it with: ${Python3_EXECUTABLE} -m pip install fonttools")
	endif()

	file(MAKE_DIRECTORY ${JVX_STB_DEJAVU_INCLUDE_DIR}/fonts)
	execute_process(
		COMMAND ${Python3_EXECUTABLE} ${JVX_STB_DEJAVU_DIR}/gen_font_header.py
			--ttf ${JVX_DEJAVU_EXTRACTED}/ttf/DejaVuSans.ttf
			--license ${JVX_DEJAVU_EXTRACTED}/LICENSE
			--out ${JVX_DEJAVU_HEADER}
		RESULT_VARIABLE res)
	if(NOT res EQUAL 0)
		file(REMOVE ${JVX_DEJAVU_HEADER})
		message(FATAL_ERROR "Generating the overlay font header <${JVX_DEJAVU_HEADER}> failed.")
	endif()
	file(WRITE ${JVX_DEJAVU_STAMP} "${stamp_expected}")
endif()
