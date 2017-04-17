/*
 */

#一些设置
set GLOBAL time_zone = '+8:00';
set GLOBAL event_scheduler=ON;

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
    `introduction`  text   null comment '用户简介 可空',
    `registerDate`  date            not null default current_timestamp comment '注册时间',
    `verification`  text    null comment '验证信息',
    primary key (`uid`),
    unique index `userinfo_nickname_unique` (`nickname` asc),
    unique index `userinfo_email_unique` (`email` asc)
) ENGINE = InnoDB comment '用户基本信息表';

/**
 * 不用uuid当主键，参见
 */
create table `tb_user_login`(
    `id`            int             unsigned not null auto_increment,
    `uuid`          char(36)        not null comment '唯一id',
    `uid`           int             unsigned not null,
    `ctime`         datetime        not null default current_timestamp comment '该条uuid创建时间',
    primary key (`id`),
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
    `auth`          binary (1)        not null default 0 comment '该用户是否可以更改权限/也是进入后台的标识',
    `wiki`          binary(3)         not null default 1 comment 'wiki页所拥有的权限',
    `question`      binary(3)         not null default 1 comment 'question功能区所拥有的权限',
    `commits`       binary(3)         not null default 7 comment '用户评论区所拥有的权限',
    foreign key (`rid`) references `tb_role`(`rid`) on delete cascade
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
    `loop`          binary(1)         not null default 0 comment '永封',
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade
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
    `outlimit`      binary(1)        not null default 0 comment '输出格式限制 0不限制 1限制',
    `num`           int             not null default 0 comment '测试文件数量',
    `uptime`        datetime        not null default current_timestamp comment '上传时间',
    `uid`           int             unsigned not null comment '创建者',
    `original`      binary(1)         not null default 1,
    `author`        nvarchar(30)    null comment '作者名',
    `link`          varchar(200)    null,
    `used`          tinyint         not null default -1 comment '表示该题目是否被使用 -1未使用 0已查看但是未使用 1使用 2隐藏',
    primary key (`qid`),
    foreign key (`qtid`) references `tb_question_typo`(`qtid`) on delete cascade,
    check (`level` >= 0 and `level` <= 5)
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
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action
)ENGINE = InnoDB comment '试卷基本信息表';

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
    `status`        tinyint         not null default 0 comment '当前状态 0表示等待测试',
    `compile_error` varchar(200)    null,
    primary key (`sid`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete cascade,
    foreign key (`qid`) references `tb_question`(`qid`) on delete cascade,
    foreign key (`pid`) references `tb_paper_info`(`pid`) on delete cascade,
    foreign key (`lid`) references `tb_language`(`lid`) on delete cascade,
    check (`status` >=0 and `status` <= 10),
    index `index_user`(`uid`, `status`)
)ENGINE = InnoDB comment '问题提交表';

create table `tb_wiki_folder`(
    `wfid`          int             unsigned not null auto_increment,
    `wfname`        nvarchar(10)    not null,
    `pre_id`        int             unsigned not null default 0,
    primary key (`wfid`)
) ENGINE = InnoDB auto_increment = 1 comment 'wiki目录层次图';

create table `tb_wiki_info`(
    `wid`           int             unsigned not null auto_increment,
    `title`         nvarchar(20)    not null,
    `uid`           int             unsigned not null,
    `ctime`         datetime        not null default current_timestamp comment '创建时间',
    `atime`         datetime        not null default current_timestamp comment '最后一次修改时间',
    `hash`          char(32)        not null,
    `pre_id`        int             unsigned not null default 0 comment '上层目录',
    `original`     binary(1)         not null default 1,
    `author`        nvarchar(20)    null comment '作者名',
    `link`          varchar(200)    null comment '外连接',
    primary key (`wid`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action
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
    `ctime`         datetime        not null default current_timestamp comment '发布时间',
    `title`         nvarchar(50)    not null,
    `content`       text   not null,
    `weight`        tinyint         not null default 0 comment '权重',
    primary key (`id`),
    foreign key (`uid`) references `tb_user_info`(`uid`) on delete no action,
    unique index `sort`(`weight`, `ctime`, `id`)
) ENGINE = InnoDB comment '系统公告';

create table `tb_message`(
    `id`            int             unsigned not null auto_increment,
    `fuid`          int             unsigned not null comment '发送',
    `tuid`          int             unsigned not null comment '接收',
    `ctime`         datetime        not null default current_timestamp comment '发布时间',
    `content`       text   not null,
    primary key (`id`),
    foreign key (`fuid`) references `tb_user_info`(`uid`) on delete no action,
    foreign key (`tuid`) references `tb_user_info`(`uid`) on delete no action
) ENGINE = InnoDB comment '私信';

/*******************************视图创建*******************************************/

/**
 * vw_notice 信息公告视图：
 * 公告序列号(id)
 * 创建者昵称(name)
 * 公告标题(title)
 * 公告内容(content)
 * 创建时间(time, yyyy-mm-dd HH:MM)
 * 公告权重(weight)
 *
 * 排序规则：
 *     高权重在前，创建时间晚的在前，id逆向
 */
create view `vw_notice` as 
    select
        `tb_notice`.`id` as `id`, `tb_user_info`.`nickname` as `name`,
        `tb_notice`.`title` as `title`, `tb_notice`.`content` as `content`,
        DATE_FORMAT(`tb_notice`.`ctime`, '%Y-%m-%d %h:%i') as `time`, `tb_notice`.`weight` as `weight`
        from `tb_notice` FORCE INDEX(`sort`) natural left outer join `tb_user_info`
        ORDER BY `weight` desc, `tb_notice`.`ctime` desc, `id` desc;

/* *
 * 角色权限视图，用于查看所有角色权限
 */
create view `vw_role_privilege` as
	select
		`tb_role`.`name` as `name`, `tb_privilege`.`auth` as `auth`, 
        `tb_privilege`.`question` as `question`, `tb_privilege`.`wiki` as `wiki`, 
        `tb_privilege`.`commits` as `commits`
		from `tb_role` natural join `tb_privilege`;
    
/* *
 * 用户登录使用，左连接内部select可以简写
 */
create view `vw_user_login` as
	select
		`tb_user_info`.`uid` as `uid`, `tb_user_info`.`nickname` as `username`,
        `tb_user_info`.`email` as `email`, `tb_user_info`.`password` as `password`,
        `tb_user_info`.`exp` as `exp`, `tb_user_info`.`avatar` as `avatar`,
        `tb_user_info`.`introduction` as `introduction`, `tb_user_info`.`verification` as `verification`,
        `tb_user_info`.`registerDate` as `registerDate`, 
        DATE_ADD(`tb_user_blacklist`.`datetime`, INTERVAL`tb_user_blacklist`.`lasttime` MINUTE) as `endtime` ,
        `tb_user_blacklist`.`loop` as `loop`, `tb_user_login`.`uuid` as `uuid`, 
        `tb_user_login`.`ctime` as `time`, `privilege`.`id` as `roleid`, `privilege`.`auth` as `auth`
		from `tb_user_info` 
			natural left join `tb_user_login`
            natural left join `tb_user_blacklist`
			left outer join (
				select 
					`tb_role`.`name` as `name`, `tb_privilege`.`auth` as `auth`, 
					`tb_privilege`.`question` as `question`, `tb_privilege`.`wiki` as `wiki`, 
					`tb_privilege`.`commits` as `commits`, `tb_user_role`.`rid` as `id`,
                    `tb_user_role`.`uid` as `uid`
					from `tb_role` 
						natural join `tb_user_role` 
                        natural join `tb_privilege`
                ) as `privilege` on (`tb_user_login`.`uid` = `privilege`.`uid`);

/* *
 * 用户信息查询使用，左连接内部select可以简写
 */            
create view `vw_user_info` as
	select
		`tb_user_info`.`uid` as `uid`, `tb_user_info`.`nickname` as `username`,
        `tb_user_info`.`email` as `email`, `tb_user_info`.`password` as `password`,
        `tb_user_info`.`exp` as `exp`, `tb_user_info`.`avatar` as `avatar`,
        `tb_user_info`.`introduction` as `introduction`,  `tb_user_info`.`registerDate` as `registerDate`, 
        DATE_ADD(`tb_user_blacklist`.`datetime`, INTERVAL`tb_user_blacklist`.`lasttime` MINUTE) as `endtime` ,
        `tb_user_blacklist`.`loop` as `loop`, `privilege`.`id` as `roleid`, `privilege`.`auth` as `auth`,
        `privilege`.`name` as `rolename`, `submission`.`status` as `status`, `submission`.`status_statistics` as `ss`
		from `tb_user_info` 
            natural left join `tb_user_blacklist`
			left outer join (
				select 
					`tb_role`.`name` as `name`, `tb_privilege`.`auth` as `auth`, 
					`tb_privilege`.`question` as `question`, `tb_privilege`.`wiki` as `wiki`, 
					`tb_privilege`.`commits` as `commits`, `tb_user_role`.`rid` as `id`,
                    `tb_user_role`.`uid` as `uid`
					from `tb_role` 
						natural join `tb_user_role` 
                        natural join `tb_privilege`
                ) as `privilege` on (`tb_user_info`.`uid` = `privilege`.`uid`)
			left outer join (
                select 
					`tb_submission`.`uid`, `tb_submission`.`status`, count(*) as `status_statistics`
					from `tb_submission`
                    group by `tb_submission`.`uid`, `tb_submission`.`status`
                ) as `submission` on (`tb_user_info`.`uid` = `submission`.`uid`);

/********************************创建定时器****************************************/
delimiter @@
create procedure delete_login_proce()
begin
	delete from `tb_user_login` 
		where DATE_FORMAT(`ctime`, '%Y-%m-%d') <= (DATE_FORMAT(now(), '%Y-%m-%d') - 365);
end@@
delimiter ;

/* *
 * 创建删除uuid事件，调用delete_login_proce过程执行
 */
create event delete_login_event
	on schedule every 1 day
    on completion preserve enable
    comment '删除uuid事件'
    do call delete_login_proce();
    
/*****************************插入部分数据*****************************************/