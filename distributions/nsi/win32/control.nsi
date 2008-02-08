Name "FBReader for Windows XP"

OutFile "FBReaderSetup-@VERSION@.exe"

InstallDir $PROGRAMFILES\FBReader
InstallDirRegKey HKCU "Software\FBReader" ""

Page directory
Page components
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "FBReader"
	SectionIn RO

	SetOutPath "$INSTDIR"

	WriteRegStr HKCU "Software\FBReader" "" $INSTDIR

	File /oname=FBReader.exe bin/FBReader
	File ../../libs/win32/*.*
	File /r share
  Delete "$INSTDIR\libiconv-2.dll"
  Delete "$INSTDIR\share\FBReader\help\MiniHelp.fb2"
  Delete "$INSTDIR\share\FBReader\hyphenationPatterns.zip"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "DisplayName" "FBReader for Windows XP"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  CreateDirectory "$SMPROGRAMS\FBReader for Windows XP"
  CreateShortCut "$SMPROGRAMS\FBReader for Windows XP\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\FBReader for Windows XP\FBReader.lnk" "$INSTDIR\FBReader.exe" "" "$INSTDIR\FBReader.exe" 0

	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "BookPath"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Options" "BookPath" "C:\Books;$PROFILE\Books" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "TouchScreenPresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "MousePresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "KeyboardPresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\Options" "KeyDelay" "0" 
	WriteRegStr HKCU "Software\FBReader\options\SmallScrolling" "ScrollingDelay" "true" 
	WriteRegStr HKCU "Software\FBReader\options\LargeScrolling" "ScrollingDelay" "true" 
	WriteRegStr HKCU "Software\FBReader\options\TapScrolling" "Enabled" "false" 
SectionEnd

Section "Create Shortcut on Desktop"
  CreateShortCut "$DESKTOP\FBReader.lnk" "$INSTDIR\FBReader.exe" "" "$INSTDIR\FBReader.exe" 0
SectionEnd

Section "Uninstall"
	RMDir /r "$INSTDIR\share"
	Delete "$INSTDIR\FBReader.exe"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\*.license"
	Delete "$INSTDIR\uninstall.exe"
	RMDir "$INSTDIR"

	RMDir /r "$SMPROGRAMS\FBReader for Windows XP"
  Delete "$DESKTOP\FBReader.lnk"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows XP"
	DeleteRegKey /ifempty HKCU "Software\FBReader"

	MessageBox MB_YESNO "Delete FBReader configuration from registry?" IDNO Skip
		DeleteRegKey HKCU "Software\FBReader"
	Skip:
SectionEnd
