#ifndef __ASSEMBLER_ELEMENT_HPP__
#define __ASSEMBLER_ELEMENT_HPP__
#include <QString>

class AssemblerElement
{
   public:
      AssemblerElement();
      ~AssemblerElement();

      void SetLineCode(QString line);
      void SetLineComment(QString line);
      void SetLineNumber(quint32 lineNumber);
      void SetLineOpCode(QString opCode);
      void SetLineAddress(quint32 address);
      void SetLineLabel(QString label);
      void SetReg1(QString reg1);
      void SetReg2(QString reg1);
      void SetDest(QString reg1);
      void SetLiteral(QString literal);

      QString LineCode();
      QString LineComment();
      quint32 LineNumber();
      QString OpCode();
      quint32 LineAddress();
      QString LineLabel();

      QString Register1();
      QString Register2();
      QString Dest();
      QString Literal();

   private:
      QString label_;
      QString line_;
      QString lineComment_;
      QString opCode_;
      quint32 lineNumber_;
      quint32 address_;

      QString register1_;
      QString register2_;
      QString dest_;
      QString literal_;
};


#endif
