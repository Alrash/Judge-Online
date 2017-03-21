/*
 */

#一些设置
set GLOBAL time_zone = '+8:00';

#创建用户，分配权限

#删除存在的数据库
drop database `PAOJ`;

#创建utf-8数据库
create database `PAOJ` default character set utf8 default collate utf8_general_ci;

use `PAOJ`;

/********************************表创建********************************************/

create table `tb_user_info`(
    `uid`           int             unsigned not null auto_increment comment '用户id 自动增长 起始值1',
    `nickname`      nvarchar(20)    not null comment '用户昵称',
    `password`      char(60)        not null comment '用户密码 记录password_hash的值',
    `email`         varchar(40)     not null comment '联络邮箱',
    `exp`           int	            unsigned not null default 0 comment '经验值',
    `avatar`        varchar(100)    not null default '/img/default_avatar.png' comment '用户头像 存放伪路径',
    `introduction`  nvarchar(200)   null comment '用户简介 可空',
    `registerDate`  date            not null default current_timestamp comment '注册时间',
    `verification`  varchar(300)    null comment '验证信息',
    primary key (`uid`),
    unique index `userinfo_nickname_unique` (`nickname` asc),
    unique index `userinfo_email_unique` (`email` asc)
) ENGINE = InnoDB comment '用户基本信息表';

create table `tb_user_login`(
    `uid`           int             unsigned not null,
    `uuid`          varchar(100)    not null,
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade
) ENGINE = InnoDB comment '用户登录表';

create table `tb_role`(
    `rid`           int             unsigned not null auto_increment comment '角色id 主键 自增长',
    `name`          nvarchar(20)    not null comment '角色名 唯一值',
    primary key (`rid`),
    unique key (`name`)
) ENGINE = InnoDB comment '角色表';

/**
 * 权限解释:
 * 同linux设置三位代表各种权限组合swr
 * s 限制w（上传者/所有拥有权限的人，实际上是修改权限）
 * w 写（所有需要文件写入，以及数据库写入的地方）
 * r 读（所有读取）
 * 范围：111 - 000，即0-7
 */
create table `tb_privilege`(
    `rid`           int             unsigned not null,
    `auth`          tinyint         unsigned not null default 0 comment '该用户是否可以更改权限/也是进入后台的标识',
    `wiki`          tinyint         unsigned not null default 1 comment 'wiki页所拥有的权限',
    `question`      tinyint         unsigned not null default 1 comment 'question功能区所拥有的权限',
    `commits`       tinyint         unsigned not null default 7 comment '用户评论区所拥有的权限',
    foreign key (`rid`) references `tb_role`(`rid`) on delete cascade,
    check (`auth` = 0 or `auth` = 1),
    check (`wiki` >= 0 and `wiki` <= 7),
    check (`question` >= 0 and `question` <= 7),
    check (`commits` >= 0 and `commits` <= 7)
) ENGINE = InnoDB comment '角色授权表';

create table `tb_user_role`(
    `uid`           int             unsigned not null,
    `rid`           int             unsigned not null,
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade,
    foreign key (`rid`) references `tb_role`(`rid`) on delete cascade
) ENGINE = InnoDB comment '角色用户映射表';

create table `tb_user_blacklist`(
    `uid`           int             unsigned not null,
    `datetime`      datetime        not null default current_timestamp comment '封禁时间',
    `lasttime`      int             unsigned not null default 60 comment '持续时间，默认单位分钟',
    `loop`          tinyint         unsigned not null default 0 comment '永封',
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade,
    check (`loop` = 0 or `loop` = 1)
) ENGINE = InnoDB comment '用户黑名单';

create table `tb_question_typo`(
    `qtid`          int             unsigned not null auto_increment,
    `name`          nvarchar(10)    not null,
    primary key (`qtid`)
) ENGINE = InnoDB comment '题目类型';

create table `tb_question`(
    `qid`           int             unsigned not null auto_increment,
    `title`         nvarchar(20)    not null,
    `maxtime`       int         unsigned not null default 1000 comment '最大运行时间 单位ms 以c为标准',
    `memory`        int         unsigned not null default 256 comment '最大运行内存 单位mb 以c为标准',
    `level`         decimal(5, 3)  not null default 2.5 comment '难度等级',
    `qtid`          int             unsigned not null,
    `outlimit`      tinyint         not null default 0 comment '输出格式限制 0不限制 1限制',
    `num`           int             not null default 0 comment '测试文件数量',
    `uptime`        datetime        not null default current_timestamp comment '上传时间',
    `uid`           int             unsigned not null comment '创建者',
    `original`      tinyint         not null default 1,
    `author`        nvarchar(20)    null comment '作者名',
    `link`          varchar(200)    null,
    `used`          tinyint         not null default -1 comment '表示该题目是否被使用 -1未使用 0已查看但是未使用 1使用 2隐藏',
    primary key (`qid`),
    foreign key (`qtid`) references `tb_question_typo`(`qtid`) on delete cascade,
    check (`level` >= 0 and `level` <= 5),
    check (`outlimit` = 0 or `outlimit` = 1),
    check (`original` = 0 or `original` = 1)
) ENGINE = InnoDB comment '问题基本信息表';

create table `tb_tag_info`(
    `tid`           int             unsigned not null auto_increment,
    `name`          nvarchar(10)    not null,
    `total`         int             unsigned not null default 0 comment '被使用次数',
    primary key (`tid`),
    unique index `tag_info_name_unique` (`name` asc)
) ENGINE = InnoDB comment '标签信息表';

create table `tb_tag_map`(
    `tid`           int             unsigned not null,
    `qid`           int             unsigned not null,
    foreign key (`tid`) references `tb_tag_info`(`tid`) on delete cascade,
    foreign key (`qid`) references `tb_question`(`qid`) on delete cascade
) ENGINE = InnoDB comment '标签映射表';

create table `tb_language`(
    `lid`           int             unsigned not null auto_increment,
    `name`          nvarchar(10)    not null,
    `complier`      varchar(100)    not null,
    primary key (`lid`)
) ENGINE = InnoDB comment '编译信息表';

create table `tb_question_language`(
    `id`            int             unsigned not null auto_increment,
    `qid`           int             unsigned not null,
    `lid`           int             unsigned not null,
    primary key (`id`),
    foreign key (`qid`) references `tb_question`(`qid`) on delete cascade,
    foreign key (`lid`) references `tb_language`(`lid`) on delete cascade
) ENGINE = InnoDB comment '可用编译语言';

create table `tb_paper_info`(
    `pid`           int             unsigned not null auto_increment,
    `title`         nvarchar(20)    not null,
    `uid`           int             unsigned not null,
    `score`         int             unsigned not null default 10,
    `start`         datetime        not null default current_timestamp,
    `lasttime`      int             not null default -1 comment '考试时间 -1表示无时间限制',
    `count`         int             unsigned not null default 0,
    primary key (`pid`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action,
    check (`pid` > 0)
)ENGINE = InnoDB comment '试卷基本信息表';
alter table tb_paper_info auto_increment = 1;

create table `tb_paper_define`(
    `id`            int             unsigned not null auto_increment,
    `pid`           int             unsigned not null,
    `qid`           int             unsigned not null,
    `qtid`          int             unsigned not null,
    `score`         int             unsigned not null default 100 comment '单题得分',
    primary key (`id`),
    foreign key (`pid`) references `tb_paper_info`(`pid`) on delete cascade,
    foreign key (`qid`) references `tb_question`(`qid`) on delete cascade,
    foreign key (`qtid`) references `tb_question_typo`(`qtid`) on delete cascade
) ENGINE = InnoDB comment '试卷单题信息表';

/**
 * 鉴于程序测试值确定，规定以下测试结果的值：
 * AC 1
 * WA 2
 * PE 3
 * RE 4
 * TLE 5
 * MLE 6
 * OLE 7
 * CE 8
 * FSC = 9 (calling forbidden system call 可归到re)
 * OTHERS = 10
 */
create table `tb_submission`(
    `sid`           int             unsigned not null auto_increment,
    `uid`           int             unsigned not null,
    `qid`           int             unsigned not null,
    `pid`           int             unsigned not null default 0,
    `lid`           int             unsigned not null,
    `score`         int             unsigned not null,
    `uptime`        datetime        not null default current_timestamp,
    `runtime`       tinyint         unsigned not null default 0 comment '实际运行时间 单位ms',
    `memory`        tinyint         unsigned not null default 0 comment '实际使用内存大小 单位kb',
    `stauts`        tinyint         not null default 0 comment '当前状态 0表示等待测试',
    `compile_error` varchar(200)    null,
    primary key (`sid`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade,
    foreign key (`qid`) references `tb_question`(`qid`) on delete cascade,
    foreign key (`pid`) references `tb_paper_info`(`pid`) on delete cascade,
    foreign key (`lid`) references `tb_language`(`lid`) on delete cascade,
    check (`stauts` >=0 and `stauts` <= 10)
)ENGINE = InnoDB comment '问题提交表';

create table `tb_wiki_folder`(
    `wfid`          int             unsigned not null auto_increment,
    `wfname`        nvarchar(10)    not null,
    `pre_id`        int             unsigned not null default 0,
    primary key (`wfid`)
) ENGINE = InnoDB comment 'wiki目录层次图';
alter table tb_wiki_folder auto_increment = 1;

create table `tb_wiki_info`(
    `wid`           int             unsigned not null auto_increment,
    `title`         nvarchar(20)    not null,
    `uid`           int             unsigned not null,
    `ctime`         datetime        not null default current_timestamp comment '创建时间',
    `atime`         datetime        not null default current_timestamp comment '最后一次修改时间',
    `hash`          char(32)        not null,
    `pre_id`        int             unsigned not null default 0 comment '上层目录',
    `original`      tinyint         not null default 1,
    `author`        nvarchar(20)    null comment '作者名',
    `link`          varchar(200)    null comment '外连接',
    primary key (`wid`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action,
    check (`original` = 0 or `original` = 1)
) ENGINE = InnoDB comment 'wiki基本信息表';

create table `tb_wiki_history`(
    `id`            int             unsigned not null auto_increment,
    `wid`           int             unsigned not null,
    `filename`      varchar(100)    not null comment '完整文件名',
    `uid`           int             unsigned not null comment '修改者',
    `atime`         datetime        not null,
    `hash`          char(32)        not null,
    primary key (`id`),
    foreign key (`wid`) references `tb_wiki_info`(`wid`) on delete cascade,
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action
) ENGINE = InnoDB comment 'wiki历史记录表';

create table `tb_notice`(
    `id`            int             unsigned not null auto_increment,
    `uid`           int             unsigned not null comment '发布者',
    `ctime`          datetime        not null default current_timestamp comment '发布时间',
    `content`       nvarchar(200)   not null,
    primary key (`id`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action
) ENGINE = InnoDB comment '系统公告';

create table `tb_message`(
    `id`            int             unsigned not null auto_increment,
    `fuid`          int             unsigned not null comment '发送',
    `tuid`          int             unsigned not null comment '接收',
    `ctime`          datetime        not null default current_timestamp comment '发布时间',
    `content`       nvarchar(200)   not null,
    primary key (`id`),
    foreign key (`fuid`) references `tb_user_info`(`uid`) on delete no action,
    foreign key (`tuid`) references `tb_user_info`(`uid`) on delete no action
) ENGINE = InnoDB comment '私信';

/*******************************视图创建*******************************************/

/*****************************插入部分数据*****************************************/
