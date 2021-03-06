// AsyncSelectDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "AsyncSelect.h"
#include "AsyncSelectDlg.h"
#include ".\asyncselectdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원

// 구현
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAsyncSelectDlg 대화 상자



CAsyncSelectDlg::CAsyncSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAsyncSelectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAsyncSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OUTPUT, m_ctOutput);
	
}

BEGIN_MESSAGE_MAP(CAsyncSelectDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE( WM_SOCKETMSG , OnSocketMsg )
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_STARTSERVER, OnBnClickedStartserver)
END_MESSAGE_MAP()

// CAsyncSelectDlg 메시지 처리기
BOOL CAsyncSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	
	return TRUE;  // 컨트롤에 대한 포커스를 설정하지 않을 경우 TRUE를 반환합니다.
}

void CAsyncSelectDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CAsyncSelectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

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
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CAsyncSelectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CAsyncSelectDlg::OnSocketMsg( WPARAM wParam , LPARAM lParam )
{

	SOCKET sock = (SOCKET)wParam;

	//에러체크
	int nError = WSAGETSELECTERROR( lParam );
	if( 0 != nError )
	{
		OutputMsg( "[에러] WSAGETSELECTERROR : %d ", nError );
		m_AsyncSocket.CloseSocket( sock );
		return false;
	}
	
	//이벤트 체크
	int nEvent = WSAGETSELECTEVENT( lParam );
	switch( nEvent )
	{
	case FD_READ:
		{
			int nRecvLen = recv( sock , m_szSocketBuf , MAX_SOCKBUF , 0 );
			if( 0 == nRecvLen )
			{
				OutputMsg("클라이언트와 연결이 종료 되었습니다.");
				m_AsyncSocket.CloseSocket( sock );
				return false;
			}
			else if( -1 == nRecvLen )
			{
				OutputMsg("[에러] recv 실패 : %d " , WSAGetLastError() );
				m_AsyncSocket.CloseSocket( sock );
			}
			m_szSocketBuf[ nRecvLen ] = NULL;
			OutputMsg( "socket[%d] , 메세지 수신 : %d bytes , 내용 : %s" 
				, sock , nRecvLen , m_szSocketBuf );
			
			int nSendLen = send( sock , m_szSocketBuf , nRecvLen , 0 );
			if( -1 == nSendLen )
			{
				OutputMsg("[에러] send 실패 : %d " , WSAGetLastError() );
				m_AsyncSocket.CloseSocket( sock );
				return false;
			}
			OutputMsg( "socket[%d] , 메세지 송신 : %d bytes , 내용 : %s" 
				, sock , nSendLen , m_szSocketBuf );
			break;
		}
	case FD_ACCEPT:
		{
			//접속된 클라이언트 주소 정보를 저장할 구조체
			SOCKADDR_IN		stClientAddr;
			int nAddrLen = sizeof( SOCKADDR_IN );

			SOCKET sockClient = accept( sock , (SOCKADDR*)&stClientAddr , &nAddrLen );
			if( INVALID_SOCKET == sockClient )
			{
				OutputMsg( "[에러] accept()함수 실패 : %d "
					, WSAGetLastError() );
				return false;
			}
			//소켓과 네트워크 이벤트를 등록한다.
			int nRet = WSAAsyncSelect( sockClient , m_hWnd , WM_SOCKETMSG , FD_READ | FD_CLOSE );
			if( SOCKET_ERROR == nRet )
			{
				OutputMsg("[에러] WSAAsyncSelect()함수 실패 : %d"
					, WSAGetLastError() );
				return false;
			}
			OutputMsg("클라이언트 접속 : IP(%s) SOCKET(%d)" 
				, inet_ntoa( stClientAddr.sin_addr ) , sockClient );

		}
		break;
	case FD_CLOSE:
		{
			OutputMsg("클라이언트 접속 종료 : SOCKET(%d)", sock );
			m_AsyncSocket.CloseSocket( sock );
		}
		break;
	}
	return true;

}
//출력 메세지
void CAsyncSelectDlg::OutputMsg( char *szOutputString , ... )
{
	char szOutStr[ 1024 ];
	va_list	argptr; 
    va_start( argptr, szOutputString );
	vsprintf( szOutStr , szOutputString, argptr );
	va_end( argptr );
	m_ctOutput.SetCurSel( m_ctOutput.AddString( szOutStr ) );
}

void CAsyncSelectDlg::OnBnClickedStartserver()
{
	//현재 다이얼로그 포인터를 넘겨준다.
	//소켓 메세지를 윈도우에 뿌리기 위해
	m_AsyncSocket.SetMainDlg( this );
	//소켓을 초기화
	m_AsyncSocket.InitSocket( m_hWnd );
	//소켓과 서버 주소를 연결하고 등록 시킨다.
	m_AsyncSocket.BindandListen( 8000 );
	//네트워크 이벤트와 윈도우를 등록 시키고 서버 시작
	m_AsyncSocket.StartServer();
}
