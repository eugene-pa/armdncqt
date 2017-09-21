#include "objmain.h"
#include "../common/rsasinc.h"

// класс предназначен для организации взаимодействия рабочих потоков с GUI
// используется для приема сообщений от интерфейсного класса PaSender
// используется при формировании функтора обратного вызова в рабочих потоках,
// в частности, в потоке threadpolling

ObjMain::ObjMain(QObject *parent) : QObject(parent)
{

}

extern int MakeData();
extern unsigned char dataOut[];
extern RsAsinc * pRS;
void ObjMain::GetMessage(PaMessage *msg)
{
    if (msg->GetAction() == PaMessage::eventReceive)
    {
        // ТЕСТИРУЕМ RS: если делать передачу непосредственно в рабочем потоке,
        // передача проходит, но получаю сообщение об ошибке при передаче параметра в функцию С
        //int l = MakeData();
        //pRS->Send(dataOut, l);
    }
    if (msg->GetText().length() > 0)
        threadsafecout(msg->GetText());
    delete msg;
}

void ObjMain::Quit()
{
    Cleanup();
}

