#ifndef CREDIT_HPP
#define CREDIT_HPP

#include "card.hpp"
#include <string>
using namespace std;

// 信用卡：允许透支至固定额度，透支后产生罚息，罚息按系统时间每分钟计算
class Credit : public Card
{
public:
    Credit(string cardId, double balance, string lastInterestTime);
    ~Credit();

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
    string cardType="Credit";
    double balance;                          // 余额，透支时为负数
    double creditLimit = 114514;             // 透支额度上限
    double penaltyRatePerMinute = 0.0001;    // 透支罚息率，每分钟按透支金额计算
    string lastInterestTime;                 // 上次罚息结算时间

    void settlePenalty();                    // 根据透支金额和时间差计算并扣除罚息
    static long long getCurrentTimestamp();
    static long long parseTimeToTimestamp(const string& timeStr);
};

#endif