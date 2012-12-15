#!/bin/sh

export PATH=$PATH:/Applications/bbndk/host_10_0_9_404/darwin/x86/usr/bin
export QMAKESPEC=/Applications/bbndk/target_10_0_9_1673/qnx6/usr/share/qt4/mkspecs/unsupported/blackberry-armv7le-qcc
export QNX_HOST=/Applications/bbndk/host_10_0_9_404/darwin/x86
export QNX_TARGET=/Applications/bbndk/target_10_0_9_1673/qnx6

make
blackberry-nativepackager  -devMode  -package FBReader.bar bb10/bar-descriptor.xml -debugToken ~/b/device_debug_token.bar
blackberry-deploy -installApp -package FBReader.bar -device 169.254.0.1 -password pass
