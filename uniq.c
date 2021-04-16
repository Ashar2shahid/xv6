#include "types.h"
#include "stat.h"
#include "user.h"

int tolower(int ch);
int strcicmp(char const *a, char const *b, int ignoreCase);
int printLine(char *currentLine, char *previousLine,int lineCounter, int countAndPrefix, int onlyDuplicate, int ignoreCase);

int
tolower(int ch)
{
  if(ch >= 'A' && ch <= 'Z') return ('a' + ch - 'A');
  else return ch;
}

int
strcicmp(char const *a, char const *b , int ignoreCase)
{
    if(!ignoreCase) return strcmp(a,b);
    for (;; a++, b++) {
        int d = tolower((unsigned char)*a) - tolower((unsigned char)*b);
        if (d != 0 || !*a)
            return d;
    }
}

int 
printLine(char *currentLine, char *previousLine,int lineCounter, int countAndPrefix, int onlyDuplicate, int ignoreCase)
{
  if(strcicmp(currentLine,previousLine,ignoreCase) != 0){
    if(countAndPrefix) {
      if(strcmp(previousLine,"") != 0) printf(1,"%d ", lineCounter);  
    }
    if(onlyDuplicate && lineCounter > 1) printf(1,"%s", previousLine);
    else if(!onlyDuplicate) printf(1,"%s",previousLine);
    return 0;
  }
  return lineCounter;
}

char buf[1024];

void
uniq(int fd, char *name, int countAndPrefix, int onlyDuplicate, int ignoreCase)
{

  if(countAndPrefix && onlyDuplicate){
    printf(1,"uniq: cannot use both -c and -d");
    exit();
  }

  int i, n, lineIndex, lineCounter;

  char currentLine[1024];
  char previousLine[1024] = "";

  lineIndex = 0;
  lineCounter = 0;

  while((n = read(fd, buf, sizeof(buf))) > 0){
    for(i=0; i<n; i++){
      currentLine[lineIndex] = buf[i];
      lineIndex++;
      currentLine[lineIndex] = '\0';
      if(buf[i] == '\n'){
       lineIndex = 0;
       lineCounter++;
       lineCounter = printLine(currentLine,previousLine,lineCounter,countAndPrefix,onlyDuplicate,ignoreCase);
       strcpy(previousLine,currentLine);
       strcpy(currentLine,"");
      }
    }
    printLine(currentLine,previousLine,lineCounter+1,countAndPrefix,onlyDuplicate,ignoreCase);
  }

  if(n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }
}

int
main(int argc, char *argv[])
{
  int fd, i,j;
  int filecheck = argc;
  int countAndPrefix, onlyDuplicate, ignoreCase;
  countAndPrefix = onlyDuplicate = ignoreCase = 0;

  for(i = 1; i < argc; i++){
    for(j = 0; j<strlen(argv[i]);j++){
      if(argv[i][0] == '-'){
	      
        if(j == 0) filecheck--;

        if(argv[i][j] =='c' || argv[i][j] =='C'){
          countAndPrefix = 1;
        }

        if(argv[i][j] =='i' || argv[i][j] =='I'){
          ignoreCase = 1;
        }

        if(argv[i][j] =='d' || argv[i][j] =='D'){
          onlyDuplicate = 1;
        }
      }
    }
  }

  if(filecheck == 1){
    uniq(0, "",countAndPrefix,onlyDuplicate,ignoreCase);
    exit();
  }

  if((fd = open(argv[argc-1], 0)) < 0){
      printf(1, "uniq: cannot open %s\n", argv[1]);
      exit();
  }
  uniq(fd, argv[argc-1],countAndPrefix,onlyDuplicate,ignoreCase);
  close(fd);

  exit();
}
