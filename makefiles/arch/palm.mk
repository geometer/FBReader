CC = m68k-palmos-g++
AR = m68k-palmos-ar rsu
LD = m68k-palmos-g++

DEPGEN = $(CC) -MM -DPALM_TEMPORARY
CFLAGS = \
	-DSTL_SECTION="__attribute__ ((section(\"stl0\")))" \
	-DSTL1_SECTION="__attribute__ ((section(\"stl1\")))" \
	-DSTL2_SECTION="__attribute__ ((section(\"stl1\")))" \
	-DXML_SECTION="__attribute__ ((section(\"zlb2\")))" \
	-DFS_SECTION="__attribute__ ((section(\"zlb2\")))" \
	-DOPTIONS_SECTION="__attribute__ ((section(\"zlb1\")))" \
	-DUTIL_SECTION="__attribute__ ((section(\"zlb0\")))" \
	-DDIALOG_SECTION="__attribute__ ((section(\"zlb1\")))" \
	-DIMAGE_SECTION="__attribute__ ((section(\"zlb1\")))" \
	-DAPPLICATION_SECTION="__attribute__ ((section(\"zlb0\")))" \
	-DZLVIEW_SECTION="__attribute__ ((section(\"zlb0\")))" \
	-DVIEW_SECTION="__attribute__ ((section(\"app0\")))" \
	-DVIEW1_SECTION="__attribute__ ((section(\"app1\")))" \
	-DMODEL_SECTION="__attribute__ ((section(\"app3\")))" \
	-DFB_VIEW_SECTION="__attribute__ ((section(\"app1\")))" \
	-DFB_SECTION="__attribute__ ((section(\"app1\")))" \
	-DTEXT_STYLE_SECTION="__attribute__ ((section(\"app0\")))" \
	-DTEXT_STYLE1_SECTION="__attribute__ ((section(\"app1\")))" \
	-DHYPHENATION_SECTION="__attribute__ ((section(\"app2\")))" \
	-DFORMATS_SECTION="__attribute__ ((section(\"app2\")))" \
	-DFB_DIALOG_SECTION="__attribute__ ((section(\"app4\")))" \
	-DPALM_TEMPORARY \
	-DUSE_OWN_XML_PARSER \
	-pipe -fno-exceptions -fno-rtti -fno-inline -Wall -Wno-ctor-dtor-privacy -W -Wno-non-template-friend
RM = rm -rvf
RM_QUIET = rm -rf
EXTERNALINCLUDE = -I $(ROOTDIR)/zlibrary/palm/pseudostl -I $(ROOTDIR)/palm-external
EXTERNALLIBS =
