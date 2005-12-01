#! /bin/sh

# A helper script to make tags

base=https://only.mawhrin.net/repos/stuff

if [ ! -r fbreader/VERSION ]
then
  echo "You are not in the right directory (fbreader/VERSION) does not exist"
  exit 2
fi

version=`cat fbreader/VERSION`

svn cp $base/trunk $base/tags/fbreader-$version -m "(automatic) tagging version $version"
