#!/bin/sh
rm ./modules/http/plugman.cpp
ln -s ../../framework/plugman.cpp ./modules/http/plugman.cpp
ln -s ../../common/plugclass.cpp ./modules/http/plugclass.cpp
ln -s ../../common/plugclass.cpp ./modules/cache/plugclass.cpp



