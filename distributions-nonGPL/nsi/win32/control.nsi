Name "EBooks FBReader for Windows XP"

OutFile "EBooksFBReaderSetup-@VERSION@.exe"

InstallDir "$PROGRAMFILES\EBooks FBReader"
InstallDirRegKey HKCU "Software\EBooks FBReader" ""

Page directory
Page components
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

Section "EBooks FBReader"
	SectionIn RO

	SetOutPath "$INSTDIR"

	WriteRegStr HKCU "Software\EBooks FBReader" "" $INSTDIR

	File /oname=EBooksFBReader.exe bin/FBReader
	File ../dlls/*.*
	File /r share
  Delete "$INSTDIR\libiconv-2.dll"
  Delete "$INSTDIR\share\FBReader\help\MiniHelp.fb2"
  Delete "$INSTDIR\share\FBReader\hyphenationPatterns.zip"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EBooks FBReader for Windows XP" "DisplayName" "EBooks FBReader for Windows XP"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EBooks FBReader for Windows XP" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EBooks FBReader for Windows XP" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EBooks FBReader for Windows XP" "NoRepair" 1
  WriteUninstaller "uninstall.exe"

  CreateDirectory "$SMPROGRAMS\EBooks FBReader for Windows XP"
  CreateShortCut "$SMPROGRAMS\EBooks FBReader for Windows XP\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
  CreateShortCut "$SMPROGRAMS\EBooks FBReader for Windows XP\EBooks FBReader.lnk" "$INSTDIR\EBooksFBReader.exe" "" "$INSTDIR\EBooksFBReader.exe" 0

	ReadRegStr $0 HKCU "Software\FBReader\options\Options" "BookPath"
	StrCmp $0 "" 0 +2
	WriteRegStr HKCU "Software\FBReader\options\Options" "BookPath" "C:\Books;$PROFILE\Books" 
	WriteRegStr HKCU "Software\FBReader\options\Help" "ShowIcon" "true" 
SectionEnd

Section "Create Shortcut on Desktop"
  CreateShortCut "$DESKTOP\EBooksFBReader.lnk" "$INSTDIR\EBooksFBReader.exe" "" "$INSTDIR\EBooksFBReader.exe" 0
SectionEnd

Section "Uninstall"
	RMDir /r "$INSTDIR\share"
	Delete "$INSTDIR\EBooksFBReader.exe"
	Delete "$INSTDIR\*.dll"
	Delete "$INSTDIR\uninstall.exe"
	RMDir "$INSTDIR"

	RMDir /r "$SMPROGRAMS\EBooks FBReader for Windows XP"
  Delete "$DESKTOP\EBooks FBReader.lnk"

  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\EBooks FBReader for Windows XP"
	DeleteRegKey /ifempty HKCU "Software\FBReader"
	DeleteRegKey /ifempty HKCU "Software\EBooks FBReader"

	MessageBox MB_YESNO "Delete EBooks FBReader configuration from registry?" IDNO Skip
		DeleteRegKey HKCU "Software\FBReader"
	Skip:
SectionEnd
