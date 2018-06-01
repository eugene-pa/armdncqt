#include <QCoreApplication>
#include <stdio.h>
#include <unistd.h>

//#include <sys/io.h>                 //#include <asm/io.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define BASEPORT 0x3f8

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    unsigned char data=0xaa;
/*
    if (ioperm(BASEPORT, 3, 1))
    {
        perror("ioperm");
        exit(1);
    }

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

    int fd1=-1;
    fd1=open("/dev/port",O_RDWR|O_NDELAY);
    if ( fd1 < 0 )
    {
        perror("error open /dev/port");
        exit(1);
    }

    unsigned char buf1[16], buf2[16];
    lseek (fd1, BASEPORT, SEEK_SET);
    write (fd1,&data,1);

    lseek (fd1, BASEPORT, SEEK_SET);
    read  (fd1, buf1, 3);
    printf("data:   %d\n", buf1[0]);
    printf("status: %d\n", buf1[1]);
    printf("status: %d\n", buf1[2]);

    exit (0);

    return a.exec();
}
