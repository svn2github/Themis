#!/bin/sh
rm ./modules/http/plugman.cpp
cd modules/http
ln -s ../../framework/plugman.cpp plugman.cpp
cd ../..

