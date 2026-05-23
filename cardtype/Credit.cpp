// Credit.cpp - 信用卡实现：允许透支，透支后产生时间累加罚息
#include "Credit.hpp"
#include <iostream>
#include <ctime>
#include <sstream>
#include <iomanip>
using namespace std;

long long Credit::getCurrentTimestamp()
{
    return time(nullptr);
}

long long Credit::parseTimeToTimestamp(const string& timeStr)
{
    struct tm tm = {0};
    istringstream ss(timeStr);
    ss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
    return mktime(&tm);
}

// 核心：透支罚息计算，仅在余额为负时生效
// 罚息 = 透支金额 * 每分钟罚息率 * 经过的分钟数
void Credit::settlePenalty()
{
    long long now = getCurrentTimestamp();
    long long last = parseTimeToTimestamp(lastInterestTime);

    long long minutesPassed = (now - last) / 60;

    if (minutesPassed > 0 && balance < 0) {
        double overdueAmount = -balance;                                 // 透支金额取正
        double penalty = overdueAmount * penaltyRatePerMinute * minutesPassed;

        if (penalty > 0) {
            balance -= penalty;                                          // 罚息增加负债
            cout << "此卡已透支 " << overdueAmount << " 元，"
                 << "经过 " << minutesPassed << " 分钟未还，"
                 << "产生罚息 " << penalty << " 元，"
                 << "当前负债：" << (-balance) << " 元" << endl;
            cout << "再不还就给你换成毫秒算利息！" << endl;
        }
    }

    time_t now_t = now;
    tm* tm_now = localtime(&now_t);
    char newtime[20];
    strftime(newtime, sizeof(newtime), "%Y-%m-%d %H:%M:%S", tm_now);
    lastInterestTime = newtime;
}

Credit::Credit(string cardId, double balance, string lastInterestTime)
{
    this->cardId = cardId;
    this->balance = balance;
    this->lastInterestTime = lastInterestTime;
}

Credit::~Credit() {}

string Credit::getcardType() const
{
    return cardType;
}

bool Credit::deposit(double amount)
{
    settlePenalty();
    
    if (amount > 0) {
        balance += amount;
        cout << "存款成功！";
        if (balance >= 0) {
            cout << "已还清透支，当前余额：" << balance << endl;
        } else {
            cout << "别得意，你还欠着钱嘞：" << (-balance) << " 元" << endl;
        }
        return true;
    } else if (amount == 0) {
        cout << "何意味?" << endl;
        return true;
    } else {
        cout << "咋，你还想亏钱？" << endl;
        return false;
    }
}

bool Credit::withdraw(double amount)
{
    settlePenalty();
    
    if (amount > 0) {
        if (amount <= balance + creditLimit) {
            balance -= amount;
            if (balance >= 0) {
                cout << "取款成功！当前余额：" << balance << endl;
            } else {
                cout << "取款成功！当前透支：" << (-balance) << " / " << creditLimit << " 元，记得还" << endl;
            }
            return true;
        } else {
            cout << "超出透支额度，真把银行当父母啊" << endl;
            return false;
        }
    } else if (amount == 0) {
        cout << "取个0？何意味" << endl;
        return true;
    } else {
        cout << "越取钱越多是吧" << endl;
        return false;
    }
}

double Credit::getBalance() const
{
    return balance;
}

double Credit::calculateInterest() const
{
    long long now = getCurrentTimestamp();
    long long last = parseTimeToTimestamp(lastInterestTime);
    long long minutesPassed = (now - last) / 60;
    
    if (minutesPassed > 0 && balance < 0) {
        double overdueAmount = -balance;
        return overdueAmount * penaltyRatePerMinute * minutesPassed;
    }
    return 0.0;
}

string Credit::getcardId() const
{
    return cardId;
}

string Credit::getLastInterestTime() const
{
    return lastInterestTime;
}

void Credit::updateInterest()
{
    settlePenalty();
}

void Credit::showcardInfo() const
{
    cout << "卡类型：" << cardType << endl;
    cout << "卡号：" << cardId << endl;
    if (balance >= 0) {
        cout << "余额：" << balance << endl;
    } else {
        cout << "透支：" << (-balance) << " / " << creditLimit << endl;
    }
    cout << "罚息率：" << penaltyRatePerMinute << " /分钟" << endl;
}