#include "Stack.hpp"


Stack::Stack()
: stackPointer_(0)
{
}
   
Stack::~Stack()
{
}
   
void Stack::Push(int value)
{
    stack_[stackPointer_++] = value;
}
   
int Stack::Pop()
{
   return   stack_[--stackPointer_];
}