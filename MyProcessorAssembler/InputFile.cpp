#include "InputFile.hpp"
#include <QFile>



InputFile::InputFile()
   : inputBuffer_(NULL),
     currentPosition_(0)
{
   inputBuffer_ = new unsigned char [0x1000000];
}

InputFile::~InputFile()
{
   delete [] inputBuffer_;
}

bool InputFile::OpenFile(QString fileName)
{
   int x = 0;
   QFile file(fileName);
   bool fileOpened = file.open(QIODevice::ReadOnly | QIODevice::Text);
   if (fileOpened)
   {
      while (!file.atEnd()) 
      {
         QByteArray line = file.readLine();
         memcpy(&inputBuffer_[x], line.data(), line.size());
         x += line.size();
      }
      inputBuffer_[x] = 0xEF; //end of file token;
   }
   //file.close();
   return fileOpened;
}


void InputFile::CloseFile(QString fileName)
{
   memset(inputBuffer_, 0, 0x100000);
}

unsigned char InputFile::GetCurrentCharacter()
{
   return inputBuffer_[currentPosition_];
}


unsigned char InputFile::GetNextCharacter()
{
  return inputBuffer_[++currentPosition_];
}

unsigned int InputFile::GetCurrentCharacterLocation()
{
   return currentPosition_;
}

void InputFile::SetCurrentCharacterLocation(unsigned int position)
{
   currentPosition_ = position;
}


