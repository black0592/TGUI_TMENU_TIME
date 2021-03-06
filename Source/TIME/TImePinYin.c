/*****************************************************************************
                         TGUI上的输入法模块-符号输入法部分
*****************************************************************************/

#include "TImePinYin.h"
#include "TGUIBase.h" //KEY
//导航键
#include <string.h>

/*****************************************************************************
                        相关函数
*****************************************************************************/

//---------------------------初始化函数---------------------------------
//切换到符号输入法时调用
void TImePinYin_Init(struct _TImePinYin *pPinYin,//输入法结构
                     unsigned char DispW)        //显示宽度,>16
{
  memset(pPinYin, 0, sizeof(struct _TImePinYin));//先初始化到0
  pPinYin->DispW = DispW;
}

//----------------------填充拼音显示行字符串函数---------------------------
//填充示例:"1pen 2ren 3sen"
void TImePinYin_pGetPinYinChar(struct _TImePinYin *pPinYin,
                               char *pBuf)              //被填充的字符
{
  unsigned char FindCount;
  const struct _PyIndex *const *ppPyIndex;
  unsigned char PyCount;  
  unsigned char Pos = 0; //拼音内部位置
  char *pEndBuf = pBuf + pPinYin->DispW; //行结束位置
  
  ppPyIndex = NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->PinYinStart;//指向所需索引表的有效起始位置
  FindCount -= pPinYin->PinYinStart;
  while(FindCount){//还有拼音要显示时,一个个填充拼音
    PyCount = strlen((const char*)(*ppPyIndex)->Py);//拼音
    if((PyCount + 1) <= (pEndBuf - pBuf)){//数字+拼音填充,直到拼音显示不下时为止
      *pBuf++ = Pos + '1'; //拼音前提示
      memcpy(pBuf, (*ppPyIndex)->Py, PyCount);
      pBuf += PyCount;   
      if(pBuf < (pEndBuf - 1)) *pBuf++ = ' '; //有位置时空格间隔
      //为下次算做准备
      FindCount--;
      ppPyIndex++;
      Pos++;
    }
    else break; //一行显示不下下个拼音了
  }
  if(pEndBuf > pBuf){//后面填充空格
    memset(pBuf, ' ', pEndBuf - pBuf);
  }
}

//----------------------填充汉字显示行字符串函数---------------------------
//填充示例:"人1 仁2 壬3 忍4"
void TImePinYin_pGetChChar(struct _TImePinYin *pPinYin,
                           char *pBuf)              //被填充的字符
{
  unsigned char FindCount;
  const unsigned char *Pymb;
  unsigned char Len;
  unsigned char Pos; //汉字内部位置
  unsigned char W = (pPinYin->DispW >> 2);//每页字符个数
  
  Pymb = (*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->CurPinYin))->Pymb;//指向所需拼音码表位置
  Len = strlen((const char*)Pymb) - pPinYin->ChStart;//当前页字符长度
  Pymb += pPinYin->ChStart; //指向当前页的汉字
  
  //填充
  for(Pos = 0; Pos < W; Pos++){
    if(Len){//还有需显示的汉字时
      Len -= 2; //全角
      *pBuf++ = *Pymb++; //上半字
      *pBuf++ = *Pymb++; //下半字
      *pBuf++ = Pos + '1';//汉字提示
      *pBuf++ = ' ';//间隔 
    }
    else{
      memset(pBuf,' ', 4);//填充空格
      pBuf += 4;
    }
  }
}

//----------------------得到当前用户选择字符函数---------------------------
//在输入完成状态后才能调用，调用后内部状态机自动复位到空闲状态
//返回汉字前两位位置
const char *TImePinYin_pGetChar(struct _TImePinYin *pPinYin)
{
  unsigned char FindCount;
  const unsigned char *pString;
  if(pPinYin->eState != eTImePinYin_Final) return NULL;
  
  //指向所需汉字位置
  pString = (*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount) + 
               pPinYin->CurPinYin))->Pymb + pPinYin->CurCh;
  //最后内部重新初始化
  TImePinYin_Init(pPinYin, pPinYin->DispW);
  
  return (const char*)pString;
}

/*****************************************************************************
                        键值处理相关函数
*****************************************************************************/

//------------拼音输入选择状态上下页键响应函数---------------------------------
static void _PinYinSelKeyUPDown(struct _TImePinYin *pPinYin,
                                char UpFlag) //上页标志,否则为下页
{
  unsigned char PinYinStart = pPinYin->PinYinStart;
  unsigned char CurPage = pPinYin->CurPage;
  
  if(UpFlag){//向上换页
    if(!PinYinStart) return; //已在第一页了
    CurPage--;    
    PinYinStart -= pPinYin->MaxPerPage[CurPage];    //减上页数
  }
  else{//向下换页
    PinYinStart += pPinYin->MaxPerPage[CurPage]; //加当前页数      
    CurPage++;
    if(!pPinYin->MaxPerPage[CurPage]) return; //无下一页了
  }
  pPinYin->PinYinStart = PinYinStart;
  pPinYin->CurPage = CurPage;
}
//------------拼音输入选择状态数字键响应函数---------------------------------
//返回： 0要响应    -1不响应
static signed char _PinYinSelKeyNum(struct _TImePinYin *pPinYin,
                             unsigned char Index) //数字键索引值,0-9
{
  unsigned char PinYinStart = pPinYin->PinYinStart;
  unsigned char CurPage = 0;
  unsigned char PyLen = 0;
  
  for(;PyLen < PinYinStart;CurPage++)
    PyLen += pPinYin->MaxPerPage[CurPage];//得到当前页:从0开始
  unsigned char CurPageCount = pPinYin->MaxPerPage[CurPage];
  
  if(Index < CurPageCount){//不超出时才响应
    pPinYin->CurPinYin = pPinYin->PinYinStart + Index;
    return 0;
  }
  return -1;
}


//----------------汉字输入选择状态上下页键响应函数-------------------------
static void _ChSelKeyUPDown(struct _TImePinYin *pPinYin,
                            char UpFlag) //上页标志,否则为下页
{
  unsigned short Len;
  unsigned char W;
  unsigned char ChNextStart;

  W = pPinYin->DispW >> 1;//一页能显示的字符(半字为单位)个数
  if(UpFlag){//上页时
    if(!pPinYin->ChStart) return;//已在第一页了
    if(pPinYin->ChStart >= W)
      pPinYin->ChStart -= W;
    else pPinYin->ChStart = 0;
  }
  else{//下一页时
    //得到所需汉字位置的汉字个数
    Len = strlen((char*)(*(NumKeyPyIme_pGetCur(&pPinYin->Ime, &ChNextStart) + 
                    pPinYin->CurPinYin))->Pymb);    
    if(Len <= W) return;//无下页    

    ChNextStart = pPinYin->ChStart + W;
    if(ChNextStart >= Len) return; //最后一页到头了
    pPinYin->ChStart = ChNextStart;
  }
}

//------------汉字输入选择状态数字键响应函数---------------------------------
static void _ChSelKeyNum(struct _TImePinYin *pPinYin,
                         unsigned char Index) //数字键索引值,0-9
{
  unsigned char WCount = pPinYin->DispW >> 2; //得到最大字符个数
  if(Index < WCount){//不超出时才响应
    pPinYin->CurCh = pPinYin->ChStart + (Index << 1);//半角为单位
  }
}

//------------------------拼音选择时页及项数量计算---------------------------
//返回：0正确    -1错误
extern signed char _MaxInPage_PY(struct _TImePinYin *pPinYin)
{
  unsigned char FindCount;
  const struct _PyIndex *const *ppPyIndex;
  unsigned char PyLen = 0;//拼音总长度
  unsigned char WinW = pPinYin->DispW; //得到窗口宽度
  unsigned char CurPage = 0;
  
  ppPyIndex = NumKeyPyIme_pGetCur(&pPinYin->Ime, &FindCount);//指向所需索引表的有效起始位置;得到数量
  while(FindCount){//还有拼音要显示时,一个个填充拼音
    PyLen += strlen((const char*)(*ppPyIndex)->Py) + 1;//前导数字占位
    if(PyLen <= WinW){//当前页能放下
      pPinYin->MaxPerPage[CurPage]++;//拼音个数增加
      FindCount--;
      ppPyIndex++;
      PyLen++; //后导空格间隔占位
    }
    else{//页增加,为下一页初始化
      CurPage++;
      //超限(预留一结束页为0用于判断)
      if(CurPage > (sizeof(pPinYin->MaxPerPage) - 1)){
        return -1; //错误返回
      }
      pPinYin->MaxPerPage[CurPage] = 0;
      PyLen = 0;//保留当前拼音长度到下页
    }
  }
  return 0;
}

//=============================按键响应函数================================
//当用户按键时响应用户请求
//返回当前状态
enum _eTImePinYin TImePinYin_eKey(struct _TImePinYin *pPinYin,
                                 char KeyNum)//键值
{
  unsigned char FindCount;
  enum _eTImePinYin eState = pPinYin->eState;  //状态机
  if((KeyNum == TGUI_KEY_DELETE) || (KeyNum == TGUI_KEY_LEFT) || 
     (KeyNum == TGUI_KEY_ESCAPE))
    KeyNum = ' ';//预先将系统清除键转换为输入要求的键值
  
  switch(eState){//状态机处理
  case eTImePinYin_Idie://空闲状态时,响应用户输入数字键输入首字母
    if((KeyNum >= '2') && (KeyNum <= '9')){
      //键值处理，首次一定能找到
      NumKeyPyIme_pKey(&pPinYin->Ime, &FindCount, KeyNum);
      eState = eTImePinYin_Input;//切换到拼音输入状态
    }
    break;
  case eTImePinYin_Input://拼音输入状态
    if((KeyNum == TGUI_KEY_ENTER) || (KeyNum == TGUI_KEY_RIGHT)){//确认拼音输入完成时
      if(!_MaxInPage_PY(pPinYin))//计算页数量和每页个数
        eState = eTImePinYin_PinYinSel;//切换到拼音选择状态
    }
    else{//响应拼音输入法
      //用户按清除键清除完成后，重新到空闲状态
      if(NumKeyPyIme_pKey(&pPinYin->Ime, &FindCount, KeyNum) == NULL)
        eState = eTImePinYin_Idie;
    }
    break;
  case eTImePinYin_PinYinSel://拼音选择状态
    if((KeyNum == TGUI_KEY_PAGEUP) || (KeyNum == TGUI_KEY_UP))//选择上一页拼音
      _PinYinSelKeyUPDown(pPinYin, 1);
    else if((KeyNum == TGUI_KEY_PAGEDOWN) || (KeyNum == TGUI_KEY_DOWN))//选择下一页拼音
      _PinYinSelKeyUPDown(pPinYin, 0);
    else if((KeyNum >= '1') && (KeyNum <= '9')){//数字键选择拼音时
      if(!_PinYinSelKeyNum(pPinYin, KeyNum - '1'))
        eState = eTImePinYin_ChSel;//切换到汉字选择状态
    }
    else if(KeyNum == ' ')//退出键退至拼音输入状态
      eState = eTImePinYin_Input;
    break;
  case eTImePinYin_ChSel://汉字选择状态
    if((KeyNum == TGUI_KEY_PAGEUP) || (KeyNum == TGUI_KEY_UP))//选择上一页拼音
      _ChSelKeyUPDown(pPinYin, 1);
    else if((KeyNum == TGUI_KEY_PAGEDOWN) || (KeyNum == TGUI_KEY_DOWN))//选择下一页拼音
      _ChSelKeyUPDown(pPinYin, 0);
    else if((KeyNum >= '1') && (KeyNum <= '9')){//数字键选择拼音时
      _ChSelKeyNum(pPinYin, KeyNum - '1');
      eState = eTImePinYin_Final;//切换到完成状态
    }
    else if(KeyNum == ' ')//退出键退至拼音选择状态
      eState = eTImePinYin_PinYinSel;
    break;
  default: break; //保持当前状态不变
  }
  pPinYin->eState = eState;
  return eState;
}



