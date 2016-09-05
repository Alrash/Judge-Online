#!/bin/sh
mkdir bin
make

mv judged/judged bin/
mv judge/judge bin/
mv executor/executor bin/
mv compiler/compiler bin/
