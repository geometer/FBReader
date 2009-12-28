Name "FBReader for Windows"

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

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows" "DisplayName" "FBReader for Windows"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  CreateDirectory "$SMPROGRAMS\FBReader for Windows"
  CreateShortCut "$SMPROGRAMS\FBReader for Windows\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\FBReader for Windows\FBReader.lnk" "$INSTDIR\FBReader.exe" "" "$INSTDIR\FBReader.exe" 0

	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "Base:fontFamily"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Style" "Base:fontFamily" "Georgia" 
	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "Base:fontSize"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Style" "Base:fontSize" "20" 
	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "BookPath"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Options" "BookPath" "C:\Books;$PROFILE\Books" 
	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "DownloadDirectory"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Options" "DownloadDirectory" "$PROFILE\Books" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "TouchScreenPresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "MousePresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\PlatformOptions" "KeyboardPresented" "true" 
	WriteRegStr HKCU "Software\FBReader\options\Options" "LeftMargin" "50" 
	WriteRegStr HKCU "Software\FBReader\options\Options" "RightMargin" "50" 
	WriteRegStr HKCU "Software\FBReader\options\Options" "KeyDelay" "0" 
	WriteRegStr HKCU "Software\FBReader\options\Scrollings" "Delay" "0" 
	WriteRegStr HKCU "Software\FBReader\options\TapScrolling" "Enabled" "true" 
SectionEnd

Section "Create Shortcut on Desktop"
  CreateShortCut "$DESKTOP\FBReader.lnk" "$INSTDIR\FBReader.exe" "" "$INSTDIR\FBReader.exe" 0
SectionEnd

Section "Uninstall"
	ClearErrors
	Delete "$INSTDIR\FBReader.exe"
	IfErrors 0 ContinueUninstallation
		MessageBox MB_OK "Cannot uninstall FBReader while the program is running.$\nPlease quit FBReader and try again."
		Quit
	ContinueUninstallation:
	RMDir /r "$INSTDIR\share"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\*.license"
	Delete "$INSTDIR\uninstall.exe"
	RMDir "$INSTDIR"

	RMDir /r "$SMPROGRAMS\FBReader for Windows"
  Delete "$DESKTOP\FBReader.lnk"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\FBReader for Windows"
	DeleteRegKey /ifempty HKCU "Software\FBReader"

	MessageBox MB_YESNO "Remove FBReader configuration from registry?" IDNO SkipConfigDeletion
		DeleteRegKey HKCU "Software\FBReader"
	SkipConfigDeletion:

	MessageBox MB_YESNO "Remove FBReader library information?" IDNO SkipLibraryDeletion
	  RMDir /r "$PROFILE\.FBReader"
	SkipLibraryDeletion:
SectionEnd
