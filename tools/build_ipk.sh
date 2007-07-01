#!/bin/sh

TMP_DIR=$1

tar czf $TMP_DIR/control.tar.gz -C $TMP_DIR ./control
tar czf $TMP_DIR/data.tar.gz -C $TMP_DIR/data .
echo 2.0 > $TMP_DIR/debian-binary
tar czf `egrep ^Filename: $TMP_DIR/control | cut -d' ' -f2` -C $TMP_DIR ./debian-binary ./control.tar.gz ./data.tar.gz
rm $TMP_DIR/debian-binary
