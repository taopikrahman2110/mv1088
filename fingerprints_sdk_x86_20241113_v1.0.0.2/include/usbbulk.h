#ifndef __USBBULK_H	
#define __USBBULK_H	


#define MV1088
//#define TBS500
//#define PM100
#ifdef PM100
#define MY_VID 0x5168
#define MY_PID 0x1386
#endif

#ifdef OFS1200
#define MY_VID 0x5E88
#define MY_PID 0x1451
//#define ImageWidth   1280
//#define ImageHeight  1024
#endif

#ifdef TBS500
#define MY_VID 0x5E88
//#define MY_VID 0x2EF1//0x5E88
#define MY_PID 0x1453
//#define ImageWidth   2896//1376 //2896
//#define ImageHeight  1824//864 //1824
#endif

#ifdef OFS200I
#define MY_VID  0x1E25
#define MY_PID  0x2140
#define ImageWidth   1600
#define ImageHeight  1200
#endif

#define MY_CONFIG 1
#define MY_INTF 0

#define FULL_W 2896
#define FULL_H 1824
#define FULL_SIZE (FULL_W*FULL_H)

#define PREV_W 1440
#define PREV_H 912
#define PREV_SIZE (PREV_W*PREV_H)

//TBS500
#ifdef TBS500
#define ROLL_W 1376
#define ROLL_H 864
#endif

//MV1088
#ifdef MV1088	
#define MY_VID 0x5E88
#define MY_PID 0x1453
#define ROLL_W 1088
#define ROLL_H 736
#endif

#define ROLL_SIZE (ROLL_W*ROLL_H)

// Device endpoint(s)
#define EP_IN       0x82
#define EP_IN1      0x86
#define EP_OUT      0x02
#define EP_INTR     (1 | USB_ENDPOINT_IN)
#define EP_DATA     (2 | USB_ENDPOINT_IN)
#define CTRL_IN     (USB_TYPE_VENDOR | USB_ENDPOINT_IN)
#define CTRL_OUT    (USB_TYPE_VENDOR | USB_ENDPOINT_IN)

//Amcap Property Define
#define WR_LEDSTATUS    0x80
#define WR_LEDFINGER    0x81
#define WR_BEEP         0x82

#define WR_SENSOR       0x83
#define RD_SENSOR       0x84
#define RD_PARAMETER    0x85

#define STREAM_START    0xE1
#define STREAM_STOP     0xE2
#define WR_DeviceID     0xc5
#define WR_EXPOSURE     0xb4
#define WR_CONTRAST     0xb5
#define RD_BRIGHTNESS   0xb6
#define RD_CONTRAST     0xb7
#define WR_BRIGHTNESS   0xb8
#define WR_CHECKCMOS    0xb9
#define RD_SENSOR_SIZE  0xE3
#define RD_EXPOSURE     0xbb

#define UPDATE_FIRMWARE 0xc0		//used for debug
#define RD_DeviceInfo   0xE0		//26 bytes
#define MODE_FULL       0xc2
#define MODE_SUB        0xc3
#define MODE_ROLL       0xc4
#define GET_PARAMETERS  0xcf        //CSD200W Serial Num

#define RD_DEVICE_ID    0xd0

// Used for floating window.
#define GET_CONFIG      0xd1
#define SET_CONFIG      0xd2
#define	SET_Backlight   0xcd

// 2.a
#define USB_VENDOR_DEVICE_OUT   (USB_TYPE_VENDOR | USB_RECIP_DEVICE | USB_ENDPOINT_OUT)


// 曝光地址（值的范围 0 - 100）
#define Exp_RegAddr     0x0202

// 写刷新地址
#define FLASH_RegAddr   0x0104

// Gain寄存器地址
#define GainR_RegAddr   0x305A
#define GainB_RegAddr   0x3058
#define GainGr_RegAddr  0x3056
#define GainGb_RegAddr  0x305C

// 曝光灰阶值
#define GREY_THRESHOLD  200//200

// 曝光的最大值/最小值
#define EXP_MAX     300
#define EXP_MIN     10
#define EXP_DEFAULT 40

//Gain Range Define
 /*
 *  1 ~ 1.98	0x1040~0x107f
 *  2 ~ 3.97	0x1840~0x187f
 *  4 ~ 7.94	0x1C40~0x1C7f
 *  8 ~ 15.875	0x1CC0~0x1CFF
 * 16 ~ 31.75	0x1DC0~0x1DFF
 */
#define GAIN_RANGE0_MIN	0x1040
#define GAIN_RANGE0_MAX	0x107F
#define GAIN_RANGE1_MIN	0x1840
#define GAIN_RANGE1_MAX	0x187F
#define GAIN_RANGE2_MIN	0x1C40
#define GAIN_RANGE2_MAX	0x1C7F
#define GAIN_RANGE3_MIN	0x1CC0
#define GAIN_RANGE3_MAX	0x1CFF
#define GAIN_RANGE4_MIN	0x1DC0
#define GAIN_RANGE4_MAX	0x1DFF
#define GAIN_DEFAULT 0x1070


// 曝光E2PROM地址(Full mode)
#define EXP_EPROMH  0x1FC0
#define EXP_EPROML  0x1FC1

//--------------------------------------------------------------

// Full Mode
#define FULL_EXP_EPROMH		0x1FC0
#define FULL_EXP_EPROML		0x1FC1
#define FULL_GainB_EPROMH	0x1FC2
#define FULL_GainB_EPROML	0x1FC3
#define FULL_GainR_EPROMH	0x1FC4
#define FULL_GainR_EPROML	0x1FC5
#define FULL_GainGr_EPROMH	0x1FC6
#define FULL_GainGr_EPROML	0x1FC7
#define FULL_GainGb_EPROMH	0x1FC8
#define FULL_GainGb_EPROML	0x1FC9

// Preview Mode
#define PREV_EXP_EPROMH		0x1FC0+10
#define PREV_EXP_EPROML		0x1FC1+10
#define PREV_GainB_EPROMH	0x1FC2+10
#define PREV_GainB_EPROML	0x1FC3+10
#define PREV_GainR_EPROMH	0x1FC4+10
#define PREV_GainR_EPROML	0x1FC5+10
#define PREV_GainGr_EPROMH	0x1FC6+10
#define PREV_GainGr_EPROML	0x1FC7+10
#define PREV_GainGb_EPROMH	0x1FC8+10
#define PREV_GainGb_EPROML	0x1FC9+10

// Roll Mode
#define ROLL_EXP_EPROMH		0x1FC0+20
#define ROLL_EXP_EPROML		0x1FC1+20
#define ROLL_GainB_EPROMH	0x1FC2+20
#define ROLL_GainB_EPROML	0x1FC3+20
#define ROLL_GainR_EPROMH	0x1FC4+20
#define ROLL_GainR_EPROML	0x1FC5+20
#define ROLL_GainGr_EPROMH	0x1FC6+20
#define ROLL_GainGr_EPROML	0x1FC7+20
#define ROLL_GainGb_EPROMH	0x1FC8+20
#define ROLL_GainGb_EPROML	0x1FC9+20












#endif