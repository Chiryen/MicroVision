
// MicroVision.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMicroVisionApp:
// �йش����ʵ�֣������ MicroVision.cpp
//

class CMicroVisionApp : public CWinApp
{
public:
	CMicroVisionApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMicroVisionApp theApp;