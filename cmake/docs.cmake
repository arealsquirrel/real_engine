
# 1. Find the Doxygen executable on the system
find_package(Doxygen REQUIRED)

if(DOXYGEN_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        doxygen_awesome_theme
        GIT_REPOSITORY https://github.com/jothepro/doxygen-awesome-css.git
        GIT_TAG        v2.3.4
    )

    FetchContent_MakeAvailable(doxygen_awesome_theme)

    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_HTML_EXTRA_STYLESHEET "${doxygen_awesome_theme_SOURCE_DIR}/doxygen-awesome.css")
    set(DOXYGEN_HTML_EXTRA_FILES
        "${doxygen_awesome_theme_SOURCE_DIR}/doxygen-awesome-darkmode-toggle.js"
        "${doxygen_awesome_theme_SOURCE_DIR}/doxygen-awesome-fragment-copy-button.js"
        "${doxygen_awesome_theme_SOURCE_DIR}/doxygen-awesome-paragraph-link.js"
        "${doxygen_awesome_theme_SOURCE_DIR}/doxygen-awesome-interactive-toc.js"
    )

    set(DOXYGEN_HTML_HEADER_FILES "") 

    set(DOXYGEN_GENERATE_TREEVIEW YES)
    set(DOXYGEN_DISABLE_INDEX NO)
    set(DOXYGEN_FULL_SIDEBAR NO)
    set(DOXYGEN_HTML_COLORSTYLE "LIGHT") # Managed dynamically by the theme

    doxygen_add_docs(docs
        "${PROJECT_SOURCE_DIR}/engine/src"
        "${PROJECT_SOURCE_DIR}/engine/include"
        COMMENT "Generating API documentation with Doxygen Awesome theme"
    )
endif()