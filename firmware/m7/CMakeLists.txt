cmake_minimum_required(VERSION 3.20)
project(gcu3_m7_display C)

set(CMAKE_C_STANDARD 11)

add_executable(gcu3_m7_display
    src/main.c
    app/startup/m7_application.c
    service/boot_manager/boot_manager.c
    service/display_manager/display_manager.c
    service/ownership_manager/ownership_manager.c
    service/ipc_manager/ipc_manager.c
    service/health_manager/health_manager.c
    service/fault_manager/fault_manager.c
    service/diagnostic_manager/diagnostic_manager.c
    hal/display/display_hal.c
    hal/mipi_dsi/mipi_dsi_hal.c
    hal/i2c/i2c_hal.c
    hal/backlight/backlight_hal.c
    hal/power/power_hal.c
    hal/ipc/ipc_hal.c
    hal/timer/timer_hal.c
    display_engine/display_engine.c
    driver/dpu/dpu_driver.c
    driver/mipi_dsi/mipi_dsi_driver.c
    driver/display_link/display_link.c
    driver/display_link/hdmi/hdmi_adapter.c
    driver/display_link/fpd_link/fpd_link_adapter.c
    driver/display_link/gmsl/gmsl_adapter.c
    bsp/imx95/imx95_bsp.c
    display_engine/framebuffer/framebuffer_assets.c
)

target_include_directories(gcu3_m7_display PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/app/startup
    ${CMAKE_CURRENT_SOURCE_DIR}/service/boot_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/display_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/ownership_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/ipc_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/health_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/fault_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/service/diagnostic_manager
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/display
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/mipi_dsi
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/i2c
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/ipc
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/timer
    ${CMAKE_CURRENT_SOURCE_DIR}/display_engine
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/dpu
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/mipi_dsi
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/display_link
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/display_link/hdmi
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/display_link/fpd_link
    ${CMAKE_CURRENT_SOURCE_DIR}/driver/display_link/gmsl
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/power
    ${CMAKE_CURRENT_SOURCE_DIR}/hal/backlight
    ${CMAKE_CURRENT_SOURCE_DIR}/bsp/imx95
)

# NOTE: MCUXpresso SDK include paths (fsl_lpi2c.h, fsl_mipi_dsi.h,
# fsl_dpu.h, rpmsg_lite.h, etc.) are provided by the west/CMake SDK
# toolchain integration (SdkRootDirPath) at the outer build-system level
# per the GCU3 Phase2-1 M7 build automation; not re-declared here to
# avoid duplicating/hardcoding SDK paths that differ between the
# west-based CI build and local IDE builds.

target_compile_options(gcu3_m7_display PRIVATE
    -Wall -Wextra -Werror
)

# GCU3: add display_engine/framebuffer/framebuffer_assets.c to the MCUXpresso source list.
