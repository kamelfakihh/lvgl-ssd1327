#include "lvgl.h"
#include "OLED_1in5.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "Debug.h"

#include <stdlib.h> // malloc() free()
#include <math.h>

#define LV_DISP_DEF_REFR_MODE LV_DISP_REFR_MODE_FULL
#define DISP_HOR_RES 128
#define DISP_VER_RES 128

int  ws_disp_init();
void ws_disp_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p);

// OLED driver image buffer
UBYTE *BlackImage;

int main(void)
{
    // initialize lvgl
    lv_init();
    if(ws_disp_init() < 0)
    {
        fprintf(stderr, "Failed to initialize display");
        exit(-1);
    }

    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[DISP_HOR_RES * DISP_VER_RES];
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, DISP_HOR_RES * DISP_VER_RES);

    static lv_disp_drv_t disp_drv;          /*A variable to hold the drivers. Must be static or global.*/
    lv_disp_drv_init(&disp_drv);            /*Basic initialization*/
    disp_drv.draw_buf = &draw_buf;          /*Set an initialized buffer*/
    disp_drv.flush_cb = ws_disp_flush_cb;   /*Set a flush callback to draw to the display*/
    disp_drv.hor_res = DISP_HOR_RES;        /*Set the horizontal resolution in pixels*/
    disp_drv.ver_res = DISP_VER_RES;        /*Set the vertical resolution in pixels*/
    disp_drv.direct_mode = 1;

    lv_disp_t * disp;
    disp = lv_disp_drv_register(&disp_drv); /*Register the driver and save the created display objects*/

    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello World!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    while (1) {
        // lv_task_handler();
        lv_timer_handler();
    }

    return 0;
}

int ws_disp_init()
{
    if(DEV_ModuleInit() != 0) {
		return -1;
	}
    OLED_1in5_Init();
	UWORD Imagesize = ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT;
	if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
			printf("Failed to apply for black memory...\r\n");
			return -1;
	}
    OLED_1in5_Clear();
	Paint_NewImage(BlackImage, OLED_1in5_WIDTH, OLED_1in5_HEIGHT, 0, BLACK);
	Paint_SetScale(16);
    return 0;
}

void ws_disp_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p)
{
    /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one
     *`put_px` is just an example, it needs to implemented by you.*/
	Paint_SelectImage(BlackImage);

    int32_t x, y;
    for(y = area->y1; y <= area->y2; y++) {
        for(x = area->x1; x <= area->x2; x++) {
            color_p++;

            uint8_t red   = color_p->ch.red;
            uint8_t green = color_p->ch.green;
            uint8_t blue  = color_p->ch.blue;

            // uint8_t gray = 0.2989 * red + 0.5870 * green + 0.1140 * blue;
            uint8_t gray = (red + green + blue) / 3;

            // Convert grayscale to 4-bit (0-15 range)
            uint8_t gray4bit = gray >> 4;
	        Paint_DrawPoint(x, y, gray4bit, DOT_PIXEL_1X1, DOT_STYLE_DFT);
        }
    }

    OLED_1in5_Display(BlackImage);


    /* IMPORTANT!!!
     * Inform the graphics library that you are ready with the flushing*/
    lv_disp_flush_ready(disp_drv);
}