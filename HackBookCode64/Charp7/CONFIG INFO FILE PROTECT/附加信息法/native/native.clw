; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CNativeDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "native.h"

ClassCount=3
Class1=CNativeApp
Class2=CNativeDlg
Class3=CAboutDlg

ResourceCount=3
Resource1=IDD_ABOUTBOX
Resource2=IDR_MAINFRAME
Resource3=IDD_NATIVE_DIALOG

[CLS:CNativeApp]
Type=0
HeaderFile=native.h
ImplementationFile=native.cpp
Filter=N

[CLS:CNativeDlg]
Type=0
HeaderFile=nativeDlg.h
ImplementationFile=nativeDlg.cpp
Filter=D
BaseClass=CDialog
VirtualFilter=dWC
LastObject=IDC_BUTTON1

[CLS:CAboutDlg]
Type=0
HeaderFile=nativeDlg.h
ImplementationFile=nativeDlg.cpp
Filter=D

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_NATIVE_DIALOG]
Type=1
Class=CNativeDlg
ControlCount=5
Control1=IDC_EDIT1,edit,1350631552
Control2=IDC_EDIT2,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_BUTTON1,button,1342242816

