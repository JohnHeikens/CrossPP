#pragma once
#include <vector>
#include "itemStack.h"


//caution! stacks might change while dividing!!!
struct stackDivider {
    std::vector<itemStack*> divideOver = std::vector<itemStack*>();
    itemStack stackHolding = itemStack();
    itemStack originalStack = itemStack();
    std::vector<int> amountsDivided = std::vector<int>();
    bool addStack(itemStack& s);
    bool interactWith(itemStack* candicate);
};
