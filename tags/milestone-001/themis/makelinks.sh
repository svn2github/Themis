#!/bin/sh
rm ./modules/http/plugman.cpp
cd modules/http
ln -s ../../framework/plugman.cpp plugman.cpp
ln -s ../../common/plugclass.cpp plugclass.cpp
cd ../cache
ln -s ../../common/plugclass.cpp plugclass.cpp
cd ../..


