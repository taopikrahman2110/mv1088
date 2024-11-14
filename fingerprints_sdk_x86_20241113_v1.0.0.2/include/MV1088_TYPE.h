#ifndef __MV1088_TYPE_H__
#define __MV1088_TYPE_H__

#include "lusb0_usb.h"
#include "usbbulk.h"

#define  MV_ERROR_NONE			 0      //成功
#define  MV_ERROR_PARAMS		-1      //参数错误
#define  MV_ERROR_MEMORY		-2      //内存分配失败
#define  MV_ERROR_FUNCTION		-3	    //功能未实现
#define  MV_ERROR_DEVICE		-4      //设备不存在
#define  MV_ERROR_INIT			-5      //设备未初始化
#define  MV_ERROR_NUMBER		-6      //非法的错误号
#define  MV_ERROR_LICENSE		-7      //没有授权
#define  MV_ERROR_MODE			-8      //接口模式设置失败
#define  MV_ERROR_LIGHT			-9	    //控制灯失败
#define  MV_ERROR_IO		    -10	    //IO控制失败
#define  MV_ERROR_USBMSG		-11	    //usb通讯失败
#define  MV_ERROR_LOCK			-12	    //建立临界区失败
#define  MV_ERROR_THREAD		-13     //线程创建失败
#define  MV_ERROR_OUTTIME		-14	    //操作超时
#define  MV_ERROR_DIRTY			-15	    //镜面不干净
#define  MV_ERROR_EXISITWIN		-16	    //已经创建的预览窗口
#define  MV_ERROR_COLSEUSB      -105    //关闭USB失败
#define  MV_ERROR_PROCESS_INIT  -106    //检测初始化失败
#define  MV_ERROR_QUALITY       -107    //分数接口错误

#define  FINGER_IMAGE_WIDTH     400
#define  FINGER_IMAGE_HEIGHT    500
#define  MAX_FINGER_COUNT        4      //最大指纹数量
#define  DEFAULT_USB_CONTROL_MSG_TIME_OUT	1000

typedef struct _FingerDetail
{
    int x;				    //指纹图片左上角坐标
    int y;				    //指纹图片左上角坐标
    int width;			    //指纹图片宽度
    int height;			    //指纹图片高度
    int quality;            //指纹质量
    unsigned char Buffer[FINGER_IMAGE_WIDTH * FINGER_IMAGE_HEIGHT];
}ZWFingerDetail;

typedef struct _ImageData
{
    void* Buffer;                   //原始图片
    int				  Width;		//图片宽度
    int               Height;       //图片高度
    int				  FingerLR;	    //左右手检测
    int               FingerEdge;   //边缘检测
    int               FingerCount;	//数量检测
    ZWFingerDetail    FingerDetails[MAX_FINGER_COUNT];
}ZWImageData;

typedef int (CALLBACK* MV_CallbackPreviewImage)(void* pContext, const ZWImageData aZWImageData);

typedef struct _DeviceInitParam
{
    const char* modelPath;  //模型文件路径
    unsigned char iBright;  //背光灯亮度值
    unsigned char iContrast;//去杂阈值
    MV_CallbackPreviewImage tCallBackPreview;//预览回调
}ZWDeviceInitParam;

#endif __MV1088_TYPE_H__
