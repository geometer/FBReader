#!/bin/sh
if [ $# != 1 ]; then
	echo usage:
	echo "  $0 <lang>"
else 
	lang=$1
  ln -sf MiniHelp.desktop.$lang.fb2 MiniHelp.maemo.$lang.fb2
  ln -sf MiniHelp.640x480.$lang.fb2 MiniHelp.opensimpad-0.9.0.$lang.fb2
  ln -sf MiniHelp.240x320.$lang.fb2 MiniHelp.openzaurus_240x320.$lang.fb2
  ln -sf MiniHelp.desktop.$lang.fb2 MiniHelp.openzaurus_640x480.$lang.fb2
  ln -sf MiniHelp.desktop.$lang.fb2 MiniHelp.pdaxrom.$lang.fb2
  ln -sf MiniHelp.desktop.$lang.fb2 MiniHelp.pepperpad3.$lang.fb2
  ln -sf MiniHelp.240x320.$lang.fb2 MiniHelp.pma400.$lang.fb2
  ln -sf MiniHelp.640x480.$lang.fb2 MiniHelp.qvfb.$lang.fb2
  ln -sf MiniHelp.240x320.$lang.fb2 MiniHelp.zaurus_240x320.$lang.fb2
  ln -sf MiniHelp.desktop.$lang.fb2 MiniHelp.zaurus_640x480.$lang.fb2
fi
