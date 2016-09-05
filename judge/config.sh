#!/bin/sh

#使用root账户更新
#echo -e "使用root账户更新"

#设置配置文件夹
mkdir -p /etc/judge/conf.d 
mkdir -p /etc/judge/bin
cp -r config/judge.conf /etc/judge/conf.d 
cp -r bin/* /etc/judge/bin

#finish
echo -e "Anything is already!"
echo -e "Have a nice day"
