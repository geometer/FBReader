BASEDIR = data/opt/QtPalmtop
DEBDIR = data/DEBIAN

all:
	@make .builddir
	@make .buildpackage
	@make .cleandir

clean: .cleandir
	@rm -f *.ipk

.builddir: .cleandir
	@mkdir -p $(DEBDIR)
	@mkdir -p $(BASEDIR)/bin
	@mkdir -p $(BASEDIR)/apps/Applications
	@mkdir -p $(BASEDIR)/pics/fbreader
	@cp control $(DEBDIR)
	@cp ../../FBReader $(BASEDIR)/bin
	@cp fbreader.desktop $(BASEDIR)/apps/Applications
	@cp ../../icons/34x28/FBReader.png $(BASEDIR)/pics/fbreader
	@cp ../../icons/34x28/FBReader/*.png $(BASEDIR)/pics/fbreader
	@cp -r ../../share  $(BASEDIR)
	@rm -rf `find $(BASEDIR)/share -name ".svn"`

.buildpackage:
	@dpkg -b data fbreader-opie_0.6.4_arm.ipk

.cleandir:
	@rm -rf data
