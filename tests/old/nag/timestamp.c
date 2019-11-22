#ifdef WINDOWS
#include <winsock.h>
#else
#include <unistd.h>
#endif

#include <stdio.h>
#include <time.h>

#define LINESIZE 2048
int main(void)
{
  char hostname[LINESIZE], line[LINESIZE];
  int r;
  time_t theTime;
  FILE *ident;
  int p;

#ifdef WINDOWS
  WORD wVersionRequested;
  WSADATA wsaData;
  int sockErr;

  wVersionRequested = MAKEWORD(1, 1);
  sockErr = WSAStartup(wVersionRequested, &wsaData);
  if (sockErr != 0)
    {
      fprintf(stderr, "Call of WSAStartup failed.\n");
      return 2;
    }

  /* Confirm that the Windows Sockets DLL supports 1.1.
     Note that if the DLL supports versions greater
     than 1.1 in addition to 1.1, it will still return
     1.1 in wVersion since that is the version we
     requested. */
  if (LOBYTE(wsaData.wVersion) != 1 ||
      HIBYTE(wsaData.wVersion) != 1)
    {
      /* Tell the user that we couldn't find a usable
         winsock.dll. */
      fprintf(stderr, "Call of WSAStartup didn't find version 1.1 sockets.\n");
      WSACleanup();
      return 3;
    }

#endif

  printf("%s\n", "%!PS-Adobe-2.0");
  printf("%s\n", "%%Title: timestamp.ps");
  printf("%s\n", "%%Creator: libm test");
  printf("%s\n", "%%Orientation: Landscape");
  printf("%s\n", "%%Pages: (atend)");
  printf("%s\n", "%%EndComments");
  printf("%s\n", "%%EndProlog");
  printf("%s\n", "%%Page: 1 1");
  printf("%s\n", "/Helvetica findfont 20 scalefont setfont");

  r = gethostname(hostname, LINESIZE);
  if (r != 0)
    sprintf(hostname, "<unknown>");

  p = 300;
  printf("%d 50 moveto\n", p);
  printf("%s\n", "90 rotate");
  printf("(libm test results generated on machine %s) show\n", hostname);
  printf("%s\n", "-90 rotate");

  theTime = time(NULL);
  p += 20;
  printf("%d 50 moveto\n", p);
  printf("%s\n", "90 rotate");
  printf("(%s) show\n", ctime(&theTime));
  printf("%s\n", "-90 rotate");

  /* See if we can find an identity file. If so, read it in. */
  ident = fopen(".ident", "r");
  if (ident)
    {
      while (!feof(ident) && fgets(line, LINESIZE, ident))
        {
          p += 20;
          printf("%d 50 moveto\n", p);
          printf("%s\n", "90 rotate");
          printf("(%s) show\n", line);
          printf("%s\n", "-90 rotate");
        }
    }

  printf("%s\n", "stroke");
  printf("%s\n", "showpage");
  printf("%s\n", "%%Trailer");
  printf("%s\n", "%%DocumentFonts: Helvetica");
  printf("%s\n", "%%Pages: 1");

#ifdef WINDOWS
  WSACleanup();
#endif

  return 0;
}
