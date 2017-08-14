#include "pasender.h"

PaSender::PaSender()
{

}

// пересылаем сообщение в подключенные слоты
void PaSender::SendMsg(paMessage *msg)
{
    emit SendMessage(msg);
}
