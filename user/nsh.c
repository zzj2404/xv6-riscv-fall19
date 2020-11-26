//参考：https://github.com/monkey2000/xv6-riscv-fall19
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"


int getcmd(char *buf, int nbuf)
{
  fprintf(2, "@ ");
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  *strchr(buf, '\n') = '\0';
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

// replace "|" with "\0"
//if '|' not exist, return NULL
//else return right side of '|'
char *parsepipe(char *p)
{
    while (*p != '\0' && *p != '|')
        p++;
    if (*p == '\0')
        return 0;
    *p = '\0';
    return p + 1;
}

// trim spaces on both side
char *trim(char *c)
{
    char *e = c;
    while (*c == ' ') *(c++) = '\0';

    while (*(e+1)) e++;
    while (*e == ' ') *(e--) = '\0';

    return c;
}

void redirect(int k, int pd[])
{
    close(k);
    dup(pd[k]);
    close(pd[0]);
    close(pd[1]);
}

char cmd_buf[1024];
char *a, *piperight;

void parseargs(char *cmd)
{
    char buf[32][32];
    char *pass[32];
    int argc = 0;

    cmd = trim(cmd);

    for (int i = 0; i < 32; i++)
        pass[i] = buf[i];

    char *c = buf[argc];
    int input_pos = 0, output_pos = 0;
    for (char *p = cmd; *p; p++)
    {
        if (*p == ' ' || *p == '\n')
        {
            *c = '\0';
            argc++;
            c = buf[argc];
        }
        else
        {
            if (*p == '<')
            {
                input_pos = argc + 1;
            }
            if (*p == '>')
            {
                output_pos = argc + 1;
            }
            *c++ = *p;
        }
    }
    *c = '\0';
    argc++;
    pass[argc] = 0;

    if (input_pos)
    {
        close(0);
        open(pass[input_pos], O_RDONLY);
    }
    if (output_pos)
    {
        close(1);
        open(pass[output_pos], O_WRONLY | O_CREATE);
    }

    char *argv[32];
    int argc2 = 0;
    for (int pos = 0; pos < argc; pos++)
    {
        if (pos == input_pos - 1)
            pos += 2; 
        if (pos == output_pos - 1)
            pos += 2;
        argv[argc2++] = pass[pos];
    }
    argv[argc2] = 0;

    if (fork() == 0)
        exec(argv[0], argv);
    wait(0);
}

void parsecmd()
{
    if (a)
    {
        int pd[2];
        pipe(pd);

        if (!fork()){
            if (piperight) redirect(1, pd);
            parseargs(a);
        }
        else if (!fork())
        {
            if (piperight)
            {
                redirect(0, pd);
                a = piperight;
                piperight = parsepipe(a);
                parsecmd();
            }
        }
        close(pd[0]);
        close(pd[1]);
        wait(0);
        wait(0);
    }
    exit(0);
}

int main(int argc, char *argv[])
{
    while (getcmd(cmd_buf, sizeof(cmd_buf)) >= 0)
    {
        if (fork() == 0)
        {
            a = cmd_buf;
            piperight = parsepipe(a);
            parsecmd();
        }
        wait(0);
    }
    exit(0);
}