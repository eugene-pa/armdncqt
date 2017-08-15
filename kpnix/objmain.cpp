#include "objmain.h"

ObjMain::ObjMain(QObject *parent) : QObject(parent)
{

}

void ObjMain::GetMessage(PaMessage *msg)
{
    threadsafecout(msg->GetText());
    delete msg;
}

void ObjMain::Quit()
{
    Cleanup();
}

