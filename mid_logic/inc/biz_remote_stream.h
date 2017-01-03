#ifndef __BIZ_REMOTE_STREAM_H__
#define __BIZ_REMOTE_STREAM_H__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#include "string.h"

#include "types.h"
#include "ctrlprotocol.h"
#include "biz_device.h"
#include "biz_msg_type.h"


#define MAX_FRAME_SIZE (1 << 20) // 1MB

//��״̬
typedef enum
{
	EM_STREAM_STATUS_DISCONNECT,	//δ���ӣ���ʼ״̬
	EM_STREAM_STATUS_RUNNING,		//�����ӣ���������
	EM_STREAM_STATUS_STOP,			//ֹͣ��stream_errno ָʾ�Ƿ����
	EM_STREAM_STATUS_WAIT_DEL,		//�ȴ�ɾ��
} EM_STREAM_STATUS_TYPE;

#if 0
//��������
typedef enum
{
	EM_STREAM_ERR_SEND,				//���ͳ���
	EM_STREAM_ERR_RECV,				//���ճ���
	EM_STREAM_ERR_TIMEOUT,			//����ʱ
	EM_STREAM_ERR_DEV_OFFLINE,		//�豸����
} EM_STREAM_ERR_TYPE;
#endif


//֡����ͷ
typedef struct
{
    u8     m_byMediaType; //ý������
    u8    *m_pData;       //���ݻ���
	u32    m_dwPreBufSize;//m_pData����ǰԤ���˶��ٿռ䣬���ڼ�
	// RTP option��ʱ��ƫ��ָ��һ��Ϊ12+4+12
	// (FIXED HEADER + Extence option + Extence bit)
    u32    m_dwDataSize;  //m_pDataָ���ʵ�ʻ����С
    u8     m_byFrameRate; //����֡��,���ڽ��ն�
	u32    m_dwFrameID;   //֡��ʶ�����ڽ��ն�
	u32    m_dwTimeStamp; //ʱ���, ���ڽ��ն�
    union
    {
        struct{
			u32		m_bKeyFrame;    //Ƶ֡����(I or P)
			u16		m_wVideoWidth;  //��Ƶ֡��
			u16		m_wVideoHeight; //��Ƶ֡��
			u32		m_wBitRate;
		}m_tVideoParam;
        u8    m_byAudioMode;//��Ƶģʽ
    };
}FRAMEHDR,*PFRAMEHDR;

//������������
class CMediaStreamManager;


/************************************************************
ý������������
��ΪԤ�����طš��ļ����ص��߼���ṹ
************************************************************/
//��ע���֡���ݴ�������
typedef void (CObject:: *PDEAL_FRAME)(u32 stream_id, FRAMEHDR *pframe_hdr);

//��ע���״̬��������
typedef void (CObject:: *PDEAL_STATUS)(SBizMsg_t *pmsg, u32 len);

class CMediaStream : public CObject
{
	friend class CMediaStreamManager;
	
public:
	CMediaStream();
	~CMediaStream();
	
	int Init();
	
private:
    CMediaStream(CMediaStream &)
	{
		
	}
	void FreeSrc();//�ͷ���Դ
	void threadRcv(uint param);//���շ���������

private:
	C_Lock *plock4param;//mutex
	//ָ�����ϲ�ṹ
	CObject *m_obj;
	PDEAL_FRAME m_deal_frame_cb;//��ע���֡���ݴ�������
	PDEAL_STATUS m_deal_status_cb;//��ע���״̬��������

	//���ڲ�����
	EM_DEV_TYPE dev_type;//����������
	u32 dev_ip;//������IP
	u32 stream_id;//�ؼ���ϵͳΨһ
	s32 stream_errno;//������
	EM_STREAM_STATUS_TYPE status;//��״̬
	ifly_TCP_Stream_Req req;//���������ݽṹ
	//������
	VD_BOOL b_thread_running;//�����߳����б�־
	VD_BOOL b_thread_exit;//�����߳��˳���־
	s32 sock_stream;
	Threadlet m_threadlet_rcv;
	CSemaphore sem_exit;//�ȴ�threadRcv�˳��ź���
};

#if 0
typedef struct _SDev_StearmRcv_t
{
	VD_BOOL b_connect;
	int	sock_fd;	
	u32 link_id;
	ifly_TCP_Stream_Req req;
	CMediaStream* pstream; //ָ���������ṹ��Ԥ�����طš��ļ�����

	_SDev_StearmRcv_t()
	: b_connect(FALSE)
	, sock_fd(INVALID_SOCKET)
	, link_id(INVALID_VALUE)
	, pstream(NULL)
	{
		memset(&req, 0, sizeof(ifly_TCP_Stream_Req));
	}
} SDev_StearmRcv_t;
#endif


//extern  API
int BizStreamInit(void);

int BizSendMsg2StreamManager(SBizMsg_t *pmsg, u32 msg_len);

//pstream_id ������ID
int BizStreamReqPlaybackByFile (
	EM_DEV_TYPE dev_type,
	u32 dev_ip,
	char *file_name,
	u32 offset,
	CObject *obj,
	PDEAL_FRAME deal_frame_cb,
	PDEAL_STATUS deal_status_cb,
	u32 *pstream_id );

int BizStreamReqStop(u32 stream_id);



#endif
