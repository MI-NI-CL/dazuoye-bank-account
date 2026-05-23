#ifndef SYSTEM_HPP
#define SYSTEM_HPP

#include <memory>
#include <string>

class User;  // 前向声明

// ATM银行系统核心类：管理主菜单流程、用户登录注册、卡操作调度
class System
{
public:
    System();
    ~System();

    void run();                          // 启动主循环，显示主菜单直至退出

private:
    std::unique_ptr<User> currentUser;   // 当前登录用户，未登录时为nullptr

    void showMainMenu();                 // 显示主菜单（登录/注册/统计/退出）
    void login();                        // 验证用户名密码，失败3次锁定
    void registerUser();                 // 注册新用户并写入文件
    void userMenu();                     // 登录后的用户操作菜单
    double calculateAverageActivity();   // 统计所有用户的平均登录次数
};

#endif