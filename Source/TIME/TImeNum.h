/*****************************************************************************
                         TGUI�ϵ����뷨ģ��-���ּ����뷨����
*****************************************************************************/
#ifndef __TIME_NUM_H
#define __TIME_NUM_H

/*****************************************************************************
                              ˵��
*****************************************************************************/
//�˺����ڲ�������κ���ʾ
//������������˵��:
//1:�л����������뷨ʱ�����ó�ʼ���������г�ʼ������
//2:�û������ּ�ʱ�����ð�������������Ӧ����
//3:�õ��ַ���: ���õõ��ַ��������õ���ʾ�������ַ���


/*****************************************************************************
                        ��ؽṹ
*****************************************************************************/
#include "TImeBase.h" //TIme_String_t;

struct _TImeNum{
  char Num;    //�û���������ּ�
};

/*****************************************************************************
                        ��غ���
*****************************************************************************/

//---------------------------��ʼ������---------------------------------
//�л����������뷨ʱ����
//void TImeNum_Init(struct _TImeNum *pNum);//���뷨�ṹ
#define TImeNum_Init(pNNum) do{}while(0)

//----------------------�õ���ǰ�û�ѡ���ַ�����---------------------------
//���ص�ǰ�û�ѡ����ַ�
//const char TImeNum_GetNum(struct _TImeNum *pNum);
#define TImeNum_GetNum(pNNum)  ((pNNum)->Num)

//-----------------------���ּ���Ӧ����---------------------------------
//���û������ּ�0-9ʱ��Ӧ�û�����
//void TImeNum_KeyNum(struct _TImeNum *pNum,
//                    char KeyNum);//��ֵ��Ӧȷ��Ϊ'0'-'9'
#define TImeNum_KeyNum(pNNum,KeyNum) do{(pNNum)->Num = KeyNum;}while(0)

#endif //#define __TIME_NUM_H
