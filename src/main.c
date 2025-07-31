/*******************************************************************
 *
 * main.c - LVGL simulator for GNU/Linux
 *
 * Based on the original file from the repository
 *
 * @note eventually this file won't contain a main function and will
 * become a library supporting all major operating systems
 *
 * To see how each driver is initialized check the
 * 'src/lib/display_backends' directory
 *
 * - Clean up
 * - Support for multiple backends at once
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

#define USE_IMG_BUTTONS 1

#if LV_COLOR_DEPTH == 32
#define BPP_DEFINE "32bpp"
#define BTN_DEFINE "wgt"
#elif LV_COLOR_DEPTH == 16
#define BPP_DEFINE "16bpp"
#if (USE_IMG_BUTTONS == 1)
#define BTN_DEFINE "img"
#else
#define BTN_DEFINE "wgt"
#endif
#else
#define BPP_DEFINE "unknown"
#endif

/* Internal functions */
static void configure_simulator(int argc, char **argv);
static void print_lvgl_version(void);
static void print_usage(void);
static void lv_scroll_test();

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char *selected_backend;

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;


/**
 * @brief Print LVGL version
 */
static void print_lvgl_version(void)
{
    fprintf(stdout, "%d.%d.%d-%s\n",
            LVGL_VERSION_MAJOR,
            LVGL_VERSION_MINOR,
            LVGL_VERSION_PATCH,
            LVGL_VERSION_INFO);
}

/**
 * @brief Print usage information
 */
static void print_usage(void)
{
    fprintf(stdout, "\nlvglsim [-V] [-B] [-b backend_name] [-W window_width] [-H window_height]\n\n");
    fprintf(stdout, "-V print LVGL version\n");
    fprintf(stdout, "-B list supported backends\n");
}

/**
 * @brief Configure simulator
 * @description process arguments recieved by the program to select
 * appropriate options
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
static void configure_simulator(int argc, char **argv)
{
    int opt = 0;

    selected_backend = NULL;
    driver_backends_register();

    const char *env_w = getenv("LV_SIM_WINDOW_WIDTH");
    const char *env_h = getenv("LV_SIM_WINDOW_HEIGHT");
    /* Default values */
    settings.window_width = atoi(env_w ? env_w : "800");
    settings.window_height = atoi(env_h ? env_h : "480");

    /* Parse the command-line options. */
    while ((opt = getopt (argc, argv, "b:fmW:H:BVh")) != -1) {
        switch (opt) {
        case 'h':
            print_usage();
            exit(EXIT_SUCCESS);
            break;
        case 'V':
            print_lvgl_version();
            exit(EXIT_SUCCESS);
            break;
        case 'B':
            driver_backends_print_supported();
            exit(EXIT_SUCCESS);
            break;
        case 'b':
            if (driver_backends_is_supported(optarg) == 0) {
                die("error no such backend: %s\n", optarg);
            }
            selected_backend = strdup(optarg);
            break;
        case 'W':
            settings.window_width = atoi(optarg);
            break;
        case 'H':
            settings.window_height = atoi(optarg);
            break;
        case ':':
            print_usage();
            die("Option -%c requires an argument.\n", optopt);
            break;
        case '?':
            print_usage();
            die("Unknown option -%c.\n", optopt);
        }
    }
}

/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char **argv)
{

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    if (driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    /* Enable for EVDEV support */
#if LV_USE_EVDEV
    if (driver_backends_init_backend("EVDEV") == -1) {
        die("Failed to initialize evdev");
    }
#endif

    /*Create a Demo*/
#if 0
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();
#elif 0
    lv_demo_benchmark();
#elif 1
    lv_scroll_test();
#endif

    /* Enter the run loop of the selected backend */
    driver_backends_run_loop();

    return 0;
}

LV_IMAGE_DECLARE(imgButton32bpp);
LV_IMAGE_DECLARE(imgButton16bpp);

LV_IMAGE_DECLARE(imgBlender);
LV_IMAGE_DECLARE(imgClock);
LV_IMAGE_DECLARE(imgCoffeePot);
LV_IMAGE_DECLARE(imgCooktop);
LV_IMAGE_DECLARE(imgEspresso);
LV_IMAGE_DECLARE(imgHome);
LV_IMAGE_DECLARE(imgMixer);
LV_IMAGE_DECLARE(imgOven);
LV_IMAGE_DECLARE(imgSettings);
LV_IMAGE_DECLARE(imgTeaMaker);

LV_IMAGE_DECLARE(imgBlenderFill);
LV_IMAGE_DECLARE(imgClockFill);
LV_IMAGE_DECLARE(imgCoffeePotFill);
LV_IMAGE_DECLARE(imgCooktopFill);
LV_IMAGE_DECLARE(imgEspressoFill);
LV_IMAGE_DECLARE(imgHomeFill);
LV_IMAGE_DECLARE(imgMixerFill);
LV_IMAGE_DECLARE(imgOvenFill);
LV_IMAGE_DECLARE(imgSettingsFill);
LV_IMAGE_DECLARE(imgTeaMakerFill);

const lv_image_dsc_t * menu_icons[] =
{
#if LV_COLOR_DEPTH == 32
    &imgBlender,
    &imgClock,
    &imgCoffeePot,
    &imgCooktop,
    &imgEspresso,
    &imgHome,
    &imgMixer,
    &imgOven,
    &imgSettings,
    &imgTeaMaker,
#elif LV_COLOR_DEPTH == 16
    &imgBlenderFill,
    &imgClockFill,
    &imgCoffeePotFill,
    &imgCooktopFill,
    &imgEspressoFill,
    &imgHomeFill,
    &imgMixerFill,
    &imgOvenFill,
    &imgSettingsFill,
    &imgTeaMakerFill,
#endif
};
   

void lv_scroll_test()
{
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x121212), 0);

    /* Static Text */
    lv_obj_t * static_text = lv_label_create(lv_screen_active());
    lv_label_set_text(static_text, "Scritte Statiche");
    lv_obj_align(static_text, LV_ALIGN_TOP_LEFT, 50, 50);
    lv_obj_set_style_text_font(static_text, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(static_text, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN | LV_STATE_DEFAULT);

    /* Static Text */
    lv_obj_t * config_text = lv_label_create(lv_screen_active());
    lv_label_set_text(config_text, "Config: "BPP_DEFINE", " BTN_DEFINE);
    lv_obj_align(config_text, LV_ALIGN_BOTTOM_LEFT, 0, 20);
    lv_obj_set_style_text_font(config_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(config_text, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * build_text = lv_label_create(lv_screen_active());
    lv_label_set_text(build_text, "Build: "__DATE__", " __TIME__);
    lv_obj_align(build_text, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_style_text_font(build_text, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(build_text, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_t * panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(panel, 800, 280);
    lv_obj_set_scroll_snap_x(panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_align(panel, LV_ALIGN_OUT_BOTTOM_MID, 0, 120);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_bg_opa(panel, LV_OPA_TRANSP, 0); // Make background transparent
    lv_obj_set_style_border_width(panel, 0, 0); // Remove border     
    lv_obj_set_style_radius(panel, 0, LV_STATE_DEFAULT); // Remove border  

    uint32_t i;
    for(i = 0; i < sizeof(menu_icons)/sizeof(lv_image_dsc_t *); i++) 
    {
#if (USE_IMG_BUTTONS == 1)
        lv_obj_t * btn = lv_imagebutton_create(panel);
        lv_obj_set_size(btn, 279, 250);

    #if (LV_COLOR_DEPTH == 32)
        lv_imagebutton_set_src(btn, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &imgButton32bpp,
                           NULL);

    #else
        lv_imagebutton_set_src(btn, LV_IMAGEBUTTON_STATE_RELEASED, NULL, &imgButton16bpp,
                           NULL);

    #endif
#else
        lv_obj_t * btn = lv_button_create(panel);
        lv_obj_set_size(btn, 279, 250);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0x2A2A2A), 0); // Black background (opacity will be handled next)        
        lv_obj_set_style_shadow_width(btn, 0, 0);
#endif        

        lv_obj_t * icon = lv_image_create(btn);
        lv_image_set_src(icon, menu_icons[i]);
        lv_obj_align(icon, LV_ALIGN_TOP_LEFT, 30, 50);        

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "Scritte %" LV_PRIu32, i);
        lv_obj_set_style_text_color(label, lv_color_make(0xff, 0xff, 0xff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_align(label, LV_ALIGN_OUT_BOTTOM_MID, 30, 130);
        lv_obj_set_style_text_font(label, &lv_font_montserrat_26, 0);
    }
    lv_obj_update_snap(panel, LV_ANIM_ON);
}
