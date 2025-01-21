include(CreateCPackCommon)

list(APPEND CPACK_GENERATOR "DEB")
set(CPACK_BINARY_DEB ON)

set(QGC_INSTALLER_SOURCE "${CMAKE_SOURCE_DIR}/deploy/linux")

set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_NAME ${CPACK_PACKAGE_NAME})
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
# set(CPACK_DEBIAN_PACKAGE_EPOCH
set(CPACK_DEBIAN_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION})
set(CPACK_DEBIAN_PACKAGE_RELEASE ${QGC_GIT_HASH})
# set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
# set(CPACK_DEBIAN_PACKAGE_DEPENDS
# set(CPACK_DEBIAN_ENABLE_COMPONENT_DEPENDS
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dronecode")
# set(CPACK_DEBIAN_PACKAGE_DESCRIPTION
# set(CPACK_DEBIAN_PACKAGE_SECTION
set(CPACK_DEBIAN_ARCHIVE_TYPE xz)
# set(CPACK_DEBIAN_COMPRESSION_TYPE
# set(CPACK_DEBIAN_PACKAGE_PRIORITY
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE ${PROJECT_HOMEPAGE_URL})
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS 1)
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS
# set(CPACK_DEBIAN_PACKAGE_DEBUG
# set(CPACK_DEBIAN_PACKAGE_PREDEPENDS
# set(CPACK_DEBIAN_PACKAGE_ENHANCES
# set(CPACK_DEBIAN_PACKAGE_BREAKS
# set(CPACK_DEBIAN_PACKAGE_CONFLICTS
# set(CPACK_DEBIAN_PACKAGE_PROVIDES
# set(CPACK_DEBIAN_PACKAGE_REPLACES
# set(CPACK_DEBIAN_PACKAGE_RECOMMENDS
# set(CPACK_DEBIAN_PACKAGE_SUGGESTS
# set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS
# set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY
# set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
# set(CPACK_DEBIAN_PACKAGE_CONTROL_STRICT_PERMISSION
# set(CPACK_DEBIAN_PACKAGE_SOURCE

# set(CPACK_DEBIAN_DEBUGINFO_PACKAGE

include(CPack)