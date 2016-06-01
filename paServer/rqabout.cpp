#include "rqabout.h"
#include <QSysInfo>


RqAbout::RqAbout()
{
    rq = rqAbout;
}

QByteArray RqAbout::Serialize()
{
    QBuffer buf;
    buf.open(QBuffer::WriteOnly);
    QDataStream out(&buf);

    SerializeBase(out);
    return buf.buffer();
}
