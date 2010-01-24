#
# This file is part of Google Wave Notifier.
#
# Google Wave Notifier is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Google Wave Notifier is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Google Wave Notifier.  If not, see <http://www.gnu.org/licenses/>.
#

LINK_LIBS=deps\curl-7.15.1\libcurl_imp.lib deps\zlib-1.2.3\zlibwapi.lib		\
	deps\gd-2.0.34\bgd.lib deps\chrome-frame\chrome-frame.lib		\
	version.lib shlwapi.lib comctl32.lib crypt32.lib ws2_32.lib winmm.lib	\
	msimg32.lib

CPP_FLAGS=/I.\deps\curl-7.15.1\include /I.\deps\zlib-1.2.3\include		\
	/I.\deps\gd-2.0.34\include /I.\deps\chrome-frame\include

OUTDIR=Deploy
TARGET=$(OUTDIR)\wave-notify.exe

LINK_OBJS=Base64.obj CAboutDialog.obj CApp.obj CAppWindow.obj CAvatar.obj	\
	CBrowser.obj CContactOnlinePopup.obj CCurl.obj				\
	CCurlAnsiStringReader.obj CCurlMonitor.obj CCurlMulti.obj		\
	CDialog.obj CFlyout.obj CLoginDialog.obj CMessagePopup.obj		\
	CMigration.obj CModelessDialogs.obj CModelessPropertySheets.obj		\
	CNotifierApp.obj CNotifyIcon.obj Compat.obj ConvertString.obj		\
	COptionsSheet.obj CPopup.obj CPopupBase.obj CPopupWindow.obj 		\
	CPropertySheet.obj CPropertySheetPage.obj CRegKey.obj CSettings.obj	\
	CThread.obj CTimer.obj CTimerCollection.obj CUnreadWave.obj 		\
	CUnreadWaveCollection.obj CUnreadWavePopup.obj				\
	CUnreadWavesFlyout.obj CUTF8Converter.obj CVersion.obj CWave.obj	\
	CWaveCollection.obj CWaveContact.obj CWaveContactCollection.obj		\
	CWaveContactStatus.obj CWaveContactStatusCollection.obj			\
	CWaveMessage.obj CWaveName.obj CWaveReader.obj				\
	CWaveRequestContactUpdates.obj CWaveRequestGetAllContacts.obj		\
	CWaveRequestGetContactDetails.obj					\
	CWaveRequestStartListening.obj CWaveRequestStopListening.obj		\
	CWaveResponse.obj CWaveSession.obj CWaveView.obj CWindow.obj		\
	CWindowHandle.obj Encryption.obj Format.obj GetFont.obj			\
	GetLanguageCode.obj GetWindowsVersion.obj Json_Reader.obj		\
	Json_Value.obj Json_Writer.obj Log.obj Main.obj StdAfx.obj		\
	SubclassStaticForLink.obj Support.obj TaskbarLocation.obj Trim.obj	\
	Unzip.obj UrlEncode.obj Wine.obj wave-notify.res

TARGET_EXTRA=deps\curl-7.15.1\libcurl.dll deps\curl-7.15.1\libeay32.dll		\
	deps\curl-7.15.1\ssleay32.dll deps\curl-7.15.1\zlib1.dll		\
	deps\zlib-1.2.3\zlibwapi.dll wave-notify.chm

###############################################################################

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
LINK=link.exe
RSC=rc.exe

LINK_LIBS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib \
	advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib \
	odbccp32.lib $(LINK_LIBS)

.c.obj::
	$(CPP) $(CPP_FLAGS) $<

.cpp.obj::
	$(CPP) $(CPP_FLAGS) $<

.rc.res:
	$(RSC) $(RSC_FLAGS) $<

CPP_FLAGS=$(CPP_FLAGS) /nologo /MT /W3 /GX /O2 /DWIN32 /DNDEBUG /D_WINDOWS /D_MBCS /DUNICODE /c
LINK_FLAGS=$(LINK_LIBS) /nologo /machine:I386
RSC_FLAGS=/l 0x409 /dNDEBUG /d_VS98 /dRT_MANIFEST=24

TARGET_TYPE=EXE

all: $(TARGET)

clean:
	-@erase $(TARGET)
	-@erase $(LINK_OBJS)

$(OUTDIR):
	if not exist $(OUTDIR)/$(NULL) mkdir $(OUTDIR)

$(TARGET): $(OUTDIR) $(DEF_FILE) $(LINK_OBJS)
	$(LINK) $(LINK_FLAGS) $(LINK_OBJS) /out:$(TARGET)
