add_library(vi MODULE
    vi.cpp
    vietnamese.cpp
)
target_link_libraries(vi Fcitx5::Core)
set_target_properties(vi PROPERTIES PREFIX "")

install(TARGETS vi DESTINATION "${FCITX_INSTALL_ADDONDIR}")

install(FILES vi-addon.conf.in
    DESTINATION "${FCITX_INSTALL_PKGDATADIR}/addon"
    RENAME vi.conf)

install(FILES vi-im-telex.conf.in
    DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod"
    RENAME vi-telex.conf)

install(FILES vi-im-vni.conf.in
    DESTINATION "${FCITX_INSTALL_PKGDATADIR}/inputmethod"
    RENAME vi-vni.conf)
