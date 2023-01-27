!# /bin/bash

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ] ;then
    echo "Usage: build-deb <base-directory> <deb-work-dir-name> <package-name>"
    exit 1
fi

BASE_DIR=$1
echo "Base directory: $BASE_DIR"
DEB_WORK_DIR_NAME=$2
echo "DEB work directory: $DEB_WORK_DIR_NAME"
PACKAGE_NAME=$3
echo "Package name: $PACKAGE_NAME"
cd $BASE_DIR
dpkg-deb --verbose --build $DEB_WORK_DIR_NAME $PACKAGE_NAME
chown 1000:1000 $PACKAGE_NAME
