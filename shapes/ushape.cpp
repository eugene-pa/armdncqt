#include "ushape.h"

UShape::UShape()
{

}

UShape::~UShape()
{

}


bool UShape::GetTypeName(QString& ret, UShapeType type)
{
    ret = "";
    switch (type)
    {
        case U_Box:     ret   = "BOX"  ;break;
        case U_Przd:    ret   = "PRZD" ; break;
        case U_Segment: ret   = "RC"   ; break;
        case U_Angle:   ret   = "ANGLE"; break;
        case U_Svtf:    ret   = "SVTF" ; break;
        case U_Strl:    ret   = "STRL" ; break;
        case U_Trnsp:   ret   = "MODE" ; break;
        case U_Name:    ret   = "NAME" ; break;
        case U_Text:    ret   = "TEXT" ; break;
        default:
            return false;
    }
    return true;
}

// статическая функция получения типа примитива по его имени
UShapeType UShape::GetTypeByName (QString typeName)
{
/*
    switch (typeName)
    {
        case   "BOX":        return UShapeType.BOX_COD;

        case   ShapeNamePrzd:       return ShapeType.Przd;
        case   ShapeNameSegment:    return ShapeType.Segment;
        case   ShapeNameAngle:      return ShapeType.Angle;
        case   ShapeNameSvtf:       return ShapeType.Svtf;
        case   ShapeNameStrl:       return ShapeType.Strl;
        case   ShapeNameTrnsp:      return ShapeType.Trnsp;
        case   ShapeNameNameSt:     return ShapeType.Name;
        case   ShapeNameText:       return ShapeType.Text;

    }
*/
    return U_Null;
}
