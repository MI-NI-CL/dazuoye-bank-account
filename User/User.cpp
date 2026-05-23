// User.cpp - 用户账户实现：文件持久化、银行卡管理
// 用户数据格式: key=value 每行一个字段
// 卡片数据格式: 卡类型|卡号|余额|上次结息时间
#include "User.hpp"
#include "../cardtype/Debit.hpp"
#include "../cardtype/Credit.hpp"
#include "../cardtype/Comp_card.hpp"
#include <fstream>
#include <iostream>
#include <cstdio>  // remove()
#include <ctime>
#include <filesystem>
namespace fs = std::filesystem;

User::User(const std::string& usingName, const std::string& password,
           const std::string& name, const std::string& gender,
           const std::string& address, const std::string& phone,
           const std::string& idCard, int loginCount,
           int failedLogin, bool isLocked)
    : usingName(usingName), password(password), name(name), gender(gender),
      address(address), phone(phone), idCard(idCard), loginCount(loginCount),
      failedLogin(failedLogin), isLocked(isLocked) {}

// 从文件加载用户信息及所有关联的银行卡
// 用户文件每行为 key=value 格式，末尾为卡片文件名列表
bool User::loadFromFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file) return false;

    std::string line;
    auto getValue = [&]() {
        std::getline(file, line);
        return line.substr(line.find('=') + 1);
    };

    usingName = getValue();
    password  = getValue();
    name      = getValue();
    gender    = getValue();
    address   = getValue();
    phone     = getValue();
    idCard    = getValue();
    loginCount = std::stoi(getValue());
    failedLogin = std::stoi(getValue());
    isLocked    = (std::stoi(getValue()) != 0);

    int cardCount = std::stoi(getValue());
    cards.clear();
    for (int i = 0; i < cardCount; ++i) {
        std::getline(file, line);
        std::string cardFileName = line;
        std::string cardPath = "cards/" + cardFileName;
        std::ifstream cf(cardPath);
        if (!cf) continue;

        std::string cardType, cardId, lastTime;
        double balance;
        std::getline(cf, cardType, '|');
        std::getline(cf, cardId, '|');
        cf >> balance;
        cf.ignore();
        std::getline(cf, lastTime);
        cf.close();

        std::unique_ptr<Card> newCard;
        try {
            newCard = makeCard(cardType, cardId, balance, lastTime);
        } catch (...) {
            continue;
        }
        if (newCard) cards.push_back(std::move(newCard));
    }
    file.close();
    return true;
}

// 保存用户基本信息和卡片文件名列表到用户文件
// 注意：卡片余额和利息时间通过 system.cpp 中退出登录时单独写入卡片文件
bool User::saveToFile(const std::string& filename) const
{
    // 确保目录存在
    fs::create_directories("users");
    std::ofstream file(filename);
    if (!file) return false;

    file << "usingName=" << usingName << "\n";
    file << "password=" << password << "\n";
    file << "name=" << name << "\n";
    file << "gender=" << gender << "\n";
    file << "address=" << address << "\n";
    file << "phone=" << phone << "\n";
    file << "idCard=" << idCard << "\n";
    file << "loginCount=" << loginCount << "\n";
    file << "failedLogin=" << failedLogin << "\n";
    file << "isLocked=" << (isLocked ? 1 : 0) << "\n";
    file << "cardCount=" << cards.size() << "\n";
    for (const auto& cp : cards)
        file << usingName + "_" + cp->getcardId() + ".txt" << "\n";
    file.close();
    return true;
}

// ---------- Getter ----------
std::string User::getUsingName() const { return usingName; }
std::string User::getPassword() const { return password; }
std::string User::getName() const { return name; }
std::string User::getGender() const { return gender; }
std::string User::getAddress() const { return address; }
std::string User::getPhone() const { return phone; }
std::string User::getIdCard() const { return idCard; }
int User::getLoginCount() const { return loginCount; }
int User::getFailedLogin() const { return failedLogin; }
bool User::getIsLocked() const { return isLocked; }

// ---------- Setter / 状态修改 ----------
void User::incrementLoginCount() { ++loginCount; }
void User::incrementFailedLogin() { ++failedLogin; if (failedLogin >= 3) isLocked = true; }
void User::resetFailedLogin() { failedLogin = 0; isLocked = false; }
void User::lockAccount() { isLocked = true; }
void User::setPassword(const std::string& newPwd) { password = newPwd; }
void User::setAddress(const std::string& newAddr) { address = newAddr; }
void User::setPhone(const std::string& newTel) { phone = newTel; }

// ---------- 银行卡管理 ----------
void User::addCard(std::unique_ptr<Card> card)
{
    // 保存卡片文件
    fs::create_directories("cards");
    std::string cardFile = "cards/" + usingName + "_" + card->getcardId() + ".txt";
    std::ofstream cf(cardFile);
    if (cf) {
        time_t now = time(nullptr);
        tm* tm_now = localtime(&now);
        char newtime[20];
        strftime(newtime, sizeof(newtime), "%Y-%m-%d %H:%M:%S", tm_now);
        cf << card->getcardType() << "|"
           << card->getcardId() << "|"
           << card->getBalance() << "|"
           << newtime << std::endl;
        cf.close();
    }
    cards.push_back(std::move(card));
    // 更新用户文件（卡片列表变化）
    saveToFile("users/" + usingName + ".txt");
    std::cout << "添加银行卡成功\n";
}

void User::removeCard(const std::string& cardId)
{
    for (auto it = cards.begin(); it != cards.end(); ++it) {
        if ((*it)->getcardId() == cardId) {
            // 删除物理文件
            std::string cardFile = "cards/" + usingName + "_" + cardId + ".txt";
            std::remove(cardFile.c_str());
            cards.erase(it);
            saveToFile("users/" + usingName + ".txt");
            std::cout << "注销银行卡成功\n";
            return;
        }
    }
    std::cout << "未找到该卡号\n";
}

void User::showAllCards() const
{
    if (cards.empty()) {
        std::cout << "暂无银行卡\n";
        return;
    }
    for (size_t i = 0; i < cards.size(); ++i) {
        std::cout << "[" << i+1 << "] ";
        cards[i]->showcardInfo();
        std::cout << "\n";
    }
}

Card* User::selectCard(int index)
{
    if (index >= 0 && index < static_cast<int>(cards.size()))
        return cards[index].get();
    return nullptr;
}

int User::getCardCount() const { return static_cast<int>(cards.size()); }

// 显示用户信息
void User::showUserInfo() const
{
    std::cout << "用户名: " << usingName << "\n"
              << "姓名: " << name << "\n"
              << "性别: " << gender << "\n"
              << "住址: " << address << "\n"
              << "电话: " << phone << "\n"
              << "身份证号: " << idCard << "\n"
              << "登录次数: " << loginCount << "\n";
}