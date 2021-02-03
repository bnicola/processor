#include "GlobalSymbolTable.hpp"

GlobalSymbolTable::GlobalSymbolTable()
  :globalSymbolTable_()
{
}

GlobalSymbolTable::~GlobalSymbolTable()
{
   CurrentGlobalSymbolTable::const_iterator it;
   for(it = globalSymbolTable_.begin(); it != globalSymbolTable_.end(); ++it)
   {
      delete it->second;
   }
   globalSymbolTable_.clear();
}

void GlobalSymbolTable::EnterSymbolToTable(QString symbol)
{
   SymbolTable* newFunction = new SymbolTable;
   globalSymbolTable_[symbol] = newFunction;
}

bool GlobalSymbolTable::SymbolExist(QString symbol)
{
    bool retVal = false;
    if (globalSymbolTable_.count(symbol) >= 1)
    {
       retVal = true;
    }
    return retVal;
}

SymbolTable* GlobalSymbolTable::GetSymbolTable(QString symbol)
{
   SymbolTable* currentSymbolTable;
   CurrentGlobalSymbolTable::const_iterator it;
   for(it = globalSymbolTable_.begin(); it != globalSymbolTable_.end(); ++it)
   {
      if (it->first == symbol)
      {
         currentSymbolTable = it->second;
      }
   }
   return currentSymbolTable;
}

