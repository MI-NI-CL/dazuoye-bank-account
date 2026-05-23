#ifndef CARD_HPP
#define CARD_HPP

#include <string>
#include <memory>
using namespace std;

// 银行卡抽象基类，定义存款、取款、利息计算等统一接口
// 储蓄卡、信用卡、复合卡均继承此类
class Card {
public:
    virtual ~Card() = default;
    virtual string getcardType() const = 0;
    virtual bool deposit(double amount) = 0;
    virtual bool withdraw(double amount) = 0;
    virtual double getBalance() const = 0;
    virtual double calculateInterest() const = 0;
    virtual string getcardId() const = 0;
    virtual string getLastInterestTime() const = 0;
    virtual void updateInterest() = 0;
    virtual void showcardInfo() const = 0;
};

unique_ptr<Card> makeCard(const string& cardType, const string& cardId, double balance, const string& lastInterestTime);

#endif