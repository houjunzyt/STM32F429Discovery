#include "sample.h"
#include "string.h"
#include "GUI.h"
#include "WM.h"	
#include "lcd_user.h"	
#include "rtthread.h"
#include <stdio.h>

#define MSG_CHANGE_TEXT (WM_USER + 0)  //�û��Զ������Ϣ����

#define SPEED           1250

static char _acInfoText[40];

static GUI_COLOR _WindowColor1 = GUI_GREEN;	//����1����ɫΪ��ɫ
static GUI_COLOR _FrameColor1  = GUI_BLUE;	//����1�ı߽�Ϊ��ɫ
static GUI_COLOR _WindowColor2 = GUI_RED;	//����2����ɫΪ��ɫ
static GUI_COLOR _FrameColor2  = GUI_YELLOW;//����2�ı߽�Ϊ��ɫ
static GUI_COLOR _ChildColor   = GUI_YELLOW;//�Ӵ��ڵ���ɫΪ��ɫ
static GUI_COLOR _ChildFrame   = GUI_BLACK;	//�Ӵ��ڵı߽�Ϊ��ɫ

//�ص�
static WM_CALLBACK * _cbBkWindowOld;


//���ھ��
static WM_HWIN _hWindow1;  	//����1
static WM_HWIN _hWindow2;	//����2
static WM_HWIN _hChild;		//�Ӵ���


//�򱳾����ڷ���һ����Ϣ��ʹ����������Ч��
//�������ڵĻص������ͻ���ʾ���͵���Ϣ
static void _ChangeInfoText(char *pStr)
{
	WM_MESSAGE Message;
	Message.MsgId = MSG_CHANGE_TEXT;//�û���Ϣ
	Message.Data.p = pStr;
	WM_SendMessage(WM_HBKWIN,&Message); 
	//�򱳾����ڷ�����Ϣ ����Ϣ�����Լ�Ҫ��ʾ���ַ�����hWin ���ھ����pMsg ��Ϣָ��
	WM_InvalidateWindow(WM_HBKWIN);	//�ᷢ�ʹ����ػ�msg���ڶ�Ӧ���ô��ھ���Ļص������������
}

//��ʾһЩ��Ϣ
static void _DrawInfoText(char *pStr)
{
	GUI_SetColor(GUI_WHITE);
	GUI_SetFont(&GUI_Font24_ASCII);
	GUI_DispStringHCenterAt("WindowManager",LCD_X_LEN/2,5);
	//��ָ��λ��ˮƽ������ʾ�ַ���const char GUI_FAR * s, int x, int y
	//x:�������ĵ� X����  y:Ҫд��Ŀͻ��˴����붥�˵ľ��룬��λ����
	GUI_SetFont(&GUI_Font8x16);
	GUI_DispStringAtCEOL(pStr,5,40); 
	//��ָ��λ����ʾ�ַ������������ĩconst char GUI_FAR * s, int x, int y
	//x,y��ʾ���ı����Ͻ�����ֵ
}


//���������ƶ�
//dy:��Ҫ�ƶ��������(������)
static void _LiftUp(int dy)
{
	int i;
	int tm;
	for(i=0;i<(dy/4);i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hWindow1,0,-4); //�ƶ�����1
		WM_MoveWindow(_hWindow2,0,-4); //�ƶ�����2
		while((GUI_GetTime()-tm)<20)
		{
			WM_Exec(); //ͨ��ִ�лص������ػ���Ч����
		}
	}

}

//���������ƶ�
//dy:��Ҫ�ƶ��������(������)
static void _LiftDown(int dy)
{
	int i;
	int tm;
	for(i=0;i<(dy/4);i++)
	{
		tm= GUI_GetTime();
		WM_MoveWindow(_hWindow1,0,4);
		WM_MoveWindow(_hWindow2,0,4);
		while((GUI_GetTime()-tm)<20)
		{
			WM_Exec();
		}
	}
}


//�������ڵĻص�����
static void sample_cbBkWindow(WM_MESSAGE *pMsg)
{
	switch(pMsg->MsgId)
	{
		case MSG_CHANGE_TEXT://�û��������Ϣ����Ҫ��ʾ������д����Ϣ��Ȼ�󴫵ݹ���
			strcpy(_acInfoText,(char const *)pMsg->Data.p);
		case WM_PAINT:
			GUI_SetBkColor(GUI_BLACK);//������ɫΪ��ɫ
			GUI_Clear();
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			GUI_DispStringHCenterAt("WindowManager",LCD_X_LEN/2,5);
			GUI_SetFont(&GUI_Font8x16);
			GUI_DispStringAt(_acInfoText,5,40);//��ʾ�Ķ���ͨ����Ϣ��ȡ
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

//����1�ص�����
static void sample_cbWindow1(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int x;
	int y;
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect); //���ؿͻ���������
			GUI_SetBkColor(_WindowColor1);
			GUI_SetColor(_FrameColor1);
			GUI_ClearRectEx(&Rect);   	//���һ����������
			GUI_DrawRectEx(&Rect);		//����һ������
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x=WM_GetWindowSizeX(pMsg->hWin); 	//��ȡ���ڵ�X���С
			y=WM_GetWindowSizeY(pMsg->hWin);	//��ȡ���ڵ�Y���С	
			GUI_DispStringHCenterAt("Window 1",x/2,(y/2)-12);
			break;
		default:
			WM_DefaultProc(pMsg);
		
	}
}


//����2�ص�����
static void _cbWindow2(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int x;
	int y;
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);	//���ؿͻ���������
			GUI_SetBkColor(_WindowColor2);
			GUI_SetColor(_FrameColor2);
			GUI_ClearRectEx(&Rect);		//���һ����������
			GUI_DrawRectEx(&Rect);		//����һ������
			GUI_SetColor(GUI_WHITE);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);	//��ȡ���ڵ�X���С
			y = WM_GetWindowSizeY(pMsg->hWin);	//��ȡ���ڵ�Y���С
			GUI_DispStringHCenterAt("Window 2", x / 2, (y / 4) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}
	
//�Ӵ��ڵĻص�����
static void _cbChild(WM_MESSAGE *pMsg)
{
	GUI_RECT Rect;
	int x;
	int y;
	
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			WM_GetInsideRect(&Rect);
			GUI_SetBkColor(_ChildColor);//������ɫ��ɫ
			GUI_SetColor(_ChildFrame);//��ɫ�߿�
			GUI_ClearRectEx(&Rect);//���һ����������
			GUI_DrawRectEx(&Rect);	//����һ������
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_Font24_ASCII);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Child window", x / 2, (y / 2) - 12);
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}
	
//����1����һ���ص�����
static void _cbDemoCallback1(WM_MESSAGE *pMsg)
{
	int x,y;
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_GREEN);
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_SetFont(&GUI_FontComic18B_1);
			x=WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 1\nanother Callback", x / 2, (y / 2) - 18);
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}


//����2����һ���ص�����
static void _cbDemoCallback2(WM_MESSAGE *pMsg)
{
	int x,y;
	switch(pMsg->MsgId)
	{
		case WM_PAINT:
			GUI_SetBkColor(GUI_MAGENTA);
			GUI_Clear();
			GUI_SetColor(GUI_YELLOW);
			GUI_SetFont(&GUI_FontComic18B_1);
			x = WM_GetWindowSizeX(pMsg->hWin);
			y = WM_GetWindowSizeY(pMsg->hWin);
			GUI_DispStringHCenterAt("Window 2\nanother Callback", x / 2, (y / 4) - 18);
			break;	
		default:
			WM_DefaultProc(pMsg);
	}
}


//���ô��ڵı�����ɫ,������ʾWM_SetDesktopColor()����
static void _DemoSetDesktopColor(void)
{
	GUI_SetBkColor(GUI_BLUE);//���ñ�����ɫ
	GUI_Clear();
	_DrawInfoText("WM_SetDesktopColor()");//�����ַ�����ʾ
	GUI_Delay(SPEED*3/2);
	WM_SetDesktopColor(GUI_BLACK);//�������洰�ڵ���ɫ
	GUI_Delay(SPEED/2);
	
	GUI_SetBkColor(GUI_BLACK);
	WM_SetDesktopColor(GUI_INVALID_COLOR);
	//GUI_INVALID_COLOR�ָ�ΪĬ������
}

//�������ڵ���ʾ����,��Ҫ��ʾWM_CreateWindow
static void _DemoCreateWindow(void)
{
	_cbBkWindowOld = WM_SetCallback(WM_HBKWIN,sample_cbBkWindow); //���ñ�������Ļص�����
	//��������
	_ChangeInfoText("WM_CreateWindow()");//�򱳾�����һ��װ����ʾ���ݵ���Ϣ����ʹ�ñ�������Ч
	GUI_Delay(SPEED);
	_hWindow1 = WM_CreateWindow(50,70,165,100,	WM_CF_SHOW|WM_CF_MEMDEV,sample_cbWindow1,0); //��������1��ʹ���Զ��洢�豸
	GUI_Delay(SPEED/3);
	_hWindow2 = WM_CreateWindow(105,125,165,100,WM_CF_SHOW|WM_CF_MEMDEV,_cbWindow2,0); //��������2��ʹ���Զ��洢�豸
	GUI_Delay(SPEED);
}


//�����Ӵ��ڣ���Ҫ��ʾWM_CreateWindowAsChild()����
static void _DemoCreateWindowAsChild(void)
{
	_ChangeInfoText("WM_CreateWindowChild()");//�򱳾�����һ��װ����ʾ���ݵ���Ϣ����ʹ�ñ�������Ч
	GUI_Delay(SPEED);
	_hChild = WM_CreateWindowAsChild(10,50,145,40,_hWindow2,WM_CF_SHOW|WM_CF_MEMDEV,_cbChild,0); //�򴰿�2����һ���Ӵ���
	//int x0, int y0,int width, int height,WM_HWIN hWinParent,U8 Style,WM_CALLBACK* cbint NumExtraBytes
	//�������ڴ��������е����� X λ�á� Y λ�á����ڳߴ�x��y�������ھ�������ڱ�ʶ���ص�ָ�롢�����ֽ���
	GUI_Delay(SPEED);
}


//��ָ���Ĵ�����Ч����Ҫ��ʾWM_InvalidateWindow()����
static void _DemoInvalidateWindow(void)
{
	_ChangeInfoText("WM_InvalidateWindow()");//�򱳾�����һ��װ����ʾ���ݵ���Ϣ����ʹ�ñ�������Ч
	_WindowColor1 = GUI_BLUE;//�ػ�ǰ�޸���ɫ
	_FrameColor1 = GUI_GREEN;
	GUI_Delay(SPEED);
	//ʹ����1��Ч�����ػ洰�ڣ�ע�ⴰ����ɫ�ı仯
	WM_InvalidateWindow(_hWindow1); //�ᷢ�ʹ����ػ�msg���ڶ�Ӧ���ô��ھ���Ļص������������
	GUI_Delay(SPEED);
}

//��ָ�����ڷ�������ͬ�����ڵ�ǰ�棬��ҪչʾWMBringToTop()������ʹ��
static void _DemoBringToTop(void)
{
	_ChangeInfoText("WM_BringToTop()");
	GUI_Delay(SPEED);
	WM_BringToTop(_hWindow1); //ʹ����1��������ͬ�����ڵ�ǰ��
	GUI_Delay(SPEED); 
}

//�ƶ����ڵ�ָ���ط�����ҪչʾWM_MoveTo()����
static void _DemoMoveTo(void)
{
	int i;
	int tm;
	int tDiff;
	
	_ChangeInfoText("WM_MoveTo()");//ָ��������Ч �ػ棬֮����ʾ��Ҫ��ʾ������
	GUI_Delay(SPEED);
	for(i=1;i<56;i++)
	{
		tm=GUI_GetTime();
		WM_MoveTo(_hWindow1,50+i,70+i);  //�ƶ�ָ�����ڵ�ָ��λ��
		WM_MoveTo(_hWindow2,105-i,125-i);
		tDiff=15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=1;i<56;i++)
	{
		tm=GUI_GetTime();
		WM_MoveTo(_hWindow1,105-i,125-i);
		WM_MoveTo(_hWindow2,50+i,70+i);
		tDiff=15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	GUI_Delay(SPEED);
}

//��ָ�����ڷ�������ͬ�����ڵ�ǰ�棬��ҪչʾWMBringToBottom()������ʹ��
static void _DemoBringToBottom(void)
{
	_ChangeInfoText("WM_BringToBottom()");
	GUI_Delay(SPEED);
	WM_BringToBottom(_hWindow1); 	//ʹ����1��������ͬ�����ڵĺ���
	GUI_Delay(SPEED);
}


//�ƶ�����ָ���ľ��룬��ҪչʾWM_MoveWindow()������ʹ��
//WM_MoveWindow()�����ǰѴ����ƶ�ָ������
//��WM_MoveTo()�ǽ������ƶ���ָ��λ��,ע����������������
static void _DemoMoveWindow(void)
{
	int i;
	int tm;
	int tDiff;
	
	_ChangeInfoText("WM_MoveWindow()");
	GUI_Delay(SPEED);
	for(i=0;i<55;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hWindow1,1,1); 	//�ƶ�����1
		WM_MoveWindow(_hWindow2,-1,-1);	//�ƶ�����2
		//���ھ���� Ҫ�ƶ���ˮƽ���롣 Ҫ�ƶ��Ĵ�ֱ���롣
		tDiff=15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=0;i<55;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hWindow1,-1,-1);
		WM_MoveWindow(_hWindow2,1,1);
		tDiff=15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	GUI_Delay(SPEED);
}

//���غ���ʾ������,��Ҫ��ʾWM_HideWindow()��WM_ShowWindow()������ʹ��
//����WM_HideWindow()����WM_ShowWindow()�����������󲢲����������ػ���
//��ʾ���ڣ���Ҫִ��WM_Exec()�����ػ棬���Ҫ�������ػ�����ʾ�Ļ�Ӧ����
//WM_Paint()������GUI_Delay()�������������WM_Exec()�������������ǿ���
//�ĺ������������ػ�����ʾ�˴���
static void _DemoHideShowParent(void)
{
	_ChangeInfoText("WM_HideWindow(Parent)");
	GUI_Delay(SPEED);
	WM_HideWindow(_hWindow2);	//���ش���2
	GUI_Delay(SPEED/3);
	WM_HideWindow(_hWindow1);	//���ش���1
	GUI_Delay(SPEED);
	
	_ChangeInfoText("WM_ShowWindow(Parent)");
	GUI_Delay(SPEED);
	WM_ShowWindow(_hWindow1);	//��ʾ����1
	WM_ShowWindow(_hWindow2);	//��ʾ����2
	GUI_Delay(SPEED);
	
}

//���غ���ʾ�Ӵ���,��Ҫ��ʾWM_HideWindow()��WM_ShowWindow()������ʹ��
static void _DemoHideShowChild(void)
{
	_ChangeInfoText("WM_HideWindow(Child)");
	GUI_Delay(SPEED);
	WM_HideWindow(_hChild);	//�����Ӵ���
	GUI_Delay(SPEED);
	
	_ChangeInfoText("WM_ShowWindow(Child)");
	GUI_Delay(SPEED);
	WM_ShowWindow(_hChild);	//��ʾ�Ӵ���
	GUI_Delay(SPEED);
}	

//��ʾ���ڲü�����
static void _DemoClipping(void)
{
	int i;
	int tm;
	int tDiff;
	
	_ChangeInfoText("Demostrating clipping of child");
	for(i=0;i<25;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hChild,1,0);
		tDiff = 15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=0;i<25;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hChild,0,1);
		tDiff = 15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=0;i<50;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hChild,-1,0);
		tDiff = 15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=0;i<25;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hChild,0,-1);
		tDiff = 15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for(i=0;i<25;i++)
	{
		tm=GUI_GetTime();
		WM_MoveWindow(_hChild,1,0);
		tDiff = 15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	GUI_Delay(SPEED);
}

//��ʾʹ�ûص�������ʵ�ִ��ڵ��ػ�
static void _DemoRedrawing(void)
{
	int i;
	int tm;
	int tDiff;
	
	_ChangeInfoText("Demonstrating redrawing");
	GUI_Delay(SPEED);
	_LiftUp(40);	//����40������
	GUI_Delay(SPEED/3);
	_ChangeInfoText("Using a callback for redrawing");
	GUI_Delay(SPEED/3);
	for (i = 0; i < 55; i++) 
	{
		tm = GUI_GetTime();
		WM_MoveWindow(_hWindow1,  1,  1);
		WM_MoveWindow(_hWindow2, -1, -1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	for (i = 0; i < 55; i++) 
	{
		tm = GUI_GetTime();
		WM_MoveWindow(_hWindow1, -1, -1);
		WM_MoveWindow(_hWindow2,  1,  1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	GUI_Delay(SPEED/4);
	_LiftDown(30);
	GUI_Delay(SPEED/2);
	_ChangeInfoText("Without redrawing");
	GUI_Delay(SPEED);
	_LiftUp(30);
	GUI_Delay(SPEED/4);
	WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);	//ȡ���������ڵĻص�����
	for (i = 0; i < 55; i++) 
	{
		tm = GUI_GetTime();
		WM_MoveWindow(_hWindow1,  1,  1);
		WM_MoveWindow(_hWindow2, -1, -1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	for (i = 0; i < 55; i++) 
	{
		tm = GUI_GetTime();
		WM_MoveWindow(_hWindow1, -1, -1);
		WM_MoveWindow(_hWindow2,  1,  1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	GUI_Delay(SPEED/3);
	WM_SetCallback(WM_HBKWIN, sample_cbBkWindow); //�ָ��������ڵĻص�����
	_LiftDown(40);
	GUI_Delay(SPEED);
}

//��ʾ���Ŵ��ڣ���Ҫ��WM_ResizeWindow()������ʹ��
static void _DemoResizeWindow(void)
{
	int i;
	int tm;
	int tDiff;
	
	_ChangeInfoText("WM_ResizeWindow()");
	GUI_Delay(SPEED);
	_LiftUp(30);
	for(i=0;i<20;i++)
	{
		tm=GUI_GetTime();
		WM_ResizeWindow(_hWindow1,1,1); //����1��X,Y��ÿ������1�����ش�С
		WM_ResizeWindow(_hWindow2,-1,-1);//����2��X,Y��ÿ�μ�Сһ�����ش�С
		tDiff=15-(GUI_GetTime()-tm);
		GUI_Delay(tDiff);
	}
	for (i = 0;i < 40; i++) 
	{
		tm = GUI_GetTime();
		WM_ResizeWindow(_hWindow1, -1, -1);
		WM_ResizeWindow(_hWindow2,  1,  1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	for (i = 0; i < 20; i++)
	{
		tm = GUI_GetTime();
		WM_ResizeWindow(_hWindow1,  1,  1);
		WM_ResizeWindow(_hWindow2, -1, -1);
		tDiff = 15 - (GUI_GetTime() - tm);
		GUI_Delay(tDiff);
	}
	_LiftDown(30);
	GUI_Delay(SPEED);
}


//���ûص���������Ҫ��ʾWM_SetCallback()������ʹ��
static void _DemoSetCallback(void)
{
	_ChangeInfoText("WM_SetCallback()");
	WM_SetCallback(_hWindow1, _cbDemoCallback1);//���»��ƻص�����
	WM_InvalidateWindow(_hWindow1);//������Ч ʹ���µĻص������ػ�
	GUI_Delay(SPEED/2);
	WM_SetCallback(_hWindow2, _cbDemoCallback2);
	WM_InvalidateWindow(_hWindow2);
	GUI_Delay(SPEED*3);
	WM_SetCallback(_hWindow1, sample_cbWindow1);//��������ԭ���Ļص�����
	WM_InvalidateWindow(_hWindow1);
	GUI_Delay(SPEED/2);
	WM_SetCallback(_hWindow2, _cbWindow2);
	WM_InvalidateWindow(_hWindow2);
	GUI_Delay(SPEED);
}

//ɾ������
static void _DemoDeleteWindow(void)
{
	_ChangeInfoText("WM_DeleteWindow()");
	GUI_Delay(SPEED);
	WM_DeleteWindow(_hWindow2); //ɾ������2
	GUI_Delay(SPEED/3);
	WM_DeleteWindow(_hWindow1);	//ɾ������1
	GUI_Delay(SPEED);
	_ChangeInfoText("");
	GUI_Delay(SPEED);

	//�ָ�������ɫ�ͻص�����
	WM_SetCallback(WM_HBKWIN, _cbBkWindowOld);
	_WindowColor1 = GUI_GREEN;
	_WindowColor2 = GUI_RED;
}


//��ʾ����
void sample_ShowDemo(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_CRC, ENABLE);	// ʹ��CRCʱ�ӣ�����emwin��֤	
	GUI_Init();
	WM_MULTIBUF_Enable(1);	// �����໺��	
	GUI_SetBkColor(GUI_BLACK);//���ñ�����ɫ
	while (1) 
	{
		_DemoSetDesktopColor();//���洰����ɫ���úͻָ�
		_DemoCreateWindow();//������������ window1��window2
		_DemoCreateWindowAsChild();//�����Ӵ��ڣ�����windows2
		_DemoInvalidateWindow();//window1������Ч��ʹ�ô���1ʵ���ػ�
		_DemoBringToTop();//ʹ����1��������ͬ�����ڵ�ǰ��
		_DemoMoveTo();
		_DemoBringToBottom();//ʹ����1��������ͬ�����ڵĺ���
		_DemoMoveWindow();
		_DemoHideShowParent();//���ش��ں���ʾ����
		_DemoHideShowChild();//�����Ӵ��ں���ʾ�Ӵ���
		_DemoClipping();//��ʾ���ڲü����� �Ӵ��ڳ��������ڵ�һ���ֽ��ᱻ���е�
		_DemoRedrawing();//��ʾ�ص�������û�лص�����������
		_DemoResizeWindow();//�����������С
		_DemoSetCallback();//�޸Ļص����������ô�����Ч��ʵ���ô�����ʾ���
		_DemoDeleteWindow();
  }
}





