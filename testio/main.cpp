#include <QCoreApplication>
#include <stdio.h>
#include <unistd.h>
#include <sys/io.h>                 //#include <asm/io.h>

#define BASEPORT 0x3f8

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    if (ioperm(BASEPORT, 3, 1))
    {
        perror("ioperm");
        exit(1);
    }

    outb(0, BASEPORT);

    usleep(100000);

    /* Read from the status port (BASE+1) and display the result */
    printf("data:   %d\n", inb(BASEPORT));
    printf("status: %d\n", inb(BASEPORT + 1));

    /* We don't need the ports anymore */
    if (ioperm(BASEPORT, 3, 0))
    {
        perror("ioperm");
        exit(1);
    }

    return a.exec();
}
