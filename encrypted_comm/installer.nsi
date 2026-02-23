; NSIS Installer Script for Encrypted Communications
; To use this, install NSIS from https://nsis.sourceforge.io/

!include "MUI2.nsh"
!include "x64.nsh"

; Configuration
Name "Encrypted Communications"
OutFile "EncryptedComm_Installer.exe"
InstallDir "$PROGRAMFILES\EncryptedComm"
InstallDirRegKey HKCU "Software\EncryptedComm" "InstallDir"

; MUI Settings
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; Installer sections
Section "Install"
    SetOutPath "$INSTDIR"
    
    ; Copy executable
    File "build\encrypted_comm.exe"
    
    ; Copy documentation
    File "README.md"
    
    ; Create shortcuts
    CreateDirectory "$SMPROGRAMS\EncryptedComm"
    CreateShortCut "$SMPROGRAMS\EncryptedComm\Encrypted Communications.lnk" "$INSTDIR\encrypted_comm.exe"
    CreateShortCut "$SMPROGRAMS\EncryptedComm\Uninstall.lnk" "$INSTDIR\uninstall.exe"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\uninstall.exe"
    
    ; Registry entry
    WriteRegStr HKCU "Software\EncryptedComm" "InstallDir" "$INSTDIR"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\EncryptedComm" "DisplayName" "Encrypted Communications"
    WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\EncryptedComm" "UninstallString" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Uninstall"
    ; Remove files
    Delete "$INSTDIR\encrypted_comm.exe"
    Delete "$INSTDIR\README.md"
    Delete "$INSTDIR\uninstall.exe"
    
    ; Remove directories
    RMDir "$INSTDIR"
    RMDir "$SMPROGRAMS\EncryptedComm"
    
    ; Remove registry entries
    DeleteRegKey HKCU "Software\EncryptedComm"
    DeleteRegKey HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\EncryptedComm"
SectionEnd
