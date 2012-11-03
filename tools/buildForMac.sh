#!/bin/sh

if [ $# -gt 1 -o $# -eq 1 -a "$1" != "-dmg" ]; then
	echo "Usage:\n\t$0 [ -dmg ]";
	exit 1;
fi

if [ "$1" == "-dmg" ]; then
  VOLUMENAME="FBReader `cat fbreader/VERSION`"
	MACFOLDER=`pwd`/mac
	IMAGEFILE_RW=FBReader-rw.dmg
	IMAGEFILE="$VOLUMENAME.dmg"

	rm -rf $MACFOLDER "$IMAGEFILE" $IMAGEFILE_RW
	mkdir -p $MACFOLDER
	DESTDIR=$MACFOLDER INSTALLDIR="/FBReader.app" make install
  cp $MACFOLDER/FBReader.app/Contents/Resources/FBReader.icns $MACFOLDER/.VolumeIcon.icns
  SetFile -c icnC $MACFOLDER/.VolumeIcon.icns
	hdiutil create -quiet -volname "$VOLUMENAME" -srcfolder $MACFOLDER -format UDRW -ov $IMAGEFILE_RW
  hdiutil attach $IMAGEFILE_RW
  SetFile -a C "/Volumes/$VOLUMENAME"
  hdiutil detach "/Volumes/$VOLUMENAME"
  open $IMAGEFILE_RW
  sleep 5
  echo "
    tell application \"Finder\"
      tell disk \"$VOLUMENAME\"
        open
        set the bounds of container window to {100, 100, 612, 386}
        set current view of container window to icon view
        set toolbar visible of container window to false
        set statusbar visible of container window to false
        set theViewOptions to the icon view options of container window
        set arrangement of theViewOptions to not arranged
        set icon size of theViewOptions to 128
        make new alias file at container window to POSIX file \"/Applications\" with properties {name:\"Applications\"}
        set position of item \"FBReader.app\" of container window to {128, 128}
        set position of item \"Applications\" of container window to {384, 128}
        update without registering applications
        delay 5
        eject
      end tell
    end tell
  " | osascript
  hdiutil convert $IMAGEFILE_RW -format UDZO -o "$IMAGEFILE"
  rm -rf $MACFOLDER $IMAGEFILE_RW
else
	rm -rf FBReader.app
	DESTDIR=`pwd` INSTALLDIR="/FBReader.app" make install
fi
