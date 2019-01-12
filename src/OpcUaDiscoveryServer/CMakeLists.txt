# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# prebuild
#
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/Build/BuildConfig.h.in"
    "${PROJECT_BINARY_DIR}/BuildConfig.h"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/etc/init/OpcUaDiscoveryServer.conf.in"
    "${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer.conf"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/etc/init.d/OpcUaDiscoveryServer.in"
    "${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer"
)

configure_file(
    "${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/usr/lib/systemd/system/OpcUaDiscoveryServer.service.in"
    "${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer.service"
)


# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# build 
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
include_directories(
    ${OPCUASTACK_INSTALL_PREFIX}/usr/include/OpcUaStack${OPCUASTACK_VERSION_MAJOR}
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${Boost_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
)

file(
    GLOB 
    OpcUaDiscoveryServer_SRC 
    ${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/*/*.cpp
)

add_library(
    OpcUaDiscoveryServer SHARED 
    ${OpcUaDiscoveryServer_SRC}
)

target_link_libraries(
    OpcUaDiscoveryServer
    ${pthread}
    ${OPCUASTACK_LIBRARY_CORE}
    ${OPCUASTACK_LIBRARY_CLIENT}
    ${OPCUASTACK_LIBRARY_SERVER}
    ${OPCUASTACK_LIBRARY_PUBSUB}
    ${Boost_LIBRARIES}
)

# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------
#
# install
# 
# -----------------------------------------------------------------------------
# -----------------------------------------------------------------------------

file (GLOB CONFIG_FILES ${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/Config/*.xml)
install(TARGETS OpcUaDiscoveryServer DESTINATION ${INSTALL_PREFIX}/lib)

if (WIN32)
    install(FILES ${CONFIG_FILES} DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaDiscoveryServer) 
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/log/README DESTINATION ${INSTALL_PREFIX}/../var/log/OpcUaStack/OpcUaDiscoveryServer)
    install(DIRECTORY DESTINATION ${INSTALL_PREFIX}/../var/log/OpcUaStack/OpcUaDiscoveryServer)
else()    
    install(FILES ${CONFIG_FILES} DESTINATION ${INSTALL_PREFIX}/../etc/OpcUaStack/OpcUaDiscoveryServer) 
    install(FILES ${PROJECT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/log/README DESTINATION /var/log/OpcUaStack/OpcUaDiscoveryServer)
    install(DIRECTORY DESTINATION /var/log/OpcUaStack/OpcUaDiscoveryServer)

    install(PROGRAMS ${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer DESTINATION /etc/init.d)
    install(FILES ${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer.conf DESTINATION /etc/init)
    install(FILES ${PROJECT_BINARY_DIR}/OpcUaDiscoveryServer.service DESTINATION /usr/lib/systemd/system)
endif()

if (CPACK_BINARY_DEB)
    set( CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/postinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/preinst"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/prerm"
                                            "${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/postrm")
endif()

if (CPACK_BINARY_RPM)
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/postinst)
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/OpcUaDiscoveryServer/linux/script/prerm)
endif()
