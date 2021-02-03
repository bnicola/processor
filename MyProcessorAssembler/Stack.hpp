#ifndef STACK_HPP
#define STACK_HPP

class Stack
{
   public:
       Stack();
   
       ~Stack();
   
      void Push(int value);
   
      int Pop();
   
   private:
       
     int stack_[0x10000];
   
     unsigned int stackPointer_;

};
#endif 

