
// NemoKeyMapper.cpp: 애플리케이션에 대한 클래스 동작을 정의합니다.
//

#include "pch.h"
#include <iostream>
#include <string>
#include <algorithm>
#include "framework.h"
#include "NemoKeyMapper.h"
#include "NemoKeyMapperDlg.h"
#include "language.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 유일한 CNemoKeyMapperApp 개체입니다.
CNemoKeyMapperApp theApp;

time_t nKeySetStartTime=0;

void TraceLow(const char* text)
{
#ifdef _DEBUG
	CFile file;
	if (file.Open(_T("log.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone)) {
		file.SeekToEnd(); // 기존 내용 끝에 쓰기 위해 파일 포인터를 끝으로 이동
		file.Write(text, strlen(text));
		file.Flush();
		file.Close();
	}
#endif
}

// CNemoKeyMapperApp

BEGIN_MESSAGE_MAP(CNemoKeyMapperApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CNemoKeyMapperApp 생성

CNemoKeyMapperApp::CNemoKeyMapperApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// CNemoKeyMapperApp 초기화

BOOL CNemoKeyMapperApp::InitInstance()
{
	// 언어
	LPWSTR oldUILang;
	GetUserLocale(&oldUILang); // 현재 사용자 인터페이스 언어 설정
	
	WCHAR strTmp[][6] = { L"ko-KR", L"en-US", L"de-DE", L"es-ES", L"fr-FR", L"it-IT", L"ja-JP", L"pt-BR", L"ru-RU", L"zh-CN", L"ar-SA", L"hi-IN" };
	int size = 12;
	// 문자열이 strTmp에 존재하는지 확인
	bool exists = std::any_of(std::begin(strTmp), std::end(strTmp), [&](const WCHAR* s) { return lstrcmpW(s, oldUILang) == 0; });
	if (exists == false) SetUserLocale(L"en-US");
	else SetUserLocale(oldUILang);

	//#ifdef _DEBUG
	// 디버깅용 콘솔 창 생성
	/*BOOL res=AllocConsole();
	std::string tmp;
	tmp=std::to_string(res);
	tmp += "BOOL TRUE[1] FALSE[0] : " + std::to_string(TRUE) + " " + std::to_string(FALSE);
	Trace(tmp.c_str());*/

	// 표준 입출력을 콘솔에 재지정
	/*FILE* fp;
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);*/

	// 콘솔 창에 메시지 출력
	/*std::cout << "디버그 모드: 콘솔 창 생성 성공!" << std::endl;
	printf("디버깅 메시지를 여기에 출력할 수 있습니다.\n");*/
	//#endif
		
	// 애플리케이션 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls={0};
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 애플리케이션 마법사에서 생성된 애플리케이션"));

	CNemoKeyMapperDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 애플리케이션이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고 응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.

	return FALSE;
}



int CNemoKeyMapperApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

#ifdef _DEBUG
	// 디버깅용 콘솔 창 제거
	//FreeConsole();
#endif
	return CWinApp::ExitInstance();
}

void CNemoKeyMapperApp::Trace(const char *text)
{
#ifdef _DEBUG
//#define LOG_SEL_1 1
//#define LOG_SEL_2 1
//#define LOG_SEL_3 1
//#define LOG_SEL_4 1
#define LOG_SEL_5 1
	 
#ifdef LOG_SEL_1
	// 1. 콘솔 창에 메시지 출력
	std::cout << "디버그 모드: 콘솔 창 생성 성공!" << std::endl;
	printf("디버깅 메시지를 여기에 출력할 수 있습니다.\n");
#endif

#ifdef LOG_SEL_2
	// 2. TRACE
	TRACE(_T("디버깅 메시지: %d\n"), 123);
#endif

#ifdef LOG_SEL_3
	// 3. OutputDebugString
	OutputDebugString(_T("디버깅 메시지를 여기에 출력합니다.\n"));
#endif

#ifdef LOG_SEL_4
	// 4. AfxMessageBox
	AfxMessageBox(_T("디버깅 메시지를 여기에 출력합니다."));
#endif

#ifdef LOG_SEL_5
	// 5. log file 생성
	CFile file;
	if (file.Open(_T("log.txt"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::shareDenyNone)) {
		file.SeekToEnd(); // 기존 내용 끝에 쓰기 위해 파일 포인터를 끝으로 이동
		file.Write(text, strlen(text));
		file.Flush();
		file.Close();
	}
#endif
#endif

}

BOOL CNemoKeyMapperApp::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinApp::PreTranslateMessage(pMsg);
}
