#!include "MUI.nsh"

Name "FBReader for Windows XP"

OutFile "FBReaderSetup-pre0.8.0.exe"

InstallDir $PROGRAMFILES\FBReader
InstallDirRegKey HKCU "Software\FBReader" ""

Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "FBReader"
	SectionIn RO

	SetOutPath "$INSTDIR"

	WriteRegStr HKCU "Software\FBReader" "" $INSTDIR

	File /oname=FBReader.exe bin/FBReader
	File ../../dlls/*.dll
	File /r share

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "DisplayName" "FBReader for Windows XP"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  CreateDirectory "$SMPROGRAMS\FBReader for Windows XP"
  CreateShortCut "$SMPROGRAMS\FBReader for Windows XP\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\FBReader for Windows XP\FBReader.lnk" "$INSTDIR\FBReader.exe" "" "$INSTDIR\FBReader.exe" 0
SectionEnd

Section "Uninstall"
	RMDir /r "$INSTDIR\share"
	Delete "$INSTDIR\FBReader.exe"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\uninstall.exe"
	RMDir "$INSTDIR"

	RMDir /r "$SMPROGRAMS\FBReader for Windows XP"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP"
	DeleteRegKey /ifempty HKCU "Software\FBReader"

	MessageBox MB_YESNO "Delete FBReader configuration from registry?" IDNO Skip
		DeleteRegKey HKCU "Software\FBReader"
	Skip:
SectionEnd
