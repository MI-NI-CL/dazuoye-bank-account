// system.cpp - ATM系统核心逻辑：主菜单、登录注册、卡操作调度、彩蛋触发
// 前置宏定义解决 Windows.h 与 winsock 的 byte 类型冲突
#define _WINSOCKAPI_
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include "system.hpp"
#include "../User/User.hpp"
#include "../cardtype/card.hpp"
#include "../cardtype/Debit.hpp"
#include "../cardtype/Credit.hpp"
#include "../cardtype/Comp_card.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <windows.h>
#include <shellapi.h>
#include <limits>
#include <string>

namespace fs = std::filesystem;

// 彩蛋函数：调用Windows图片查看器弹出图片
void showRealImage(const char* imagePath)
{
    ShellExecuteA(NULL, "open", imagePath, NULL, NULL, SW_SHOWNORMAL);
}

System::System() = default;
System::~System() = default;

void System::showMainMenu()
{
    std::cout << "\n========== ATM 银行系统 ==========\n";
    std::cout << "1. 登录\n";
    std::cout << "2. 注册\n";
    std::cout << "3. 统计平均活跃度\n";
    std::cout << "4. 退出\n";
    std::cout << "请选择: ";
}

void System::login()
{
    std::string name, pwd;
    std::cout << "用户名: ";
    std::cin >> name;
    std::cout << "密码: ";
    std::cin >> pwd;

    // 尝试加载用户
    User temp;
    if (!temp.loadFromFile("users/" + name + ".txt")) {
        std::cout << "用户不存在！\n";
        return;
    }

    if (temp.getIsLocked()) {
        std::cout << "账户已被锁定，请稍后再试。\n";
        return;
    }

    if (temp.getPassword() == pwd) {
        // 密码正确：构建currentUser并迁移已加载的银行卡数据
        currentUser = std::make_unique<User>(
            temp.getUsingName(), temp.getPassword(), temp.getName(),
            temp.getGender(), temp.getAddress(), temp.getPhone(),
            temp.getIdCard(), temp.getLoginCount(),
            temp.getFailedLogin(), temp.getIsLocked()
        );

        for (auto& card : temp.cards) {
            currentUser->cards.push_back(std::move(card));
        }

        currentUser->resetFailedLogin();
        currentUser->incrementLoginCount();
        currentUser->saveToFile("users/" + name + ".txt");
        std::cout << "登录成功！\n";
        userMenu();
    } else {
        // 密码错误：累加失败次数，满3次自动锁定
        temp.incrementFailedLogin();
        temp.saveToFile("users/" + name + ".txt");
        std::cout << "密码错误！";
        if (temp.getFailedLogin() >= 3)
            std::cout << " 由于连续失败，账户已被锁定。";
        std::cout << std::endl;
    }
}

void System::registerUser()
{
    std::string usingName, password, name, gender, address, phone, idCard;
    std::cout << "请输入用户名: ";
    std::cin >> usingName;
    // 检查是否已存在
    if (fs::exists("users/" + usingName + ".txt")) {
        std::cout << "用户名已存在！\n";
        return;
    }
    std::cout << "密码: "; std::cin >> password;
    std::cout << "姓名: "; std::cin >> name;
    std::cout << "性别: "; std::cin >> gender;
    std::cout << "住址: "; std::cin.ignore(); std::getline(std::cin, address);
    std::cout << "电话: "; std::cin >> phone;
    std::cout << "身份证号: "; std::cin >> idCard;

    User newUser(usingName, password, name, gender, address, phone, idCard);
    newUser.saveToFile("users/" + usingName + ".txt");
    // 将用户名添加到全局用户列表文件（用于统计）
    std::ofstream userList("users.txt", std::ios::app);
    if (userList) userList << usingName << "\n";
    std::cout << "注册成功！请登录。\n";
}

double System::calculateAverageActivity()
{
    std::ifstream userList("users.txt");
    if (!userList) return 0.0;

    int total = 0, count = 0;
    std::string uname;
    while (std::getline(userList, uname)) {
        if (uname.empty()) continue;
        User u;
        if (u.loadFromFile("users/" + uname + ".txt")) {
            total += u.getLoginCount();
            ++count;
        }
    }
    if (count == 0) return 0.0;
    return static_cast<double>(total) / count;
}

void System::userMenu()
{
    if (!currentUser) return;

    while (true) {
        std::cout << "\n========== 用户菜单 ==========\n";
        std::cout << "1. 显示个人信息\n";
        std::cout << "2. 修改密码\n";
        std::cout << "3. 修改住址\n";
        std::cout << "4. 修改电话\n";
        std::cout << "5. 查看所有银行卡\n";
        std::cout << "6. 添加银行卡\n";
        std::cout << "7. 注销银行卡\n";
        std::cout << "8. 操作银行卡（存款/取款/查询/利息）\n";
        std::cout << "9. 查看个人活跃度\n";
        std::cout << "0. 退出登录\n";
        std::cout << "请选择: ";
        int choice;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效选择\n";
            continue;
        }

        switch (choice) {
            case 1:
                currentUser->showUserInfo();
                break;
            case 2: {
                std::string newPwd;
                std::cout << "新密码: "; std::cin >> newPwd;
                currentUser->setPassword(newPwd);
                currentUser->saveToFile("users/" + currentUser->getUsingName() + ".txt");
                std::cout << "密码修改成功\n";
                break;
            }
            case 3: {
                std::string newAddr;
                std::cout << "新地址: "; std::cin.ignore(); std::getline(std::cin, newAddr);
                currentUser->setAddress(newAddr);
                currentUser->saveToFile("users/" + currentUser->getUsingName() + ".txt");
                std::cout << "地址修改成功\n";
                break;
            }
            case 4: {
                std::string newTel;
                std::cout << "新电话: "; std::cin >> newTel;
                currentUser->setPhone(newTel);
                currentUser->saveToFile("users/" + currentUser->getUsingName() + ".txt");
                std::cout << "电话修改成功\n";
                break;
            }
            case 5:
                currentUser->showAllCards();
                break;
            case 6: {
                // 添加银行卡（交互）
                int type;
                std::cout << "卡类型: 1-储蓄卡 2-信用卡 3-复合卡: ";
                if (!(std::cin >> type)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效类型\n";
                    break;
                }
                std::string cardId;
                double balance;
                std::cout << "卡号: "; std::cin >> cardId;
                std::cout << "初始余额: ";
                std::string balInput;
                std::cin >> balInput;
                try {
                    balance = std::stod(balInput);
                } catch (...) {
                    std::cout << "无效余额\n";
                    break;
                }
                std::unique_ptr<Card> newCard;
                if (type == 1)
                    newCard = std::make_unique<Debit>(cardId, balance, "2000-01-01 00:00:00");
                else if (type == 2)
                    newCard = std::make_unique<Credit>(cardId, balance, "2000-01-01 00:00:00");
                else if (type == 3)
                    newCard = std::make_unique<Comp_card>(cardId, balance, "2000-01-01 00:00:00");
                else {
                    std::cout << "无效类型\n";
                    break;
                }
                currentUser->addCard(std::move(newCard));
                break;
            }
            case 7: {
                std::string cid;
                std::cout << "请输入要注销的卡号: ";
                std::cin >> cid;
                currentUser->removeCard(cid);
                break;
            }
            case 8: {  // 银行卡操作子菜单
                if (currentUser->getCardCount() == 0) {
                    std::cout << "暂无银行卡\n";
                    break;
                }
                currentUser->showAllCards();
                int idx;
                std::cout << "请选择卡编号: ";
                if (!(std::cin >> idx)) {
                    std::cin.clear();
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                    std::cout << "无效编号\n";
                    break;
                }
                Card* c = currentUser->selectCard(idx - 1);
                if (!c) {
                    std::cout << "无效编号\n";
                    break;
                }
                c->updateInterest();
                bool back = false;
                while (!back) {
                    std::cout << "\n==== 卡操作 ====\n";
                    std::cout << "1. 存款\n2. 取款\n3. 查询余额\n4. 计算利息\n5. 返回\n";
                    int op;
                    // 卡操作菜单输入错误处理
                    if (!(std::cin >> op)) {
                        std::cin.clear();
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        std::cout << "无效选项\n";
                        continue;
                    }
                    switch (op) {
                        case 1: {
                            std::cout << "存款金额: ";
                            std::string input;
                            std::cin >> input;
                            try {
                                double amt = std::stod(input);
                                c->deposit(amt);
                            } catch (...) {
                                std::cout << "无效金额\n";
                            }
                            break;
                        }
                        case 2: {
                            std::cout << "取款金额: ";
                            std::string input;
                            std::cin >> input;
                            // 彩蛋：取款金额超过19位（超出double精度范围），弹出彩蛋图片
                            if (input.size() > 19) {
                                showRealImage("caidan.png");
                                std::cout << "取款失败\n";
                                break;
                            }
                            try {
                                double amt = std::stod(input);
                                c->withdraw(amt);
                            } catch (...) {
                                std::cout << "无效金额\n";
                            }
                            break;
                        }
                        case 3:
                            std::cout << "当前余额: " << c->getBalance() << "\n";
                            break;
                        case 4:
                            std::cout << "利息: " << c->calculateInterest() << "\n";
                            break;
                        case 5:
                            back = true;
                            break;
                        default:
                            std::cout << "无效选项\n";
                    }
                }
                break;
            }
            case 9:
                std::cout << "您的登录次数（个人活跃度）: " << currentUser->getLoginCount() << "\n";
                break;
            case 0:
                currentUser->saveToFile("users/" + currentUser->getUsingName() + ".txt");
                // 保存所有卡片数据到文件
                for (const auto& card : currentUser->cards) {
                    fs::create_directories("cards");
                    std::string cardFile = "cards/" + currentUser->getUsingName() + "_" + card->getcardId() + ".txt";
                    std::ofstream cf(cardFile);
                    if (cf) {
                        cf << card->getcardType() << "|"
                           << card->getcardId() << "|"
                           << card->getBalance() << "|"
                           << card->getLastInterestTime() << std::endl;
                    }
                }
                currentUser.reset();
                std::cout << "已退出登录\n";
                return;
            default:
                std::cout << "无效选择\n";
        }
    }
}

void System::run()
{
    while (true) {
        showMainMenu();
        int choice;
        // 主菜单输入错误处理
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "无效选择\n";
            continue;
        }
        switch (choice) {
            case 1: login(); break;
            case 2: registerUser(); break;
            case 3:
                std::cout << "所有用户的平均活跃度: " << calculateAverageActivity() << "\n";
                break;
            case 4:
                std::cout << "再见！\n";
                return;
            default:
                std::cout << "无效选择\n";
        }
    }
}