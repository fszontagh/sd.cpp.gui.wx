
set(OUTPUT_NAME "stablediffusiongui")

# install paths

set(locale_INSTALL_PATH_NSIS "share/locale")


if (WIN32)
	set(locale_INSTALL_PATH_NSIS ".")
endif()


find_package(Gettext)
if (Gettext_FOUND)
    message(STATUS "Gettext found")
else ()
    message(STATUS "Gettext Not found!")
endif ()

find_program(GETTEXT_XGETTEXT_EXECUTABLE xgettext PATHS "C:\\msys64\\usr\\bin\\")
find_program(GETTEXT_MSGMERGE_EXECUTABLE msgmerge PATHS "C:\\msys64\\usr\\bin\\")
find_program(GETTEXT_MSGFMT_EXECUTABLE msgfmt PATHS "C:\\msys64\\usr\\bin\\")


if (GETTEXT_XGETTEXT_EXECUTABLE)
    file(GLOB_RECURSE CPP_FILES CONFIGURE_DEPENDS RELATIVE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.h ${CMAKE_SOURCE_DIR}/extprocess/*.cpp ${CMAKE_SOURCE_DIR}/extprocess/*.h)
    add_custom_target(
        pot-update
        COMMENT "pot-update: Done."
        DEPENDS ${CMAKE_SOURCE_DIR}/locale/${OUTPUT_NAME}.pot
    )

    add_custom_command(
        TARGET pot-update
        PRE_BUILD
        COMMAND
            ${GETTEXT_XGETTEXT_EXECUTABLE}
            --from-code=utf-8
            --c++
            --force-po
            --output=${CMAKE_SOURCE_DIR}/locale/${OUTPUT_NAME}.pot
            --keyword=_
            --width=80
            --default-domain=${OUTPUT_NAME}
            --copyright-holder="${CMAKE_PROJECT_NAME}"
            --package-name="${CMAKE_PROJECT_NAME}"
            --package-version="${PROJECT_VERSION}"
            --msgid-bugs-address="sdgui@fsociety.hu"
            ${CPP_FILES}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "pot-update: Pot file generated: ${CMAKE_SOURCE_DIR}/locale/${OUTPUT_NAME}.pot"
    )

endif (GETTEXT_XGETTEXT_EXECUTABLE)

if (GETTEXT_MSGMERGE_EXECUTABLE)

    add_custom_target(
        pot-merge
        COMMENT "pot-merge: Done."
        DEPENDS ${CMAKE_SOURCE_DIR}/locale/${OUTPUT_NAME}.pot
    )

    file(GLOB PO_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/locale/*/${OUTPUT_NAME}.po)
    foreach(PO_FILE IN ITEMS ${PO_FILES})
        message(STATUS " Adding msgmerge for: ${PO_FILE}")
        add_custom_command(
            TARGET pot-merge
            PRE_BUILD
            COMMAND
                ${GETTEXT_MSGMERGE_EXECUTABLE}
                -o ${PO_FILE}
                ${PO_FILE}
                ${CMAKE_SOURCE_DIR}/locale/${OUTPUT_NAME}.pot
            COMMENT "pot-merge: ${PO_FILE}"
        )
    endforeach()

endif (GETTEXT_MSGMERGE_EXECUTABLE)

if (GETTEXT_MSGFMT_EXECUTABLE)

    message(DEBUG " msgmerge: ${GETTEXT_MSGFMT_EXECUTABLE}")
    file(GLOB PO_LANGS
        RELATIVE ${CMAKE_SOURCE_DIR}/locale
        LIST_DIRECTORIES true
        ${CMAKE_SOURCE_DIR}/locale/*
    )
    message(TRACE " PO_LANGS: ${PO_LANGS}")

    add_custom_target(
        po-compile
        COMMENT "po-compile: Done."
    )

    foreach(PO_LANG IN ITEMS ${PO_LANGS})
        if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/locale/${PO_LANG})
        message(STATUS " Adding msgfmt for: ${CMAKE_BINARY_DIR}/locale/${PO_LANG}/LC_MESSAGES/${OUTPUT_NAME}.mo")
        add_custom_command(
            TARGET po-compile
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/locale/${PO_LANG}/LC_MESSAGES
            COMMAND
                ${GETTEXT_MSGFMT_EXECUTABLE}
                -c
                --output-file=${CMAKE_BINARY_DIR}/locale/${PO_LANG}/LC_MESSAGES/${OUTPUT_NAME}.mo
                ${OUTPUT_NAME}.po
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/locale/${PO_LANG}
            COMMENT "po-compile: ${PO_LANG}"
        )
        install(FILES ${CMAKE_BINARY_DIR}/locale/${PO_LANG}/LC_MESSAGES/${OUTPUT_NAME}.mo DESTINATION ${locale_INSTALL_PATH_NSIS}/${PO_LANG}/LC_MESSAGES COMPONENT "${CMAKE_PROJECT_NAME}")
        else()
            message(STATUS " Skipping msgfmt for: ${PO_LANG}")
        endif()
    endforeach()

endif (GETTEXT_MSGFMT_EXECUTABLE)
