
  #include <sys/types.h>
  #include <sys/stat.h>
  #include <fcntl.h>
  #include <termios.h>
  #include <stdio.h>
  #include <stdlib.h>
  #include <string.h>
  #include <unistd.h>
  #include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>

  /* Baudrate 설정은 <asm/termbits.h>에 정의되어 있다. */
  /* <asm/termbits.h>는 <termios.h>에서 include된다. */
  #define BAUDRATE B38400
  /* 여기의 포트 장치 파일을 바꾼다. COM1="/dev/ttyS1, COM2="/dev/ttyS2 */
  // #define MODEMDEVICE "/dev/ttyS1" 집접 찾아줌
  #define _POSIX_SOURCE 1 /* POSIX 호환 소스 */

  #define FALSE 0
  #define TRUE 1

  volatile int STOP=FALSE;

void sig_hnd(int sig){ (void)sig; printf("sig : %d", sig); }

  int main(void)
  {
    int fd,c, res;
    struct termios oldtio,newtio;
    char buf[255];

  /* 읽기/쓰기 모드로 모뎀 장치를 연다.(O_RDWR)
     데이터 전송 시에 <CTRL>-C 문자가 오면 프로그램이 종료되지 않도록
     하기 위해 controlling tty가 안되도록 한다.(O_NOCTTY)
  */
 	
   //>>> fix kyoulee:: fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
   char *modemdevice;
   modemdevice = ttyname(0);
   printf("modemdevice L %s \n", modemdevice);
	fd = open(modemdevice, O_RDWR | O_NOCTTY );
   if (fd <0) {perror(modemdevice); exit(-1); }

   tcgetattr(fd, &oldtio); /* save current serial port settings */
   //bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */

  /*
    BAUDRATE: 전송 속도. cfsetispeed() 및 cfsetospeed() 함수로도 세팅 가능
    CRTSCTS : 하드웨어 흐름 제어. (시리얼 케이블이 모든 핀에 연결되어 있는
              경우만 사용하도록 한다. Serial-HOWTO의 7장을 참조할 것.)
    CS8     : 8N1 (8bit, no parity, 1 stopbit)
    CLOCAL  : Local connection. 모뎀 제어를 하지 않는다.
    CREAD   : 문자 수신을 가능하게 한다.
  */
   //newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;

  /*
   IGNPAR   : Parity 에러가 있는 문자 바이트를 무시한다.
   ICRNL    : CR 문자를 NL 문자로 변환 처리한다. (이 설정을 안하면 다른
              컴퓨터는 CR 문자를 한 줄의 종료문자로 인식하지 않을 수 있다.)
    otherwise make device raw (no other input processing)
  */
   //newtio.c_iflag = IGNPAR | ICRNL;

  /*
   Raw output.
  */
   //newtio.c_oflag = 0;

   /*
   ICANON   : canonical 입력을 가능하게 한다.
    disable all echo functionality, and don't send signals to calling program
  */
//   newtio.c_lflag = ICANON;

	
	printf("oldtio.c_iflag %lu\n", oldtio.c_iflag);
	printf("oldtio.c_oflag %lu\n", oldtio.c_oflag);
	printf("oldtio.c_cflag %lu\n", oldtio.c_cflag);
	printf("oldtio.c_lflag %lu\n", oldtio.c_lflag);
	printf("oldtio.c_ispeed %lu\n", oldtio.c_ispeed);
	printf("oldtio.c_ospeed %lu\n", oldtio.c_ospeed);


   printf("oldtio.c_cc[VEOF]  %d\n", oldtio.c_cc[VEOF]     );
   printf("oldtio.c_cc[VEOL]  %d\n", oldtio.c_cc[VEOL]     );
   printf("oldtio.c_cc[VEOL2]  %d\n", oldtio.c_cc[VEOL2]    );
   printf("oldtio.c_cc[VERASE]  %d\n", oldtio.c_cc[VERASE]   );
   printf("oldtio.c_cc[VWERASE]  %d\n", oldtio.c_cc[VWERASE]  );
   printf("oldtio.c_cc[VKILL]  %d\n", oldtio.c_cc[VKILL]    );
   printf("oldtio.c_cc[VREPRINT]  %d\n", oldtio.c_cc[VREPRINT] );
   printf("oldtio.c_cc[VINTR]  %d\n", oldtio.c_cc[VINTR]    );
   printf("oldtio.c_cc[VQUIT]  %d\n", oldtio.c_cc[VQUIT]    );
   printf("oldtio.c_cc[VSUSP]  %d\n", oldtio.c_cc[VSUSP]    );
   printf("oldtio.c_cc[VDSUSP]  %d\n", oldtio.c_cc[VDSUSP]   );
   printf("oldtio.c_cc[VSTART]  %d\n", oldtio.c_cc[VSTART]   );
   printf("oldtio.c_cc[VSTOP]  %d\n", oldtio.c_cc[VSTOP]    );
   printf("oldtio.c_cc[VLNEXT]  %d\n", oldtio.c_cc[VLNEXT]   );
   printf("oldtio.c_cc[VDISCARD]  %d\n", oldtio.c_cc[VDISCARD] );
   printf("oldtio.c_cc[VMIN]  %d\n", oldtio.c_cc[VMIN]     );
   printf("oldtio.c_cc[VTIME]  %d\n", oldtio.c_cc[VTIME]    );
   printf("oldtio.c_cc[VSTATUS]  %d\n", oldtio.c_cc[VSTATUS]  );

	close(fd);
  /*
    모든 제어 문자들을 초기화한다.
    디폴트 값은 <termios.h> 헤어 파일에서 찾을 수 있다. 여기 comment에도
    추가로 달아놓았다.
	https://man7.org/linux/man-pages/man3/termios.3.html
  */
	newtio = oldtio;
   //newtio.c_cc[VEOF]     = 0;     /* Ctrl-d */ 2개만 사용
   newtio.c_cc[VEOL]     = 0;     /* '\0 ' */
   newtio.c_cc[VEOL2]    = 0;     /* '\0 ' */
   newtio.c_cc[VERASE]   = 0;     /* Ctrl-? del('\177') */
   newtio.c_cc[VWERASE]  = 0;     /* Ctrl-W */
   newtio.c_cc[VKILL]    = 0;     /* Ctrl-U */
   newtio.c_cc[VREPRINT] = 0;     /* Ctrl-R */
   //newtio.c_cc[VINTR]    = 0;     /* Ctrl-C */
   newtio.c_cc[VQUIT]    = 0;     /* Ctrl-\\  */
   newtio.c_cc[VSUSP]    = 0;     /* Ctrl-Z */
   newtio.c_cc[VDSUSP]   = 0;     /* Ctrl-Y */

   newtio.c_cc[VSTART]   = 0;     /* Ctrl-Q */
   newtio.c_cc[VSTOP]    = 0;     /* Ctrl-S */
   newtio.c_cc[VLNEXT]   = 0;     /* Ctrl-V */
   newtio.c_cc[VDISCARD] = 0;     /* Ctrl-O */
   //newtio.c_cc[VMIN]     = 1;     /* blocking read until 1 character arrives */
   //newtio.c_cc[VTIME]    = 0;     /* inter-character timer unused */
   newtio.c_cc[VSTATUS]    = 0;     /* Ctrl-T */
/*
           VEOF          EOF                  ^D
           VEOL          EOL                  _POSIX_VDISABLE
           VEOL2         EOL2                 _POSIX_VDISABLE
           VERASE        ERASE                ^? ‘\177’
           VWERASE       WERASE               ^W
           VKILL         KILL                 ^U
           VREPRINT      REPRINT              ^R
           VINTR         INTR                 ^C
           VQUIT         QUIT                 ^\\ ‘\34’
           VSUSP         SUSP                 ^Z
           VDSUSP        DSUSP                ^Y
           VSTART        START                ^Q
           VSTOP         STOP                 ^S
           VLNEXT        LNEXT                ^V
           VDISCARD      DISCARD              ^O
           VMIN          ---                  1
           VTIME         ---                  0
           VSTATUS       STATUS               ^T
		   */
  /*
    이제 modem 라인을 초기화하고 포트 세팅을 마친다.
  */
//    tcflush(fd, TCIFLUSH);
   tcsetattr(fd,TCSANOW,&newtio);
//	signal( SIGINT, sig_hnd );
  /*
    터미널 세팅이 끝났고, 이제는 입력을 처리한다.
    이 예제에서는 한 줄의 맨 첫 문자를 'z'로 했을 때 프로그램을
    종료한다.
//   */
//    while (STOP==FALSE) {     /* 종료 조건(STOP==TRUE)가 될 때까지 루프 */
//    /* read()는 라인 종료 문자가 나올 때까지 255 문자를 넘어가더라도
//       block 된다. read 하고자 하는 문자 개수가 입력 가능한 문자 개수보다
//       적은 경우에는 또 한번의 read를 하여 나머지를 읽어낼 수 있다.
//       res는 read에 의해서 실제로 읽혀진 문자의 개수를 갖게 된다. */
      
//       res = read(fd,buf,255);
//       buf[res]=0;             /* set end of string, so we can printf */
//       printf(":%s:%d", buf, res);

//       //>>> fix kyoulee :: if (buf[0]=='z') STOP=TRUE;
//       if (!strcmp(buf, "q\n")) STOP=TRUE;
//    }
   /* restore the old port settings */

	// ternimal readline
	//
	while (1)
	{
		printf("readline_echoing_p :%d\n", readline_echoing_p);
		printf("rl_readline_name :%s\n", rl_readline_name);
		printf("rl_basic_word_break_characters :%s\n", rl_basic_word_break_characters);
		printf("rl_completer_word_break_characters :%s\n", rl_completer_word_break_characters);
		printf("rl_completer_quote_characters :%s\n", rl_completer_quote_characters);
		printf("rl_already_prompted : %d\n", rl_already_prompted);
		printf("====== minishell end ======\n\n\n");
		printf("====== minishell start ======\n");
		readline("minishell-0.3v : ");
		printf("prompt :%s\n", rl_prompt);
		printf("rl_line_buffer : %s\n", rl_line_buffer);
		printf("history_base :%d\n", history_base);
		printf("history_length :%d\n", history_length);
		printf("max_input_history : %d\n", max_input_history);
		printf("rl_terminal_name : %s\n", rl_terminal_name);
		//rl_display_match_list(completion_matches(rl_line_buffer, NULL)); // 이거 쓰는법 찾아야함 그리고 함수 만들어야함
		if (printf("rl_completion_word_break_hook ptr : %p\n", rl_completion_word_break_hook) && rl_completion_word_break_hook)
			printf("rl_completion_word_break_hook %s\n", (*rl_completion_word_break_hook)());
		add_history(rl_line_buffer);
		rl_redisplay();
		if (rl_line_buffer[0] == 'q')
		{
			break;
		}
	}
   tcsetattr(fd,TCSANOW,&oldtio);

  }
