# To find the cmake version run
# $ cmake --version
cmake_minimum_required(VERSION 2.8)

# Set the project name
project (lvgl_httem)

# -DCMAKE_EXPORT_COMPILE_COMMANDS=1
# output compile_json.txt
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)


set(CROSS_PATH /home/luoyang/App/GCC4Embed/hisi-linux/x86-arm/arm-hisiv600-linux)

# Target operating system name.
set(CMAKE_SYSTEM_NAME Linux)

# Where to look for the target environment. (More paths can be added here)
set(CMAKE_FIND_ROOT_PATH "${CROSS_PATH}")

# Adjust the default behavior of the FIND_XXX() commands:
# search programs in the host environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# Search headers and libraries in the target environment only.
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


# Name of C compiler.
set(CMAKE_C_COMPILER "${CROSS_PATH}/bin/arm-hisiv600-linux-gnueabi-gcc")
set(CMAKE_CXX_COMPILER "${CROSS_PATH}/bin/arm-hisiv600-linux-gnueabi-g++")

# c flags -fno-aggressive-loop-optimizations
# set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DLINUX -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon -mno-unaligned-access -ffunction-sections" CACHE STRING "Set C Compiler Flags" FORCE)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DWEBS -DUEMF -DOS=LINUX -std=gnu99 -DLV_CONF_INCLUDE_SIMPLE" CACHE STRING "Set C Compiler Flags" FORCE)

# link flags
set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -mcpu=cortex-a9 -mfloat-abi=softfp -mfpu=neon -mno-unaligned-access -fno-aggressive-loop-optimizations"  CACHE STRING "Set link Flags" FORCE)

# mmp libs
set(MPP_PATH /media/luoyang/Workspace/Hi3531DV100_SDK_V2.0.4.0/mpp)
set(LVGL_PATH /home/luoyang/workspace.lnk/libs/littlevgl-new)
set(HINK_PATH /home/luoyang/Dev/cross/hi3531d_v600/hink)

# Create a sources variable with link to all c files to compile
set(SOURCES
    src/main.c
    src/ui_main.c
    src/keypad.c
    src/ui_setting_panel.c
    src/ui_fb.c
    )

set(FONTS
    font/puhui_regular_16.c
    # font/puhui_regular_24.c
    # font/puhui_regular_28.c
    )

# Include header files
include_directories(
    ${MPP_PATH}/include
    ${HINK_PATH}/include
    )

# link libs path
link_directories(
    ${MPP_PATH}/lib
    ${LVGL_PATH}
    ${HINK_PATH}/lib
#    /home/luoyang/App/GCC4Embed/hisi-linux/x86-arm/arm-hisiv600-linux/arm-hisiv600-linux-gnueabi/lib
    )


set(LVGL_SRCS
    ${LVGL_PATH}/lvgl/src/lv_core/lv_group.c
    ${LVGL_PATH}/lvgl/src/lv_core/lv_indev.c
    ${LVGL_PATH}/lvgl/src/lv_core/lv_disp.c
    ${LVGL_PATH}/lvgl/src/lv_core/lv_obj.c
    ${LVGL_PATH}/lvgl/src/lv_core/lv_refr.c
    ${LVGL_PATH}/lvgl/src/lv_core/lv_style.c
    ${LVGL_PATH}/lvgl/src/lv_hal/lv_hal_disp.c
    ${LVGL_PATH}/lvgl/src/lv_hal/lv_hal_indev.c
    ${LVGL_PATH}/lvgl/src/lv_hal/lv_hal_tick.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_arc.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_bar.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_checkbox.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_cpicker.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_dropdown.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_keyboard.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_line.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_msgbox.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_spinner.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_roller.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_table.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_tabview.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_tileview.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_btn.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_calendar.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_chart.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_canvas.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_gauge.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_label.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_list.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_slider.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_textarea.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_spinbox.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_btnmatrix.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_cont.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_img.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_imgbtn.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_led.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_linemeter.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_page.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_switch.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_win.c
    ${LVGL_PATH}/lvgl/src/lv_widgets/lv_objmask.c
    ${LVGL_PATH}/lvgl/src/lv_font/lv_font.c
    ${LVGL_PATH}/lvgl/src/lv_font/lv_font_fmt_txt.c
    ${LVGL_PATH}/lvgl/src/lv_font/lv_font_loader.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_12.c
    ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_14.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_16.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_18.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_20.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_22.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_24.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_26.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_28.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_30.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_32.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_34.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_36.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_38.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_40.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_42.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_44.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_46.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_48.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_12_subpx.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_montserrat_28_compressed.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_unscii_8.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c
    # ${LVGL_PATH}/lvgl/src/lv_font/lv_font_simsun_16_cjk.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_area.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_task.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_fs.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_anim.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_mem.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_ll.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_color.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_txt.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_txt_ap.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_math.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_log.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_gc.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_utils.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_async.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_printf.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_bidi.c
    ${LVGL_PATH}/lvgl/src/lv_misc/lv_debug.c
    ${LVGL_PATH}/lvgl/src/lv_themes/lv_theme.c
    ${LVGL_PATH}/lvgl/src/lv_themes/lv_theme_material.c
    ${LVGL_PATH}/lvgl/src/lv_themes/lv_theme_mono.c
    ${LVGL_PATH}/lvgl/src/lv_themes/lv_theme_empty.c
    ${LVGL_PATH}/lvgl/src/lv_themes/lv_theme_template.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_mask.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_blend.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_rect.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_label.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_line.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_img.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_arc.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_draw_triangle.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_img_decoder.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_img_cache.c
    ${LVGL_PATH}/lvgl/src/lv_draw/lv_img_buf.c
    ${LVGL_PATH}/lv_drivers/display/fbdev.c
    ${LVGL_PATH}/lv_drivers/display/monitor.c
    ${LVGL_PATH}/lv_drivers/display/R61581.c
    ${LVGL_PATH}/lv_drivers/display/SSD1963.c
    ${LVGL_PATH}/lv_drivers/display/ST7565.c
    ${LVGL_PATH}/lv_drivers/display/UC1610.c
    ${LVGL_PATH}/lv_drivers/display/SHARP_MIP.c
    ${LVGL_PATH}/lv_drivers/indev/FT5406EE8.c
    ${LVGL_PATH}/lv_drivers/indev/keyboard.c
    ${LVGL_PATH}/lv_drivers/indev/mouse.c
    ${LVGL_PATH}/lv_drivers/indev/mousewheel.c
    ${LVGL_PATH}/lv_drivers/indev/evdev.c
    ${LVGL_PATH}/lv_drivers/indev/libinput.c
    ${LVGL_PATH}/lv_drivers/indev/XPT2046.c
    # ${LVGL_PATH}/lv_drivers/indev/mouse_hid.c
    )


# Include header files
include_directories(
    ${LVGL_PATH}
    ${LVGL_PATH}/lvgl
    ${LVGL_PATH}/lvgl/src/lv_core
    ${LVGL_PATH}/lvgl/src/lv_hal
    ${LVGL_PATH}/lvgl/src/lv_widgets
    ${LVGL_PATH}/lvgl/src/lv_font
    ${LVGL_PATH}/lvgl/src/lv_misc
    ${LVGL_PATH}/lvgl/src/lv_themes
    ${LVGL_PATH}/lvgl/src/lv_draw
    ${LVGL_PATH}/lv_drivers
    ${LVGL_PATH}/lv_drivers/display
    ${LVGL_PATH}/lv_drivers/indev
    )



# Add an executable
add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${LVGL_SRCS}
    ${FONTS}
    )

# Set the directories that should be included in the build command for this target
# when running gcc these will be included as -I/directories/path/
target_include_directories(${PROJECT_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/inc

    )


# Link libs
target_link_libraries( ${PROJECT_NAME}
    PRIVATE
    hink
    libmpi.a
    libhdmi.a
    libVoiceEngine.a
    libjpeg.a
    libupvqe.a
    libdnvqe.a
    rt
    pthread
    dl
    m
    )
