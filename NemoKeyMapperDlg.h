
// NemoKeyMapperDlg.h: 헤더 파일
//
//#pragma comment(linker, " /manifestUAC:\"level='requireAdministrator' uiAccess='false'\"")
#pragma once

#include <vector>

#define WM_MY_TRAY_NOTIFICATION (WM_USER + 1)
#define WM_GETINPUT1 (WM_USER + 2)
#define WM_GETINPUT2 (WM_USER + 3)

// CNemoKeyMapperDlg 대화 상자
class CNemoKeyMapperDlg : public CDialogEx
{
// 생성입니다.
public:
	CNemoKeyMapperDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEMOKEYMAPPER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;
	NOTIFYICONDATA m_nid;
	BOOL m_isEditRegList;
	int m_selNoRegList;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnDestroy();
protected:
	afx_msg LRESULT OnMyTrayNotification(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnQuit();
	afx_msg void OnSelchangeInputList();
	afx_msg void OnSelchangeMapList();
	afx_msg void OnClickedInput1();
	afx_msg void OnClickedInput2();
	afx_msg LRESULT OnGetInput1(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnGetInput2(WPARAM wParam, LPARAM lParam);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnClickedSave();
	afx_msg void OnClickedAdd();
	afx_msg void OnClickedCancelListSelect();
	afx_msg void OnSelchangeRegList();

	void SaveKeyMappingsToJson();
	void LoadKeyMappingsFromJson(BOOL isStart);


//	BOOL StartKeyMapping();
//	BOOL StopKeyMapping();
//
//	afx_msg void OnSelchangeRunType();
//	afx_msg void OnClickedStart();
//	afx_msg void OnClickedStop();

	//afx_msg void OnRawInput(UINT nInputcode, HRAWINPUT hRawInput);
	void CNemoKeyMapperDlg::LoadRegistrySettings();
	void CNemoKeyMapperDlg::LoadListWithKeys(const BYTE* bytes, DWORD dwSize);
	void CNemoKeyMapperDlg::SaveMappingsToRegistry();
	std::vector<BYTE> CNemoKeyMapperDlg::DefineScancodeMap();
	afx_msg void OnClickedWriteRegist();
	afx_msg void OnNMClickSyslink1(NMHDR* pNMHDR, LRESULT* pResult);
};
