/* ----------------------------------------------------------------------------
 *                TMenu�û��������˵�ʵ��
 �������˵����˵���������ʾ�����������û�����,���еİ���Ҳ�����û����д���
 * --------------------------------------------------------------------------*/

#ifndef __T_MENU_BUF_H 
#define __T_MENU_BUF_H 

#include "TMenuBase.h"

/**********************************************************************
                      ������
**********************************************************************/
//��

/**********************************************************************
                     �û�ʹ�����
**********************************************************************/
//��Ϊ��ѡ��ʱ,�˵��ṹ�����¶���:
//typedef struct _TMenu{
//  TMTYPE_BUF;             //�˵�����Ϊ������ģʽ
//  unsigned char Size;     //0
//  LanguageCode_t *pgHeader; //�˵�ͷ,ΪNULLʱ�ӻص����ȡ
//  struct _TMenu *pParent;   //���ѵĸ��˵�
//  const LanguageCode_t*;   //NULL
//  TMenuNotify_t Notify;     //���û��ռ佻����ͨ������
//}TMenu_t;

//��ģʽ���û�����־

//����˵����ݽṹ:
struct _BufUser{
  unsigned char Key;               //�û�����
  unsigned char Notify;            //�û���˵���ͨ��
  unsigned char w;                 //����������
  unsigned char h;                 //�������߶�
  char *pBuf;			                 //������
};

//����,����ʱ,Notify��4λ����Ϊ:
#define     TM_BUF_NOTIFY_MASK   0x0f
#define     TM_BUF_NOTIFY_NON    0 //��ͨ��
#define     TM_BUF_NOTIFY_DISP   1 //�û�ͨ���������ʾ

//����λ����Ϊ��
#define     TM_BUF_NOTIFY_EXIT_FLAG  0x80 //�û�ͨ���˳��˲˵�

//�û��ص�������Ӧ���ͼ���ʵ�ֵĹ�����:
//1:����Ϊ:TM_NOTIFY_GET_DATAʱ,ͨ������������ʾ
//�Զ�������Ϊ:
#define	TM_NOTIFY_BUF_KEY  (TM_NOTIFY_WIDGET_BASE + 0)//���û���Ӧ�İ���(�˳�������)


/**********************************************************************
                     �ڲ����ݽṹ
**********************************************************************/
struct _BufData{
 TWinHandle_t hWin;       //������˵��Ĵ���
 unsigned char Flag;      //�ڲ���־
 struct _BufUser User;   //�û�����
};

//------------------------��������ʵ��----------------------------
//1:���������Ͳ˵�,���û���������ݲ�������ʾ
//���ش����ɹ�:
signed char TMenu_BufCreate(const TMenu_t *pMenu,    //ָ��Ĳ˵�ϵͳ
                                 TWinHandle_t hWin,       //������˵��Ĵ���
                                 unsigned char Data,    //�������������
                                 void *pv);      //�ɹ�ʹ�õ����ɻ�����

//------------------------��Ӧ��������ʵ��----------------------------
//2:��Ӧ��������������������û��������ݽ���,��������ʾ
//����ֵ����Ϊ: ��ֵ:������Ӧ�Ӳ˵�,-1,����,-2:�����ϲ�˵�
signed char TMenu_BufKey(const TMenu_t *pMenu,    //ָ��Ĳ˵�ϵͳ
                         unsigned char Key,       //����ļ�ֵ
                         void *pv);      //�ѱ�ʹ�õ��������ݻ�����

//--------------------------ǿ��ˢ�º���ʵ��-----------------------------
void TMenu_BufRefresh(const TMenu_t *pMenu,    //ָ��Ĳ˵�ϵͳ
                      void *pv);      //�ѱ�ʹ�õ��������ݻ�����


#endif