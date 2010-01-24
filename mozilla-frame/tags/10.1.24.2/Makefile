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

LINK_LIBS=

CPP_FLAGS=

OUTDIR=Deploy
TARGET=$(OUTDIR)\mozilla-frame.dll

LINK_OBJS=CMozillaFrame.obj CMozillaFrameModule.obj DllMain.obj MozillaFrame.obj \
	StdAfx.obj Support.obj mozilla-frame.res

TARGET_EXTRA=

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

CPP_FLAGS=$(CPP_FLAGS) /nologo /MT /W3 /GX /O2 /DWIN32 /DNDEBUG /D_WINDOWS \
	/D_MBCS /DUNICODE /D_UNICODE /DMOZILLAFRAME_EXPORTS /c
LINK_FLAGS=$(LINK_LIBS) /nologo /machine:I386 /dll
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
