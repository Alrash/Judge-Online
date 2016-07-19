<h1 align="center">Simple Analysis</h1>

##大前提
大家创造

##需求功能
everyone:
> 1. 登录、注册、找回密码、修改个人信息       管理、恶意使用
> 2. 上传--头像图片、源代码(提交时) 下载--测试样例
> 3. 聊天室(各页面的独立窗口)
> 4. 推荐在线阅读，提供离线版本？
> 5. 题目星级评定、用户等级系统
> 6. 信息显示--个人信息、提交程序测试信息(自己)、各个题目提交信息
> 7. 提交页面程序、查看提交(包括源代码)
> 8. 上传题目，修改已有阅读
> 0. 测试引擎(多线程)、题目推荐有限算法、自动生成输入样例引擎 **重要**
> 0. 分页显示搜索结果

管理员:
> 1. 对于上传题目的通过与否，删除、忽略题目(已有、未有)
> 2. 管理聊天室、在线阅读(版本还原)
> 3. 导入信息，从execl里


##需求页面
> 1. index
> 2. Sign in and Register(one or two)
> 3. show problem
> 4. search
> 5. information(including submitions)
> 6. show details of one submition
> 7. chatroom of one problem
> 8. knowledge
> 9. the others


##用户提交后需记录的信息
> 1. who
> 2. which problem
> 3. status of this problem
> 3.1. For problem of database, write down 5 status or two status which is true or false and total count pluses 1.
> 3.2. For user, record the detail of this submit, including status, goal of each test and tid(test or submit id).
