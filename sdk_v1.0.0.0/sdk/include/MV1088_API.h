#ifndef __MV1088_API_H__
#define __MV1088_API_H__

#include "MV1088_TYPE.h"

#ifdef __cplusplus
extern "C"
{
#endif
    /**
     * @brief 初始化指纹仪
     * @param ptInfo [in] 输入结构体参数
     * @return 0 success 非0 fail
     */
    __declspec(dllexport) int MV1088_InitDevice(ZWDeviceInitParam* ptInfo);

    /**
     * @brief 预览指纹图像
     * @return 0 success 非0 fail
     */
    __declspec(dllexport) int MV1088_SetVideoWindow(HWND hWnd);

    /**
     * @brief 开始采集指纹图像
     * @param finger 0-左手, 1-右手, 2-双拇指
     * @return 0 success 非0 fail
     */
    __declspec(dllexport) int MV1088_StartCapture(int finger);

    /**
     * @brief 停止采集指纹图像
     * @return 0 success 非0 fail
     */
    __declspec(dllexport) int MV1088_StopCapture();

    /**
     * @brief 释放设备
     * @return 0 success 非0 fail
     */
    __declspec(dllexport) int MV1088_UninitDevice();

#ifdef __cplusplus
} // extern "C"
#endif

#endif