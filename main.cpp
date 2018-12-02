#include <iostream>
#include <vector>
#include <stdio.h>
#include <ctime>
#include <sys/ioctl.h>
#include <map>
#include <deque>
#include <unistd.h> //for getopt
using namespace std;

typedef struct
{
  char c;
  int live;
} Code;

//
// Initialize functions
//

void render(deque<map<int, Code>> &matrix, char color);

void usage(void);

int msleep(unsigned long milisec);

void setcolor(char colorcode);

char getrandchar(int modifier);

//
// Main loop
//

int main(int argc, char *argv[])
{
  int optchr, input, charmod;
  int speed = 5;     // default fall speed.
  printf("\033[0m"); //set to default text color
  char color; // set rainbow off

  /* Get Options */
  while ((optchr = getopt(argc, argv, "hunbRs:C:")) != EOF)
  {
    switch (optchr)
    {
      case 'h':
      case '?':
        usage();
        exit(0);
      case 'R':
        color = 'R';
        break;
      case 'u':
        charmod = 26;
        break;
      case 'n':
        charmod = 86;
        break;
      case 's':
        input = atoi(optarg);
        if (0 <= input && input <= 10)
        {
          speed = input;
          break;
        }
        else
        {
          printf("Input out of range.\n");
          exit(0);
        }
        break;
      case 'b':
        printf("\033[1m");
        break;
      case 'C':
        setcolor(optarg[0]);
        break;
      default:
      {
        usage();
        exit(0);
      }
    }
  }

  // get terminal size
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  int col_size = w.ws_col;
  int row_size = w.ws_row;

  deque<map<int, Code>> matrix;
  matrix.push_front(map<int, Code>());
  for (;;)
  {
    map<int, Code> new_map = matrix[0];
    int new_col = rand() % col_size + 1;  // set column location restricted by screen width
    int new_live = rand() % row_size + 1; // sets column length restricted by screen height
    Code empty_code = {'0', new_live};
    new_map[new_col] = empty_code;

    for (map<int, Code>::iterator it = new_map.begin(); it != new_map.end();)
    {
      int new_c = getrandchar(charmod);
      it->second.c = new_c;
      it->second.live--;
      
      if (it->second.live == 0)
      {
        new_map.erase(it);
      }
      else
      {
        it++;
      }
    }
    matrix.push_front(new_map);

    // clear row thats out of terminal
    if (matrix.size() > row_size)
    {
      matrix.erase(matrix.begin() + row_size, matrix.end());
    }

    render(matrix, color);
    msleep(speed * 20);
  }
  msleep(1000L);
  return 0;
}

//
// Functions
//

void render(deque<map<int, Code>> &matrix, char color)
{

  printf("\033[2J\033[1;1H"); // clear screen
  // render
  int c = 31;
  for (int i = 0; i < matrix.size(); i++)
  {
    for (map<int, Code>::iterator it = matrix[i].begin(); it != matrix[i].end(); it++)
    {
      int print_col = it->first;
      Code print_code = it->second;

      // Rainbow color
      if (color == 'R'){
        printf("\033[%dm", c++);
        if(c > 37) c=31;
      }

      printf("\033[%d;%dH", i, print_col); //, 0, print_col);

      if  (i==1 || i == matrix.size()-1){ // first and list line on screen are random characters
        printf("%c", getrandchar(0));
      }else{
        printf("%c", print_code.c);
      }
    }
  }
  fflush(stdout);
}

char getrandchar(int modifier){
  static const char alpha[] =
      "0123456789"
      "!@#$%^&*()_+-=`~<>?:{}|\\\"[],./;'"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      " abcdefghijklmnopqrstuvwxyz";
  int alphaSize = sizeof(alpha) / sizeof(char);
  return alpha[rand() % (alphaSize-modifier)];
}

void usage(void)
{
  printf("\n Usage: CMatrix -[hunbR] [-s speed] [-C color]\n"); // "[abBfhlsVx] [-u delay] [-C color]" to be added
  printf(" -h: Prints this (h)elp menu\n");
  printf(" -b: (b)old\n");
  printf(" -u: no lower case letters\n");
  printf(" -n: (n)umbers only\n");
  printf(" -R: (R)ainbow colors\n");
  printf(" -s: (s)peed (0 - 10; default 5)\n");
  printf(" -C: (C)olor {(g)reen, (r)ed, (b)lue, (y)ellow, (c)yan, (m)agenta}\n");
  printf("\n");
}

int msleep(unsigned long milisec)
{
  struct timespec req = {0};
  time_t sec = (int)(milisec / 1000);
  milisec = milisec - (sec * 1000);
  req.tv_sec = sec;
  req.tv_nsec = milisec * 1000000L;
  while (nanosleep(&req, NULL) == -1)
    continue;
  return 1;
}

void setcolor(char colorcode){
  switch(colorcode){
    case 'r':
      printf("\033[31m"); // red
      break;
    case 'g':
      printf("\033[32m"); // green
      break;
    case 'y':
      printf("\033[33m"); // yellow
      break;
    case 'b':
      printf("\033[34m"); // blue
      break;
    case 'm':
      printf("\033[35m"); // magenta
      break;
    case 'c':
      printf("\033[36m"); // cyan
      break;
    default:
      printf("Color code not recognized.\n");
      printf("Use -h for help.\n");
      exit(0);
  }
}