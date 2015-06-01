#include "strl.h"
#include "properties.h"

QHash<QString, class IdentityType *> Strl::propertyIds;     //  множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Strl::methodIds;       //  множество шаблонов возможных методов РЦ

Strl::Strl()
{

}

Strl::~Strl()
{

}

// проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
bool Strl::AddTemplate(IdentityType * ident)
{
    if (ident->ObjType() == "СТРЛ")
    {
        if (ident->PropType() == "TC")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}

