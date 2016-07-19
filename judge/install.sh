#!/bin/sh

mkdir -p bin

make

mv compiler/compiler bin
mv executor/executor bin
mv judged/judged bin
mv judge/judge judgeAppend
