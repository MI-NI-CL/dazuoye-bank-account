
// card.cpp - 银行卡工厂函数，根据卡类型字符串创建对应子类实例
#include "card.hpp"
#include "Comp_card.hpp"
#include "Credit.hpp"
#include "Debit.hpp"
#include <memory>
#include <stdexcept>
using namespace std;

unique_ptr<Card> makeCard(const string& cardType, const string& cardId, double balance, const string& lastInterestTime) {
    if (cardType == "Comp_card") {
        return make_unique<Comp_card>(cardId, balance, lastInterestTime);
    } else if (cardType == "Credit") {
        return make_unique<Credit>(cardId, balance, lastInterestTime);
    } else if (cardType == "Debit") {
        return make_unique<Debit>(cardId, balance, lastInterestTime);
    } else {
        throw invalid_argument("未知卡类型: " + cardType);
    }
}
    

