#include "svtf.h"
#include "properties.h"

QHash<QString, class IdentityType *> Svtf::propertyIds;     //  множество шаблонов возможных свойств РЦ
QHash<QString, class IdentityType *> Svtf::methodIds;       //  множество шаблонов возможных методов РЦ

Svtf::Svtf()
{

}

Svtf::~Svtf()
{

}

// проверить шаблон и при необходимости добавить в список шаблонов свойств или методов
bool Svtf::AddTemplate(IdentityType * ident)
{
    if (ident->ObjType() == "СВТФ")
    {
        if (ident->PropType() == "TC")
            propertyIds[ident->Name()] = ident;
        else
            methodIds[ident->Name()] = ident;
        return true;
    }
    return false;
}
