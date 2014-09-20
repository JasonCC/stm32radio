/**********************************************************************************************************
*	                                  
*	ģ������ : da����ģ��
*	�ļ����� : pcm1770.h
*	��    �� : V1.0
*	˵    �� : ͨ��iis�ӿ�����daоƬ
*	�޸ļ�¼ :
*		�汾��  ����        ����           ˵��
*		
*		v1.0    2012-10-19  wangkai        
*
*	Copyright (C), 2010-2011, UP MCU ������
*   �Ա��꣺   http://shop73275611.taobao.com
*   QQ����Ⱥ�� 258043068
*
**********************************************************************************************************/
#ifndef __PCM1770_H
#define __PCM1770_H

#ifdef __cplusplus
 extern "C" {
#endif



void PCM1770Init(void);
void PCM_WriteData(const u8 Reg, const u8 Data);
void Volume_Dec(void);
void Volume_Add(void);
u8 GetCurrentVolume(void);
void PCM1770_Mute(void);
void PCM1770_VolumeSet(vu8 vol);

#ifdef __cplusplus
}
#endif

#endif /* __PCM1770_H */


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
