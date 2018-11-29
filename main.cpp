#include<iostream>
#include<vector>
#include<stdio.h>
#include<ctime>
#include<sys/ioctl.h>
#include<map>
#include<deque>
#include<unistd.h> //for getopt
#include <string.h> //for color
using namespace std;

typedef struct{
  char c;
  int live;
}Code;

void render(deque<map<int, Code> > &matrix, int speed);

void usage(void); 

int msleep(unsigned long milisec);

int main(int argc, char *argv[]){
  static const char alpha[] =
      "0123456789"
      "!@#$%^&*()_+-=`~<>?:{}|\\\"[],./;'"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      " abcdefghijklmnopqrstuvwxyz";
  int alphaSize = sizeof(alpha) / sizeof(char);
  //cout<<alphaSize;
  int optchr, input;
  int speed = 5; // default fall speed.
  printf("\033[0m"); //set to default text color

  /* Get Options */
  while ((optchr = getopt(argc, argv, "hus:C:")) != EOF)
  {
    switch (optchr)
    {
    case 'h':
    case '?':
      usage();
      exit(0);

    case 'u':
      alphaSize -= 26;
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
    case 'C':
      switch (optarg[0])
      {
      case 'r':
        printf("\033[0;31m");
        break;
      case 'g':
        printf("\033[0;32m");
        break;
      default:
        printf("Color code not recognized");
        exit(0);
      }
      break;
    }
  }

  // get terminal size
  struct winsize w;
  ioctl(0, TIOCGWINSZ, &w);
  int col_size = w.ws_col;
  int row_size = w.ws_row;
  //cout<< col_size<< " "<<row_size<<endl;

  deque<map<int, Code>> matrix;
  matrix.push_front(map<int, Code>());
  for (;;)
  {
    map<int, Code> new_map = matrix[0];
    int new_col = rand() % col_size + 1;  // set column size proportional to screen width
    int new_live = rand() % row_size + 1; // sets column length restricted by screen height
    Code empty_code = {'0', new_live};
    new_map[new_col] = empty_code;
    
    for (map<int, Code>::iterator it = new_map.begin(); it != new_map.end();){
      int new_c = alpha[rand() % alphaSize];
      it->second.c = new_c;
      it->second.live--;
      // clear 0 live code
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

    render(matrix, speed);
  }
  msleep(1000L);
  return 0;
}

void render(deque<map<int, Code> > &matrix, int speed){
  // clear screen
  printf("\033[2J\033[1;1H");
  // render
  for(int i = 0; i < matrix.size(); i++){
    for(map<int,Code>::iterator it = matrix[i].begin(); it!=matrix[i].end(); it++){
      int print_col = it->first;
      Code print_code = it->second;
//     if (it == matrix[i].begin()){
//           printf("\033[%d;%dH",i,print_col);//, 0, print_col);
//            printf("%c\n", rand()%sizeof(char));      
//      }else{
        printf("\033[%d;%dH",i,print_col);//, 0, print_col);      
        printf("%c",print_code.c);
//      }
    }
  }
  fflush(stdout);
  msleep(speed*20);
}

void usage(void) {
  printf("\n Usage: CMatrix -[hu] [-s speed] [-C color]\n"); // "[abBfhlsVx] [-u delay] [-C color]" to be added
  printf(" -h: Prints this help menu\n");
  printf(" -s: delay (0 - 10, default 5): Screen update speed \n");
  printf(" -u: no lower case letters\n");
  printf(" -C: print in color ie green, red, \n");
  printf("\n");
}

int msleep(unsigned long milisec){
  struct timespec req = {0};
  time_t sec = (int)(milisec / 1000);
  milisec = milisec - (sec * 1000);
  req.tv_sec = sec;
  req.tv_nsec = milisec * 1000000L;
  while (nanosleep(&req, NULL) == -1)
    continue;
  return 1;
}