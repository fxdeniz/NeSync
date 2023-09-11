############################################################################################
#      NSIS Installation Script created by NSIS Quick Setup Script Generator v1.09.18
#               Entirely Edited with NullSoft Scriptable Installation System                
#              by Vlasis K. Barkas aka Red Wine red_wine@freemail.gr Sep 2006               
############################################################################################

Unicode True

!define BIN_SOURCE_DIR "YOUR_OUTPUT_DIR_HERE"

!define APP_NAME "NeSync"
!define COMP_NAME "Deniz Yilmazok (github.com/fxdeniz)"
!define WEB_SITE "github.com/fxdeniz/NeSync"
!define VERSION "1.9.0.0"
!define COPYRIGHT "2023 - Deniz Yilmazok, GPLv3"
!define DESCRIPTION "NeSync Installer"
!define INSTALLER_NAME "${BIN_SOURCE_DIR}\nesync_${VERSION}_win64_setup.exe"
!define MAIN_APP_EXE "NeSync.exe"
!define INSTALL_TYPE "SetShellVarContext current"
!define REG_ROOT "HKCU"
!define REG_APP_PATH "Software\Microsoft\Windows\CurrentVersion\App Paths\${MAIN_APP_EXE}"
!define UNINSTALL_PATH "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP_NAME}"

!define REG_START_MENU "Start Menu Folder"
!define LICENSE_TXT "${BIN_SOURCE_DIR}\license\gpl-3.0.rtf"


var SM_Folder

######################################################################

VIProductVersion  "${VERSION}"
VIAddVersionKey "ProductName"  "${APP_NAME}"
VIAddVersionKey "CompanyName"  "${COMP_NAME}"
VIAddVersionKey "LegalCopyright"  "${COPYRIGHT}"
VIAddVersionKey "FileDescription"  "${DESCRIPTION}"
VIAddVersionKey "FileVersion"  "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"

######################################################################

SetCompressor LZMA
Name "${APP_NAME}"
Caption "${APP_NAME}"
OutFile "${INSTALLER_NAME}"
BrandingText "${APP_NAME} - ${VERSION}"
XPStyle on
InstallDirRegKey "${REG_ROOT}" "${REG_APP_PATH}" ""
InstallDir "$APPDATA\NeSync"

######################################################################

!include "MUI.nsh"

!define MUI_ICON "${BIN_SOURCE_DIR}\images\app_icon.ico"
!define MUI_UNICON "${BIN_SOURCE_DIR}\images\app_icon.ico"
!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING

!define MUI_WELCOMEFINISHPAGE_BITMAP  "${BIN_SOURCE_DIR}\images\installer_bold.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${BIN_SOURCE_DIR}\images\installer_bold.bmp"
!insertmacro MUI_PAGE_WELCOME

!ifdef LICENSE_TXT
!insertmacro MUI_PAGE_LICENSE "${LICENSE_TXT}"
!endif

!insertmacro MUI_PAGE_DIRECTORY

!ifdef REG_START_MENU
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "NeSync"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${REG_ROOT}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${UNINSTALL_PATH}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${REG_START_MENU}"
!insertmacro MUI_PAGE_STARTMENU Application $SM_Folder
!endif

!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\${MAIN_APP_EXE}"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM

!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"


######################################################################

Section -MainProgram
${INSTALL_TYPE}
SetOverwrite ifnewer
SetDetailsView show
SetOutPath "$INSTDIR"
File "${BIN_SOURCE_DIR}\libbz2-1.dll"
File "${BIN_SOURCE_DIR}\libefsw.dll"
File "${BIN_SOURCE_DIR}\libgcc_s_seh-1.dll"
File "${BIN_SOURCE_DIR}\libquazip1-qt6.dll"
File "${BIN_SOURCE_DIR}\libstdc++-6.dll"
File "${BIN_SOURCE_DIR}\libwinpthread-1.dll"
File "${BIN_SOURCE_DIR}\NeSync.exe"
File "${BIN_SOURCE_DIR}\Qt6Concurrent.dll"
File "${BIN_SOURCE_DIR}\Qt6Core.dll"
File "${BIN_SOURCE_DIR}\Qt6Core5Compat.dll"
File "${BIN_SOURCE_DIR}\Qt6Gui.dll"
File "${BIN_SOURCE_DIR}\Qt6Sql.dll"
File "${BIN_SOURCE_DIR}\Qt6Widgets.dll"
File "${BIN_SOURCE_DIR}\zlib1.dll"
SetOutPath "$INSTDIR\styles"
File "${BIN_SOURCE_DIR}\styles\qwindowsvistastyle.dll"
SetOutPath "$INSTDIR\sqldrivers"
File "${BIN_SOURCE_DIR}\sqldrivers\qsqlite.dll"
SetOutPath "$INSTDIR\platforms"
File "${BIN_SOURCE_DIR}\platforms\qwindows.dll"
SetOutPath "$INSTDIR\license"
File "${BIN_SOURCE_DIR}\license\gpl-3.0.txt"
SectionEnd

######################################################################

Section -Icons_Reg
SetOutPath "$INSTDIR"
WriteUninstaller "$INSTDIR\uninstall.exe"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
CreateDirectory "$SMPROGRAMS\$SM_Folder"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!insertmacro MUI_STARTMENU_WRITE_END
!endif

!ifndef REG_START_MENU
CreateDirectory "$SMPROGRAMS\NeSync"
CreateShortCut "$SMPROGRAMS\NeSync\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${MAIN_APP_EXE}"
CreateShortCut "$SMPROGRAMS\NeSync\Uninstall ${APP_NAME}.lnk" "$INSTDIR\uninstall.exe"

!ifdef WEB_SITE
WriteIniStr "$INSTDIR\${APP_NAME} website.url" "InternetShortcut" "URL" "${WEB_SITE}"
CreateShortCut "$SMPROGRAMS\NeSync\${APP_NAME} Website.lnk" "$INSTDIR\${APP_NAME} website.url"
!endif
!endif

WriteRegStr ${REG_ROOT} "${REG_APP_PATH}" "" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayName" "${APP_NAME}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "UninstallString" "$INSTDIR\uninstall.exe"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayIcon" "$INSTDIR\${MAIN_APP_EXE}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "DisplayVersion" "${VERSION}"
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "Publisher" "${COMP_NAME}"

!ifdef WEB_SITE
WriteRegStr ${REG_ROOT} "${UNINSTALL_PATH}"  "URLInfoAbout" "${WEB_SITE}"
!endif
SectionEnd

######################################################################

Section Uninstall
${INSTALL_TYPE}
ExecWait "TaskKill /IM ${MAIN_APP_EXE} /F"
SetDetailsView show
Delete "$INSTDIR\libbz2-1.dll"
Delete "$INSTDIR\libefsw.dll"
Delete "$INSTDIR\libgcc_s_seh-1.dll"
Delete "$INSTDIR\libquazip1-qt6.dll"
Delete "$INSTDIR\libstdc++-6.dll"
Delete "$INSTDIR\libwinpthread-1.dll"
Delete "$INSTDIR\NeSync.exe"
Delete "$INSTDIR\Qt6Concurrent.dll"
Delete "$INSTDIR\Qt6Core.dll"
Delete "$INSTDIR\Qt6Core5Compat.dll"
Delete "$INSTDIR\Qt6Gui.dll"
Delete "$INSTDIR\Qt6Sql.dll"
Delete "$INSTDIR\Qt6Widgets.dll"
Delete "$INSTDIR\zlib1.dll"
Delete "$INSTDIR\settings.ini"
Delete "$INSTDIR\styles\qwindowsvistastyle.dll"
Delete "$INSTDIR\sqldrivers\qsqlite.dll"
Delete "$INSTDIR\platforms\qwindows.dll"
Delete "$INSTDIR\license\gpl-3.0.txt"
 
RmDir "$INSTDIR\platforms"
RmDir "$INSTDIR\sqldrivers"
RmDir "$INSTDIR\styles"
RmDir "$INSTDIR\license"
 
Delete "$INSTDIR\uninstall.exe"
!ifdef WEB_SITE
Delete "$INSTDIR\${APP_NAME} website.url"
!endif

RmDir "$INSTDIR"

!ifdef REG_START_MENU
!insertmacro MUI_STARTMENU_GETFOLDER "Application" $SM_Folder
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\$SM_Folder\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\$SM_Folder\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\$SM_Folder"
!endif

!ifndef REG_START_MENU
Delete "$SMPROGRAMS\NeSync\${APP_NAME}.lnk"
Delete "$SMPROGRAMS\NeSync\Uninstall ${APP_NAME}.lnk"
!ifdef WEB_SITE
Delete "$SMPROGRAMS\NeSync\${APP_NAME} Website.lnk"
!endif
Delete "$DESKTOP\${APP_NAME}.lnk"

RmDir "$SMPROGRAMS\NeSync"
!endif

DeleteRegKey ${REG_ROOT} "${REG_APP_PATH}"
DeleteRegKey ${REG_ROOT} "${UNINSTALL_PATH}"
SectionEnd

######################################################################

