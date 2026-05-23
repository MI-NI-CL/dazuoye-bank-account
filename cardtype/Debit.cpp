// Debit.cpp - 储蓄卡实现：存款有利息，不允许透支
// 利息计算基于系统时间，每次操作前自动结算时间差对应的利息
#include "Debit.hpp"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

long long Debit::getCurrentTimestamp()
{
    return time(nullptr);
}

long long Debit::parseTimeToTimestamp(const string& timeStr)
{
    struct tm tm = {0};
    istringstream ss(timeStr);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

// 核心：根据系统时间差结算利息
// 利息 = 余额 * 每分钟利率 * 经过的分钟数
void Debit::settleInterest()
{
    long long now = getCurrentTimestamp();                    // 当前Unix时间戳
    long long last = parseTimeToTimestamp(lastInterestTime);  // 上次结息时间

    long long minutesPassed = (now - last) / 60;              // 计算经过的分钟数

    if (minutesPassed > 0) {
        double interest = balance * interestRatePerMinute * minutesPassed;
        if (interest > 0) {
            balance += interest;
            cout << "距离上次动账经过 " << minutesPassed << " 分钟，获得利息 "
                 << interest << " 元，当前余额：" << balance << endl;
        }
    }

    // 更新结息时间为当前系统时间
    time_t now_t = now;
    tm* tm_now = localtime(&now_t);
    char newtime[20];
    strftime(newtime, sizeof(newtime), "%Y-%m-%d %H:%M:%S", tm_now);
    lastInterestTime = newtime;
}

Debit::Debit(string cardId, double balance, string lastInterestTime)
{
    this->cardId = cardId;
    this->balance = balance;
    this->lastInterestTime = lastInterestTime;
}

Debit::~Debit() {}

string Debit::getcardType() const
{
    return cardType;
}

bool Debit::deposit(double amount)
{
    settleInterest();
    
    if (amount > 0) {
        balance += amount;
        cout << "存款成功！当前余额：" << balance << endl;
        return true;
    } else {
        cout << "存款金额必须大于0" << endl;
        return false;
    }
}

bool Debit::withdraw(double amount)
{
    settleInterest();
    
    if (amount > 0 && amount <= balance) {
        balance -= amount;
        cout << "取款成功！当前余额：" << balance << endl;
        return true;
    } else {
        cout << "余额不足" << endl;
        return false;
    }
}

double Debit::getBalance() const
{
    return balance;
}

double Debit::calculateInterest() const
{
    long long now = getCurrentTimestamp();
    long long last = parseTimeToTimestamp(lastInterestTime);
    long long minutesPassed = (now - last) / 60;
    
    if (minutesPassed > 0) {
        return balance * interestRatePerMinute * minutesPassed;
    }
    return 0.0;
}

string Debit::getcardId() const
{
    return cardId;
}

string Debit::getLastInterestTime() const
{
    return lastInterestTime;
}

void Debit::updateInterest()
{
    settleInterest();
}

void Debit::showcardInfo() const
{
    cout << "卡类型：" << cardType << endl;
    cout << "卡号：" << cardId << endl;
    cout << "余额：" << balance << endl;
    cout << "利率：" << interestRatePerMinute << " /分钟" << endl;
}