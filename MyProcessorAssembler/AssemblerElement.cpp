#include "AssemblerElement.hpp"

AssemblerElement::AssemblerElement()
{}


AssemblerElement::~AssemblerElement()
{
}


void AssemblerElement::SetLineCode(QString line)
{
   line_ = line;
}


void AssemblerElement::SetLineComment(QString comment)
{
   lineComment_ = comment;
}


void AssemblerElement::SetLineNumber(quint32 lineNumber)
{
   lineNumber_ = lineNumber;
}

void AssemblerElement::SetLineOpCode(QString opCode)
{
   opCode_ = opCode;
}


void AssemblerElement::SetLineAddress(quint32 address)
{
   address_ = address;
}


void AssemblerElement::SetLineLabel(QString label)
{
   label_ = label;
}


QString AssemblerElement::LineCode()
{
   return line_;
}



QString AssemblerElement::LineComment()
{
   return lineComment_;
}



quint32 AssemblerElement::LineNumber()
{
   return lineNumber_;
}


QString AssemblerElement::OpCode()
{
   return opCode_;
}


quint32 AssemblerElement::LineAddress()
{
   return address_;
}


QString AssemblerElement::LineLabel()
{
   return label_;
}

void AssemblerElement::SetReg1(QString reg1)
{
   register1_ = reg1;
}

void AssemblerElement::SetReg2(QString reg1)
{
   register2_ = reg1;
}

void AssemblerElement::SetDest(QString reg1)
{
   dest_ = reg1;
}

void AssemblerElement::SetLiteral(QString literal)
{
   literal_ = literal;
}



QString AssemblerElement::Register1()
{
   return register1_;
}


QString AssemblerElement::Register2()
{
   return register2_;
}


QString AssemblerElement::Dest()
{
   return dest_;
}


QString AssemblerElement::Literal()
{
   return literal_;
}
