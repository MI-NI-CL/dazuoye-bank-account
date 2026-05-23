#ifndef DEBIT_HPP
#define DEBIT_HPP

#include "card.hpp"
#include <string>
using namespace std;

// 储蓄卡：不允许透支，存款有利息，利息按系统时间每分钟结算一次
class Debit : public Card
{
public:
    Debit(string cardId, double balance, string lastInterestTime);
    ~Debit();

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
    string cardType="Debit";
    double balance;
    double interestRatePerMinute = 0.0001;  // 每分钟利率，基于系统时间差计算
    string lastInterestTime;                // 上次结息时间，用于计算时间差

    void settleInterest();                  // 根据时间差结算利息并更新余额
    static long long getCurrentTimestamp();         // 获取当前Unix时间戳
    static long long parseTimeToTimestamp(const string& timeStr);  // "YYYY-MM-DD HH:MM:SS" -> 时间戳
};

#endif