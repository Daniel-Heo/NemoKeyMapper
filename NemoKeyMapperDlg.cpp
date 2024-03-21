
// NemoKeyMapperDlg.cpp: 구현 파일

#include "pch.h"
#include <fstream>
#include "framework.h"
#include "NemoKeyMapper.h"
#include "NemoKeyMapperDlg.h"
#include "afxdialogex.h"
#include "json.hpp" 
#include "StrConvert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 트레이 아이콘 사용 여부
//#define TRAY_ICON_ACTIVE
#define KEY_MAP_MAX 1024
#define NUM_SHIFT 1
#define NUM_CTRL 2
#define NUM_ALT 3

// nlohmann 라이브러리의 네임스페이스 사용
using json = nlohmann::json;

// 전역변수
json key_list= json::array();

// 키 등록시 사용하는 변수들
BOOL isInput1;
BOOL isInput2;
DWORD keyValue1;
DWORD keyValue2;
int subValue1; // SHIFT, CTRL, ALT
int subValue2; // SHIFT, CTRL, ALT

// 키 매핑 실행 여부
BOOL isRun = FALSE; 

// 키 매핑 데이터
BYTE inputKey[KEY_MAP_MAX];
int subInputKey[KEY_MAP_MAX]; // SHIFT 1, CTRL 2, ALT 3, None 0
BYTE mapKey[KEY_MAP_MAX];
int subMapKey[KEY_MAP_MAX]; // SHIFT 1, CTRL 2, ALT 3, None 0
int mapKeyCount = 0;

// 전역 변수로 키보드 후크 핸들을 선언
//HHOOK hKeyboardHook = NULL;

// 후킹 실행
//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNemoKeyMapperDlg 대화 상자

CNemoKeyMapperDlg::CNemoKeyMapperDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEMOKEYMAPPER_DIALOG, pParent)
{
	m_isEditRegList = FALSE;
	m_selNoRegList = -1;
	// m_nid 초기화
	memset(&m_nid, 0, sizeof(m_nid));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNemoKeyMapperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNemoKeyMapperDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION, &CNemoKeyMapperDlg::OnMyTrayNotification)
	ON_MESSAGE(WM_GETINPUT1, &CNemoKeyMapperDlg::OnGetInput1)
	ON_MESSAGE(WM_GETINPUT2, &CNemoKeyMapperDlg::OnGetInput2)
	ON_COMMAND(ID_QUIT, &CNemoKeyMapperDlg::OnQuit)
	ON_LBN_SELCHANGE(IDC_INPUT_LIST, &CNemoKeyMapperDlg::OnSelchangeInputList)
	ON_LBN_SELCHANGE(IDC_MAP_LIST, &CNemoKeyMapperDlg::OnSelchangeMapList)
	ON_BN_CLICKED(IDC_INPUT1, &CNemoKeyMapperDlg::OnClickedInput1)
	ON_BN_CLICKED(IDC_INPUT2, &CNemoKeyMapperDlg::OnClickedInput2)

	ON_BN_CLICKED(IDC_SAVE, &CNemoKeyMapperDlg::OnClickedSave)
	ON_BN_CLICKED(IDC_ADD, &CNemoKeyMapperDlg::OnClickedAdd)
	ON_BN_CLICKED(IDC_DELETE, &CNemoKeyMapperDlg::OnClickedCancelListSelect)
	ON_LBN_SELCHANGE(IDC_REG_LIST, &CNemoKeyMapperDlg::OnSelchangeRegList)
	ON_BN_CLICKED(IDC_WRITE_REGIST, &CNemoKeyMapperDlg::OnClickedWriteRegist)
	ON_NOTIFY(NM_CLICK, IDC_SYSLINK1, &CNemoKeyMapperDlg::OnNMClickSyslink1)
END_MESSAGE_MAP()


// CNemoKeyMapperDlg 메시지 처리기

BOOL CNemoKeyMapperDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// 화면 해상도를 얻습니다.
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// 대화 상자의 크기를 얻습니다.
	CRect rect;
	GetWindowRect(&rect);
	int dialogWidth = rect.Width();
	int dialogHeight = rect.Height();

	// 새 위치를 계산합니다.
	int x = (screenWidth - dialogWidth) / 2;
	int y = (screenHeight - dialogHeight) / 2;

	// 대화 상자의 위치를 설정합니다.
	SetWindowPos(NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);

	ShowWindow(SW_SHOW);

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
#ifdef TRAY_ICON_ACTIVE
	// 트레이 아이콘 설정
	m_nid.cbSize = sizeof(NOTIFYICONDATA);
	m_nid.hWnd = this->m_hWnd; // 메시지를 받을 윈도우 핸들
	m_nid.uID = 1;             // 아이콘 식별자
	m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	m_nid.uCallbackMessage = WM_MY_TRAY_NOTIFICATION; // 사용자 정의 메시지
	m_nid.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	m_nid.szTip;
	WCHAR tmp[100]= L"NemoKeyMapper";
	wcscpy_s(m_nid.szTip, _countof(tmp), tmp); // 툴팁 텍스트
	Shell_NotifyIcon(NIM_ADD, &m_nid);
#endif

	// json 파일을 읽어서 키 리스트 등록
	std::ifstream file("keymap.json");
	if (file.is_open()) {
		file >> key_list;
		//std::cout << "입력 받은 문자열 :: " << file.dump() << std::endl;
	}
	else {
		// alert
		CString strErr = StringTableToCString(IDS_ERR_OPENJSON);
		MessageBox(strErr);
	}

	// 키 리스트 셋팅
	CListBox* pList = (CListBox*)GetDlgItem(IDC_INPUT_LIST);
	CListBox* pList2 = (CListBox*)GetDlgItem(IDC_MAP_LIST);
	for (json::iterator it = key_list.begin(); it != key_list.end(); ++it) {
		std::string key = it.key();
		std::string key_value = key + " " + it.value().get<std::string>();

		// IDC_INPUT_LIST에 키 리스트와 값 추가
		pList->AddString(StringToWCHAR(key_value).c_str());
		pList->SetItemData(pList->GetCount() - 1, (DWORD_PTR)StringToWCHAR(key).c_str());

		pList2->AddString(StringToWCHAR(key_value).c_str());
		pList2->SetItemData(pList->GetCount() - 1, (DWORD_PTR)StringToWCHAR(key).c_str());
	}

	// Regist에서 데이터 읽어옴.
	LoadRegistrySettings();


	// 키보드의 RAW 입력을 등록
	//RAWINPUTDEVICE Rid[1];
	//Rid[0].usUsagePage = 0x01; // 키보드의 사용 페이지
	//Rid[0].usUsage = 0x06; // 키보드의 사용
	//Rid[0].dwFlags = RIDEV_INPUTSINK; // 이 프로그램이 포커스를 잃어도 메시지를 받겠다는 옵션
	//Rid[0].hwndTarget = GetSafeHwnd(); // 메시지를 받을 윈도우 핸들

	/*if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0])))
	{
		MessageBox(_T("RAW 입력 디바이스 등록 실패"));
		return FALSE;
	}*/

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CNemoKeyMapperDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ((nID & 0xFFF0) == SC_MINIMIZE)
	{
#ifdef		TRAY_ICON_ACTIVE
		ShowWindow(SW_HIDE); // 윈도우 숨기기
#endif
		ShowWindow(SW_MINIMIZE);
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNemoKeyMapperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNemoKeyMapperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CNemoKeyMapperDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
#ifdef TRAY_ICON_ACTIVE
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
#endif
}


afx_msg LRESULT CNemoKeyMapperDlg::OnMyTrayNotification(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			menu.LoadMenu(IDR_MENU1); // 컨텍스트 메뉴 리소스 로드
			CMenu* pSubMenu = menu.GetSubMenu(0); // 첫 번째 서브 메뉴 가져오기

			if (pSubMenu)
			{
				POINT pt;
				GetCursorPos(&pt); // 마우스 위치 가져오기
				SetForegroundWindow(); // 메뉴가 올바르게 닫히도록 윈도우를 활성화
				pSubMenu->TrackPopupMenu(TPM_RIGHTBUTTON, pt.x, pt.y, this);
			}
			menu.DestroyMenu(); // 메뉴 리소스 정리
			return TRUE;
		}
		break;
		case WM_LBUTTONDOWN:
		{
			ShowWindow(SW_RESTORE);
		}
		break;
	}

	return 0;
}


void CNemoKeyMapperDlg::OnQuit()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	PostQuitMessage(0);
}


void CNemoKeyMapperDlg::OnSelchangeInputList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pList = (CListBox*)GetDlgItem(IDC_INPUT_LIST);
	CString strItemText;
	pList->GetText(pList->GetCurSel(), strItemText);
	int pos = strItemText.Find(L" ");
	// 데이터를 CString으로 변환합니다.
	CStatic* pStaticControl = (CStatic*)GetDlgItem(IDC_KEY_VALUE1);
	pStaticControl->SetWindowText(strItemText.Left(pos));
}


void CNemoKeyMapperDlg::OnSelchangeMapList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CListBox* pList = (CListBox*)GetDlgItem(IDC_MAP_LIST);
	CString strItemText;
	pList->GetText(pList->GetCurSel(), strItemText);
	int pos = strItemText.Find(L" ");
	// 데이터를 CString으로 변환합니다.
	CStatic* pStaticControl = (CStatic*)GetDlgItem(IDC_KEY_VALUE2);
	pStaticControl->SetWindowText(strItemText.Left(pos));
}


void CNemoKeyMapperDlg::OnClickedInput1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	isInput1 = TRUE;
}


void CNemoKeyMapperDlg::OnClickedInput2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	isInput2= TRUE;
}

LRESULT CNemoKeyMapperDlg::OnGetInput1(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD key;
	key = keyValue1;

	BYTE key1 = (key >> 24) & 0x01;
	BYTE key2 = (key >> 16) & 0xFF;

	if (key1) key1 = 0xE0;
	if (key2 == 0x00) {
		key2 = (subValue1)& 0xFF;
		if (key2 == 0xAF) key2 = 0x30;
		else if (key2 == 0xAE) key2 = 0x2E;

		CString cst = StringTableToCString(IDS_CAUTION_NOMAP);
		CString cst2 = StringTableToCString(IDS_ALERT);
		MessageBox(cst, cst2, MB_OK | MB_ICONINFORMATION);
	}

	// Double Key 지원일 경우
	//if ((0x0D < key && key < 0x13) || (0x9F < key && key < 0xA6)) // CTRL, Shift, ALT일 경우 skip
	//	return 0;

	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	CString key_str;
	if (key1 == 0xE0)
		key_str.Format(_T("%02X_%02X"), key1, key2);
	else
		key_str.Format(_T("%02X"), key2);
	// 데이터를 CString으로 변환합니다.
	CStatic* pStaticControl = (CStatic*)GetDlgItem(IDC_KEY_VALUE1);
	pStaticControl->SetWindowText(key_str);

	return 0;
}


LRESULT CNemoKeyMapperDlg::OnGetInput2(WPARAM wParam, LPARAM lParam)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	DWORD key;
	key = keyValue2;

	BYTE key1 = (key >> 24) & 0x01;
	BYTE key2 = (key >> 16) & 0xFF;

	if (key1) key1 = 0xE0;
	if (key2 == 0x00) {
		key2 = (subValue2) & 0xFF;
		if (key2 == 0xAF) key2 = 0x30;
		else if (key2 == 0xAE) key2 = 0x2E;

		CString cst = StringTableToCString(IDS_CAUTION_NOMAP);
		CString cst2 = StringTableToCString(IDS_ALERT);
		MessageBox(cst, cst2, MB_OK | MB_ICONINFORMATION);
	}

	//if ((0x0D < key && key < 0x13) || (0x9F < key && key < 0xA6)) // CTRL, Shift, ALT일 경우 skip
	//	return 0;

	PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
	CString key_str;
	if (key1 == 0xE0)
		key_str.Format(_T("%02X_%02X"), key1, key2);
	else
		key_str.Format(_T("%02X"), key2);
	// 데이터를 CString으로 변환합니다.
	CStatic* pStaticControl = (CStatic*)GetDlgItem(IDC_KEY_VALUE2);
	pStaticControl->SetWindowText(key_str);

	return 0;
}

BOOL CNemoKeyMapperDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// 특정 키 입력을 확인하고 처리
	if (isInput1 || isInput2)
	{
		//DWORD key;
		DWORD scan_key;
		int subValue = 0;

		if (pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN || pMsg->message == WM_APPCOMMAND)
		//if (pMsg->message == WM_APPCOMMAND)
		{
			// Shift, Ctrl, Alt 키의 상태를 확인
			//if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000) subValue = NUM_CTRL;  // CTRL 키가 눌렸을 때
			//else if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000) subValue = NUM_SHIFT; // Shift 키가 눌렸을 때
			//else if (GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000) 	subValue = NUM_ALT; // Alt 키가 눌렸을 때

			// 키보드 등의 입력시 :  HID -> Scan Code -> Virtual Key Code ( 미디어 키는 HID로만 입력됨 - HID API로 raw level input을 받아야함. )
			//key = pMsg->wParam; // Virtual Key Code
			scan_key = pMsg->lParam; // Scan Code

			//if( !(0x0D< key && key < 0x13) && !( 0x9F < key && key < 0xA6) ) // CTRL, Shift, ALT가 아닌 경우
			//{
				// 설정에서 키보드 입력을 받을 때
				if (isInput1 == TRUE) {
					keyValue1 = scan_key;
					subValue1 = pMsg->wParam;
					theApp.m_pMainWnd->PostMessage(WM_GETINPUT1, pMsg->wParam, pMsg->lParam);
				}
				else if (isInput2 == TRUE) {
					keyValue2 = scan_key;
					subValue2 = pMsg->wParam;
					theApp.m_pMainWnd->PostMessage(WM_GETINPUT2, pMsg->wParam, pMsg->lParam);
				}
				isInput1 = FALSE;
				isInput2 = FALSE;
			//}

			return TRUE; // Enter 키나 Esc 키 입력을 무시하고 소리를 방지
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CNemoKeyMapperDlg::OnClickedSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// IDC_REG_LIST에 IDC_KEY_VALUE1과 IDC_KEY_VALUE2의 값을 추가 중간에 >>를 넣음
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	CString strItemText;
	CStatic* pStaticControl1 = (CStatic*)GetDlgItem(IDC_KEY_VALUE1);
	CStatic* pStaticControl2 = (CStatic*)GetDlgItem(IDC_KEY_VALUE2);
	CString key1;
	CString key2;
	pStaticControl1->GetWindowText(key1);
	pStaticControl2->GetWindowText(key2);

	// key 체크 확인
	if (key1.IsEmpty() || key2.IsEmpty()) {
		CString cst = StringTableToCString(IDS_CAUTION_NOKEY);
		CString cst2 = StringTableToCString(IDS_ALERT);
		MessageBox(cst, cst2, MB_OK | MB_ICONINFORMATION);
		return;
	}

	std::string key_str = CStringToString(key1);
	CString text1 = StringToCString(key_list.value(key_str, ""));
	key_str = CStringToString(key2);
	CString text2 = StringToCString(key_list.value(key_str, ""));
	strItemText = key1 +" "+text1 + L" >> " + key2+" " + text2;
	// 만약 m_isEditRegList가 TRUE이면 m_selNoRegList의 항목을 strItemText로 변경
	if (m_isEditRegList) {
		pList->DeleteString(m_selNoRegList);
		pList->InsertString(m_selNoRegList, strItemText);
	}
	else {
		pList->AddString(strItemText);
	}

	// JSON 파일에 저장
	//SaveKeyMappingsToJson();

}


void CNemoKeyMapperDlg::OnClickedAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// IDC_KEY_VALUE1과 IDC_KEY_VALUE2의 값을 초기화
	// m_isEditRegList를 FALSE로 설정
	CStatic* pStaticControl1 = (CStatic*)GetDlgItem(IDC_KEY_VALUE1);
	CStatic* pStaticControl2 = (CStatic*)GetDlgItem(IDC_KEY_VALUE2);
	pStaticControl1->SetWindowText(L"");
	pStaticControl2->SetWindowText(L"");
	m_isEditRegList = FALSE;
	m_selNoRegList = -1;
	// IDC_REG_LIST 선택을 해제
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	pList->SetCurSel(-1);
}


void CNemoKeyMapperDlg::OnClickedCancelListSelect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// IDC_REG_LIST에서 선택된 항목을 삭제
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	pList->DeleteString(pList->GetCurSel());
	OnClickedAdd(); // 기존 삭제와 연결된 것이 있을 수 있으므로 초기화
}


void CNemoKeyMapperDlg::OnSelchangeRegList()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// IDC_REG_LIST의 선택값을 >>로 나누어서 1번키와 2번키로 하고 1번키와 2번키를 '+'로 나누어서 '+'가 있으면 앞에 있는 텍스트를 IDC_SUB_KEY1과 IDC_SUB_KEY2의 선택값으로 설정 뒤의 값은 IDC_KEY_VALUE1과 IDC_KEY_VALUE2의 값으로 설정
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	if (pList->GetCurSel() == -1) return;
	CString strItemText;
	pList->GetText(pList->GetCurSel(), strItemText);
	int pos = strItemText.Find(L">>");
	CString key1 = strItemText.Left(pos);
	CString key2 = strItemText.Right(strItemText.GetLength() - pos - 3);

	int pos2 = key1.Find(L" ");
	if (pos2 != -1) key1 = key1.Left(pos2);

	pos2 = key2.Find(L" ");
	if (pos2 != -1) key2 = key2.Left(pos2);

	CStatic* pStaticControl1 = (CStatic*)GetDlgItem(IDC_KEY_VALUE1);
	CStatic* pStaticControl2 = (CStatic*)GetDlgItem(IDC_KEY_VALUE2);
	pStaticControl1->SetWindowText(key1);
	pStaticControl2->SetWindowText(key2);
	m_isEditRegList = TRUE;
	m_selNoRegList = pList->GetCurSel();
}

void CNemoKeyMapperDlg::SaveKeyMappingsToJson() {
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	const std::string& filePath = "config.json";

	json j; // JSON 객체 생성

	// ListBox에서 항목을 순회하며 JSON 객체에 추가
	int itemCount = pList->GetCount();
	for (int i = 0; i < itemCount; ++i) {
		CString itemText;
		pList->GetText(i, itemText); // 항목의 텍스트를 가져옴
		CStringA itemTextA(itemText);
		std::string key = std::to_string(i + 1);
		std::string value = itemTextA.GetBuffer(); // CString을 std::string으로 변환

		j[key] = value; // JSON 객체에 매핑 추가
	}
}

void CNemoKeyMapperDlg::LoadKeyMappingsFromJson(BOOL isStart) {
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	const std::string& filePath = "config.json";

	std::ifstream file(filePath);
	json j;
	if (file.is_open()) {
		file >> j; // 파일에서 JSON 데이터를 읽음
		file.close();
	}

	int i = 1;
	while (1) {
		std::string key = std::to_string(i);
		if (j.find(key) == j.end()) break; // 키가 존재하지 않으면 종료
		std::string value = j[key].get<std::string>(); // JSON 데이터를 읽음
		pList->AddString(CString(value.c_str())); // 항목 추가
		++i;
	}
}

// CString에서 Hex 문자열을 BYTE로 변환하는 함수
BYTE ConvertHexStringToByte(const CString& hexStr) {
	if (hexStr.GetLength() < 2) return 0; // 2자리 이상이 아니면 변환할 수 없음

	char ch;

	if(hexStr[0] >= 'A' && hexStr[0] <= 'F') ch = hexStr[0]-'A'+10;
	else if(hexStr[0] >= 'a' && hexStr[0] <= 'f') ch = hexStr[0]-'a'+10;
	else ch = hexStr[0]-'0';

	ch = ch << 4;

	if(hexStr[1] >= 'A' && hexStr[1] <= 'F') ch += hexStr[1]-'A'+10;
	else if(hexStr[1] >= 'a' && hexStr[1] <= 'f') ch += hexStr[1]-'a'+10;
	else ch += hexStr[1]-'0';

	return ch;
}

void CNemoKeyMapperDlg::OnClickedWriteRegist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SaveMappingsToRegistry();
}

void CNemoKeyMapperDlg::LoadRegistrySettings()
{
	HKEY hKey;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\Keyboard Layout"), 0, KEY_READ, &hKey) == ERROR_SUCCESS)
	{
		DWORD dwSize = 0;
		if (RegQueryValueEx(hKey, _T("Scancode Map"), NULL, NULL, NULL, &dwSize) == ERROR_SUCCESS)
		{
			BYTE* pBytes = new BYTE[dwSize];
			if (RegQueryValueEx(hKey, _T("Scancode Map"), NULL, NULL, pBytes, &dwSize) == ERROR_SUCCESS)
			{
				LoadListWithKeys(pBytes, dwSize);
			}
			delete[] pBytes;
		}
		RegCloseKey(hKey);
	}
}

void CNemoKeyMapperDlg::LoadListWithKeys(const BYTE* bytes, DWORD dwSize)
{
	if (dwSize > 8) // 첫 8바이트는 헤더
	{
		DWORD nMappings = bytes[8] + (bytes[9] << 8) + (bytes[10] << 16) + (bytes[11] << 24);
		nMappings -= 1; // 마지막 널 매핑 제외
		CString strFromCode, strToCode, strItemText;
		CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
		DWORD offset;

		for (DWORD i = 0; i < nMappings; ++i)
		{
			offset = 12 + (i * 4);

			if(bytes[offset + 3]==0) strFromCode.Format(_T("%02X"), bytes[offset + 2]);
			else	strFromCode.Format(_T("%02X_%02X"), bytes[offset + 3], bytes[offset + 2]);
			if(bytes[offset + 1]==0) strToCode.Format(_T("%02X"), bytes[offset]);
			else	strToCode.Format(_T("%02X_%02X"), bytes[offset + 1], bytes[offset]);

			std::string key_str = CStringToString(strFromCode);
			CString text1 = StringToCString(key_list.value(key_str, ""));
			key_str = CStringToString(strToCode);
			CString text2 = StringToCString(key_list.value(key_str, ""));
			strItemText = strFromCode + " " + text1 + L" >> " + strToCode + " " + text2;
			pList->AddString(strItemText);
		}
	}
}

CString BytesToHexCString(const BYTE* pBytes, size_t size)
{
	CString strResult, strHex;
	for (size_t i = 0; i < size; ++i)
	{
		// 현재 바이트를 2자리 16진수 문자열로 변환합니다.
		strHex.Format(_T("%02X "), pBytes[i]);
		strResult += strHex;

		// 8바이트마다 줄바꿈을 추가합니다. (0부터 시작하므로 i+1을 사용)
		if ((i + 1) % 8 == 0)
		{
			strResult.TrimRight(); // 줄의 마지막 공백 제거
			strResult += _T("\n"); // 줄바꿈 추가
		}
	}

	// 마지막에 추가된 공백과 줄바꿈이 있다면 제거합니다.
	strResult.TrimRight();
	return strResult;
}

void CNemoKeyMapperDlg::SaveMappingsToRegistry()
{
	/*std::vector<BYTE> bytes = DefineScancodeMap();
	CString str = BytesToHexCString(&bytes[0], bytes.size());
	MessageBox(str);
	return;*/
	CWaitCursor wait; // 대기 커서 설정

	CRegKey regKey;
	LONG lResult = regKey.Open(HKEY_LOCAL_MACHINE, _T("System\\CurrentControlSet\\Control\\Keyboard Layout"), KEY_WRITE);
	if (lResult != ERROR_SUCCESS)
	{
		CString cst = StringTableToCString(IDS_ERROR_WRITE_REGIST);
		CString cst2 = StringTableToCString(IDS_ERROR_WRITE_REGIST);
		CString cst3 = StringTableToCString(IDS_ERROR);

		CString str;
		// 영문으로 된 경고 메시지를 작성.
		if(lResult==5) str.Format(cst);
		else str.Format(_T("%s %d"), cst2, lResult);
		
		// 한글로 된 경고 메시지를 작성.
		//if(lResult==5) str.Format(_T("키 맵핑 정보 쓰기 열기 실패! 관리자 권한으로 실행하세요!"));
		//else str.Format(_T("키 맵핑 정보 쓰기 열기 실패! 오류 코드: %d"), lResult);
		
		MessageBox(str, cst3, MB_ICONERROR);
		return;
	} 


	std::vector<BYTE> bytes = DefineScancodeMap();
	if (bytes.empty())
	{
		regKey.DeleteValue(_T("Scancode Map"));
	}
	else
	{
		regKey.SetBinaryValue(_T("Scancode Map"), &bytes[0], bytes.size());
	}

	CString strSuc = StringTableToCString(IDS_SUCCESS_WRITE_REGIST);

	MessageBox(strSuc);
}

std::vector<BYTE> CNemoKeyMapperDlg::DefineScancodeMap()
{
	// 예제에서는 CListCtrl에 대한 참조가 필요합니다.
	// lvKeys.Items.Count 등의 부분을 CListCtrl에서 얻어온 항목 개수로 대체합니다.
	CListBox* pList = (CListBox*)GetDlgItem(IDC_REG_LIST);
	int nCount = pList->GetCount();
	CString strItemText;
	int pos, pos2;
	CString key1, key2, subkey1, subkey2;

	// 바이트 배열 생성
	std::vector<BYTE> bytes(8 + 4 + (nCount * 4) + 4, 0); // 초기값 0으로 채움
	*(reinterpret_cast<DWORD*>(&bytes[8])) = nCount + 1; // 매핑 수 설정

	//CString str;
	//str.Format(_T("매핑 수: %d"), nCount);
	//MessageBox(str);

	for (int i = 0; i < nCount; ++i)
	{
		// 여기에 리스트에서 항목을 얻어와서 스캔 코드로 변환하는 로직 구현
		// bytes[(i * 4) + 12 + 0] = ...;
		// bytes[(i * 4) + 12 + 1] = ...;
		// bytes[(i * 4) + 12 + 2] = ...;
		// bytes[(i * 4) + 12 + 3] = ...;
		strItemText = L"";
		pList->GetText(i, strItemText);
		pos = strItemText.Find(L">>");
		if (pos >= 0) {
			key1 = strItemText.Left(pos);
			key2 = strItemText.Right(strItemText.GetLength() - pos - 3);

			pos2 = key1.Find(L" ");
			if (pos2 != -1) key1 = key1.Left(pos2);

			pos2 = key2.Find(L" ");
			if (pos2 != -1) key2 = key2.Left(pos2);

			// key1의 사이즈를 확인하여 4바이트보다 크면 XX_XX로 2바이트씩 나누어 저장
			if (key1.GetLength() > 4) {
				bytes[(i * 4) + 12 + 3] = ConvertHexStringToByte(key1.Left(2));
				bytes[(i * 4) + 12 + 2] = ConvertHexStringToByte(key1.Mid(3,2));
			}
			else {
				bytes[(i * 4) + 12 + 3] = 0;
				bytes[(i * 4) + 12 + 2] = ConvertHexStringToByte(key1);
			}
			if (key2.GetLength() > 4) {
				bytes[(i * 4) + 12 + 1] = ConvertHexStringToByte(key2.Left(2)); 
				bytes[(i * 4) + 12 + 0] = ConvertHexStringToByte(key2.Mid(3, 2));
			}
			else {
				bytes[(i * 4) + 12 + 1] = 0;
				bytes[(i * 4) + 12 + 0] = ConvertHexStringToByte(key2);
			}

		}
	}

	// 마지막 4바이트는 이미 0으로 초기화됨

	return bytes;
}

void CNemoKeyMapperDlg::OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	PNMLINK pNMLink = (PNMLINK)pNMHDR;
	::ShellExecute(NULL, L"open", pNMLink->item.szUrl, NULL, NULL, SW_SHOW);

	*pResult = 0;
}


// 후킹형 매핑 : 키 매핑 신호가 입력되면 맵핑된 키를 발생시키는 함수
//LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
//	if (nCode >= 0) {
//		// KBDLLHOOKSTRUCT 구조체로 lParam을 캐스팅하여 키 정보 얻기
//		PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
//		int subValue = 0;
//
//		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
//			if (GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000) subValue = NUM_CTRL;  // CTRL 키가 눌렸을 때
//			else if (GetAsyncKeyState(VK_LSHIFT) & 0x8000 || GetAsyncKeyState(VK_RSHIFT) & 0x8000) subValue = NUM_SHIFT; // Shift 키가 눌렸을 때
//			else if (GetAsyncKeyState(VK_LMENU) & 0x8000 || GetAsyncKeyState(VK_RMENU) & 0x8000) 	subValue = NUM_ALT; // Alt 키가 눌렸을 때
//			else subValue = 0;
//
//			// inputKey와  mapKey를 순회하며 키가 일치하면 매핑된 키로 변경
//			for (int i = 0; i < mapKeyCount; ++i) {
//				if (p->vkCode == inputKey[i] && subValue == subInputKey[i]) {
//					switch (subMapKey[i]) {
//						case NUM_SHIFT:
//							keybd_event(VK_SHIFT, 0, 0, 0); // Shift 키 다운
//							break;
//						case NUM_CTRL:
//							keybd_event(VK_CONTROL, 0, 0, 0); // Ctrl 키 다운
//							break;
//						case NUM_ALT:
//							keybd_event(VK_MENU, 0, 0, 0); // Alt 키 다운
//							break;
//					}
//
//					keybd_event((BYTE)mapKey[i], 0, 0, 0); // 매핑된 키 다운
//					keybd_event((BYTE)mapKey[i], 0, KEYEVENTF_KEYUP, 0); // 매핑된 키 업
//
//					switch (subMapKey[i]) {
//						case NUM_SHIFT:
//							keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0); // Shift 키 업
//							break;
//						case NUM_CTRL:
//							keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0); // Ctrl 키 업
//							break;
//						case NUM_ALT:
//							keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0); // Alt 키 업
//							break;
//					}
//
//					return 1; // 원래의 키 이벤트는 무시하고 종료
//				}
//			}
//		}
//	}
//
//	// 다른 키 이벤트는 기본 처리
//	return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
//}

// -----------------------------------------
// RAWINPUTDEVICE Sample : 구현해봤지만 Media key 입력받지 못했다. 시간이 많이 걸릴 것 같아서 여기까지만 해봄.
// -----------------------------------------
// RAWINPUTDEVICE 구조체를 사용하여 애플리케이션에서 처리할 HID 장치를 등록합니다.
// 예제 코드에서는 키보드 입력을 등록합니다.
//RAWINPUTDEVICE Rid[1];
//Rid[0].usUsagePage = 0x01;
//Rid[0].usUsage = 0x06; // 키보드
//Rid[0].dwFlags = RIDEV_INPUTSINK;
//Rid[0].hwndTarget = hWnd; // 메시지를 받을 윈도우 핸들
//if (!RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]))) {
//	// 등록 실패 처리
//}

// WM_INPUT 메시지를 처리하기 위한 메시지 핸들러를 구현합니다.
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch (message)
//	{
//	case WM_INPUT:
//	{
//		UINT dwSize;
//
//		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
//		LPBYTE lpb = new BYTE[dwSize];
//		if (lpb == NULL) {
//			return 0;
//		}
//
//		if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize) {
//			delete[] lpb;
//			return 0;
//		}
//
//		RAWINPUT* raw = (RAWINPUT*)lpb;
//
//		if (raw->header.dwType == RIM_TYPEKEYBOARD) {
//			// 여기에서 키보드 입력 처리
//			// 예: raw->data.keyboard.VKey, raw->data.keyboard.Flags 등을 사용
//		}
//
//		delete[] lpb;
//	}
//	break;
//	// 다른 메시지 처리
//	}
//
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}

