#ifndef INPUTFILE_HPP
#define INPUTFILE_HPP

#include <QString>



class InputFile
{

public :
    InputFile();

    ~InputFile();

    bool OpenFile(QString fileName);
    
    void CloseFile(QString fileName);

    unsigned char GetCurrentCharacter();

    unsigned char GetNextCharacter();

    unsigned int GetCurrentCharacterLocation();
    
    void SetCurrentCharacterLocation(unsigned int position);

private:

   unsigned char* inputBuffer_;

   unsigned int currentPosition_;

};



#endif
