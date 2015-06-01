#ifndef SVTF_H
#define SVTF_H


#include "../common/logger.h"
#include "sprbase.h"

class Svtf
{
public:
    Svtf();
    ~Svtf();

    // открытые статические функции
    static bool AddTemplate(class IdentityType *);          // проверить шаблон и при необходимости добавить в список шаблонов свойств или методов

private:

    static QHash<QString, class IdentityType *> propertyIds;//  множество шаблонов возможных свойств СВТФ
    static QHash<QString, class IdentityType *> methodIds;  //  множество шаблонов возможных методов СВТФ

};

#endif // SVTF_H
