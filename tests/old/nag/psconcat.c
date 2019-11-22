#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 *  Concatenates a bunch of PostScript files given as arguments.
 *  The PostScript files should be as produced by gnuplot.
 *  The result is written to standard output.
 */

int copyUntilString(FILE *infile, const char *inFileName,
                    const char *string, char *line, int lenLine);

int copyUntilStringSubstitutingPageNumber(FILE *infile, const char *inFileName,
                                          const char *string, int *pagecount,
                                          char *line, int lenLine);

int skipUntilString(FILE *infile, const char *inFileName,
                    const char *string, char *line, int lenLine);

int main(int argc, char *argv[])
{
  char *progname;
  char line[4096];
  int i, r, pagecount;
  FILE *infile;

  progname = argv[0];

  if (argc == 1)
    {
      printf ("Usage: %s file1 file2 ...\n", progname);
      return 1;
    }

  for (i = 1; i < argc; i++)
    {
      infile = fopen(argv[i], "r");
      if (!infile)
        {
          printf ("Cannot open file %s for reading.\n", argv[i]);
          return 2;
        }

      fgets(line, sizeof(line), infile);
      if (strncmp(line, "%!PS-Adobe", strlen("%!PS-Adobe")))
        {
          fprintf(stderr,
                  "File %s appears not to contain PostScript; first line is\n%s\n",
                  argv[i], line);
        }
      else
        {
          if (i == 1)
            {
              printf("%s", line);
              fgets(line, sizeof(line), infile);
              printf("%s\n", "%%Title: results.ps");
              r = copyUntilString(infile, argv[i], "%%Pages: ", line, sizeof(line));
              printf("%s\n", "%%Pages: (atend)");              
              r = copyUntilString(infile, argv[i], "%%EndProlog", line, sizeof(line));
              if (r != 0)
                return 3;
              printf("%s", line);
              pagecount = 0;
            }
          else
            {
              r = skipUntilString(infile, argv[i], "%%EndProlog", line, sizeof(line));
              if (r != 0)
                return 3;
            }

          r = copyUntilStringSubstitutingPageNumber(infile, argv[i], "%%Trailer", &pagecount,
                                                 line, sizeof(line));
          if (r != 0)
            return 3;

          if (i == argc - 1)
            {
              printf("%s", "%%Trailer");
              r = copyUntilString(infile, argv[i], "%%Pages: ", line, sizeof(line));
              if (r != 0)
                return 3;
              printf("%s %d\n", "%%Pages:", pagecount);
            }
        }
      fclose(infile);
    }

  return 0;
}


int copyUntilString(FILE *infile, const char *inFileName,
                    const char *string, char *line, int lenLine)
{
  int retval = 0;

  while (!feof(infile) && fgets(line, lenLine, infile) &&
         strncmp(line, string, strlen(string)))
    printf("%s", line);

  if (feof(infile))
    {
      fprintf(stderr, "Unexpected end of file %s while searching for string\n"
              "\"%s\"\n", inFileName, string);
      retval = 1;
    }

  return retval;
}


int copyUntilStringSubstitutingPageNumber(FILE *infile, const char *inFileName,
                                          const char *string, int *pagecount,
                                          char *line, int lenLine)
{
  int retval = 0;

  while (!feof(infile) && fgets(line, lenLine, infile) &&
         strncmp(line, string, strlen(string)))
    {
      if (strncmp(line, "%%Page: ", strlen("%%Page: ")))
        printf("%s", line);
      else
        {
          *pagecount = *pagecount + 1;
          printf("%s %d %d\n", "%%Page:", *pagecount, *pagecount);
        }
    }

  if (feof(infile))
    {
      fprintf(stderr, "Unexpected end of file %s while searching for string\n"
              "\"%s\"\n", inFileName, string);
      retval = 1;
    }

  return retval;
}


int skipUntilString(FILE *infile, const char *inFileName,
                    const char *string, char *line, int lenLine)
{
  int retval = 0;

  while (!feof(infile) && fgets(line, lenLine, infile) &&
         strncmp(line, string, strlen(string)));

  if (feof(infile))
    {
      fprintf(stderr, "Unexpected end of file %s while searching for string\n"
              "\"%s\"\n", inFileName, string);
      retval = 1;
    }

  return retval;
}
