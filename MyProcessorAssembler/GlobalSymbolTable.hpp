#ifndef GLOBAL_SYMBOL_TABLE_HPP_
#define GLOBAL_SYMBOL_TABLE_HPP_

#include "SymbolTable.hpp"
#include <QString>
#include <map>


class GlobalSymbolTable
{
public :
   GlobalSymbolTable();

   ~GlobalSymbolTable();

   void EnterSymbolToTable(QString symbol);

   bool SymbolExist(QString symbol);

   SymbolTable* GetSymbolTable(QString symbol);

private:

   typedef std::map<QString, SymbolTable*> CurrentGlobalSymbolTable;
   CurrentGlobalSymbolTable globalSymbolTable_;
};

#endif