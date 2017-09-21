#ifndef JSONIREADER_H
#define JSONIREADER_H

void Log (std::wstring s);                                          // лог

class JsoniReader
{
public:
    JsoniReader(const char * file, int addr);
private:
    const char * file;
    std::wstring wfilename;                             // имя в виде wstring для отображения
    void Read();
    int  addr;                                          // адрес станции считанный с переключателей
};

#endif // JSONIREADER_H
