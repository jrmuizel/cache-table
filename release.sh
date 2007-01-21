#!/bin/bash

RELEASE="$1"
echo "Releasing CacheTable $RELEASE"

# Create a Tag in repository
svn cp https://cache-table.googlecode.com/svn/trunk \
        https://cache-table.googlecode.com/svn/tags/$RELEASE \
        -m "Tagged release $RELEASE."

# Download the tagged version
svn co https://cache-table.googlecode.com/svn/tags/$RELEASE cache-table-$RELEASE
cd cache-table-$RELEASE

# Create ChangeLog
svn2cl --group-by-day -i --reparagraph --authors=authors.xml

# Create Documentation
doxygen

# Remove subversion directories
find . -name '.svn' | xargs rm -rf

# Remove not needed files 
rm release.sh authors.xml

# Create the tarball
cd ..
tar cvfz cache-table-$RELEASE.tar.gz cache-table-$RELEASE
rm -rf cache-table-$RELEASE
