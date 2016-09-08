#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
    #include <platform/mt_i2c.h> 
    #include <platform/mt_pmic.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
//  #include <mach/mt_gpio.h>
//  #include <mach/mt_pm_ldo.h>
//  #include <mach/upmu_common.h>
//    #include <linux/i2c.h>
#endif
#include "../nt50358_dcdc.h"
// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (1080)
#define FRAME_HEIGHT (1920)
#define REGFLAG_DELAY             							0XFEFF
#define REGFLAG_END_OF_TABLE      							0xFFFF   // END OF REGISTERS MARKER

#define LCM_ID_NT35596 (0x96)

#define GPIO_LCD_RST_EN      (GPIO146| 0x80000000)
#define GPIO_LCD_ENP_EN      (GPIO82 | 0x80000000)
#define GPIO_LCD_ENN_EN      (GPIO88 | 0x80000000)

#define LCM_SCL1_GPIO		(GPIO50 | 0x80000000)
#define LCM_SDA1_GPIO		(GPIO49 | 0x80000000)

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))

// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)   


#define   LCM_DSI_CMD_MODE							0

struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};


static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

    for(i = 0; i < count; i++) {
		
        unsigned cmd;
        cmd = table[i].cmd;
		
        switch (cmd) {
			
            case REGFLAG_DELAY :
                MDELAY(table[i].count);
                break;
				
            case REGFLAG_END_OF_TABLE :
                break;
				
            default:
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       	}
    }
	
}



static struct LCM_setting_table lcm_initialization_setting[] = {
    {0xFF, 1 , {0xEE}},
    {0xFB, 1 , {0x01}},
    {0x18, 1 , {0x40}},
    {REGFLAG_DELAY, 10, {}},
    {0x18, 1 , {0x00}},
    {REGFLAG_DELAY, 20, {}},
    //CMD2_P4}},
    {0xFF, 1 , {0x05}},
    {0xC5, 1 , {0x31}},
    {0xFB, 1 , {0x01}},
    {REGFLAG_DELAY, 20, {}},

    //CMD3_PA}},
    {0xFF, 1 , {0xEE}},
    {0x7C, 1 , {0x31}},
    {0xFB, 1 , {0x01}},

    //CMD2_P0}},
    {0xFF, 1 , {0x01}},
    {0x00, 1 , {0x01}},
    {0x01, 1 , {0x55}},
    {0x02, 1 , {0x40}},
    {0x05, 1 , {0x50}},
    {0x06, 1 , {0x4A}},
    {0x07, 1 , {0x29}},
    {0x08, 1 , {0x0C}},
    {0x0B, 1 , {0x9B}},
    {0x0C, 1 , {0x9B}},
    {0x0E, 1 , {0xB0}},
    {0x0F, 1 , {0xB3}},
    {0x11, 1 , {0x10}},
    {0x12, 1 , {0x10}},
    {0x13, 1 , {0x03}},
    {0x14, 1 , {0x4A}},
    {0x15, 1 , {0x12}},
    {0x16, 1 , {0x12}},
    {0x18, 1 , {0x00}},
    {0x19, 1 , {0x77}},
    {0x1A, 1 , {0x55}},
    {0x1B, 1 , {0x13}},
    {0x1C, 1 , {0x00}},
    {0x1D, 1 , {0x00}},
    {0x1E, 1 , {0x00}},
    {0x1F, 1 , {0x00}},
    {0x58, 1 , {0x82}},
    {0x59, 1 , {0x02}},
    {0x5A, 1 , {0x02}},
    {0x5B, 1 , {0x02}},
    {0x5C, 1 , {0x82}},
    {0x5D, 1 , {0x82}},
    {0x5E, 1 , {0x02}},
    {0x5F, 1 , {0x02}},
    {0x72, 1 , {0x31}},
    {0xFB, 1 , {0x01}},

    //CMD2_P4}},
    {0xFF, 1 , {0x05}},
    {0x00, 1 , {0x01}},
    {0x01, 1 , {0x0B}},
    {0x02, 1 , {0x0C}},
    {0x03, 1 , {0x09}},
    {0x04, 1 , {0x0A}},
    {0x05, 1 , {0x00}},
    {0x06, 1 , {0x0F}},
    {0x07, 1 , {0x10}},
    {0x08, 1 , {0x00}},
    {0x09, 1 , {0x00}},
    {0x0A, 1 , {0x00}},
    {0x0B, 1 , {0x00}},
    {0x0C, 1 , {0x00}},
    {0x0D, 1 , {0x13}},
    {0x0E, 1 , {0x15}},
    {0x0F, 1 , {0x17}},
    {0x10, 1 , {0x01}},
    {0x11, 1 , {0x0B}},
    {0x12, 1 , {0x0C}},
    {0x13, 1 , {0x09}},
    {0x14, 1 , {0x0A}},
    {0x15, 1 , {0x00}},
    {0x16, 1 , {0x0F}},
    {0x17, 1 , {0x10}},
    {0x18, 1 , {0x00}},
    {0x19, 1 , {0x00}},
    {0x1A, 1 , {0x00}},
    {0x1B, 1 , {0x00}},
    {0x1C, 1 , {0x00}},
    {0x1D, 1 , {0x13}},
    {0x1E, 1 , {0x15}},
    {0x1F, 1 , {0x17}},
    {0x20, 1 , {0x00}},
    {0x21, 1 , {0x03}},
    {0x22, 1 , {0x01}},
    {0x23, 1 , {0x36}},
    {0x24, 1 , {0x36}},
    {0x25, 1 , {0xED}},
    {0x29, 1 , {0x58}},
    {0x2A, 1 , {0x12}},
    {0x2B, 1 , {0x01}},
    {0x4B, 1 , {0x06}},
    {0x4C, 1 , {0x11}},
    {0x4D, 1 , {0x20}},
    {0x4E, 1 , {0x02}},
    {0x4F, 1 , {0x02}},
    {0x50, 1 , {0x20}},
    {0x51, 1 , {0x61}},
    {0x52, 1 , {0x01}},
    {0x53, 1 , {0x72}},
    {0x54, 1 , {0x75}},
    {0x55, 1 , {0xED}},
    {0x5B, 1 , {0x00}},
    {0x5C, 1 , {0x00}},
    {0x5F, 1 , {0x15}},
    {0x60, 1 , {0x75}},
    {0x63, 1 , {0x00}},
    {0x64, 1 , {0x00}},
    {0x67, 1 , {0x00}},
    {0x68, 1 , {0x04}},
    {0x6C, 1 , {0x40}},
    {0x7A, 1 , {0x80}},
    {0x7B, 1 , {0xA3}},
    {0x7C, 1 , {0xD8}},
    {0x7D, 1 , {0x60}},
    {0x7E, 1 , {0x08}},
    {0x7F, 1 , {0x1C}},
    {0x80, 1 , {0x00}},
    {0x83, 1 , {0x00}},
    {0x93, 1 , {0x08}},
    {0x94, 1 , {0x0A}},
    {0x8A, 1 , {0x00}},
    {0x9B, 1 , {0x0F}},
    {0xE7, 1 , {0x80}},
    {0xFB, 1 , {0x01}},

    //page selection cmd start}},
    {0xFF, 1 , {0x01}},
    {0xFB, 1 , {0x01}},
    //page selection cmd end}},
    //R(+) MCR cmd}},
    {0x75, 1, {0x00}},
    {0x76, 1, {0x00}},
    {0x77, 1, {0x00}},
    {0x78, 1, {0x5E}},
    {0x79, 1, {0x00}},
    {0x7A, 1, {0x8C}},
    {0x7B, 1, {0x00}},
    {0x7C, 1, {0xAA}},
    {0x7D, 1, {0x00}},
    {0x7E, 1, {0xC2}},
    {0x7F, 1, {0x00}},
    {0x80, 1, {0xD5}},
    {0x81, 1, {0x00}},
    {0x82, 1, {0xE6}},
    {0x83, 1, {0x00}},
    {0x84, 1, {0xF6}},
    {0x85, 1, {0x01}},
    {0x86, 1, {0x03}},
    {0x87, 1, {0x01}},
    {0x88, 1, {0x30}},
    {0x89, 1, {0x01}},
    {0x8A, 1, {0x54}},
    {0x8B, 1, {0x01}},
    {0x8C, 1, {0x8C}},
    {0x8D, 1, {0x01}},
    {0x8E, 1, {0xB7}},
    {0x8F, 1, {0x01}},
    {0x90, 1, {0xFB}},
    {0x91, 1, {0x02}},
    {0x92, 1, {0x31}},
    {0x93, 1, {0x02}},
    {0x94, 1, {0x33}},
    {0x95, 1, {0x02}},
    {0x96, 1, {0x66}},
    {0x97, 1, {0x02}},
    {0x98, 1, {0xA0}},
    {0x99, 1, {0x02}},
    {0x9A, 1, {0xC5}},
    {0x9B, 1, {0x02}},
    {0x9C, 1, {0xFA}},
    {0x9D, 1, {0x03}},
    {0x9E, 1, {0x1C}},
    {0x9F, 1, {0x03}},
    {0xA0, 1, {0x4D}},
    {0xA2, 1, {0x03}},
    {0xA3, 1, {0x5B}},
    {0xA4, 1, {0x03}},
    {0xA5, 1, {0x6B}},
    {0xA6, 1, {0x03}},
    {0xA7, 1, {0x7C}},
    {0xA9, 1, {0x03}},
    {0xAA, 1, {0x87}},
    {0xAB, 1, {0x03}},
    {0xAC, 1, {0x9F}},
    {0xAD, 1, {0x03}},
    {0xAE, 1, {0xC0}},
    {0xAF, 1, {0x03}},
    {0xB0, 1, {0xFE}},
    {0xB1, 1, {0x03}},
    {0xB2, 1, {0xFF}},
    //R(-) MCR cmd}},
    {0xB3, 1, {0x00}},
    {0xB4, 1, {0x00}},
    {0xB5, 1, {0x00}},
    {0xB6, 1, {0x5E}},
    {0xB7, 1, {0x00}},
    {0xB8, 1, {0x8C}},
    {0xB9, 1, {0x00}},
    {0xBA, 1, {0xAA}},
    {0xBB, 1, {0x00}},
    {0xBC, 1, {0xC2}},
    {0xBD, 1, {0x00}},
    {0xBE, 1, {0xD5}},
    {0xBF, 1, {0x00}},
    {0xC0, 1, {0xE6}},
    {0xC1, 1, {0x00}},
    {0xC2, 1, {0xF6}},
    {0xC3, 1, {0x01}},
    {0xC4, 1, {0x03}},
    {0xC5, 1, {0x01}},
    {0xC6, 1, {0x30}},
    {0xC7, 1, {0x01}},
    {0xC8, 1, {0x54}},
    {0xC9, 1, {0x01}},
    {0xCA, 1, {0x8C}},
    {0xCB, 1, {0x01}},
    {0xCC, 1, {0xB7}},
    {0xCD, 1, {0x01}},
    {0xCE, 1, {0xFB}},
    {0xCF, 1, {0x02}},
    {0xD0, 1, {0x31}},
    {0xD1, 1, {0x02}},
    {0xD2, 1, {0x33}},
    {0xD3, 1, {0x02}},
    {0xD4, 1, {0x66}},
    {0xD5, 1, {0x02}},
    {0xD6, 1, {0xA0}},
    {0xD7, 1, {0x02}},
    {0xD8, 1, {0xC5}},
    {0xD9, 1, {0x02}},
    {0xDA, 1, {0xFA}},
    {0xDB, 1, {0x03}},
    {0xDC, 1, {0x1C}},
    {0xDD, 1, {0x03}},
    {0xDE, 1, {0x4D}},
    {0xDF, 1, {0x03}},
    {0xE0, 1, {0x5B}},
    {0xE1, 1, {0x03}},
    {0xE2, 1, {0x6B}},
    {0xE3, 1, {0x03}},
    {0xE4, 1, {0x7C}},
    {0xE5, 1, {0x03}},
    {0xE6, 1, {0x87}},
    {0xE7, 1, {0x03}},
    {0xE8, 1, {0x9F}},
    {0xE9, 1, {0x03}},
    {0xEA, 1, {0xC0}},
    {0xEB, 1, {0x03}},
    {0xEC, 1, {0xFE}},
    {0xED, 1, {0x03}},
    {0xEE, 1, {0xFF}},
    //G(+) MCR cmd}},
    {0xEF, 1, {0x00}},
    {0xF0, 1, {0x00}},
    {0xF1, 1, {0x00}},
    {0xF2, 1, {0x51}},
    {0xF3, 1, {0x00}},
    {0xF4, 1, {0x82}},
    {0xF5, 1, {0x00}},
    {0xF6, 1, {0xA2}},
    {0xF7, 1, {0x00}},
    {0xF8, 1, {0xBB}},
    {0xF9, 1, {0x00}},
    {0xFA, 1, {0xD0}},
    //page selection cmd start}},
    {0xFF, 1 , {0x02}},
    {0xFB, 1 , {0x01}},
    //page selection cmd end}},
    {0x00, 1, {0x00}},
    {0x01, 1, {0xE1}},
    {0x02, 1, {0x00}},
    {0x03, 1, {0xF1}},
    {0x04, 1, {0x00}},
    {0x05, 1, {0xFF}},
    {0x06, 1, {0x01}},
    {0x07, 1, {0x2D}},
    {0x08, 1, {0x01}},
    {0x09, 1, {0x51}},
    {0x0A, 1, {0x01}},
    {0x0B, 1, {0x89}},
    {0x0C, 1, {0x01}},
    {0x0D, 1, {0xB5}},
    {0x0E, 1, {0x01}},
    {0x0F, 1, {0xF9}},
    {0x10, 1, {0x02}},
    {0x11, 1, {0x30}},
    {0x12, 1, {0x02}},
    {0x13, 1, {0x31}},
    {0x14, 1, {0x02}},
    {0x15, 1, {0x64}},
    {0x16, 1, {0x02}},
    {0x17, 1, {0x9D}},
    {0x18, 1, {0x02}},
    {0x19, 1, {0xC1}},
    {0x1A, 1, {0x02}},
    {0x1B, 1, {0xF6}},
    {0x1C, 1, {0x03}},
    {0x1D, 1, {0x18}},
    {0x1E, 1, {0x03}},
    {0x1F, 1, {0x47}},
    {0x20, 1, {0x03}},
    {0x21, 1, {0x55}},
    {0x22, 1, {0x03}},
    {0x23, 1, {0x64}},
    {0x24, 1, {0x03}},
    {0x25, 1, {0x75}},
    {0x26, 1, {0x03}},
    {0x27, 1, {0x7B}},
    {0x28, 1, {0x03}},
    {0x29, 1, {0x8E}},
    {0x2A, 1, {0x03}},
    {0x2B, 1, {0xC0}},
    {0x2D, 1, {0x03}},
    {0x2F, 1, {0xD6}},
    {0x30, 1, {0x03}},
    {0x31, 1, {0xFF}},
    //G(-) MCR cmd}},
    {0x32, 1, {0x00}},
    {0x33, 1, {0x00}},
    {0x34, 1, {0x00}},
    {0x35, 1, {0x51}},
    {0x36, 1, {0x00}},
    {0x37, 1, {0x82}},
    {0x38, 1, {0x00}},
    {0x39, 1, {0xA2}},
    {0x3A, 1, {0x00}},
    {0x3B, 1, {0xBB}},
    {0x3D, 1, {0x00}},
    {0x3F, 1, {0xD0}},
    {0x40, 1, {0x00}},
    {0x41, 1, {0xE1}},
    {0x42, 1, {0x00}},
    {0x43, 1, {0xF1}},
    {0x44, 1, {0x00}},
    {0x45, 1, {0xFF}},
    {0x46, 1, {0x01}},
    {0x47, 1, {0x2D}},
    {0x48, 1, {0x01}},
    {0x49, 1, {0x51}},
    {0x4A, 1, {0x01}},
    {0x4B, 1, {0x89}},
    {0x4C, 1, {0x01}},
    {0x4D, 1, {0xB5}},
    {0x4E, 1, {0x01}},
    {0x4F, 1, {0xF9}},
    {0x50, 1, {0x02}},
    {0x51, 1, {0x30}},
    {0x52, 1, {0x02}},
    {0x53, 1, {0x31}},
    {0x54, 1, {0x02}},
    {0x55, 1, {0x64}},
    {0x56, 1, {0x02}},
    {0x58, 1, {0x9D}},
    {0x59, 1, {0x02}},
    {0x5A, 1, {0xC1}},
    {0x5B, 1, {0x02}},
    {0x5C, 1, {0xF6}},
    {0x5D, 1, {0x03}},
    {0x5E, 1, {0x18}},
    {0x5F, 1, {0x03}},
    {0x60, 1, {0x47}},
    {0x61, 1, {0x03}},
    {0x62, 1, {0x55}},
    {0x63, 1, {0x03}},
    {0x64, 1, {0x64}},
    {0x65, 1, {0x03}},
    {0x66, 1, {0x75}},
    {0x67, 1, {0x03}},
    {0x68, 1, {0x7B}},
    {0x69, 1, {0x03}},
    {0x6A, 1, {0x8E}},
    {0x6B, 1, {0x03}},
    {0x6C, 1, {0xC0}},
    {0x6D, 1, {0x03}},
    {0x6E, 1, {0xD6}},
    {0x6F, 1, {0x03}},
    {0x70, 1, {0xFF}},
    //B(+) MCR cmd}},
    {0x71, 1, {0x00}},
    {0x72, 1, {0x00}},
    {0x73, 1, {0x00}},
    {0x74, 1, {0x3F}},
    {0x75, 1, {0x00}},
    {0x76, 1, {0x72}},
    {0x77, 1, {0x00}},
    {0x78, 1, {0x94}},
    {0x79, 1, {0x00}},
    {0x7A, 1, {0xAE}},
    {0x7B, 1, {0x00}},
    {0x7C, 1, {0xC4}},
    {0x7D, 1, {0x00}},
    {0x7E, 1, {0xD6}},
    {0x7F, 1, {0x00}},
    {0x80, 1, {0xE7}},
    {0x81, 1, {0x00}},
    {0x82, 1, {0xF6}},
    {0x83, 1, {0x01}},
    {0x84, 1, {0x26}},
    {0x85, 1, {0x01}},
    {0x86, 1, {0x4C}},
    {0x87, 1, {0x01}},
    {0x88, 1, {0x85}},
    {0x89, 1, {0x01}},
    {0x8A, 1, {0xB2}},
    {0x8B, 1, {0x01}},
    {0x8C, 1, {0xF8}},
    {0x8D, 1, {0x02}},
    {0x8E, 1, {0x2E}},
    {0x8F, 1, {0x02}},
    {0x90, 1, {0x30}},
    {0x91, 1, {0x02}},
    {0x92, 1, {0x61}},
    {0x93, 1, {0x02}},
    {0x94, 1, {0x99}},
    {0x95, 1, {0x02}},
    {0x96, 1, {0xBE}},
    {0x97, 1, {0x02}},
    {0x98, 1, {0xF5}},
    {0x99, 1, {0x03}},
    {0x9A, 1, {0x1F}},
    {0x9B, 1, {0x03}},
    {0x9C, 1, {0x61}},
    {0x9D, 1, {0x03}},
    {0x9E, 1, {0x7A}},
    {0x9F, 1, {0x03}},
    {0xA0, 1, {0xA3}},
    {0xA2, 1, {0x03}},
    {0xA3, 1, {0xE9}},
    {0xA4, 1, {0x03}},
    {0xA5, 1, {0xEB}},
    {0xA6, 1, {0x03}},
    {0xA7, 1, {0xED}},
    {0xA9, 1, {0x03}},
    {0xAA, 1, {0xEF}},
    {0xAB, 1, {0x03}},
    {0xAC, 1, {0xF6}},
    {0xAD, 1, {0x03}},
    {0xAE, 1, {0xFF}},
    //B(-) MCR cmd}},
    {0xAF, 1, {0x00}},
    {0xB0, 1, {0x00}},
    {0xB1, 1, {0x00}},
    {0xB2, 1, {0x3F}},
    {0xB3, 1, {0x00}},
    {0xB4, 1, {0x72}},
    {0xB5, 1, {0x00}},
    {0xB6, 1, {0x94}},
    {0xB7, 1, {0x00}},
    {0xB8, 1, {0xAE}},
    {0xB9, 1, {0x00}},
    {0xBA, 1, {0xC4}},
    {0xBB, 1, {0x00}},
    {0xBC, 1, {0xD6}},
    {0xBD, 1, {0x00}},
    {0xBE, 1, {0xE7}},
    {0xBF, 1, {0x00}},
    {0xC0, 1, {0xF6}},
    {0xC1, 1, {0x01}},
    {0xC2, 1, {0x26}},
    {0xC3, 1, {0x01}},
    {0xC4, 1, {0x4C}},
    {0xC5, 1, {0x01}},
    {0xC6, 1, {0x85}},
    {0xC7, 1, {0x01}},
    {0xC8, 1, {0xB2}},
    {0xC9, 1, {0x01}},
    {0xCA, 1, {0xF8}},
    {0xCB, 1, {0x02}},
    {0xCC, 1, {0x2E}},
    {0xCD, 1, {0x02}},
    {0xCE, 1, {0x30}},
    {0xCF, 1, {0x02}},
    {0xD0, 1, {0x61}},
    {0xD1, 1, {0x02}},
    {0xD2, 1, {0x99}},
    {0xD3, 1, {0x02}},
    {0xD4, 1, {0xBE}},
    {0xD5, 1, {0x02}},
    {0xD6, 1, {0xF5}},
    {0xD7, 1, {0x03}},
    {0xD8, 1, {0x1F}},
    {0xD9, 1, {0x03}},
    {0xDA, 1, {0x61}},
    {0xDB, 1, {0x03}},
    {0xDC, 1, {0x7A}},
    {0xDD, 1, {0x03}},
    {0xDE, 1, {0xA3}},
    {0xDF, 1, {0x03}},
    {0xE0, 1, {0xE9}},
    {0xE1, 1, {0x03}},
    {0xE2, 1, {0xEB}},
    {0xE3, 1, {0x03}},
    {0xE4, 1, {0xED}},
    {0xE5, 1, {0x03}},
    {0xE6, 1, {0xEF}},
    {0xE7, 1, {0x03}},
    {0xE8, 1, {0xF6}},
    {0xE9, 1, {0x03}},
    {0xEA, 1, {0xFF}},
    {0xFB, 1 , {0x01}},

    {0xFF, 1 , {0x00}},
    {0xD3, 1 , {0x06}},
    {0xD4, 1 , {0x04}},
    {0x35, 1 , {0x00}},

    {0x11, 1 , {0x00}},
    {REGFLAG_DELAY, 100, {}},

    {0x29, 1 , {0x00}},
};



static void lcd_power_en(unsigned char enabled)
{
    if (enabled)
    {
		//pmic_config_interface(MT6328_PMIC_RG_VGP1_VOSEL_ADDR, 0x6, MT6328_PMIC_RG_VGP1_VOSEL_MASK, MT6328_PMIC_RG_VGP1_VOSEL_SHIFT);
		//pmic_config_interface(MT6328_PMIC_RG_VGP1_EN_ADDR, 0x1, MT6328_PMIC_RG_VGP1_EN_MASK, MT6328_PMIC_RG_VGP1_EN_SHIFT);

        //MDELAY(5);
    #ifdef LCM_USE_NT50358_DCDC
        lcm_gpio_output(1,1);
    
        if (get_dcdc_type()) {
            dw8769_set_discharge_status(0x03,0x80);// REG0x03: 0x83:disbale ;0x80: enbale
        } else {
            nt50358_dcdc_set_bost(0x05,0x30);//bost volatge:5.6V
        }
        //MDELAY(2);
        nt50358_dcdc_set_avdd(0x00,0x0e);//+5.5V
        MDELAY(5);
        lcm_gpio_output(2,1);
        nt50358_dcdc_set_avee(0x01,0x0e);//-5.5V
        MDELAY(5);
    #endif
    } else {
        //pmic_config_interface(MT6328_PMIC_RG_VGP1_VOSEL_ADDR, 0x0, MT6328_PMIC_RG_VGP1_VOSEL_MASK, MT6328_PMIC_RG_VGP1_VOSEL_SHIFT);
        //pmic_config_interface(MT6328_PMIC_RG_VGP1_EN_ADDR, 0x0, MT6328_PMIC_RG_VGP1_EN_MASK, MT6328_PMIC_RG_VGP1_EN_SHIFT);
    #ifdef LCM_USE_NT50358_DCDC
        lcm_gpio_output(1,0);
        lcm_gpio_output(2,0);
    #endif
    }
}


// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
	memset(params, 0, sizeof(LCM_PARAMS));

	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

    params->physical_width = 87;
    params->physical_height = 125;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
	params->dsi.switch_mode = SYNC_PULSE_VDO_MODE;
#else
    params->dsi.mode = SYNC_EVENT_VDO_MODE;
	params->dsi.switch_mode = CMD_MODE;
#endif
	params->dsi.switch_mode_enable = 0;

	// DSI
	/* Command mode setting */
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.color_order 	= LCM_COLOR_ORDER_RGB;
	params->dsi.data_format.trans_seq   	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
	params->dsi.data_format.padding     	= LCM_DSI_PADDING_ON_LSB;
	params->dsi.data_format.format      		= LCM_DSI_FORMAT_RGB888;

	// Highly depends on LCD driver capability.
	params->dsi.packet_size=256;
	// Video mode setting		
	params->dsi.intermediat_buffer_num = 0;// 0;

	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	params->dsi.word_count=FRAME_WIDTH*3;	
		
	params->dsi.vertical_sync_active				= 2;
	params->dsi.vertical_backporch					= 3;
	params->dsi.vertical_frontporch					= 20;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 10;
	params->dsi.horizontal_backporch				= 118;
	params->dsi.horizontal_frontporch				= 118;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
   // params->dsi.TA_GO =5;
	//params->dsi.compatibility_for_nvk = 1;

	// Bit rate calculation
	params->dsi.PLL_CLOCK = 500;

    params->dsi.cont_clock=0; //1;

	params->dsi.clk_lp_per_line_enable = 0;
	params->dsi.esd_check_enable = 1;
	params->dsi.customization_esd_check_enable = 1;
	params->dsi.lcm_esd_check_table[0].cmd          = 0x0a;
	params->dsi.lcm_esd_check_table[0].count        = 1;
	params->dsi.lcm_esd_check_table[0].para_list[0] = 0x9c;

}

#if 0
static void lcm_init_power(void)
{
	//printk("otm1282c %s", __func__);
	lcd_power_en(1);
}

static void lcm_suspend_power(void)
{
		//printk("otm1282c %s", __func__);
}

static void lcm_resume_power(void)
{
	//printk("otm1282c %s", __func__);
}
#endif

static void lcm_init(void)
{
    //unsigned int data_array[16];
    printk("nt35596 %s", __func__);
    lcd_power_en(0);
    MDELAY(1000); 
    //MDELAY(10);
    lcm_gpio_output(0,0);
    MDELAY(10);
    lcd_power_en(1);
    MDELAY(50);
    lcm_gpio_output(0,0);
    MDELAY(10);
    lcm_gpio_output(0,1);
    MDELAY(50);

	//init_lcm_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}

static void lcm_suspend(void)
{
    unsigned int data_array[16];
    //unsigned char buffer[2];

    data_array[0] = 0x00280500; // Display Off
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);
    data_array[0] = 0x00100500;  // Sleep In
    dsi_set_cmdq(data_array, 1, 1);
    MDELAY(120);

    lcm_gpio_output(0,0);

	lcd_power_en(0);
}


static void lcm_resume(void)
{
   //1 do lcm init again to solve some display issue
    lcd_power_en(1);
    MDELAY(50);
    lcm_gpio_output(0,0);
    MDELAY(10);
    lcm_gpio_output(0,1);
    MDELAY(50);  
	//init_lcm_registers();
	push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	printk("%s, zhangchongyong\n", __func__);
}
         
#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00290508; //HW bug, so need send one HS packet
	dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif

static unsigned int lcm_compare_id(void)
{


    //unsigned int id0,id1,id=0;
    unsigned char buffer[2];
    unsigned int array[16];

#if 0
    mt_set_gpio_mode(GPIO_LCD_RST_EN, GPIO_MODE_00);
    mt_set_gpio_dir(GPIO_LCD_RST_EN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);
        lcd_power_en(1);
    MDELAY(10);
    mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ZERO);
    MDELAY(50);
    mt_set_gpio_out(GPIO_LCD_RST_EN, GPIO_OUT_ONE);
    MDELAY(500);
#else
    SET_RESET_PIN(1);
    SET_RESET_PIN(0);
    MDELAY(1);

    SET_RESET_PIN(1);
    MDELAY(20);

#endif
/*
	array[0]=0x00DE0500;
    	dsi_set_cmdq(array, 1, 1);

    	array[0]=0x32B41500; 
    	dsi_set_cmdq(array, 1, 1);

    	array[0]=0x00DF0500;
    	dsi_set_cmdq(array, 1, 1);
*/
	array[0] = 0x00013700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0xF4, buffer, 2);
	
    #ifdef BUILD_LK
	printf("%s, LK huarui ly nt35596 id = 0x%08x\n", __func__, buffer[0]);
   #else
	printk("%s, Kernel huarui ly nt35596 id = 0x%08x\n", __func__, buffer[0]);
   #endif
  
   return (0x96 == buffer[0])?1:0; 


}



static unsigned int lcm_ata_check(unsigned char *buffer)
{
    extern int lcm_ata_check_flag;

    return (lcm_ata_check_flag > 0) ? 1 : 0;
}

LCM_DRIVER nt35596_fhd_dsi_vdo_ykl_ly_lcm_drv = 
{
    .name			= "nt35596_fhd_dsi_vdo_ykl_ly",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id    = lcm_compare_id,

	.ata_check		= lcm_ata_check,

	//.esd_check = lcm_esd_check,
 	//.esd_recover = lcm_esd_recover,
    #if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
    #endif
    };
