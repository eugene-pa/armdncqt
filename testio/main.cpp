#include <QCoreApplication>
#include <stdio.h>
#include <unistd.h>

//#include <sys/io.h>                 //#include <asm/io.h> - вод-вывод в intel-архитектурах

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




// mapped ввод-вывод в любой архитектуре (для MIPS - единственный способ
// ввод символа
// в случае недоступности файла - исключение int -1
unsigned char inp(quint16 addr)
{
    unsigned char buf[1] = { 0 };
    int fd;
    if ( (fd = open("/dev/port",O_RDONLY|O_NDELAY)) >0 )
    {
        lseek (fd, addr, SEEK_SET);
        read  (fd, buf, 1);
    }
    else
        throw -1;
    return buf[0];
}

// вывод символа
// в случае недоступности файла - исключение int -1
void outp(quint16 addr, unsigned char ch)
{
    int fd;
    if ( (fd = open("/dev/port",O_WRONLY|O_NDELAY) >0 ))
    {
        lseek (fd, addr, SEEK_SET);
        write  (fd, &ch, 1);
    }
    else
        throw -1;
}

int main(int argc, char *argv[])
{
    #define BASEPORT 0x3f8

    QCoreApplication a(argc, argv);


/*
    // вод-вывод в intel-архитектурах
    if (ioperm(BASEPORT, 3, 1))
    {
        perror("ioperm");
        exit(1);
    }

    unsigned char data=0xaa;
    outb(data, BASEPORT);

    usleep(100000);

    //  Read from the status port (BASE+1) and display the result
    printf("data:   %d\n", inb(BASEPORT));
    printf("status: %d\n", inb(BASEPORT + 1));
    printf("+2:     %d\n", inb(BASEPORT + 2));

    //  We don't need the ports anymore
    if (ioperm(BASEPORT, 3, 0))
    {
        perror("ioperm");
        exit(1);
    }
*/

    // ввод-вывод в любой архитектуре (для MIPS - единственный способ
    try
    {
        outp(BASEPORT, 0xaa);
        printf ("\n%x[0,1,2] = %x %x %x\n", BASEPORT, inp(BASEPORT), inp(BASEPORT+1),inp(BASEPORT+2));
    }
    catch (int i)
    {
        printf("\nerror open /dev/port\n");
    }

    exit (0);

    return a.exec();
}
