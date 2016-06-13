/*****************************************************************************
                         TGUI�ϵ����뷨ģ��-�������뷨����
*****************************************************************************/

#include "TImePinYin.h"
#include "TGUIBase.h" //KEY
//������
#include <string.h>

/*****************************************************************************
                        ��غ���
*****************************************************************************/

//---------------------------��ʼ������---------------------------------
//�л����������뷨ʱ����
void TImePinYin_Init(struct _TImePinYin *pPinYin,//���뷨�ṹ
                     unsigned char DispW)        //��ʾ����,>16
{
  memset(pPinYin, 0, sizeof(struct _TImePinYin));//�ȳ�ʼ����0
  pPinYin->DispW = DispW;
}

//----------------------���ƴ����ʾ���ַ�������---------------------------
//���ʾ��:"1pen 2ren 3sen"
void TImePinYin_pGetPinYinChar(struct _TImePinYin *pPinYin,
                               char *pBuf)              //�������ַ�
{
  unsigned char FindCount;
  const struct _PyIndex *const *ppPyIndex;
  unsigned char PyCount;  
  unsigned char Pos = 0; //ƴ���ڲ�λ��
  char *pEndBuf = pBuf + pPinYin->DispW; //�н���λ��
  
  ppPyIndex = NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->PinYinStart;//ָ����������������Ч��ʼλ��
  FindCount -= pPinYin->PinYinStart;
  while(FindCount){//����ƴ��Ҫ��ʾʱ,һ�������ƴ��
    PyCount = strlen((const char*)(*ppPyIndex)->Py);//ƴ��
    if((PyCount + 1) <= (pEndBuf - pBuf)){//����+ƴ�����,ֱ��ƴ����ʾ����ʱΪֹ
      *pBuf++ = Pos + '1'; //ƴ��ǰ��ʾ
      memcpy(pBuf, (*ppPyIndex)->Py, PyCount);
      pBuf += PyCount;   
      if(pBuf < (pEndBuf - 1)) *pBuf++ = ' '; //��λ��ʱ�ո���
      //Ϊ�´�����׼��
      FindCount--;
      ppPyIndex++;
      Pos++;
    }
    else break; //һ����ʾ�����¸�ƴ����
  }
  if(pEndBuf > pBuf){//�������ո�
    memset(pBuf, ' ', pEndBuf - pBuf);
  }
}

//----------------------��人����ʾ���ַ�������---------------------------
//���ʾ��:"��1 ��2 ��3 ��4"
void TImePinYin_pGetChChar(struct _TImePinYin *pPinYin,
                           char *pBuf)              //�������ַ�
{
  unsigned char FindCount;
  const unsigned char *Pymb;
  unsigned char Len;
  unsigned char Pos; //�����ڲ�λ��
  unsigned char W = (pPinYin->DispW >> 2);//ÿҳ�ַ�����
  
  Pymb = (*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->CurPinYin))->Pymb;//ָ������ƴ�����λ��
  Len = strlen((const char*)Pymb) - pPinYin->ChStart;//��ǰҳ�ַ�����
  Pymb += pPinYin->ChStart; //ָ��ǰҳ�ĺ���
  
  //���
  for(Pos = 0; Pos < W; Pos++){
    if(Len){//��������ʾ�ĺ���ʱ
      Len -= 2; //ȫ��
      *pBuf++ = *Pymb++; //�ϰ���
      *pBuf++ = *Pymb++; //�°���
      *pBuf++ = Pos + '1';//������ʾ
      *pBuf++ = ' ';//��� 
    }
    else{
      memset(pBuf,' ', 4);//���ո�
      pBuf += 4;
    }
  }
}

//----------------------�õ���ǰ�û�ѡ���ַ�����---------------------------
//���������״̬����ܵ��ã����ú��ڲ�״̬���Զ���λ������״̬
//���غ���ǰ��λλ��
const char *TImePinYin_pGetChar(struct _TImePinYin *pPinYin)
{
  unsigned char FindCount;
  const unsigned char *pString;
  if(pPinYin->eState != eTImePinYin_Final) return NULL;
  
  //ָ�����躺��λ��
  pString = (*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->CurPinYin))->Pymb + pPinYin->CurCh;
  //����ڲ����³�ʼ��
  TImePinYin_Init(pPinYin, pPinYin->DispW);
  
  return (const char*)pString;
}

/*****************************************************************************
                        ��ֵ������غ���
*****************************************************************************/

//------------ƴ������ѡ��״̬����ҳ����Ӧ����---------------------------------
static void _PinYinSelKeyUPDown(struct _TImePinYin *pPinYin,
                                char UpFlag) //��ҳ��־,����Ϊ��ҳ
{
  unsigned char PinYinStart = pPinYin->PinYinStart;
  unsigned char CurPage = pPinYin->CurPage;
  
  if(UpFlag){//���ϻ�ҳ
    if(!PinYinStart) return; //���ڵ�һҳ��
    CurPage--;    
    PinYinStart -= pPinYin->MaxPerPage[CurPage];    //����ҳ��
  }
  else{//���»�ҳ
    PinYinStart += pPinYin->MaxPerPage[CurPage]; //�ӵ�ǰҳ��      
    CurPage++;
    if(!pPinYin->MaxPerPage[CurPage]) return; //����һҳ��
  }
  pPinYin->PinYinStart = PinYinStart;
  pPinYin->CurPage = CurPage;
}
//------------ƴ������ѡ��״̬���ּ���Ӧ����---------------------------------
//���أ� 0Ҫ��Ӧ    -1����Ӧ
static signed char _PinYinSelKeyNum(struct _TImePinYin *pPinYin,
                             unsigned char Index) //���ּ�����ֵ,0-9
{
  unsigned char PinYinStart = pPinYin->PinYinStart;
  unsigned char CurPage = 0;
  unsigned char PyLen = 0;
  
  for(;PyLen < PinYinStart;CurPage++)
    PyLen += pPinYin->MaxPerPage[CurPage];//�õ���ǰҳ:��0��ʼ
  unsigned char CurPageCount = pPinYin->MaxPerPage[CurPage];
  
  if(Index < CurPageCount){//������ʱ����Ӧ
    pPinYin->CurPinYin = pPinYin->PinYinStart + Index;
    return 0;
  }
  return -1;
}


//----------------��������ѡ��״̬����ҳ����Ӧ����-------------------------
static void _ChSelKeyUPDown(struct _TImePinYin *pPinYin,
                            char UpFlag) //��ҳ��־,����Ϊ��ҳ
{
  unsigned short Len;
  unsigned char W;
  unsigned char ChNextStart;

  W = pPinYin->DispW >> 1;//һҳ����ʾ���ַ�(����Ϊ��λ)����
  if(UpFlag){//��ҳʱ
    if(!pPinYin->ChStart) return;//���ڵ�һҳ��
    if(pPinYin->ChStart >= W)
      pPinYin->ChStart -= W;
    else pPinYin->ChStart = 0;
  }
  else{//��һҳʱ
    //�õ����躺��λ�õĺ��ָ���
    Len = strlen((char*)(*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &ChNextStart) + 
                    pPinYin->CurPinYin))->Pymb);    
    if(Len <= W) return;//����ҳ    

    ChNextStart = pPinYin->ChStart + W;
    if(ChNextStart >= Len) return; //���һҳ��ͷ��
    pPinYin->ChStart = ChNextStart;
  }
}

//------------��������ѡ��״̬���ּ���Ӧ����---------------------------------
static void _ChSelKeyNum(struct _TImePinYin *pPinYin,
                         unsigned char Index) //���ּ�����ֵ,0-9
{
  unsigned char WCount = pPinYin->DispW >> 2; //�õ�����ַ�����
  if(Index < WCount){//������ʱ����Ӧ
    pPinYin->CurCh = pPinYin->ChStart + (Index << 1);//���Ϊ��λ
  }
}

//------------------------ƴ��ѡ��ʱҳ������������---------------------------
//���أ�0��ȷ    -1����
extern signed char _MaxInPage_PY(struct _TImePinYin *pPinYin)
{
  unsigned char FindCount;
  const struct _PyIndex *const *ppPyIndex;
  unsigned char PyLen = 0;//ƴ���ܳ���
  unsigned char WinW = pPinYin->DispW; //�õ����ڿ���
  unsigned char CurPage = 0;
  
  ppPyIndex = NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount);//ָ����������������Ч��ʼλ��;�õ�����
  while(FindCount){//����ƴ��Ҫ��ʾʱ,һ�������ƴ��
    PyLen += strlen((const char*)(*ppPyIndex)->Py) + 1;//ǰ������ռλ
    if(PyLen <= WinW){//��ǰҳ�ܷ���
      pPinYin->MaxPerPage[CurPage]++;//ƴ����������
      FindCount--;
      ppPyIndex++;
      PyLen++; //�󵼿ո���ռλ
    }
    else{//ҳ����,Ϊ��һҳ��ʼ��
      CurPage++;
      //����(Ԥ��һ����ҳΪ0�����ж�)
      if(CurPage > (sizeof(pPinYin->MaxPerPage) - 1)){
        return -1; //���󷵻�
      }
      pPinYin->MaxPerPage[CurPage] = 0;
      PyLen = 0;//������ǰƴ�����ȵ���ҳ
    }
  }
  return 0;
}

//=============================������Ӧ����================================
//���û�����ʱ��Ӧ�û�����
//���ص�ǰ״̬
enum _eTImePinYin TImePinYin_eKey(struct _TImePinYin *pPinYin,
                                 char KeyNum)//��ֵ
{
  unsigned char FindCount;
  enum _eTImePinYin eState = pPinYin->eState;  //״̬��
  if((KeyNum == TGUI_KEY_DELETE) || (KeyNum == TGUI_KEY_LEFT) || 
     (KeyNum == TGUI_KEY_ESCAPE))
    KeyNum = ' ';//Ԥ�Ƚ�ϵͳ�����ת��Ϊ����Ҫ��ļ�ֵ
  
  switch(eState){//״̬������
  case eTImePinYin_Idie://����״̬ʱ,��Ӧ�û��������ּ���������ĸ
    if((KeyNum >= '2') && (KeyNum <= '9')){
      //��ֵ�������״�һ�����ҵ�
      NumKeyPyIme_pKey(&pPinYin->Ime, &FindCount, KeyNum);
      eState = eTImePinYin_Input;//�л���ƴ������״̬
    }
    break;
  case eTImePinYin_Input://ƴ������״̬
    if((KeyNum == TGUI_KEY_ENTER) || (KeyNum == TGUI_KEY_RIGHT)){//ȷ��ƴ���������ʱ
      if(!_MaxInPage_PY(pPinYin))//����ҳ������ÿҳ����
        eState = eTImePinYin_PinYinSel;//�л���ƴ��ѡ��״̬
    }
    else{//��Ӧƴ�����뷨
      //�û�������������ɺ����µ�����״̬
      if(NumKeyPyIme_pKey(&pPinYin->Ime, &FindCount, KeyNum) == NULL)
        eState = eTImePinYin_Idie;
    }
    break;
  case eTImePinYin_PinYinSel://ƴ��ѡ��״̬
    if((KeyNum == TGUI_KEY_PAGEUP) || (KeyNum == TGUI_KEY_UP))//ѡ����һҳƴ��
      _PinYinSelKeyUPDown(pPinYin, 1);
    else if((KeyNum == TGUI_KEY_PAGEDOWN) || (KeyNum == TGUI_KEY_DOWN))//ѡ����һҳƴ��
      _PinYinSelKeyUPDown(pPinYin, 0);
    else if((KeyNum >= '1') && (KeyNum <= '9')){//���ּ�ѡ��ƴ��ʱ
      if(!_PinYinSelKeyNum(pPinYin, KeyNum - '1'))
        eState = eTImePinYin_ChSel;//�л�������ѡ��״̬
    }
    else if(KeyNum == ' ')//�˳�������ƴ������״̬
      eState = eTImePinYin_Input;
    break;
  case eTImePinYin_ChSel://����ѡ��״̬
    if((KeyNum == TGUI_KEY_PAGEUP) || (KeyNum == TGUI_KEY_UP))//ѡ����һҳƴ��
      _ChSelKeyUPDown(pPinYin, 1);
    else if((KeyNum == TGUI_KEY_PAGEDOWN) || (KeyNum == TGUI_KEY_DOWN))//ѡ����һҳƴ��
      _ChSelKeyUPDown(pPinYin, 0);
    else if((KeyNum >= '1') && (KeyNum <= '9')){//���ּ�ѡ��ƴ��ʱ
      _ChSelKeyNum(pPinYin, KeyNum - '1');
      eState = eTImePinYin_Final;//�л������״̬
    }
    else if(KeyNum == ' ')//�˳�������ƴ��ѡ��״̬
      eState = eTImePinYin_PinYinSel;
    break;
  default: break; //���ֵ�ǰ״̬����
  }
  pPinYin->eState = eState;
  return eState;
}


