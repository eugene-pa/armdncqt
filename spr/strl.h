#ifndef STRL_H
#define STRL_H

#include "../common/logger.h"
#include "sprbase.h"



class Strl
{
public:
    Strl();
    ~Strl();

    // открытые статические функции
    static bool AddTemplate(class IdentityType *);          // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов

private:

    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств СТРЛ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов СТРЛ

};

#endif // STRL_H
