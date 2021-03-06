/*
 * 此处有一个问题：auto_increment在insert错误的时候，再次插入时，记录值会多增加1
 * 例：表里id为1,插入错误1次后，再次插入，成功，这时id为3，而非2；插入错误5次后，成功插入，id为9
 */

create user 'JudgeOnline' identified by 'judgement';
grant all on JudgeOnline.* to 'JudgeOnline'@'127.0.0.1' identified by 'judgement';

create database `JudgeOnline`;

use `JudgeOnline`;

/*
 * UId  userid 无符号，主键，自动增长
 * Nickname 昵称，并建立唯一索引
 * passwd 使用php的第三方phpass库，暂时使用默认方法加密，长度大小为60
 * Note 个人简介
 * Email 个人邮箱，并建立唯一索引
 * Trust -- 用户权限 0:只是一般用户 10:超级管理员 中间值:拥有编写权限
 * Nickname与Email可做登录使用，UId不能
 * Status -- 邮箱是否激活(为什么用tinyint而不用enum，只因看了一下enum，感觉太坑)
 * Extra -- 保留
 * Status与Extra暂时无用
 */
create table `UserInfo`
(
    `UId`         bigint          unsigned not null auto_increment,
    `Nickname`    nvarchar(20)    not null,
    `Passwd`      char(60)        not null,
    `Email`       varchar(30)     not null,
    `Image`       varchar(100)    not null default "/img/default_image.jpg",
    `Note`        nvarchar(200)   null,
    `Trust`       tinyint         not null default 0,
    `Status`      tinyint         not null default 0,
    `Extra`       varchar(200)    null,
    primary key (`UId`),
    unique index `nickname_Unique` (`Nickname` asc),
    unique index `email_Unique` (`Email` asc),
    check (`Trust` >= 0 and `Trust` <= 10),
    check (`Status` = 0 or `Status` = 1)
);

/*
 * 记录可能经常修改的字段
 * 部分字段解释
 * Exp -- 经验值，用于计算等级
 * AC -- accepted, 完全正确
 * WA -- wrong answer，答案完全错误
 * PE -- presentation, 答案对，但格式错误
 * RE -- runtime error，运行时出错
 * TLE -- time limit exceeded，超时
 * MLE -- memory limit exceeded，超内存
 * OLE -- output limit exceeded，输出太多
 * CE -- compilation error，编译错误
 */
create table `UserStatistics`
(
    `UId`         bigint        unsigned not null,
    `Exp`         int	        unsigned not null default 0,
    `AC`          int       	unsigned default 0,
    `WA`          int        	unsigned default 0,
    `PE`          int   	    unsigned default 0,
    `RE`          int	        unsigned default 0,
    `TLE`         int	        unsigned default 0,
    `MLE`         int	        unsigned default 0,
    `OLE`         int	        unsigned default 0,
    `CE`          int	        unsigned default 0,
    `others`      int           unsigned default 0,
    `C`           int	        unsigned default 0,
    `C++`         int	        unsigned default 0,
    `C++11`       int	        unsigned default 0,
    `Java`        int	        unsigned default 0,
    `Python`      int	        unsigned default 0,
    foreign key (`UId`) references UserInfo(`UId`) on delete cascade
);

/*
 * 存放问题信息
 * hard -- 问题难度，范围1～5
 * TestNumber -- 测试用例的数目
 * time -- 以C为标准的限制时间，例：1.00s
 * memory -- 同样以C为标准的内存限制，例：254M
 * note -- 存放原题、原作者信息(url or author)
 * Type -- 题目类型，0 大题 1 填空题
 * label -- 问题符合的标签，例：BSF(;分割)
 */
create table `QuestionInfo`
(
    `PId`         int           unsigned not null auto_increment,
    `Title`       nvarchar(31)  not null,
    `Time`        int           unsigned not null,
    `Memory`      int           unsigned not null,
    `Hard`        int           not null default 3,
    `Type`        int           not null default 0,
    `TestNumber`  int           not null default 10,
    `Note`        varchar(51)   not null,
    `Label`       nvarchar(101) null,
    primary key (`PId`),
    check (`Hard` >= 1 and `Hard` <= 5),
    check (`Type` >= 0 and `Type` <= 1)
);

/*
 * right 与 wrong，仅是计数使用
 */
create table `QuestionStatistics`
(
    `PId`         int           unsigned not null,
    `Right`       bigint        unsigned default 0,
    `Wrong`       bigint        unsigned default 0,
    foreign key (`PId`) references QuestionInfo(`PId`) on delete cascade
);

/*
 * SId -- submission id 提交id
 * UId -- user id 是userinfo的外键
 * PId -- problem id 是QuestionInfo的外键
 * timestamp -- 时间戳
 * goal -- 得分
 * Runtime -- 实际运行时间(ms)
 * Runmemory -- 实际运行内存大小(B)
 * Status -- 是否测试，0未测试，其余已测试(可映射为8大状态)
 * check 约束goal，但是在mysql中无用→_→
 */
create table `Submission`
(
    `SId`         bigint        unsigned not null auto_increment,
    `UId`         bigint        unsigned not null,
    `PId`         int           unsigned not null,
    `goal`        decimal(5,2)  not null default 0,
    `timestamp`   timestamp(6)  not null default current_timestamp(),
    `compiler`    varchar(10)   not null default "c",
    `Runtime`     int           unsigned not null default 0,
    `Runmemory`   int           unsigned not null default 0,
    `Status`      tinyint       not null default 0,
    primary key (`SId`),
    foreign key (`UId`) references UserInfo(`UId`),
    foreign key (`PId`) references QuestionInfo(`PId`),
    check (`goal` >= 0 and `goal` <= 100)
);

/*
 * 临时问题表，用于普通用户提交问题的临时存储，等待管理员处理
 * 因为需删除，所以TPId未设auto_increment，此处要注意控制
 * Visited -- 查看过0，未查看-1，其余表示已查看次数
 * tpid -- 上传题目时，近取max + 1
 * 具体题目与样例存放在TPID的文件夹下
 */
create table `TempQuestion`
(
    `TPId`        int           unsigned not null,
    `Visited`     tinyint       not null default -1,
    `Title`       nvarchar(30)  not null,
    `Time`        char(5)       not null,
    `Memory`      char(5)       not null,
    `Hard`        int           not null default 3,
    `Type`        int           not null default 0,
    `TestNumber`  int           not null default 10,
    `Note`        varchar(51)   not null,
    `Label`       nvarchar(101) null,
    primary key (`TPId`),
    check (`Hard` >= 1 and `Hard` <= 5),
    check (`Type` >= 0 and `Type` <= 1)
);

/*
 * 触发器设置
 * 需要以下几个：
 * UserInfo 插入、删除时，对UserStatistics的操作
 * QuestionInfo 插入、删除时，对QuestionStatistics的操作
 * submission check约束goal
 * UserInfo 约束UId = 1的超级管理员，不能删，Trust不能改，其余Trust在0～10之间
 */
delimiter $$

/*
 * 当注册一个人的时候，自动插入用户状态表（初始化）
 */
create trigger `User_Insert_Tri` after insert on `UserInfo`
for each row
begin
    if ((new.`Status` != 0 ) and (new.`Status` != 1))
    then
        update `UserInfo` set `Status` = 0 where `UId` = new.`UId`;
    end if;

    insert into UserStatistics (`UId`) values(new.`UId`);
end$$

/*
 * 特别需求：
 *     本触发器限制使用mysql，其余请修改
 * 当删除某个用户时，先删除其状态表中的内容
 * 当该用户为super user时，抛出异常
 */
create trigger `User_Delete_Tri` before delete on `UserInfo`
for each row
begin
    declare msg varchar(200);
    if old.`UId` = 1
    then
        set msg = "禁止删除超级管理员用户!\nForbid deleting the super user!";
        #signal SQLSTATE 'HY000' set message_text = msg;     #version >= 5.5
        update nullTable set err = msg;                    #version < 5.5
    end if;

    delete from `UserStatistics` where `UserStatistics`.`UId` = old.`UId`;
end$$

/*
 * 限制trust在0到10之间，其中Uid为1的用户不可更改
 */
create trigger `User_Update_On_Trust_Tri` before update on `UserInfo`
for each row
begin
    if (new.`UId` = 1)
    then
        set new.`Trust` = 10;
    elseif ((new.`Trust` > 10) or (new.`Trust` < 0))
    then
        set new.`Trust` = 0;
    end if;

    if ((new.`Status` != 0 ) and (new.`Status` != 1))
    then
        set new.`Status` = 0;
    end if;
end$$

/*
 * 添加一条题目时，将PId自动插入问题状态表（初始化）
 */
create trigger `Question_Insert_Tri` after insert on `QuestionInfo`
for each row
begin
    if ((new.`Hard` < 1) or (new.`Hard` > 5))
    then
        update `QuestionInfo` set `Hard` = 3 where `PId` = new.`PId`;
    end if;
    if ((new.`Type` < 0) or (new.`Type` > 1))
    then
        update `QuestionInfo` set `Type` = 0 where `PId` = new.`PId`;
    end if;

    insert into QuestionStatistics (`PId`) values(new.`PId`);
end$$

/*
 * 删除某个问题时，先删除其状态表中的内容
 */
create trigger `Question_Delete_Tri` before delete on `QuestionInfo`
for each row
begin
    delete from `QuestionStatistics` where `QuestionStatistics`.`PId` = old.`PId`;
end$$

/* *
 * 因为后写，使用两个而不是一个，请看下一条注释
 * 目的：防止Hard值越界
 * 为什么TempQuestion表不使用？
 *     1. QuestionInfo表的内容是从TempQuestion表中插入的，insert规则可防止错误的发生
 *     2. 懒Σ( ° △ °||| )︴！！！！！！！
 */
create trigger `QuestionInfo_Check_Insert_Tri` before insert on `QuestionInfo`
for each row
begin
    if ((new.`Hard` > 5) or (new.`Hard` < 1))
    then
        set new.`Hard` = 3;
    end if;
end$$
create trigger `QuestionInfo_Check_Update_Tri` before update on `QuestionInfo`
for each row
begin
    if ((new.`Hard` > 5) or (new.`Hard` < 1))
    then
        set new.`Hard` = 3;
    end if;
end$$

/*
 * 当插入或更新Submission时，若得分不在0～100之间，将其置为-1,表示需重新检查一遍
 * 更新的情况只有当goal为-1时，进允许更新这一列
 * 不能写成insert or update，神奇的报错-_-|||
 */
create trigger `Submission_Check_Insert_Tri` before insert on `Submission`
for each row
begin
    if ((new.`goal` > 100.0) or (new.`goal` < 0.0))
    then
        set new.`goal` = -1;
    end if;
end$$
create trigger `Submission_Check_Update_Tri` before update on `Submission`
for each row
begin
    if ((new.`goal` > 100.0) or (new.`goal` < 0.0))
    then
        set new.`goal` = -1;
    end if;
end$$

delimiter ;

/*插入超级管理员，昵称为JudgeOnline，密码为JudgeOnline，注意大小写*/
insert into `UserInfo`(`Nickname`, `Passwd`, `Email`, `Image`, `Note`, `Trust`, `Status`) values('JudgeOnline', '$2a$08$vh/od2dwgRU4wmDAWFAr.epPeVHp3FbMXOw4VW3ye3iti9xeiE.IC', 'null', '/img/default_image.jpg', 'Super Administrator', 10, 0);

/*创建视图*/
/**
 * 用户信息视图
 * 含以下字段：用户UId、昵称、邮箱、头像、简介、用户权限、经验值、
 *             正确题数、总题数、各语言提交数
 */
create view `User_View`
as
    select
        `UserInfo`.`UId`, `UserInfo`.`Nickname`, `UserInfo`.`Email`,
        `UserInfo`.`Image`, `UserInfo`.`Note`, `UserInfo`.`Trust`,
        `UserStatistics`.`Exp`, `UserStatistics`.`AC` as `Right`,
        (`UserStatistics`.`AC` + `UserStatistics`.`WA` +`UserStatistics`.`PE` +`UserStatistics`.`RE` +`UserStatistics`.`TLE` +`UserStatistics`.`MLE` +`UserStatistics`.`OLE` +`UserStatistics`.`CE`) as `Total`, 
        `UserStatistics`.`C`, `UserStatistics`.`C++`, `UserStatistics`.`C++11`,
        `UserStatistics`.`Java`, `UserStatistics`.`Python` 
    from `UserInfo` inner join  `UserStatistics`
         on `UserInfo`.`UId` = `UserStatistics`.`UId`;

/**问题*/
create view `Question_View`
as
    select 
        `QuestionInfo`.`PId`, `QuestionInfo`.`Title`,.`QuestionInfo`.`Time`,
        `QuestionInfo`.`Memory`, `QuestionInfo`.`Hard`, `QuestionInfo`.`Type`,
        `QuestionInfo`.`TestNumber`, `QuestionInfo`.`Note`, `QuestionInfo`.`Label`,
        (`QuestionStatistics`.`Wrong` + `QuestionStatistics`.`Right`) AS `Total`,
        ROUND(((`QuestionStatistics`.`Right` * 100) / (`QuestionStatistics`.`Wrong` + `QuestionStatistics`.`Right`)), 2) AS `Per`,
        (CASE
            WHEN (`JudgeOnline`.`QuestionInfo`.`Type` = 1) THEN '填空题' ELSE '综合题'
        END) AS `Type_CN`,
        (CASE
            WHEN (`JudgeOnline`.`QuestionInfo`.`Type` = 1) THEN 'filled' ELSE 'fixed'
        END) AS `Type_EN`
    from `QuestionInfo` join `QuestionStatistics`
        where `QuestionInfo`.`PId` = `QuestionStatistics`.`PId`;

/*提交信息视图*/
create view `Submission_View`
as 
    select
        `Submission`.`SId`, `Submission`.`PId`, `Submission`.`UId`,
        `Submission`.`compiler`, `Submission`.`timestamp`,
        `UserInfo`.`Nickname`, `QuestionInfo`.`Title`, `Submission`.`goal`,
        `Submission`.`Runtime`, `Submission`.`Runmemory`,
        (CASE `Submission`.`Status`
            WHEN 0 THEN 'testing'
            WHEN 1 THEN 'AC'
            WHEN 2 THEN 'WA'
            WHEN 3 THEN 'PE'
            WHEN 4 THEN 'RE'
            WHEN 5 THEN 'TLE'
            WHEN 6 THEN 'MLE'
            WHEN 7 THEN 'OLE'
            WHEN 8 THEN 'CE'
            ELSE 'others'
        END) AS `Status`
    from `UserInfo` inner join
         `QuestionInfo` inner join
         `Submission` on `QuestionInfo`.`PId` = `Submission`.`PId` and `UserInfo`.`UId` = `Submission`.`UId`;
