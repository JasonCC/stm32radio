#include <finsh.h>
#include <dfs_posix.h>
#include "netbuffer.h"
#include "stm32f10x.h"
#include "codec.h"
#include "player_bg.h"
#include "player_ui.h"

/*UP MCU ��������ԭ�������������Ӧ�޸�   ���ܷ���wav�ļ����ļ�ͷȻ�����ò����ʲ���
*   �Ա��꣺   http://shop73275611.taobao.com
*   QQ����Ⱥ�� 258043068
    ��֧�� ������Ϊ2�ֽ� ��wav
*/
extern  rt_uint16_t BufData[2][2304];	
extern struct rt_semaphore s_semForPlay;
static rt_uint8_t s_BuffIndex;

typedef struct tWAVEFORMATEX{
	rt_uint16_t wFormatTag;	  //  ��ʽ��־��01��ʾpcm  û��ѹ����
	rt_uint16_t nChannels;	  //  ������  1Ϊ������  2Ϊ������   4Ϊ4����
	rt_uint32_t nSamplesPerSec;//  ����Ƶ�� 0x5622����22050 ����11025  44100��
	rt_uint32_t nAvgBytesPerSec;// ÿ��Ҫ���ŵ��ֽ��� =nChannels* nBlockAlign
	rt_uint16_t nBlockAlign;	   // ����һ����ʱ�����ֽ��� 16λ������Ϊ2  16λ˫����Ϊ4  
	rt_uint16_t wBitsPerSample; // ��Ƶ������С
	rt_uint32_t cbSize;		   // ��Ƶ���ݵĴ�С
}WAVEFORMATEX;


void get_wav_info(const char* filename, struct tag_info* info)
{
	WAVEFORMATEX winfo;
	int fd;
	char buff[60];
	
	memset(&winfo, 0, sizeof(winfo));
	fd = open(filename, O_RDONLY, 0);
	if (read(fd, (char*)buff, 0x2c)==0) return;
		
	if (strncmp("WAVEfmt", (char *)buff + 8, 7) != 0 )
	{//��wav��ʽ�۲�����
		rt_kprintf("format is not support !\n\r");
		close(fd);
		return;
	}	
	
	//����Ƶ���ݵ���ʼץȡ ��Ϣ�����ڲ��Ų��������õ�
	winfo.wFormatTag		= 0x01;			 //��ʾpcm����
	winfo.nChannels		    = *(rt_uint16_t *)(buff+0x16);
	winfo.nSamplesPerSec	= *(rt_uint32_t *)(buff+0x18);
	winfo.nAvgBytesPerSec	= *(rt_uint32_t *)(buff+0x1c);
	winfo.nBlockAlign		= *(rt_uint16_t *)(buff+0x20);
	winfo.wBitsPerSample	= *(rt_uint16_t *)(buff+0x22);
	winfo.cbSize            = *(rt_uint32_t *)(buff+0x28);  
	
	info->duration=(rt_uint32_t)(winfo.cbSize/winfo.nAvgBytesPerSec);
	info->data_start=(rt_uint32_t)0x2c;
	info->bit_rate= winfo.nAvgBytesPerSec*8;
	
	close(fd);
	
}


void wav(const char* filename)
{
	extern rt_uint32_t current_offset;
    int fd;
	rt_err_t result;
	WAVEFORMATEX winfo;
	char buff[60];
	
	s_BuffIndex = 0;
    fd = open(filename, O_RDONLY, 0);
    if (fd >= 0)
    {
		rt_size_t 	len;
	    result = rt_sem_init(&s_semForPlay, "semforplay", 1, RT_IPC_FLAG_FIFO);
	    if (result != RT_EOK)
	    {
			rt_kprintf(" semaphore: semforplay init failed./r/n");
			rt_sem_detach(&s_semForPlay);/* �����ź������� */
			
	    }
		
		len = read(fd, (char*)buff, 0x2c);
		if (strncmp("WAVEfmt", (char *)buff + 8, 7) != 0 )
		{//��wav��ʽ�۲�����
			rt_kprintf("format is not support !\n\r");
			close(fd);
			return;
		}	
		memset(&winfo, 0, sizeof(winfo));	
		//����Ƶ���ݵ���ʼץȡ ��Ϣ�����ڲ��Ų��������õ�
		winfo.wFormatTag		= 0x01;			 //��ʾpcm����
		winfo.nChannels		    = *(rt_uint16_t *)(buff+0x16);
		winfo.nSamplesPerSec	= *(rt_uint32_t *)(buff+0x18);
		winfo.nAvgBytesPerSec	= *(rt_uint32_t *)(buff+0x1c);
		winfo.nBlockAlign		= *(rt_uint16_t *)(buff+0x20);
		winfo.wBitsPerSample	= *(rt_uint16_t *)(buff+0x22);
		winfo.cbSize            = *(rt_uint32_t *)(buff+0x28);   

		rt_kprintf("\nSample Rate = %d, Channels = %d, BitsPerSample = %d, file size = %dM Bytes\n",
	    winfo.nSamplesPerSec, winfo.nChannels, winfo.wBitsPerSample, winfo.cbSize/1024/1024);
		
		if (winfo.wBitsPerSample!=16)//2byte
		{	
			rt_kprintf("8bit format is not support !\n\r");
			close(fd);
			return;
		}
		current_offset = 0;
		
		len = read(fd, (char *)(&BufData[s_BuffIndex][0]), (2048*2));
		IIS_Config(winfo.nSamplesPerSec, (u32)(&BufData[0][0]), (2048));	   
		do
		{
			if (player_is_playing() != RT_TRUE)
			{	
				break;
			}
			result = rt_sem_take(&s_semForPlay, RT_WAITING_FOREVER);
			if (result != RT_EOK)
	        {
				rt_kprintf(" semaphore: semforplay init failed./r/n");
				rt_sem_detach(&s_semForPlay);/* �����ź������� */
	        }
		
			DMA_Transmit((u32)((&BufData[s_BuffIndex][0])), (2048));	
			if (++s_BuffIndex>=2) s_BuffIndex = 0;
			len = read(fd, (char *)(&BufData[s_BuffIndex][0]), (2048*2));
			if (len != (2048*2))
			{
				rt_kprintf("file read fail or there is no more data!\n\r");
				close(fd);
				return;
			}
			current_offset+=len;
		
			if (player_get_mode() != PLAYER_PLAY_RADIO)
		       player_set_position(current_offset);
			
		} while (len != 0);
		close(fd);
    }
}

FINSH_FUNCTION_EXPORT(wav, wav test)

