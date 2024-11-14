#ifndef __MV1088_TYPE_H__
#define __MV1088_TYPE_H__

#include "lusb0_usb.h"
#include "usbbulk.h"

#define  MV_ERROR_NONE			 0      //�ɹ�
#define  MV_ERROR_PARAMS		-1      //��������
#define  MV_ERROR_MEMORY		-2      //�ڴ����ʧ��
#define  MV_ERROR_FUNCTION		-3	    //����δʵ��
#define  MV_ERROR_DEVICE		-4      //�豸������
#define  MV_ERROR_INIT			-5      //�豸δ��ʼ��
#define  MV_ERROR_NUMBER		-6      //�Ƿ��Ĵ����
#define  MV_ERROR_LICENSE		-7      //û����Ȩ
#define  MV_ERROR_MODE			-8      //�ӿ�ģʽ����ʧ��
#define  MV_ERROR_LIGHT			-9	    //���Ƶ�ʧ��
#define  MV_ERROR_IO		    -10	    //IO����ʧ��
#define  MV_ERROR_USBMSG		-11	    //usbͨѶʧ��
#define  MV_ERROR_LOCK			-12	    //�����ٽ���ʧ��
#define  MV_ERROR_THREAD		-13     //�̴߳���ʧ��
#define  MV_ERROR_OUTTIME		-14	    //������ʱ
#define  MV_ERROR_DIRTY			-15	    //���治�ɾ�
#define  MV_ERROR_EXISITWIN		-16	    //�Ѿ�������Ԥ������
#define  MV_ERROR_COLSEUSB      -105    //�ر�USBʧ��
#define  MV_ERROR_PROCESS_INIT  -106    //����ʼ��ʧ��
#define  MV_ERROR_QUALITY       -107    //�����ӿڴ���

#define  FINGER_IMAGE_WIDTH     400
#define  FINGER_IMAGE_HEIGHT    500
#define  MAX_FINGER_COUNT        4      //���ָ������
#define  DEFAULT_USB_CONTROL_MSG_TIME_OUT	1000

typedef struct _FingerDetail
{
    int x;				    //ָ��ͼƬ���Ͻ�����
    int y;				    //ָ��ͼƬ���Ͻ�����
    int width;			    //ָ��ͼƬ���
    int height;			    //ָ��ͼƬ�߶�
    int quality;            //ָ������
    unsigned char Buffer[FINGER_IMAGE_WIDTH * FINGER_IMAGE_HEIGHT];
}ZWFingerDetail;

typedef struct _ImageData
{
    void* Buffer;                   //ԭʼͼƬ
    int				  Width;		//ͼƬ���
    int               Height;       //ͼƬ�߶�
    int				  FingerLR;	    //�����ּ��
    int               FingerEdge;   //��Ե���
    int               FingerCount;	//�������
    ZWFingerDetail    FingerDetails[MAX_FINGER_COUNT];
}ZWImageData;

typedef int (CALLBACK* MV_CallbackPreviewImage)(void* pContext, const ZWImageData aZWImageData);

typedef struct _DeviceInitParam
{
    const char* modelPath;  //ģ���ļ�·��
    unsigned char iBright;  //���������ֵ
    unsigned char iContrast;//ȥ����ֵ
    MV_CallbackPreviewImage tCallBackPreview;//Ԥ���ص�
}ZWDeviceInitParam;

#endif __MV1088_TYPE_H__
