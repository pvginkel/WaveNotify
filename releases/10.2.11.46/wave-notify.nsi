;WaveNotify installation script

;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Google Wave Notifier"
  OutFile "wave-notify-10.2.6.45.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\WaveNotify"

  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\WaveNotify" "InstallDir"

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user
  
;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !define MUI_FINISHPAGE_RUN "$INSTDIR\wave-notify.exe"

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "lgpl-3.0.txt"
  
  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\WaveNotify" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Google Wave Notifier"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  !insertmacro MUI_PAGE_INSTFILES
  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Google Wave Notifier" SecDummy

  DetailPrint "Stopping Google Wave Notifier"
  
  ; First try the new version
  
  !include WinMessages.nsh
  
  FindWindow $0 'GoogleWaveNotifier'
  
  IntCmp $0 0 oldVersion
  
  fct::fct /NOUNLOAD /WC 'GoogleWaveNotifier' /ASYNC
  fct::wait
  
  Pop $0
  
  Goto install
  
oldVersion:
  
  ; Then try the old style; the C# application

  FindProcDLL::FindProc "wavenotify.exe"
  IntCmp $R0 0 install
  
  KillProcDLL::KillProc "wavenotify.exe"
  Sleep 2000

install:

  SetOutPath "$INSTDIR"
  
  File /r /x .svn "distrib\*.*"
  
  ;Store installation folder
  WriteRegStr HKCU "Software\WaveNotify" "InstallDir" $INSTDIR

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"

    ;Create shortcuts
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Google Wave Notifier.lnk" "$INSTDIR\wave-notify.exe"
    CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

  CreateShortcut "$SMSTARTUP\Google Wave Notifier.lnk" "$INSTDIR\wave-notify.exe"
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  DetailPrint "Stopping Google Wave Notifier"
  
  ; First try the new version
  
  !include WinMessages.nsh
  
  FindWindow $0 'GoogleWaveNotifier'
  
  IntCmp $0 0 oldVersion
  
  fct::fct /NOUNLOAD /WC 'GoogleWaveNotifier' /ASYNC
  fct::wait
  
  Pop $0
  
  Goto remove
  
oldVersion:
  
  ; Then try the old style; the C# application

  FindProcDLL::FindProc "wavenotify.exe"
  IntCmp $R0 0 remove
  
  KillProcDLL::KillProc "wavenotify.exe"
  Sleep 2000

remove:

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMSTARTUP\Google Wave Notifier.lnk"
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"
  RMDir /r "$INSTDIR"

SectionEnd
