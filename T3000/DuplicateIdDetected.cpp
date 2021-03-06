// DuplicateIdDetected.cpp : implementation file
//

#include "stdafx.h"
#include "T3000.h"
#include "DuplicateIdDetected.h"
#include "afxdialogex.h"
#include "globle_function.h"

// CDuplicateIdDetected dialog
DWORD WINAPI HanlePanelNumber(LPVOID lpvoid);
DWORD WINAPI DuplicatedProcess(LPVOID lPvoid);
CString temp_controller_ip;
int controller_port = 0;
bool first_need_changed = false;
bool second_need_changed = false;
CString cs_show_info;
int auto_close_seconds = 30;
int first_propose_id = 0;
int second_propose_id = 0;

vector <int> id_exsit_in_db;
IMPLEMENT_DYNAMIC(CDuplicateIdDetected, CDialogEx)


CDuplicateIdDetected::CDuplicateIdDetected(int temp,CWnd* pParent /*=NULL*/)
	: CDialogEx(CDuplicateIdDetected::IDD, pParent)
{
	duplicate_mode = temp;
}

CDuplicateIdDetected::CDuplicateIdDetected(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDuplicateIdDetected::IDD, pParent)
{

}

CDuplicateIdDetected::~CDuplicateIdDetected()
{
}

void CDuplicateIdDetected::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_DUPLICATE_TITLE, m_static_title);
	DDX_Control(pDX, IDC_STATIC_NETWORK_TITLE, m_network_title);
	DDX_Control(pDX, IDC_STATIC_DUP_NETWORK_ID, m_dup_network_id);
	DDX_Control(pDX, IDC_STATIC_DUP_NETWORK_CURRENT_ID, m_dup_current_id);
	DDX_Control(pDX, IDC_STATIC_DUP_NETWORK_PROPOSED_ID, m_dup_propsed_id);
	DDX_Control(pDX, IDC_STATIC_DUP_SERIAL_NUMBER, m_dup_serial_number);
	DDX_Control(pDX, IDC_STATIC_DUP_MODEL, m_dup_model);
	DDX_Control(pDX, IDC_STATIC_DUP_LABLE, m_dup_user_test);
	DDX_Control(pDX, IDC_STATIC_DUP_IN_DATABASE, m_dup_in_database);
	DDX_Control(pDX, IDC_STATIC_DUP_IN_DATABASE_INFO, m_dup_id_from_database);
	DDX_Control(pDX, IDC_STATIC_DUP_CURRENT_DEVICE_ID1, m_current_id_1);
	DDX_Control(pDX, IDC_STATIC_DUP_CURRENT_DEVICE_ID2, m_current_id_2);
	DDX_Control(pDX, IDC_EDIT_DEVICE_1, m_edit_propsed_id_1);
	DDX_Control(pDX, IDC_EDIT_DEVICE_2, m_edit_propsed_id_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_SERIAL_1, m_static_serial_1);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_SERIAL_2, m_static_serial_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_MODEL_1, m_device_model_1);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_MODEL_2, m_device_model_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_USERNAME_1, m_user_text_1);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_USERNAME_2, m_user_text_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_YES_NO_1, m_device_yes_no_1);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_YES_NO_2, m_device_yes_no_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_DATABASE_ID_1, m_id_from_db_1);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_DATABASE_ID_2, m_id_from_db_2);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_1, m_device1_title);
	DDX_Control(pDX, IDC_STATIC_DUP_DEVICE_2, m_device2_title);
	DDX_Control(pDX, IDC_STATIC_DUPLICATE_INFO, m_duplicate_ret_info);
}


BEGIN_MESSAGE_MAP(CDuplicateIdDetected, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DUPLICATE_DONE, &CDuplicateIdDetected::OnBnClickedButtonDuplicateDone)
	ON_BN_CLICKED(IDC_BUTTON_DUPLICATE_CANCEL, &CDuplicateIdDetected::OnBnClickedButtonDuplicateCancel)
	ON_WM_TIMER()
	ON_STN_CLICKED(IDC_STATIC_NETWORK_TITLE, &CDuplicateIdDetected::OnStnClickedStaticNetworkTitle)
END_MESSAGE_MAP()


// CDuplicateIdDetected message handlers
HANDLE DuplicatedThread = NULL;
HANDLE PanelNumberThread = NULL;

BOOL CDuplicateIdDetected::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	first_need_changed = false;
	second_need_changed = false;
	auto_close_seconds = 30;
	cs_show_info.Empty();
	// TODO:  Add extra initialization here
	Initial_static();
	SetTimer(1,500,NULL);
	SetTimer(2,1000,NULL);
	GetDlgItem(IDC_EDIT_DEVICE_2)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

DWORD WINAPI HanlePanelNumber(LPVOID lpvoid)
{
	CDuplicateIdDetected *mparent = (CDuplicateIdDetected *)lpvoid;

	if(first_need_changed)
	{
		 bool bret=Open_Socket2(device_id_data_1.ip_address,device_id_data_1.nport);
		 SetCommunicationType(1);
		 if(bret)
		 {
			 //first_propose_id
			 int write_ret = 0 ;
			write_ret =  write_one(255,36,first_propose_id);
			if(write_ret <0)
			{
				cs_show_info.Format(_T("try to change the first panel id failed..."));
				Sleep(1000);
				goto ret_label_panel_ret;
			}
		 }
	}

	if(second_need_changed)
	{
		bool bret=Open_Socket2(device_id_data_2.ip_address,device_id_data_2.nport);
		SetCommunicationType(1);
		if(bret)
		{
			//first_propose_id
			int write_ret = 0 ;
			write_ret =  write_one(255,36,second_propose_id);
			if(write_ret <0)
			{
				cs_show_info.Format(_T("try to change %s panel id failed..."),device_id_data_2.ip_address);
				Sleep(1000);
				goto ret_label_panel_ret;
			}
		}
	}
	cs_show_info.Format(_T("Operation Done!"));
	Sleep(2000);
	PostMessage(mparent->m_hWnd,WM_CLOSE,NULL,NULL);
	PanelNumberThread = NULL;
	return 1;

ret_label_panel_ret:
	mparent->GetDlgItem(IDC_BUTTON_DUPLICATE_DONE)->EnableWindow(TRUE);
	mparent->GetDlgItem(IDC_BUTTON_DUPLICATE_CANCEL)->EnableWindow(TRUE);

	PanelNumberThread = NULL;
	return TRUE;
}

DWORD WINAPI DuplicatedProcess(LPVOID lPvoid)
{
	CDuplicateIdDetected * mparent = (CDuplicateIdDetected *)lPvoid;
	
	 bool b=Open_Socket2(temp_controller_ip,controller_port);
	 SetCommunicationType(1);
	 Sleep(2000);
	 if(b)
	 {
		 cs_show_info.Format(_T("Ready to changing the ID . Please wait."));
		 unsigned short temp_buffer[10];
		
		 if(first_need_changed)
		 {

			 temp_buffer[0] = 0;
			 temp_buffer[1] =  (unsigned short)(device_id_data_1.nSerial & 0x000000ff);
			 temp_buffer[2] =  (unsigned short)((device_id_data_1.nSerial & 0x0000ff00)>>8);
			 temp_buffer[3] =   (unsigned short)((device_id_data_1.nSerial & 0x00ff0000)>>16);
			 temp_buffer[4] =  (unsigned short)((device_id_data_1.nSerial & 0xff000000)>>24);
			 temp_buffer[5] =  device_id_data_1.modbusID;
			 temp_buffer[6] = first_propose_id;
			 int ret=Write_Multi_org_short(255,(unsigned short *)temp_buffer,CONTROLLER_DUPLICATE_ID,7,10);
			 if(ret < 0)
			 {
				cs_show_info.Format(_T("try to change the first sub id failed..."));
				Sleep(1000);
				goto ret_label_write_ret;
			 }

		 }

		 if(second_need_changed)
		 {
			 temp_buffer[0] = 0;
			 temp_buffer[1] =  (unsigned short)(device_id_data_2.nSerial & 0x000000ff);
			 temp_buffer[2] =  (unsigned short)((device_id_data_2.nSerial & 0x0000ff00)>>8);
			 temp_buffer[3] =   (unsigned short)((device_id_data_2.nSerial & 0x00ff0000)>>16);
			 temp_buffer[4] =  (unsigned short)((device_id_data_2.nSerial & 0xff000000)>>24);
			 temp_buffer[5] =  device_id_data_2.modbusID;
			 temp_buffer[6] = second_propose_id;
			 int ret=Write_Multi_org_short(255,(unsigned short *)temp_buffer,CONTROLLER_DUPLICATE_ID,7,10);
			 if(ret < 0)
			 {
				cs_show_info.Format(_T("try to change the second sub id failed..."));
				Sleep(1000);
				goto ret_label_write_ret;
			 }

		 }



	 }
	 else
	 {
		 cs_show_info.Format(_T("Connect to panel failed ,please try again."));
	 }

	 int ret_ret = 0;
	 int recheck_time = 0;
	 do 
	 {
		ret_ret  = read_one(255,CONTROLLER_DUPLICATE_ID);
		if(ret_ret == 3)
		{
			cs_show_info.Format(_T("try to change the sub id failed..."));
			break;
		}
		cs_show_info.Format(_T("Waiting... (%d)"),10 - recheck_time);
		recheck_time ++ ;
		Sleep(1000);
	 } while ((ret_ret != 2) && (recheck_time < 10));
	 
	 if(ret_ret != 2)
	 {
		 cs_show_info.Format(_T("Operation failed..."));
	 }
	 else
	 {
		  cs_show_info.Format(_T("Operation Done!"));
		  Sleep(2000);
		  PostMessage(mparent->m_hWnd,WM_CLOSE,NULL,NULL);
		  DuplicatedThread = NULL;
		  return 1;
	 }

ret_label_write_ret:
	mparent->GetDlgItem(IDC_BUTTON_DUPLICATE_DONE)->EnableWindow(TRUE);
	mparent->GetDlgItem(IDC_BUTTON_DUPLICATE_CANCEL)->EnableWindow(TRUE);

	
	DuplicatedThread = NULL;
	return 1;
}

BOOL CDuplicateIdDetected::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class

	return CDialogEx::PreTranslateMessage(pMsg);
}


void CDuplicateIdDetected::Initial_static()
{
	id_exsit_in_db.clear();
	if(duplicate_mode == 0)
		m_static_title.SetWindowTextW(_T("Each device on the RS485 network needs a unique network ID or communications will fail.\r\nYou can change the ID using a plug and play feature built into the device.\r\nWould you like to change the address to the new ID?"));
	else if(duplicate_mode == 1)
		m_static_title.SetWindowTextW(_T("Each device on the same network needs a unique panel number or communications will mess.\r\nWould you like to change the panel number to the new value?"));
	m_static_title.textColor(RGB(0,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_static_title.setFont(28,16,NULL,_T("Arial"));

	temp_controller_ip = device_id_data_1.ip_address;
	controller_port = device_id_data_1.nport;
	//device_id_data_1
	CString temp_subnet;
	if(duplicate_mode == 0)
		temp_subnet.Format(_T("IP:%s  Panel %d:RS485 'MAIN' subnet"),temp_controller_ip,device_id_data_1.panal_number);
	else if(duplicate_mode == 1)
	{
		temp_subnet.Format(_T("%s - %s panel number conflict"),device_id_data_1.ip_address,device_id_data_2.ip_address);
	}


	CString temp_serial_1;CString temp_serial_2;
	CString temp_current_id_1;CString temp_current_id_2;
	CString temp_user_text_1;CString temp_user_text_2;
	CString temp_product_id_1;CString temp_product_id_2;

	temp_serial_1.Format(_T("%u"),device_id_data_1.nSerial);
	temp_serial_2.Format(_T("%u"),device_id_data_2.nSerial);
	if(duplicate_mode == 0)
	{
		temp_current_id_1.Format(_T("%u"),device_id_data_1.modbusID);
		temp_current_id_2.Format(_T("%u"),device_id_data_2.modbusID);
	}
	else if(duplicate_mode == 1)
	{
		temp_current_id_1.Format(_T("%u"),device_id_data_1.panal_number);
		temp_current_id_2.Format(_T("%u"),device_id_data_2.panal_number);
	}


	temp_user_text_1 = device_id_data_1.show_label_name;
	temp_user_text_2 = device_id_data_2.show_label_name;

	temp_product_id_1 = GetProductName(device_id_data_1.product_id);
	temp_product_id_2 = GetProductName(device_id_data_2.product_id);

	m_network_title.SetWindowTextW(temp_subnet);
	m_network_title.textColor(RGB(0,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_network_title.setFont(24,16,NULL,_T("Arial"));


	
	m_duplicate_ret_info.textColor(RGB(0,0,255));
	//m_duplicate_ret_info.bkColor(RGB(0,255,255));
	m_duplicate_ret_info.setFont(24,16,NULL,_T("Arial"));


	m_dup_network_id.textColor(RGB(0,0,0));
	//m_static.bkColor(RGB(0,255,255));
	m_dup_network_id.setFont(24,16,NULL,_T("Arial"));

	if(duplicate_mode == 0)
	{
		m_dup_current_id.SetWindowTextW(_T("Current ID :"));
		m_dup_propsed_id.SetWindowTextW(_T("Proposed ID :"));
	}
	else if(duplicate_mode == 1)
	{
		m_dup_current_id.SetWindowTextW(_T("Current Panel:"));
		m_dup_propsed_id.SetWindowTextW(_T("Proposed Panel:"));
	}
	m_dup_current_id.textColor(RGB(0,0,0));
	//m_dup_current_id.bkColor(RGB(0,255,255));
	m_dup_current_id.setFont(24,16,NULL,_T("Arial"));

	m_dup_propsed_id.textColor(RGB(0,0,0));
	//m_dup_propsed_id.bkColor(RGB(0,255,255));
	m_dup_propsed_id.setFont(24,16,NULL,_T("Arial"));


	m_dup_serial_number.textColor(RGB(0,0,0));
	//m_dup_serial_number.bkColor(RGB(0,255,255));
	m_dup_serial_number.setFont(24,16,NULL,_T("Arial"));

	m_dup_model.textColor(RGB(0,0,0));
	//m_dup_model.bkColor(RGB(0,255,255));
	m_dup_model.setFont(24,16,NULL,_T("Arial"));

	m_dup_user_test.textColor(RGB(0,0,0));
	//m_dup_user_test.bkColor(RGB(0,255,255));
	m_dup_user_test.setFont(24,16,NULL,_T("Arial"));

	m_dup_in_database.textColor(RGB(0,0,0));
	//m_dup_in_database.bkColor(RGB(0,255,255));
	m_dup_in_database.setFont(24,16,NULL,_T("Arial"));

	m_dup_id_from_database.textColor(RGB(0,0,0));
	//m_dup_id_from_database.bkColor(RGB(0,255,255));
	m_dup_id_from_database.setFont(24,16,NULL,_T("Arial"));


	m_device1_title.textColor(RGB(0,0,0));
	//m_device1_title.bkColor(RGB(0,255,255));
	m_device1_title.setFont(24,16,NULL,_T("Arial"));

	m_device2_title.textColor(RGB(0,0,0));
	//m_device2_title.bkColor(RGB(0,255,255));
	m_device2_title.setFont(24,16,NULL,_T("Arial"));


	m_current_id_1.SetWindowTextW(temp_current_id_1);
	m_current_id_1.textColor(RGB(0,0,0));
	//m_current_id_1.bkColor(RGB(0,255,255));
	m_current_id_1.setFont(24,16,NULL,_T("Arial"));

	m_current_id_2.SetWindowTextW(temp_current_id_2);
	m_current_id_2.textColor(RGB(0,0,0));
	//m_current_id_2.bkColor(RGB(0,255,255));
	m_current_id_2.setFont(24,16,NULL,_T("Arial"));

	m_edit_propsed_id_1.SetWindowTextW(temp_current_id_1);
	m_edit_propsed_id_1.textColor(RGB(0,0,0));
	m_edit_propsed_id_1.bkColor(RGB(255,255,255));
	m_edit_propsed_id_1.setFont(24,16,NULL,_T("Arial"));

	m_edit_propsed_id_2.textColor(RGB(0,0,0));
	m_edit_propsed_id_2.bkColor(RGB(255,255,255));
	m_edit_propsed_id_2.setFont(24,16,NULL,_T("Arial"));

	m_static_serial_1.SetWindowTextW(temp_serial_1);
	m_static_serial_1.textColor(RGB(0,0,0));
	//m_static_serial_1.bkColor(RGB(0,255,255));
	m_static_serial_1.setFont(24,16,NULL,_T("Arial"));

	m_static_serial_2.SetWindowTextW(temp_serial_2);
	m_static_serial_2.textColor(RGB(0,0,0));
	//m_static_serial_2.bkColor(RGB(0,255,255));
	m_static_serial_2.setFont(24,16,NULL,_T("Arial"));
	
	m_device_model_1.SetWindowTextW(temp_product_id_1);
	m_device_model_1.textColor(RGB(0,0,0));
	//m_device_model_1.bkColor(RGB(0,255,255));
	m_device_model_1.setFont(24,16,NULL,_T("Arial"));

	m_device_model_2.SetWindowTextW(temp_product_id_2);
	m_device_model_2.textColor(RGB(0,0,0));
	//m_device_model_2.bkColor(RGB(0,255,255));
	m_device_model_2.setFont(24,16,NULL,_T("Arial"));

	m_user_text_1.SetWindowTextW(temp_user_text_1);
	m_user_text_1.textColor(RGB(0,0,0));
	//m_user_text_1.bkColor(RGB(0,255,255));
	m_user_text_1.setFont(24,16,NULL,_T("Arial"));

	m_user_text_2.SetWindowTextW(temp_user_text_2);
	m_user_text_2.textColor(RGB(0,0,0));
	//m_user_text_2.bkColor(RGB(0,255,255));
	m_user_text_2.setFont(24,16,NULL,_T("Arial"));

	m_device_yes_no_1.textColor(RGB(0,0,0));
	//m_device_yes_no_1.bkColor(RGB(0,255,255));
	m_device_yes_no_1.setFont(24,16,NULL,_T("Arial"));

	m_device_yes_no_2.textColor(RGB(0,0,0));
	//m_device_yes_no_2.bkColor(RGB(0,255,255));
	m_device_yes_no_2.setFont(24,16,NULL,_T("Arial"));

	m_id_from_db_1.textColor(RGB(0,0,0));
	//m_id_from_db_1.bkColor(RGB(0,255,255));
	m_id_from_db_1.setFont(24,16,NULL,_T("Arial"));

	m_id_from_db_2.textColor(RGB(0,0,0));
	//m_id_from_db_2.bkColor(RGB(0,255,255));
	m_id_from_db_2.setFont(24,16,NULL,_T("Arial"));


	if((device_id_data_1.nSerial != 0) && (device_id_data_2.nSerial != 0))
	{
		CString strsql;
		 

        m_SqliteDBBuilding.open((UTF8MBSTR)g_strCurBuildingDatabasefilePath);



		strsql.Format(_T("select * from ALL_NODE where Serial_ID = '%s'"),temp_serial_1);
		 m_q = m_SqliteDBBuilding.execQuery((UTF8MBSTR)strsql);
         m_table = m_SqliteDBBuilding.getTable((UTF8MBSTR)strsql);

		int temp_count =  m_table.numRows();
		CString temp_pid;
		  _variant_t temp_variant;
		if(temp_count > 0)
		{
			while(!m_q.eof())//次Com port 下面的所有 设备;
			{


				 
                temp_pid = m_q.getValuebyName(L"Product_ID");
				m_device_yes_no_1.SetWindowTextW(_T("Yes"));
				m_id_from_db_1.SetWindowTextW(temp_pid);

				break;
			}
		}
		else
		{
			m_device_yes_no_1.SetWindowTextW(_T("No"));
			m_id_from_db_1.SetWindowTextW(_T("-"));
		}
        

		strsql.Format(_T("select * from ALL_NODE where Serial_ID = '%s'"),temp_serial_2);
        m_q = m_SqliteDBBuilding.execQuery((UTF8MBSTR)strsql);
        m_table = m_SqliteDBBuilding.getTable((UTF8MBSTR)strsql);
		temp_count = m_table.numRows();

		if(temp_count > 0)
		{
			while(!m_q.eof())//次Com port 下面的所有 设备;
			{
				 
                    temp_pid = m_q.getValuebyName(L"Product_ID");
				m_device_yes_no_2.SetWindowTextW(_T("Yes"));
				m_id_from_db_2.SetWindowTextW(temp_pid);

				break;
			}
		}
		else
		{
			m_device_yes_no_2.SetWindowTextW(_T("No"));
			m_id_from_db_2.SetWindowTextW(_T("-"));
		}
	 

		CString temp_parent_serial;
		temp_parent_serial.Format(_T("%u"),device_id_data_1.parent_serial_number);

		strsql.Format(_T("select * from ALL_NODE where Parent_SerialNum = '%s' order by Product_ID ASC"),temp_parent_serial);
        m_q = m_SqliteDBBuilding.execQuery((UTF8MBSTR)strsql);
        m_table = m_SqliteDBBuilding.getTable((UTF8MBSTR)strsql);
		 
		temp_count = m_table.numRows();

		if(temp_count > 0)
		{
			while(!m_q.eof())//次Com port 下面的所有 设备;
			{
				 
                    temp_pid = m_q.getValuebyName(L"Product_ID");
				int temp_db_id = _wtoi(temp_pid);
				id_exsit_in_db.push_back(temp_db_id);
				m_q.nextRow();
			}
		}

		m_SqliteDBBuilding.closedb();
	}

	int db_id_aviliable = 0;
	for (int i=10;i<250;i++)
	{
		int find_id = false;
		for (int j=0;j<id_exsit_in_db.size();j++)
		{
			if(i == id_exsit_in_db.at(j))
			{
				find_id = true;
				break;
			}
		}

		if(find_id == false)
		{
			db_id_aviliable = i;
			break;
		}
	}

	CString temp_10;

	temp_10.Format(_T("%u"),db_id_aviliable);
	m_edit_propsed_id_2.SetWindowTextW(temp_10);
	m_edit_propsed_id_2.textColor(RGB(255,0,0));
}


void CDuplicateIdDetected::OnBnClickedButtonDuplicateDone()
{
	// TODO: Add your control notification handler code here
	KillTimer(2);
	GetDlgItem(IDC_BUTTON_DUPLICATE_DONE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_DUPLICATE_CANCEL)->EnableWindow(FALSE);
	CString original_id;
	CString TEMP_ID_1;CString TEMP_ID_2;
	GetDlgItem(IDC_STATIC_DUP_CURRENT_DEVICE_ID1)->GetWindowTextW(original_id);
	GetDlgItem(IDC_EDIT_DEVICE_1)->GetWindowTextW(TEMP_ID_1);
	GetDlgItem(IDC_EDIT_DEVICE_2)->GetWindowTextW(TEMP_ID_2);

	original_id.Trim();
	TEMP_ID_1.Trim();
	TEMP_ID_2.Trim();

	first_propose_id = _wtoi(TEMP_ID_1);
	second_propose_id = _wtoi(TEMP_ID_2);


	

	if(original_id.CompareNoCase(TEMP_ID_1) != 0)
	{
		first_need_changed = true;
		if(duplicate_mode == 1)
		{
			vector <int>::iterator itter;
			for (itter = exsit_panel_number.begin();itter != exsit_panel_number.end();++itter)
			{
				if(*itter == first_propose_id)
				{
					MessageBox(_T("Panel number already exsit in the network,please change a new one."));
					GetDlgItem(IDC_BUTTON_DUPLICATE_DONE)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_DUPLICATE_CANCEL)->EnableWindow(TRUE);
					return;
				}
			}
		}

	}

	if(original_id.CompareNoCase(TEMP_ID_2) != 0)
	{
		second_need_changed = true;
		if(duplicate_mode == 1)
		{
			vector <int>::iterator itter;
			for (itter = exsit_panel_number.begin();itter != exsit_panel_number.end();++itter)
			{
				if(*itter == second_propose_id)
				{
					MessageBox(_T("Panel number already exsit in the network,please change a new one."));
					GetDlgItem(IDC_BUTTON_DUPLICATE_DONE)->EnableWindow(TRUE);
					GetDlgItem(IDC_BUTTON_DUPLICATE_CANCEL)->EnableWindow(TRUE);
					return;
				}
			}
		}
	}



	if(duplicate_mode == 0)
	{
		if(DuplicatedThread == NULL)
			DuplicatedThread = CreateThread(NULL,NULL,DuplicatedProcess,this,NULL, NULL);
	}
	else if(duplicate_mode == 1)
	{
		if(PanelNumberThread == NULL)
			PanelNumberThread = CreateThread(NULL,NULL,HanlePanelNumber,this,NULL, NULL);
	}

}


void CDuplicateIdDetected::OnBnClickedButtonDuplicateCancel()
{
	// TODO: Add your control notification handler code here
	KillTimer(2);
	edit_confilct_mode = false;
	PostMessage(WM_CLOSE,NULL,NULL);
}


void CDuplicateIdDetected::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case 1:
		m_duplicate_ret_info.SetWindowTextW(cs_show_info);
		break;
	case 2:
		{
			if(auto_close_seconds == 0)
			{
				KillTimer(2);
				PostMessage(WM_CLOSE,NULL,NULL);
			}
			else
			{
				auto_close_seconds --;
				cs_show_info.Format(_T("Close after (%d)"),auto_close_seconds);
			}
			
		}
		break;
	default:
		break;
	}
	
	CDialogEx::OnTimer(nIDEvent);
}


void CDuplicateIdDetected::OnStnClickedStaticNetworkTitle()
{
	// TODO: Add your control notification handler code here
}
