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

ResourceCount=2
Resource1=IDR_MAINFRAME
Resource2=IDD_NATIVE_DIALOG

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

[DLG:IDD_NATIVE_DIALOG]
Type=1
Class=CNativeDlg
ControlCount=5
Control1=IDC_BUTTON1,button,1342242816
Control2=IDC_EDIT1,edit,1350631552
Control3=IDC_STATIC,static,1342308352
Control4=IDC_EDIT2,edit,1350631552
Control5=IDC_STATIC,static,1342308352

