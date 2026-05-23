#ifndef COMP_CARD_HPP
#define COMP_CARD_HPP

#include "card.hpp"
#include <string>
using namespace std;

// 复合卡：兼具储蓄和信用功能。正余额时享有高利率收益，透支时产生罚息
// 利率 (0.6/分钟) 为刻意设计的高回报率，体现复合卡的高风险高收益特性
class Comp_card : public Card
{
public:
    Comp_card(string cardId, double balance, string lastInterestTime);
    ~Comp_card();

    string getcardType() const override final;
    bool deposit(double amount) override final;
    bool withdraw(double amount) override final;
    double getBalance() const override final;
    double calculateInterest() const override final;
    string getcardId() const override final;
    string getLastInterestTime() const override final;
    void updateInterest() override final;
    void showcardInfo() const override final;

private:
    string cardId;
    string cardType="Comp_card";
    double balance;                          // 余额，正数为储蓄、负数为透支
    double creditLimit = 114514;             // 透支额度上限
    double interestRatePerMinute = 0.6;      // 正余额时的利率（高收益设计）
    double penaltyRatePerMinute = 0.0001;    // 透支时的罚息率
    string lastInterestTime;                 // 上次结息时间

    void settleInterestAndPenalty();         // 根据余额正负分别结算利息或罚息
    static long long getCurrentTimestamp();
    static long long parseTimeToTimestamp(const string& timeStr);
};

#endif