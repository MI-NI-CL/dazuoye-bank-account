#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include <memory>
#include "../cardtype/card.hpp"

// 用户账户类：管理用户个人信息、登录状态及名下所有银行卡
// 支持文件持久化，用户数据和卡片数据分别存储
class User
{
private:
    std::string usingName;      // 用户名
    std::string password;       // 密码
    std::string name;           // 姓名
    std::string gender;         // 性别
    std::string address;        // 住址
    std::string phone;          // 电话
    std::string idCard;         // 身份证号
    int loginCount;             // 登录次数（活跃度）
    int failedLogin;            // 连续失败次数
    bool isLocked;              // 是否锁定

    // 银行卡列表
    std::vector<std::unique_ptr<Card>> cards;

    friend class System;

public:
    // 构造函数（用于创建新用户或从文件加载后填充）
    User(const std::string& usingName = "", const std::string& password = "",
         const std::string& name = "", const std::string& gender = "",
         const std::string& address = "", const std::string& phone = "",
         const std::string& idCard = "", int loginCount = 0,
         int failedLogin = 0, bool isLocked = false);

    // 文件读写
    bool loadFromFile(const std::string& filename);
    bool saveToFile(const std::string& filename) const;

    // Getter / Setter
    std::string getUsingName() const;
    std::string getPassword() const;
    std::string getName() const;
    std::string getGender() const;
    std::string getAddress() const;
    std::string getPhone() const;
    std::string getIdCard() const;
    int getLoginCount() const;
    int getFailedLogin() const;
    bool getIsLocked() const;

    void incrementLoginCount();
    void incrementFailedLogin();
    void resetFailedLogin();
    void lockAccount();
    void setPassword(const std::string& newPwd);
    void setAddress(const std::string& newAddr);
    void setPhone(const std::string& newTel);

    // 银行卡管理
    void addCard(std::unique_ptr<Card> card);          // 添加一张卡（同时创建卡片文件）
    void removeCard(const std::string& cardId);        // 删除一张卡（同时删除文件）
    void showAllCards() const;
    Card* selectCard(int index);                       // 按索引选择卡
    int getCardCount() const;

    // 显示用户基本信息
    void showUserInfo() const;
};

#endif