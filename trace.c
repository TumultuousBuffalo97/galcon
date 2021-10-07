#define CTRACE 1
#include <signal.h>
#line 1 "galcon.c"
/*
 * GALCON.C -- Galactic Conquest
 *
 * Version update Notes
 *
 * v2.2	 by Stan Bartsch (date unknown)
 * v2.21 by David E. Brooks Jr (29-Dec-86)
 *       - Changed displays to allow names to be displayed instead
 *         of player's numbers.  Also, added <K>wick command, Convienient
 *         means of changing certain game parameters and fixed bug in
 *         root() function.  Time displays were changed to a shorter format.
 *
 * v2.22 by David E. Brooks Jr (16-Jan-87)
 *	 - Added the Zend (send) command.
 * 
 * v2.23 by David E. Brooks Jr (11-Mar-87)
 *       - Changed initialization code to deny entrance to the game
 *         after the first 20 minutes have elapsed.
 *
 * v3.00 by Todd Ambrose (July 1988)
 *       - Added VT100 mode, accessed by the 'V' command. Added
 *         both long and short galaxy maps accessed by 'L' and 'R'.
 *         Made WARP fleets that arrive 1/2 the time a normal fleet
 *         takes. Added pirate fleets that randomly attack certain
 *         fleets. Added a private msg toggle for all prv msgs
 *         to send to a specific person. Also added Harold's
 *         WHO, and an auto-clear for detached players.
 *         And, added a penalty for returning players.
 *
 * v4.00 by Steve Snow (February 1989)
 * 	- Converted to UNIX System V
 *
 */

#include <stdio.h>
#line 35 "galcon.c"

#include <ctype.h>
#line 36 "galcon.c"

#include <signal.h>
#line 37 "galcon.c"

#include <sys/fcntl.h>
#line 38 "galcon.c"

#include <termio.h>
#line 39 "galcon.c"

#include "share.h"
#line 40 "galcon.c"


#define ESC '\033'
#line 42 "galcon.c"


/*Declare all the shared variables */

struct s_region {
	long start,alive[8];
	int xpos[40],ypos[40],zpos[40],owner[40],produ[40],ships[40],game;
	int twicej[15],twiceg[15];
	long lstup[40],credit[10];
	int meslok,mesnum,combat[10][2],comnum;
	char names[10][20],public[5][100],status[10],scaner[16][16];
	int pppn[20],pnppn[20];
};

/*Declare all my global variables */

int inchar,inlen,stack[99][2],toggle,twice,pirsiz,pirflt;
long events[99],pirtim;
int myp,mypn;
char vtmode,mymes,mycom,mynum,home;
int numpl[10],numpr[10],numsh[10];
char scr[80], scr2[30], inbuf[2];
long game_len,lsttim;
char x_siz,y_siz,z_siz;
long prod_time, trav_time;
struct termio tdes;
struct termio tsave;
int flags;
double drand48();

/* Set up the shared region*/
struct s_region *s;
int shmid1 = 0;

struct s_region *get_shared_region()
{
	char *shmat();
char *_ct;t_ct_("\n 76 get_shared_region()"); t_ct_("\n 80 \tshmid1=\n\t    shmget(SHMKEY1,sizeof(struct s_region),0600|IPC_CREAT|IPC_EXCL);"); 
	(shmid1=
	    shmget((u_ct_("\t/* SHMKEY1",sizeof(SHMKEY1),SHMKEY1), SHMKEY1),sizeof(struct s_region),0600|(u_ct_("\t/* IPC_CREAT",sizeof(IPC_CREAT),IPC_CREAT), IPC_CREAT)|(u_ct_("\t/* IPC_EXCL",sizeof(IPC_EXCL),IPC_EXCL), IPC_EXCL)), u_ct_("\t/* shmid1",sizeof(shmid1),shmid1), shmid1);
t_ct_("\n 83 \tif (shmid1<0)"); 
	if ((u_ct_("\t/* shmid1",sizeof(shmid1),shmid1), shmid1)<0){
	{t_ct_("\n 84 \t{"); t_ct_("\n 85 \t\tprintf(\"\\nRegion in use or already created..atempting to attach..\\n\");"); 
		printf("\nRegion in use or already created..atempting to attach..\n");t_ct_("\n 86 \t\tshmid1 = shmget(SHMKEY1,sizeof(struct s_region),0600|IPC_EXCL);"); 
		(shmid1 = shmget((u_ct_("\t\t/* SHMKEY1",sizeof(SHMKEY1),SHMKEY1), SHMKEY1),sizeof(struct s_region),0600|(u_ct_("\t\t/* IPC_EXCL",sizeof(IPC_EXCL),IPC_EXCL), IPC_EXCL)), u_ct_("\t\t/* shmid1",sizeof(shmid1),shmid1), shmid1);t_ct_("\n 87 \t}"); 
	}}
t_ct_("\n 89 \treturn((struct s_region *)shmat(shmid1,0,0));"); 
	return((struct s_region *)shmat((u_ct_("\t/* shmid1",sizeof(shmid1),shmid1), shmid1),0,0));
}

remove_shared_region(region)
int region;
{char *_ct;t_ct_("\n 92 remove_shared_region(region)"); t_ct_("\n 95 \tprintf(\"Removing shared region\\n\");"); 
	printf("Removing shared region\n");t_ct_("\n 96 \tshmctl(shmid1,IPC_RMID,(struct shmid_ds *)0);"); 
	shmctl((u_ct_("\t/* shmid1",sizeof(shmid1),shmid1), shmid1),(u_ct_("\t/* IPC_RMID",sizeof(IPC_RMID),IPC_RMID), IPC_RMID),(struct shmid_ds *)0);t_ct_("\n    /* return */"); 
}



int irand(value)
 unsigned int value;
 {
  int i;
char *_ct;t_ct_("\n101 irand(value)"); t_ct_("\n106   if (value == 0)"); 
  if ((u_ct_("  /* value",sizeof(value),value), value) == 0){t_ct_(" return(0);");  return(0);}  t_ct_("\n107   i = (int) (value)*drand48();"); 
  (i = ((int) ((value), u_ct_("  /* (value)",sizeof((value)),(value)), (value)), u_ct_("  /* (int) (value)",sizeof((int) (value)),(int) (value)), (int) (value))*drand48(), u_ct_("  /* i",sizeof(i),i), i);t_ct_("\n108   return(i);"); 
  return(u_ct_("  /* (i)",sizeof((i)),(i)), (i));
 }         

/*
 * sdate() - alternative function  for ctime()
 */
char *sdate(value)
long value;

{
	long timex, whole, decimal;
	extern long time();
	char *ptr;
	static char sdatebfr[10];
char *_ct;t_ct_("\n114 sdate(value)"); t_ct_("\n123 \tif (value == 0L)"); 
	if ((u_ct_("\t/* value",sizeof(value),value), value) == 0L){
	{t_ct_("\n124 \t{"); t_ct_("\n125 \t\ttimex = time(0) - s->start;"); 
		(timex = time(0) - (s->start, u_ct_("\t\t/* s->start",sizeof(s->start),s->start), s->start), u_ct_("\t\t/* timex",sizeof(timex),timex), timex);t_ct_("\n126 \t}"); 
	}}
	else{t_ct_("\n127 \telse"); 
	{t_ct_("\n128 \t{"); t_ct_("\n129 \t\ttimex = value - s->start;"); 
		(timex = (u_ct_("\t\t/* value",sizeof(value),value), value) - (s->start, u_ct_("\t\t/* s->start",sizeof(s->start),s->start), s->start), u_ct_("\t\t/* timex",sizeof(timex),timex), timex); /* make the number reasonable to look at */t_ct_("\n130 \t}"); 
	}}t_ct_("\n131 \twhole = timex/60L;"); 
	(whole = (u_ct_("\t/* timex",sizeof(timex),timex), timex)/60L, u_ct_("\t/* whole",sizeof(whole),whole), whole);t_ct_("\n132 \tdecimal = timex - (whole * 60L);"); 
	(decimal = (u_ct_("\t/* timex",sizeof(timex),timex), timex) - ((u_ct_("\t/* whole",sizeof(whole),whole), whole) * 60L), u_ct_("\t/* decimal",sizeof(decimal),decimal), decimal);t_ct_("\n133 \tsprintf(sdatebfr,\"%02ld.%02ld\",whole,decimal);"); 
	sprintf((u_ct_("\t/* sdatebfr",sizeof(sdatebfr),sdatebfr), sdatebfr),"%02ld.%02ld",(u_ct_("\t/* whole",sizeof(whole),whole), whole),(u_ct_("\t/* decimal",sizeof(decimal),decimal), decimal));t_ct_("\n134 \tptr = sdatebfr;"); 
	(ptr = (sdatebfr, u_ct_("\t/* sdatebfr",sizeof(sdatebfr),sdatebfr), sdatebfr), u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr);t_ct_("\n135 \treturn(ptr);"); 
	return(u_ct_("\t/* (ptr)",sizeof((ptr)),(ptr)), (ptr));
}


/*
 * start of Main()
 */

main()
{
	long atol();
	long now;
	char *ctime();
	int i,j,k;
	extern long time();
	extern int wrapup();
	long years;char *_ct;t_ct_("\n143 main()"); t_ct_("\n152 \ttoggle = -1;"); 
	(toggle = -1, u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle);
t_ct_("\n154 \tprintf(\"\\nGALCON - (v4.0) Galactic Conquest\\n\");"); 
	printf("\nGALCON - (v4.0) Galactic Conquest\n");t_ct_("\n155 \tprintf(\"\\n\");"); 
	printf("\n");t_ct_("\n156 \tgetppn(&myp, &mypn);"); 
	getppn(&myp, &mypn);

	/*remove_shared_region(SHMKEY1);*/
t_ct_("\n160 \ts = get_shared_region();"); 
	(s = get_shared_region(), u_ct_("\t/* s",sizeof(s),s), s);t_ct_("\n161 \tif (s == (struct s_region*) -1)"); 
	if ((u_ct_("\t/* s",sizeof(s),s), s) == (struct s_region*) -1){ {t_ct_(" {"); t_ct_("\n162 \t\tprintf(\"Can't get shared region.\\n\");"); 
		printf("Can't get shared region.\n");t_ct_("\n163 \t}"); 
	}}
t_ct_("\n165 \tif (1 == 1)"); 
	if (1 == 1){
	{t_ct_("\n166 \t{"); t_ct_("\n167 \t\tgame_len = 4500L;"); 
		(game_len = 4500L, u_ct_("\t\t/* game_len",sizeof(game_len),game_len), game_len);        /* changed to desired values and recompile */t_ct_("\n168 \t\tx_siz = 15;"); 
		(x_siz = 15, u_ct_("\t\t/* x_siz",sizeof(x_siz),x_siz), x_siz);t_ct_("\n169 \t\ty_siz = 14;"); 
		(y_siz = 14, u_ct_("\t\t/* y_siz",sizeof(y_siz),y_siz), y_siz);t_ct_("\n170 \t\tz_siz = 1;"); 
		(z_siz = 1, u_ct_("\t\t/* z_siz",sizeof(z_siz),z_siz), z_siz); /* Make Galaxy 2-D */t_ct_("\n171 \t\tprod_time = 45L;"); 
		(prod_time = 45L, u_ct_("\t\t/* prod_time",sizeof(prod_time),prod_time), prod_time);t_ct_("\n172 \t\ttrav_time = 30L;"); 
		(trav_time = 30L, u_ct_("\t\t/* trav_time",sizeof(trav_time),trav_time), trav_time);t_ct_("\n173 \t}"); 
	}}
t_ct_("\n175 \tprintf(\"Game length is %ld minutes\\n\",game_len/60);"); 
	printf("Game length is %ld minutes\n",(u_ct_("\t/* game_len",sizeof(game_len),game_len), game_len)/60);
t_ct_("\n177 \tsrand48(time());"); 
	srand48(time());t_ct_("\n178 \tsetrand();"); 
	setrand();
	/*
Get the current fcntl flags 
*/t_ct_("\n182 \tflags = fcntl(0, F_GETFL, 0);"); 
	(flags = fcntl(0, (u_ct_("\t/* F_GETFL",sizeof(F_GETFL),F_GETFL), F_GETFL), 0), u_ct_("\t/* flags",sizeof(flags),flags), flags);

	/*  
Setup terminal for noicanonical, nointerupt, 1 char input every 2/10 seconds
*/t_ct_("\n187 \tioctl(0, TCGETA, &tdes);"); 
	ioctl(0, (u_ct_("\t/* TCGETA",sizeof(TCGETA),TCGETA), TCGETA), &tdes);t_ct_("\n188 \ttsave = tdes;"); 
	(tsave = (tdes, u_ct_("\t/* tdes",sizeof(tdes),tdes), tdes), u_ct_("\t/* tsave",sizeof(tsave),tsave), tsave);t_ct_("\n189 \ttdes.c_lflag\t&= ~ICANON;"); 
	(tdes.c_lflag	&= ~(ICANON, u_ct_("\t/* ICANON",sizeof(ICANON),ICANON), ICANON), u_ct_("\t/* tdes.c_lflag",sizeof(tdes.c_lflag),tdes.c_lflag), tdes.c_lflag);t_ct_("\n190 \ttdes.c_lflag  &= ~ISIG;"); 
	(tdes.c_lflag  &= ~(ISIG, u_ct_("\t/* ISIG",sizeof(ISIG),ISIG), ISIG), u_ct_("\t/* tdes.c_lflag",sizeof(tdes.c_lflag),tdes.c_lflag), tdes.c_lflag);t_ct_("\n191 \ttdes.c_cc[VMIN] = 1;"); 
	(tdes.c_cc[(u_ct_("\t/* VMIN",sizeof(VMIN),VMIN), VMIN)] = 1, u_ct_("\t/* tdes.c_cc[VMIN]",sizeof(tdes.c_cc[VMIN]),tdes.c_cc[VMIN]), tdes.c_cc[VMIN]);t_ct_("\n192 \ttdes.c_cc[VTIME] = 2;"); 
	(tdes.c_cc[(u_ct_("\t/* VTIME",sizeof(VTIME),VTIME), VTIME)] = 2, u_ct_("\t/* tdes.c_cc[VTIME]",sizeof(tdes.c_cc[VTIME]),tdes.c_cc[VTIME]), tdes.c_cc[VTIME]);t_ct_("\n193 \tioctl(0, TCSETAF, &tdes);"); 
	ioctl(0, (u_ct_("\t/* TCSETAF",sizeof(TCSETAF),TCSETAF), TCSETAF), &tdes);
t_ct_("\n195 \tif(s->game<0 || s->game>10)"); 
	if((u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game)<0 || (u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game)>10){t_ct_(" s->game=0;");  (s->game=0, u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game);}t_ct_("\n196 \tif(s->meslok<0 || s->meslok>s->game)"); 
	if((u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok)<0 || (u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok)>(u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game)){t_ct_(" s->meslok=0;");  (s->meslok=0, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok);}t_ct_("\n197 \tlock();"); 
	lock();t_ct_("\n198 \tif(s->game==0 || s->lstup[0]+300L < time(0) || s->lstup[0]>time(0))"); 
	if((u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game)==0 || (u_ct_("\t/* s->lstup[0]",sizeof(s->lstup[0]),s->lstup[0]), s->lstup[0])+300L < time(0) || (u_ct_("\t/* s->lstup[0]",sizeof(s->lstup[0]),s->lstup[0]), s->lstup[0])>time(0)){
	{t_ct_("\n199 \t{"); t_ct_("\n200 \t\tprintf(\"Starting a new game...\\n\");"); 
		printf("Starting a new game...\n");t_ct_("\n201 \t\tsetup();"); 
		setup();t_ct_("\n202 \t\ts->game=0;"); 
		(s->game=0, u_ct_("\t\t/* s->game",sizeof(s->game),s->game), s->game);t_ct_("\n203 \t}"); 
	}}t_ct_("\n204 \tfor (j=0;j<99;j++)"); 
	for ((j=0, u_ct_("\t/* j",sizeof(j),j), j);(u_ct_("\t/* j",sizeof(j),j), j)<99;t_ct_("\n204 \tfor (j=0;j<99;j++)"),(j++, u_ct_("\t/* j",sizeof(j),j), j-1)){t_ct_(" events[j]=0L;");  (events[(u_ct_("\t/* j",sizeof(j),j), j)]=0L, u_ct_("\t/* events[j]",sizeof(events[j]),events[j]), events[j]);}t_ct_("\n205 \tif (s->game>8)"); 
	if ((u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game)>8){
	{t_ct_("\n206 \t{"); t_ct_("\n207 \t\tprintf(\"\\nSorry, too many players as it is!\\n\");"); 
		printf("\nSorry, too many players as it is!\n");t_ct_("\n208 \t\tleave();"); 
		leave();t_ct_("\n209 \t}"); 
	}}t_ct_("\n210 \tfor(i=0;i<20;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<20;t_ct_("\n210 \tfor(i=0;i<20;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n211 \t\tif(s->pppn[i]==myp && s->pnppn[i]==mypn)"); 
		if((s->pppn[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->pppn[i]",sizeof(s->pppn[i]),s->pppn[i]), s->pppn[i])==(u_ct_("\t\t/* myp",sizeof(myp),myp), myp) && (s->pnppn[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->pnppn[i]",sizeof(s->pnppn[i]),s->pnppn[i]), s->pnppn[i])==(u_ct_("\t\t/* mypn",sizeof(mypn),mypn), mypn)){
		{t_ct_("\n212 \t\t{"); t_ct_("\n213 \t\t\tprintf(\"\\nSorry, you are locked out for the remainder of the game.\\n\");"); 
			printf("\nSorry, you are locked out for the remainder of the game.\n");t_ct_("\n214 \t\t\tleave();"); 
			leave();t_ct_("\n215 \t\t}"); 
		}}}t_ct_("\n216 \tfor(i=0;i<15;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<15;t_ct_("\n216 \tfor(i=0;i<15;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n217 \t\tif(s->twicej[i]==myp && s->twiceg[i]==mypn)"); 
		if((s->twicej[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->twicej[i]",sizeof(s->twicej[i]),s->twicej[i]), s->twicej[i])==(u_ct_("\t\t/* myp",sizeof(myp),myp), myp) && (s->twiceg[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->twiceg[i]",sizeof(s->twiceg[i]),s->twiceg[i]), s->twiceg[i])==(u_ct_("\t\t/* mypn",sizeof(mypn),mypn), mypn)){t_ct_(" twice=1;");  (twice=1, u_ct_("\t\t/* twice",sizeof(twice),twice), twice);}t_ct_("\n218 \t}"); 
	}}t_ct_("\n219 \tmymes=s->mesnum;"); 
	(mymes=(s->mesnum, u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum), u_ct_("\t/* mymes",sizeof(mymes),mymes), mymes); t_ct_("\n220 \tmycom=s->comnum;"); 
	(mycom=(s->comnum, u_ct_("\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum), u_ct_("\t/* mycom",sizeof(mycom),mycom), mycom);t_ct_("\n221 \ts->game++;"); 
	(s->game++, u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game-1);t_ct_("\n222 \thome=0;"); 
	(home=0, u_ct_("\t/* home",sizeof(home),home), home);t_ct_("\n223 \tmynum=0;"); 
	(mynum=0, u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum);
	{t_ct_("\n224 \t{"); t_ct_("\n225 \t\tfor(home=0;s->names[home][0]!='\\0';home++)"); 
		for((home=0, u_ct_("\t\t/* home",sizeof(home),home), home);(s->names[(u_ct_("\t\t/* home",sizeof(home),home), home)][0], u_ct_("\t\t/* s->names[home][0]",sizeof(s->names[home][0]),s->names[home][0]), s->names[home][0])!='\0';t_ct_("\n225 \t\tfor(home=0;s->names[home][0]!='\\0';home++)"),(home++, u_ct_("\t\t/* home",sizeof(home),home), home-1)){t_ct_(";"); ;}t_ct_("\n226 \t}"); 
	}t_ct_("\n227 \ts->names[home][0]='\\n';"); 
	(s->names[(u_ct_("\t/* home",sizeof(home),home), home)][0]='\n', u_ct_("\t/* s->names[home][0]",sizeof(s->names[home][0]),s->names[home][0]), s->names[home][0]);t_ct_("\n228 \tmynum = home;"); 
	(mynum = (home, u_ct_("\t/* home",sizeof(home),home), home), u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum);t_ct_("\n229 \thome = -1;"); 
	(home = -1, u_ct_("\t/* home",sizeof(home),home), home);t_ct_("\n230 \ts->meslok--;"); 
	(s->meslok--, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);
t_ct_("\n232 \ts->alive[mynum]=0;"); 
	(s->alive[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)]=0, u_ct_("\t/* s->alive[mynum]",sizeof(s->alive[mynum]),s->alive[mynum]), s->alive[mynum]);t_ct_("\n233 \tk=0;"); 
	(k=0, u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n234 \ts->names[mynum][k]=1;"); 
	(s->names[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)][(u_ct_("\t/* k",sizeof(k),k), k)]=1, u_ct_("\t/* s->names[mynum][k]",sizeof(s->names[mynum][k]),s->names[mynum][k]), s->names[mynum][k]);
	while(t_ct_("\n235 \twhile(s->names[mynum][0]==1)"),(s->names[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)][0], u_ct_("\t/* s->names[mynum][0]",sizeof(s->names[mynum][0]),s->names[mynum][0]), s->names[mynum][0])==1){
	{t_ct_("\n236 \t{"); t_ct_("\n237 \t\tk=0;"); 
		(k=0, u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n238 \t\tfcntl(0, F_SETFL, flags & ~O_NDELAY);"); 
		fcntl(0, (u_ct_("\t\t/* F_SETFL",sizeof(F_SETFL),F_SETFL), F_SETFL), (u_ct_("\t\t/* flags",sizeof(flags),flags), flags) & ~(u_ct_("\t\t/* O_NDELAY",sizeof(O_NDELAY),O_NDELAY), O_NDELAY));t_ct_("\n239 \t\tprintf(\"\\nEnter your name, please: \");"); 
		printf("\nEnter your name, please: ");t_ct_("\n240 \t\tgets(scr);"); 
		(_ct = gets(scr), s_ct_("\t\t/* scr",scr), _ct);t_ct_("\n241 \t\tscr[10] = 0;"); 
		(scr[10] = 0, u_ct_("\t\t/* scr[10]",sizeof(scr[10]),scr[10]), scr[10]); /* force an EOS */t_ct_("\n242 \t\tsprintf(scr2,\"%-10s [%3d,%-3d\",scr,myp,mypn);"); 
		sprintf((u_ct_("\t\t/* scr2",sizeof(scr2),scr2), scr2),"%-10s [%3d,%-3d",(u_ct_("\t\t/* scr",sizeof(scr),scr), scr),(u_ct_("\t\t/* myp",sizeof(myp),myp), myp),(u_ct_("\t\t/* mypn",sizeof(mypn),mypn), mypn));t_ct_("\n243 \t\tfor (k = 0; k<20; k++)"); 
		for ((k = 0, u_ct_("\t\t/* k",sizeof(k),k), k); (u_ct_("\t\t/* k",sizeof(k),k), k)<20;t_ct_("\n243 \t\tfor (k = 0; k<20; k++)"),(k++, u_ct_("\t\t/* k",sizeof(k),k), k-1)){t_ct_("\n244 \t\t\ts->names[mynum][k] = scr2[k];"); 
			(s->names[(u_ct_("\t\t\t/* mynum",sizeof(mynum),mynum), mynum)][(u_ct_("\t\t\t/* k",sizeof(k),k), k)] = (scr2[(u_ct_("\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t/* scr2[k]",sizeof(scr2[k]),scr2[k]), scr2[k]), u_ct_("\t\t\t/* s->names[mynum][k]",sizeof(s->names[mynum][k]),s->names[mynum][k]), s->names[mynum][k]);}t_ct_("\n245 \t}"); 
	}}t_ct_("\n246 \tlock();"); 
	lock();t_ct_("\n247 \tk=s->mesnum+1;"); 
	(k=(u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)+1, u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n248 \tif(k>4)"); 
	if((u_ct_("\t/* k",sizeof(k),k), k)>4){t_ct_(" k=0;");  (k=0, u_ct_("\t/* k",sizeof(k),k), k);}t_ct_("\n249 \tsprintf(s->public[k],\"  #%2d %s] has joined the battle.\\0\",mynum,s->names[mynum]);"); 
	sprintf((s->public[(u_ct_("\t/* k",sizeof(k),k), k)], u_ct_("\t/* s->public[k]",sizeof(s->public[k]),s->public[k]), s->public[k]),"  #%2d %s] has joined the battle.\0",(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum),(s->names[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)], u_ct_("\t/* s->names[mynum]",sizeof(s->names[mynum]),s->names[mynum]), s->names[mynum]));t_ct_("\n250 \ts->public[k][1]='E';"); 
	(s->public[(u_ct_("\t/* k",sizeof(k),k), k)][1]='E', u_ct_("\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]);t_ct_("\n251 \ts->public[k][0]=mynum;"); 
	(s->public[(u_ct_("\t/* k",sizeof(k),k), k)][0]=(mynum, u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0]);t_ct_("\n252 \ts->mesnum=k;"); 
	(s->mesnum=(k, u_ct_("\t/* k",sizeof(k),k), k), u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n253 \tprintf(\"\\n\\n\");"); 
	printf("\n\n");t_ct_("\n254 \thome = -1;"); 
	(home = -1, u_ct_("\t/* home",sizeof(home),home), home);
	while(t_ct_("\n255 \twhile(home==-1)"),(u_ct_("\t/* home",sizeof(home),home), home)==-1){
	{t_ct_("\n256 \t{"); t_ct_("\n257 \t\tk=irand(40);"); 
		(k=irand(40), u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n258 \t\tif (s->owner[k]==-1)"); 
		if ((s->owner[(u_ct_("\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t/* s->owner[k]",sizeof(s->owner[k]),s->owner[k]), s->owner[k])==-1){
		{t_ct_("\n259 \t\t{"); t_ct_("\n260 \t\t\ts->owner[k]=mynum;"); 
			(s->owner[(u_ct_("\t\t\t/* k",sizeof(k),k), k)]=(mynum, u_ct_("\t\t\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t\t\t/* s->owner[k]",sizeof(s->owner[k]),s->owner[k]), s->owner[k]);t_ct_("\n261 \t\t\tif(twice)"); 
			if((u_ct_("\t\t\t/* twice",sizeof(twice),twice), twice)){t_ct_(" s->ships[k]=250;");  (s->ships[(u_ct_("\t\t\t/* k",sizeof(k),k), k)]=250, u_ct_("\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k]);}
			else{t_ct_("\n262 \t\t\telse"); t_ct_(" s->ships[k]=450;");  (s->ships[(u_ct_("\t\t\t/* k",sizeof(k),k), k)]=450, u_ct_("\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k]);}t_ct_("\n263 \t\t\tif(twice)"); 
			if((u_ct_("\t\t\t/* twice",sizeof(twice),twice), twice)){t_ct_(" s->produ[k]=5;");  (s->produ[(u_ct_("\t\t\t/* k",sizeof(k),k), k)]=5, u_ct_("\t\t\t/* s->produ[k]",sizeof(s->produ[k]),s->produ[k]), s->produ[k]);}
			else{t_ct_("\n264 \t\t\telse"); t_ct_(" s->produ[k]=15;");  (s->produ[(u_ct_("\t\t\t/* k",sizeof(k),k), k)]=15, u_ct_("\t\t\t/* s->produ[k]",sizeof(s->produ[k]),s->produ[k]), s->produ[k]);}t_ct_("\n265 \t\t\thome=k;"); 
			(home=(k, u_ct_("\t\t\t/* k",sizeof(k),k), k), u_ct_("\t\t\t/* home",sizeof(home),home), home);t_ct_("\n266 \t\t}"); 
		}}t_ct_("\n267 \t}"); 
	}}t_ct_("\n268 \tprintf(\"\\nYour home planet is #%d, located at (%d,%d)\\n\",\n\t    home,s->xpos[home],s->ypos[home]);"); 
	printf("\nYour home planet is #%d, located at (%d,%d)\n",
	    (u_ct_("\t/* home",sizeof(home),home), home),(s->xpos[(u_ct_("\t/* home",sizeof(home),home), home)], u_ct_("\t/* s->xpos[home]",sizeof(s->xpos[home]),s->xpos[home]), s->xpos[home]),(s->ypos[(u_ct_("\t/* home",sizeof(home),home), home)], u_ct_("\t/* s->ypos[home]",sizeof(s->ypos[home]),s->ypos[home]), s->ypos[home]));
t_ct_("\n271 \ts->meslok--;"); 
	(s->meslok--, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n272 \tprompt();"); 
	prompt();t_ct_("\n273 \tinchar=0;"); 
	(inchar=0, u_ct_("\t/* inchar",sizeof(inchar),inchar), inchar);
	while(t_ct_("\n274 \twhile(1==1)"),1==1){
	{
		/*
Turn on non-blocking io
*/t_ct_("\n275 \t{"); t_ct_("\n279 \t\tfcntl(0, F_SETFL, flags | O_NDELAY);"); 
		fcntl(0, (u_ct_("\t\t/* F_SETFL",sizeof(F_SETFL),F_SETFL), F_SETFL), (u_ct_("\t\t/* flags",sizeof(flags),flags), flags) | (u_ct_("\t\t/* O_NDELAY",sizeof(O_NDELAY),O_NDELAY), O_NDELAY));
t_ct_("\n281 \t\tif (inbuf[0])"); 
		if ((u_ct_("\t\t/* inbuf[0]",sizeof(inbuf[0]),inbuf[0]), inbuf[0])){t_ct_(" getchar();");  getchar();}t_ct_("\n282 \t\tinbuf[0] = 0;"); 
		(inbuf[0] = 0, u_ct_("\t\t/* inbuf[0]",sizeof(inbuf[0]),inbuf[0]), inbuf[0]);t_ct_("\n283 \t\tinchar = 0;"); 
		(inchar = 0, u_ct_("\t\t/* inchar",sizeof(inchar),inchar), inchar);t_ct_("\n284 \t\tinlen = read(0, inbuf, 1);"); 
		(inlen = read(0, (u_ct_("\t\t/* inbuf",sizeof(inbuf),inbuf), inbuf), 1), u_ct_("\t\t/* inlen",sizeof(inlen),inlen), inlen);t_ct_("\n285 \t\tif(inlen != 0 )"); 
		if((u_ct_("\t\t/* inlen",sizeof(inlen),inlen), inlen) != 0 ){
		{t_ct_("\n286 \t\t{"); t_ct_("\n287 \t\t\tprintf(\"\\n\");"); 
			printf("\n");
			/*
Turn off non-blocking io
*/t_ct_("\n291 \t\t\tfcntl(0, F_SETFL, flags & ~O_NDELAY);"); 
			fcntl(0, (u_ct_("\t\t\t/* F_SETFL",sizeof(F_SETFL),F_SETFL), F_SETFL), (u_ct_("\t\t\t/* flags",sizeof(flags),flags), flags) & ~(u_ct_("\t\t\t/* O_NDELAY",sizeof(O_NDELAY),O_NDELAY), O_NDELAY));
t_ct_("\n293 \t\t\tinchar = inbuf[0];"); 
			(inchar = (inbuf[0], u_ct_("\t\t\t/* inbuf[0]",sizeof(inbuf[0]),inbuf[0]), inbuf[0]), u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar);t_ct_("\n294 \t\t\tif (vtmode)"); 
			if ((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_("gotoxy(24,1);"); gotoxy(24,1);}t_ct_("\n295 \t\t\tinchar = tolower(inchar);"); 
			(inchar = tolower((u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar)), u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar);t_ct_("\n296 \t\t\tif(!vtmode || inchar!='y' && inchar!='u' && inchar!='l')"); 
			if(!(u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode) || (u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar)!='y' && (u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar)!='u' && (u_ct_("\t\t\t/* inchar",sizeof(inchar),inchar), inchar)!='l'){t_ct_(" putchar('\\n');");  putchar('\n');}t_ct_("\n297 \t\t}"); 
		}}t_ct_("\n298 \t\tswitch(inchar)"); 
		switch((u_ct_("\t\t/* inchar",sizeof(inchar),inchar), inchar))
		{
		case   0:t_ct_("\n300 \t\tcase   0:");  t_ct_("\n301 \t\t\tupdate();"); 
			update();t_ct_("\n302 \t\t\tsleep(1);"); 
			sleep(1);t_ct_("\n303 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'a':t_ct_("\n304 \t\tcase 'a':");  t_ct_("\n305 \t\t\tarea();"); 
			area();t_ct_("\n306 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'b':t_ct_("\n307 \t\tcase 'b':");  t_ct_("\n308 \t\t\tbroadcast();"); 
			broadcast();t_ct_("\n309 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'c':t_ct_("\n310 \t\tcase 'c':");  t_ct_("\n311 \t\t\tcompute();"); 
			compute();t_ct_("\n312 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'd':t_ct_("\n313 \t\tcase 'd':");  t_ct_("\n314 \t\t\twarp();"); 
			warp();t_ct_("\n315 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'e':t_ct_("\n316 \t\tcase 'e':");  t_ct_("\n317 \t\t\tclrgam();"); 
			clrgam();t_ct_("\n318 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'f':t_ct_("\n319 \t\tcase 'f':");  t_ct_("\n320 \t\t\tflts();"); 
			flts();t_ct_("\n321 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'g':t_ct_("\n322 \t\tcase 'g':");  t_ct_("\n323 \t\t\tgalaxy();"); 
			galaxy();t_ct_("\n324 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'i':t_ct_("\n325 \t\tcase 'i':");  t_ct_("\n326 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" vtindiv();");  vtindiv();}
			else{t_ct_("\n327 \t\t\telse"); t_ct_(" individual();");  individual();}t_ct_("\n328 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'k':t_ct_("\n329 \t\tcase 'k':");  t_ct_("\n330 \t\t\tkwick();"); 
			kwick();t_ct_("\n331 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'l':t_ct_("\n332 \t\tcase 'l':");  t_ct_("\n333 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" vtscan();");  vtscan();}
			else{t_ct_("\n334 \t\t\telse"); t_ct_(" scan();");  scan();}t_ct_("\n335 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'm':t_ct_("\n336 \t\tcase 'm':");  t_ct_("\n337 \t\t\tprvtog();"); 
			prvtog();t_ct_("\n338 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'n':t_ct_("\n339 \t\tcase 'n':");   t_ct_("\n340 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){{t_ct_("\n341 \t\t\t\tvtmode=0;"); 
				(vtmode=0, u_ct_("\t\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode);t_ct_("\n342 \t\t\t\tnrmscr();"); 
				nrmscr();t_ct_("\n343 \t\t\t\tgotoxy(24,1);"); 
				gotoxy(24,1);t_ct_("\n344 \t\t\t\tprintf(\"VT100 Mode Disabled.\\n\");"); 
				printf("VT100 Mode Disabled.\n");t_ct_("\n345 \t\t\t}"); 
			}}t_ct_("\n346 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'o':t_ct_("\n347 \t\tcase 'o':");  t_ct_("\n348 \t\t\toneplanet();"); 
			oneplanet();t_ct_("\n349 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'p':t_ct_("\n350 \t\tcase 'p':");  t_ct_("\n351 \t\t\tprvmsg();"); 
			prvmsg();t_ct_("\n352 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'q':t_ct_("\n353 \t\tcase 'q':");  t_ct_("\n354 \t\t\tprintf(\"\\n%cAre you sure you want to Quit? \",7);"); 
			printf("\n%cAre you sure you want to Quit? ",7);t_ct_("\n355 \t\t\tk=getchar();"); 
			(k=getchar(), u_ct_("\t\t\t/* k",sizeof(k),k), k);t_ct_("\n356 \t\t\tk=tolower(k);"); 
			(k=tolower((u_ct_("\t\t\t/* k",sizeof(k),k), k)), u_ct_("\t\t\t/* k",sizeof(k),k), k);
			while(t_ct_("\n357 \t\t\twhile(getchar()!='\\n')"),getchar()!='\n'){t_ct_(";"); ;}t_ct_("\n358 \t\t\tif(k=='y')"); 
			if((u_ct_("\t\t\t/* k",sizeof(k),k), k)=='y'){{t_ct_("{"); t_ct_("\n359 \t\t\t\tleave();"); 
				leave();t_ct_("\n360 \t\t\t}"); 
			}}t_ct_("\n361 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'r':t_ct_("\n362 \t\tcase 'r':");  t_ct_("\n363 \t\t\tshtscn();"); 
			shtscn();t_ct_("\n364 \t\t\tgoto endinc;"); 
			goto endinc;
		case 's':t_ct_("\n365 \t\tcase 's':");  t_ct_("\n366 \t\t\tsend();"); 
			send();t_ct_("\n367 \t\t\tgoto endinc;"); 
			goto endinc;
		case 't':t_ct_("\n368 \t\tcase 't':");  t_ct_("\n369 \t\t\tprintf(\"\\nCurrent time is %s\\n\",sdate(0L));"); 
			printf("\nCurrent time is %s\n",sdate(0L));t_ct_("\n370 \t\t\tyears= (time(0) - s->start)/60L;"); 
			(years= (time(0) - (u_ct_("\t\t\t/* s->start",sizeof(s->start),s->start), s->start))/60L, u_ct_("\t\t\t/* years",sizeof(years),years), years);t_ct_("\n371 \t\t\tprintf(\"Years           %ld\\n\",years);"); 
			printf("Years           %ld\n",(u_ct_("\t\t\t/* years",sizeof(years),years), years));t_ct_("\n372 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'u':t_ct_("\n373 \t\tcase 'u':");  t_ct_("\n374 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" plupd();");  plupd();}t_ct_("\n375 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'v':t_ct_("\n376 \t\tcase 'v':");  t_ct_("\n377 \t\t\tif(!vtmode)"); 
			if(!(u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){{t_ct_("\n378 \t\t\t\tvtmode=1;"); 
				(vtmode=1, u_ct_("\t\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode);t_ct_("\n379 \t\t\t\tsetscr();"); 
				setscr();t_ct_("\n380 \t\t\t\tsetit();"); 
				setit();t_ct_("\n381 \t\t\t\tvtscan();"); 
				vtscan();t_ct_("\n382 \t\t\t\tplupd();"); 
				plupd();t_ct_("\n383 \t\t\t\tgotoxy(24,1);"); 
				gotoxy(24,1);t_ct_("\n384 \t\t\t\tprintf(\"VT100 Mode Activated.\\n\");"); 
				printf("VT100 Mode Activated.\n");t_ct_("\n385 \t\t\t}"); 
			}}t_ct_("\n386 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'w':t_ct_("\n387 \t\tcase 'w':");  t_ct_("\n388 \t\t\twho();"); 
			who();t_ct_("\n389 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'x':t_ct_("\n390 \t\tcase 'x':");  t_ct_("\n391 \t\t\tautosd();"); 
			autosd();t_ct_("\n392 \t\t\tgoto endinc;"); 
			goto endinc;
		case 'y':t_ct_("\n393 \t\tcase 'y':");  t_ct_("\n394 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" vtflts();");  vtflts();}t_ct_("\n395 \t\t\tgoto endinc;"); 
			goto endinc;
endinc:t_ct_("\n396 endinc:");  t_ct_("\n397 \t\t\tfleets();"); 
			fleets();t_ct_("\n398 \t\t\tchkend();"); 
			chkend();t_ct_("\n399 \t\t\tif(mymes!=s->mesnum)"); 
			if((u_ct_("\t\t\t/* mymes",sizeof(mymes),mymes), mymes)!=(u_ct_("\t\t\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)){t_ct_(" getpub();");  getpub();}t_ct_("\n400 \t\t\tif(mycom!=s->comnum)"); 
			if((u_ct_("\t\t\t/* mycom",sizeof(mycom),mycom), mycom)!=(u_ct_("\t\t\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum)){t_ct_(" getcom();");  getcom();}t_ct_("\n401 \t\t\tnow=time(0);"); 
			(now=time(0), u_ct_("\t\t\t/* now",sizeof(now),now), now);t_ct_("\n402 \t\t\tif(vtmode && lsttim<=(now-5L))"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode) && (u_ct_("\t\t\t/* lsttim",sizeof(lsttim),lsttim), lsttim)<=((u_ct_("\t\t\t/* now",sizeof(now),now), now)-5L)){{t_ct_("{"); t_ct_("\n403 \t\t\t\tlsttim=now;"); 
				(lsttim=(now, u_ct_("\t\t\t\t/* now",sizeof(now),now), now), u_ct_("\t\t\t\t/* lsttim",sizeof(lsttim),lsttim), lsttim);t_ct_("\n404 \t\t\t\tupdtim();"); 
				updtim();t_ct_("\n405 \t\t\t\tprompt();"); 
				prompt();t_ct_("\n406 \t\t\t}"); 
			}}t_ct_("\n407 \t\t}"); 
		}t_ct_("\n408 \t\tif(inlen != 0)"); 
		if((u_ct_("\t\t/* inlen",sizeof(inlen),inlen), inlen) != 0){t_ct_(" prompt();");  prompt();}t_ct_("\n409 \t}"); 
	}}t_ct_("\n    /* return */"); 
}

setrand()
{
	int j;
	long loop;
/*	loop=tod();*/char *_ct;t_ct_("\n412 setrand()"); t_ct_("\n417 \tloop = 1;"); 
	(loop = 1, u_ct_("\t/* loop",sizeof(loop),loop), loop);t_ct_("\n418 \tloop=loop % 3600L;"); 
	(loop=(u_ct_("\t/* loop",sizeof(loop),loop), loop) % 3600L, u_ct_("\t/* loop",sizeof(loop),loop), loop);
	while(t_ct_("\n419 \twhile(loop>0)"),(u_ct_("\t/* loop",sizeof(loop),loop), loop)>0){
	{t_ct_("\n420 \t{"); t_ct_("\n421 \t\tloop--;"); 
		(loop--, u_ct_("\t\t/* loop",sizeof(loop),loop), loop+1);t_ct_("\n422 \t\tj=irand(loop);"); 
		(j=irand((u_ct_("\t\t/* loop",sizeof(loop),loop), loop)), u_ct_("\t\t/* j",sizeof(j),j), j);t_ct_("\n423 \t}"); 
	}}t_ct_("\n424 \tj=irand(irand(irand(j)));"); 
	(j=irand(irand(irand((u_ct_("\t/* j",sizeof(j),j), j)))), u_ct_("\t/* j",sizeof(j),j), j);t_ct_("\n    /* return */"); 
}

chkend()
{
	int i,j,k;char *_ct;t_ct_("\n427 chkend()"); t_ct_("\n430 \tj=0;"); 
	(j=0, u_ct_("\t/* j",sizeof(j),j), j);t_ct_("\n431 \tk=0;"); 
	(k=0, u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n432 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n432 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n433 \t\tif(s->owner[i]==mynum)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_(" j++;");  (j++, u_ct_("\t\t/* j",sizeof(j),j), j-1);}}t_ct_("\n434 \tfor(i=0;i<99;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<99;t_ct_("\n434 \tfor(i=0;i<99;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n435 \t\tif(stack[i][0]!=0)"); 
		if((stack[(u_ct_("\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t/* stack[i][0]",sizeof(stack[i][0]),stack[i][0]), stack[i][0])!=0){t_ct_(" k++;");  (k++, u_ct_("\t\t/* k",sizeof(k),k), k-1);}}t_ct_("\n436 \tif(j==0 && k==0)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)==0 && (u_ct_("\t/* k",sizeof(k),k), k)==0){
	{t_ct_("\n437 \t{"); t_ct_("\n438 \t\tprintf(\"\\nYou have lost this round.\\n\");"); 
		printf("\nYou have lost this round.\n");t_ct_("\n439 \t\ti=0;"); 
		(i=0, u_ct_("\t\t/* i",sizeof(i),i), i);
		while(t_ct_("\n440 \t\twhile(s->pppn[i]!=0)"),(s->pppn[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->pppn[i]",sizeof(s->pppn[i]),s->pppn[i]), s->pppn[i])!=0){t_ct_("i++;"); (i++, u_ct_("\t\t/* i",sizeof(i),i), i-1);}t_ct_("\n441 \t\ts->pppn[i]=myp;"); 
		(s->pppn[(u_ct_("\t\t/* i",sizeof(i),i), i)]=(myp, u_ct_("\t\t/* myp",sizeof(myp),myp), myp), u_ct_("\t\t/* s->pppn[i]",sizeof(s->pppn[i]),s->pppn[i]), s->pppn[i]);t_ct_("\n442 \t\ts->pnppn[i]=mypn;"); 
		(s->pnppn[(u_ct_("\t\t/* i",sizeof(i),i), i)]=(mypn, u_ct_("\t\t/* mypn",sizeof(mypn),mypn), mypn), u_ct_("\t\t/* s->pnppn[i]",sizeof(s->pnppn[i]),s->pnppn[i]), s->pnppn[i]);t_ct_("\n443 \t\tleave();"); 
		leave();t_ct_("\n444 \t}"); 
	}}t_ct_("\n445 \tif(j==40)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)==40){
	{t_ct_("\n446 \t{"); t_ct_("\n447 \t\tprintf(\"\\nYou have completely won the war!!\\n\");"); 
		printf("\nYou have completely won the war!!\n");t_ct_("\n448 \t\ts->game=0;"); 
		(s->game=0, u_ct_("\t\t/* s->game",sizeof(s->game),s->game), s->game);t_ct_("\n449 \t\tleave();"); 
		leave();t_ct_("\n450 \t}"); 
	}}
t_ct_("\n452 \tif(time(0)<s->start+game_len)"); 
	if(time(0)<(u_ct_("\t/* s->start",sizeof(s->start),s->start), s->start)+(u_ct_("\t/* game_len",sizeof(game_len),game_len), game_len)){t_ct_(" return(0);");  return(0);}
t_ct_("\n454 \tif(vtmode)"); 
	if((u_ct_("\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){{t_ct_("{"); t_ct_("\n455 \t\tnrmscr();"); 
		nrmscr();t_ct_("\n456 \t\tcls();"); 
		cls();t_ct_("\n457 \t}"); 
	}}t_ct_("\n458 \tprintf(\"\\nThe War is OVER!\\nTime limit exceeded\");"); 
	printf("\nThe War is OVER!\nTime limit exceeded");t_ct_("\n459 \tprintf(\"\\nThe points break down as follows:\\n\");"); 
	printf("\nThe points break down as follows:\n");t_ct_("\n460 \tfor(i=0;i<10;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<10;t_ct_("\n460 \tfor(i=0;i<10;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){
	{t_ct_("\n461 \t{"); t_ct_("\n462 \t\tnumpl[i]=0;"); 
		(numpl[(u_ct_("\t\t/* i",sizeof(i),i), i)]=0, u_ct_("\t\t/* numpl[i]",sizeof(numpl[i]),numpl[i]), numpl[i]); t_ct_("\n463 \t\tnumpr[i]=0;"); 
		(numpr[(u_ct_("\t\t/* i",sizeof(i),i), i)]=0, u_ct_("\t\t/* numpr[i]",sizeof(numpr[i]),numpr[i]), numpr[i]); t_ct_("\n464 \t\tnumsh[0]=0;"); 
		(numsh[0]=0, u_ct_("\t\t/* numsh[0]",sizeof(numsh[0]),numsh[0]), numsh[0]);t_ct_("\n465 \t}"); 
	}}t_ct_("\n466 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n466 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n467 \t\tif(s->owner[i]!=-1)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])!=-1){
		{t_ct_("\n468 \t\t{"); t_ct_("\n469 \t\t\tnumpl[s->owner[i]]++;"); 
			(numpl[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])]++, u_ct_("\t\t\t/* numpl[s->owner[i]]",sizeof(numpl[s->owner[i]]),numpl[s->owner[i]]), numpl[s->owner[i]]-1); t_ct_("\n470 \t\t\tnumpr[s->owner[i]]=numpr[s->owner[i]]+s->produ[i];"); 
			(numpr[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])]=(numpr[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])], u_ct_("\t\t\t/* numpr[s->owner[i]]",sizeof(numpr[s->owner[i]]),numpr[s->owner[i]]), numpr[s->owner[i]])+(s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]), u_ct_("\t\t\t/* numpr[s->owner[i]]",sizeof(numpr[s->owner[i]]),numpr[s->owner[i]]), numpr[s->owner[i]]);t_ct_("\n471 \t\t\tnumsh[s->owner[i]]=numsh[s->owner[i]]+s->ships[i];"); 
			(numsh[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])]=(numsh[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])], u_ct_("\t\t\t/* numsh[s->owner[i]]",sizeof(numsh[s->owner[i]]),numsh[s->owner[i]]), numsh[s->owner[i]])+(s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]), u_ct_("\t\t\t/* numsh[s->owner[i]]",sizeof(numsh[s->owner[i]]),numsh[s->owner[i]]), numsh[s->owner[i]]);t_ct_("\n472 \t\t}"); 
		}}}t_ct_("\n473 \tfor(i=0;i<10;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<10;t_ct_("\n473 \tfor(i=0;i<10;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n474 \t\tif((s->names[i][0] != '\\0') && (s->names[i][0] != 1))"); 
		if(((s->names[(u_ct_("\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]) != '\0') && ((s->names[(u_ct_("\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]) != 1)){t_ct_("\n475 \t\t\tprintf(\"%20s] Planets:%3d production:%4d Ships:%5d  Score:%6d\\n\",\n\t\t\t    s->names[i],numpl[i],numpr[i],numsh[i],(numpl[i]*10+numpr[i]*5+numsh[i]));"); 
			printf("%20s] Planets:%3d production:%4d Ships:%5d  Score:%6d\n",
			    (s->names[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->names[i]",sizeof(s->names[i]),s->names[i]), s->names[i]),(numpl[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* numpl[i]",sizeof(numpl[i]),numpl[i]), numpl[i]),(numpr[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* numpr[i]",sizeof(numpr[i]),numpr[i]), numpr[i]),(numsh[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* numsh[i]",sizeof(numsh[i]),numsh[i]), numsh[i]),((numpl[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* numpl[i]",sizeof(numpl[i]),numpl[i]), numpl[i])*10+numpr[i]*5+numsh[i]));}}t_ct_("\n477 \ts->game--;"); 
	(s->game--, u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game+1);t_ct_("\n478 \tleave();"); 
	leave();t_ct_("\n    /* return */"); 
}

/*
 * who() -- Display the names and PPN's of all player's in the game
 */
who()
{
	int i;
char *_ct;t_ct_("\n484 who()"); t_ct_("\n488 \tfor(i = 0; i < 8; i++)"); 
	for((i = 0, u_ct_("\t/* i",sizeof(i),i), i); (u_ct_("\t/* i",sizeof(i),i), i) < 8;t_ct_("\n488 \tfor(i = 0; i < 8; i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){
	{t_ct_("\n489 \t{"); t_ct_("\n490 \t\tif (s->names[i][0] != '\\0')"); 
		if ((s->names[(u_ct_("\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]) != '\0'){t_ct_("\n491 \t\t\tprintf(\"Player #%d %20s]\\n\",i,s->names[i]);"); 
			printf("Player #%d %20s]\n",(u_ct_("\t\t\t/* i",sizeof(i),i), i),(s->names[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->names[i]",sizeof(s->names[i]),s->names[i]), s->names[i]));}t_ct_("\n492 \t}"); 
	}}t_ct_("\n    /* return */"); 
}

prompt()
{char *_ct;t_ct_("\n495 prompt()"); t_ct_("\n497 \tif(vtmode)"); 
	if((u_ct_("\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(14,12);");  gotoxy(14,12);}
	else{t_ct_("\n498 \telse"); t_ct_(" printf(\"\\nCommand: \");");  printf("\nCommand: ");}t_ct_("\n499 \tfflush(stdout);"); 
	fflush(stdout);t_ct_("\n    /* return */"); 
}

wrapup()
{
	int i,k;char *_ct;t_ct_("\n502 wrapup()"); t_ct_("\n505 \tif(s->names[mynum][0]!=1)"); 
	if((s->names[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)][0], u_ct_("\t/* s->names[mynum][0]",sizeof(s->names[mynum][0]),s->names[mynum][0]), s->names[mynum][0])!=1){{t_ct_("{"); t_ct_("\n506 \t\tlock();"); 
		lock();t_ct_("\n507 \t\tk=s->mesnum+1;"); 
		(k=(u_ct_("\t\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)+1, u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n508 \t\tif(k>4)"); 
		if((u_ct_("\t\t/* k",sizeof(k),k), k)>4){t_ct_(" k=0;");  (k=0, u_ct_("\t\t/* k",sizeof(k),k), k);}t_ct_("\n509 \t\tsprintf(s->public[k],\"  #%2d %s] has left the battle.\\0\",mynum,s->names[mynum]);"); 
		sprintf((s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t/* s->public[k]",sizeof(s->public[k]),s->public[k]), s->public[k]),"  #%2d %s] has left the battle.\0",(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum),(s->names[(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)], u_ct_("\t\t/* s->names[mynum]",sizeof(s->names[mynum]),s->names[mynum]), s->names[mynum]));t_ct_("\n510 \t\ts->public[k][1]='E';"); 
		(s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)][1]='E', u_ct_("\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]);t_ct_("\n511 \t\ts->public[k][0]=mynum;"); 
		(s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)][0]=(mynum, u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0]);t_ct_("\n512 \t\ts->mesnum=k;"); 
		(s->mesnum=(k, u_ct_("\t\t/* k",sizeof(k),k), k), u_ct_("\t\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n513 \t\ts->meslok--;"); 
		(s->meslok--, u_ct_("\t\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n514 \t}"); 
	}}t_ct_("\n515 \tif(vtmode)"); 
	if((u_ct_("\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){ {t_ct_(" {"); t_ct_("\n516 \t\tnrmscr();"); 
		nrmscr();t_ct_("\n517 \t\tcls();"); 
		cls();t_ct_("\n518 \t}"); 
	}}t_ct_("\n519 \tif(!twice && !s->twicej[14])"); 
	if(!(u_ct_("\t/* twice",sizeof(twice),twice), twice) && !(u_ct_("\t/* s->twicej[14]",sizeof(s->twicej[14]),s->twicej[14]), s->twicej[14])){{t_ct_("{"); t_ct_("\n520 \t\ti=0;"); 
		(i=0, u_ct_("\t\t/* i",sizeof(i),i), i);
		while(t_ct_("\n521 \t\twhile(s->twicej[i])"),(s->twicej[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->twicej[i]",sizeof(s->twicej[i]),s->twicej[i]), s->twicej[i])){t_ct_(" i++;");  (i++, u_ct_("\t\t/* i",sizeof(i),i), i-1);}t_ct_("\n522 \t\ts->twicej[i]=myp;"); 
		(s->twicej[(u_ct_("\t\t/* i",sizeof(i),i), i)]=(myp, u_ct_("\t\t/* myp",sizeof(myp),myp), myp), u_ct_("\t\t/* s->twicej[i]",sizeof(s->twicej[i]),s->twicej[i]), s->twicej[i]);t_ct_("\n523 \t\ts->twiceg[i]=mypn;"); 
		(s->twiceg[(u_ct_("\t\t/* i",sizeof(i),i), i)]=(mypn, u_ct_("\t\t/* mypn",sizeof(mypn),mypn), mypn), u_ct_("\t\t/* s->twiceg[i]",sizeof(s->twiceg[i]),s->twiceg[i]), s->twiceg[i]);t_ct_("\n524 \t}"); 
	}}t_ct_("\n525 \tfor (i=0;i<40;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n525 \tfor (i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_(" if (s->owner[i]==mynum)");  if ((s->owner[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){
	{t_ct_("\n526 \t{"); t_ct_("\n527 \t\ts->owner[i] = -1;"); 
		(s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)] = -1, u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i]);t_ct_("\n528 \t\tif(s->produ[i]!=10)"); 
		if((s->produ[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i])!=10){t_ct_("\n529 \t\t\ts->produ[i]=prandom(10);"); 
			(s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=prandom(10), u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]);}t_ct_("\n530 \t}"); 
	}}}t_ct_("\n531 \ts->names[mynum][0]='\\0';"); 
	(s->names[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)][0]='\0', u_ct_("\t/* s->names[mynum][0]",sizeof(s->names[mynum][0]),s->names[mynum][0]), s->names[mynum][0]);
t_ct_("\n533 \ts->game--;"); 
	(s->game--, u_ct_("\t/* s->game",sizeof(s->game),s->game), s->game+1);t_ct_("\n534 \tprintf(\"\\nBye Bye\\n\");"); 
	printf("\nBye Bye\n");t_ct_("\n535 \tleave();"); 
	leave();t_ct_("\n    /* return */"); 
}

individual()
{
	int i,j;char *_ct;t_ct_("\n538 individual()"); t_ct_("\n541 \tprintf(\"\\nSummary of player #:\");"); 
	printf("\nSummary of player #:");t_ct_("\n542 \ti=getnum(&j);"); 
	(i=getnum(&j), u_ct_("\t/* i",sizeof(i),i), i);t_ct_("\n543 \tprintf(\"\\n\");"); 
	printf("\n");t_ct_("\n544 \tif(j>0 && j<10)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)>0 && (u_ct_("\t/* j",sizeof(j),j), j)<10){t_ct_(" printf(\"Name: %s]\\n\",s->names[j]);");  printf("Name: %s]\n",(s->names[(u_ct_("\t/* j",sizeof(j),j), j)], u_ct_("\t/* s->names[j]",sizeof(s->names[j]),s->names[j]), s->names[j]));}t_ct_("\n545 \tfor (i=0;i<40;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n545 \tfor (i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n546 \t\tif(s->owner[i]==j)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* j",sizeof(j),j), j)){t_ct_("\n547 \t\t\tprintf(\"Planet #%2d  Distance from #%2d = %4d\\n\",\n\t\t\t    i,home,prange(home,i));"); 
			printf("Planet #%2d  Distance from #%2d = %4d\n",
			    (u_ct_("\t\t\t/* i",sizeof(i),i), i),(u_ct_("\t\t\t/* home",sizeof(home),home), home),prange((u_ct_("\t\t\t/* home",sizeof(home),home), home),(u_ct_("\t\t\t/* i",sizeof(i),i), i)));}
}t_ct_("\n550 \tif (s->owner[home]!=mynum)"); 
	if ((s->owner[(u_ct_("\t/* home",sizeof(home),home), home)], u_ct_("\t/* s->owner[home]",sizeof(s->owner[home]),s->owner[home]), s->owner[home])!=(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_("\n551 \t\tprintf(\"\\nNOTE: You no longer control your home world!\\n\");"); 
		printf("\nNOTE: You no longer control your home world!\n");}t_ct_("\n    /* return */"); 
}

/*
 * Initialize the Game Board
 */
setup()
{
	int i;
	int locx,locy,j;char *_ct;t_ct_("\n557 setup()"); t_ct_("\n561 \ts->mesnum=s->comnum=s->meslok=0;"); 
	(s->mesnum=(s->comnum=(s->meslok=0, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok), u_ct_("\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum), u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n562 \ts->start=time(0);"); 
	(s->start=time(0), u_ct_("\t/* s->start",sizeof(s->start),s->start), s->start);t_ct_("\n563 \tfor(i=0;i<10;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<10;t_ct_("\n563 \tfor(i=0;i<10;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("s->names[i][0]='\\0';"); (s->names[(u_ct_("\t/* i",sizeof(i),i), i)][0]='\0', u_ct_("\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]);}t_ct_("\n564 \tfor(i=0;i<20;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<20;t_ct_("\n564 \tfor(i=0;i<20;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_(" s->pppn[i]=0;");  (s->pppn[(u_ct_("\t/* i",sizeof(i),i), i)]=0, u_ct_("\t/* s->pppn[i]",sizeof(s->pppn[i]),s->pppn[i]), s->pppn[i]);}t_ct_("\n565 \tfor(i=0;i<15;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<15;t_ct_("\n565 \tfor(i=0;i<15;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_(" s->twicej[i]=0;");  (s->twicej[(u_ct_("\t/* i",sizeof(i),i), i)]=0, u_ct_("\t/* s->twicej[i]",sizeof(s->twicej[i]),s->twicej[i]), s->twicej[i]);}

	/* Clear Galaxy */t_ct_("\n568 \tfor(i=1;i<16;i++)"); 
	for((i=1, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<16;t_ct_("\n568 \tfor(i=1;i<16;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n569 \t\tfor(j=1;j<16;j++)"); 
		for((j=1, u_ct_("\t\t/* j",sizeof(j),j), j);(u_ct_("\t\t/* j",sizeof(j),j), j)<16;t_ct_("\n569 \t\tfor(j=1;j<16;j++)"),(j++, u_ct_("\t\t/* j",sizeof(j),j), j-1)){{t_ct_("{"); t_ct_("\n570 \t\t\ts->scaner[i][j] = -1;"); 
			(s->scaner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)][(u_ct_("\t\t\t/* j",sizeof(j),j), j)] = -1, u_ct_("\t\t\t/* s->scaner[i][j]",sizeof(s->scaner[i][j]),s->scaner[i][j]), s->scaner[i][j]);t_ct_("\n571 \t\t}"); 
		}}t_ct_("\n572 \t}"); 
	}}t_ct_("\n573 \tfor (i=0;i<40;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n573 \tfor (i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){
	{t_ct_("\n574 \t{"); t_ct_("\n575 \t\ts->owner[i] = -1;"); 
		(s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)] = -1, u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i]);t_ct_("\n576 \t\ts->xpos[i]=prandom(x_siz);"); 
		(s->xpos[(u_ct_("\t\t/* i",sizeof(i),i), i)]=prandom((u_ct_("\t\t/* x_siz",sizeof(x_siz),x_siz), x_siz)), u_ct_("\t\t/* s->xpos[i]",sizeof(s->xpos[i]),s->xpos[i]), s->xpos[i]);t_ct_("\n577 \t\tlocx=s->xpos[i];"); 
		(locx=(s->xpos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->xpos[i]",sizeof(s->xpos[i]),s->xpos[i]), s->xpos[i]), u_ct_("\t\t/* locx",sizeof(locx),locx), locx);t_ct_("\n578 \t\ts->ypos[i]=prandom(y_siz);"); 
		(s->ypos[(u_ct_("\t\t/* i",sizeof(i),i), i)]=prandom((u_ct_("\t\t/* y_siz",sizeof(y_siz),y_siz), y_siz)), u_ct_("\t\t/* s->ypos[i]",sizeof(s->ypos[i]),s->ypos[i]), s->ypos[i]);t_ct_("\n579 \t\tlocy=s->ypos[i];"); 
		(locy=(s->ypos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->ypos[i]",sizeof(s->ypos[i]),s->ypos[i]), s->ypos[i]), u_ct_("\t\t/* locy",sizeof(locy),locy), locy);t_ct_("\n580 \t\ts->zpos[i]=prandom(z_siz);"); 
		(s->zpos[(u_ct_("\t\t/* i",sizeof(i),i), i)]=prandom((u_ct_("\t\t/* z_siz",sizeof(z_siz),z_siz), z_siz)), u_ct_("\t\t/* s->zpos[i]",sizeof(s->zpos[i]),s->zpos[i]), s->zpos[i]);t_ct_("\n581 \t\ts->produ[i]=prandom(10);"); 
		(s->produ[(u_ct_("\t\t/* i",sizeof(i),i), i)]=prandom(10), u_ct_("\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]);t_ct_("\n582 \t\ts->lstup[i]=time(0);"); 
		(s->lstup[(u_ct_("\t\t/* i",sizeof(i),i), i)]=time(0), u_ct_("\t\t/* s->lstup[i]",sizeof(s->lstup[i]),s->lstup[i]), s->lstup[i]);t_ct_("\n583 \t\ts->ships[i]=prandom(s->produ[i]*25);"); 
		(s->ships[(u_ct_("\t\t/* i",sizeof(i),i), i)]=prandom((s->produ[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i])*25), u_ct_("\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]);t_ct_("\n584 \t\ts->scaner[locx][locy]=i;"); 
		(s->scaner[(u_ct_("\t\t/* locx",sizeof(locx),locx), locx)][(u_ct_("\t\t/* locy",sizeof(locy),locy), locy)]=(i, u_ct_("\t\t/* i",sizeof(i),i), i), u_ct_("\t\t/* s->scaner[locx][locy]",sizeof(s->scaner[locx][locy]),s->scaner[locx][locy]), s->scaner[locx][locy]);t_ct_("\n585 \t}"); 
	}}
	/* Get 2 Planets RANDOMLY and make their prod to 25 */t_ct_("\n587 \tfor (i=0;i<2;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<2;t_ct_("\n587 \tfor (i=0;i<2;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n588 \t\tj=prandom(40);"); 
		(j=prandom(40), u_ct_("\t\t/* j",sizeof(j),j), j);t_ct_("\n589 \t\ts->produ[j]=25;"); 
		(s->produ[(u_ct_("\t\t/* j",sizeof(j),j), j)]=25, u_ct_("\t\t/* s->produ[j]",sizeof(s->produ[j]),s->produ[j]), s->produ[j]);t_ct_("\n590 \t}"); 
	}}t_ct_("\n591 \tfor(i=0;i<10;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<10;t_ct_("\n591 \tfor(i=0;i<10;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n592 \t\ts->names[i][0]='\\0';"); 
		(s->names[(u_ct_("\t\t/* i",sizeof(i),i), i)][0]='\0', u_ct_("\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]);}t_ct_("\n    /* return */"); 
}

area()
{
	int base,vol,i;
char *_ct;t_ct_("\n595 area()"); t_ct_("\n599 \tprintf(\"\\nBase Planet: \");"); 
	printf("\nBase Planet: ");t_ct_("\n600 \ti=getnum(&base);"); 
	(i=getnum(&base), u_ct_("\t/* i",sizeof(i),i), i);t_ct_("\n601 \tif(base<0 || base>39)"); 
	if((u_ct_("\t/* base",sizeof(base),base), base)<0 || (u_ct_("\t/* base",sizeof(base),base), base)>39){t_ct_("\n602 \t\treturn(0);"); 
		return(0);}t_ct_("\n603 \tprintf(\"Range: \");"); 
	printf("Range: ");t_ct_("\n604 \ti=getnum(&vol);"); 
	(i=getnum(&vol), u_ct_("\t/* i",sizeof(i),i), i);
t_ct_("\n606 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n606 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n607 \t\tif(prange(base,i)<=vol)"); 
		if(prange((u_ct_("\t\t/* base",sizeof(base),base), base),(u_ct_("\t\t/* i",sizeof(i),i), i))<=(u_ct_("\t\t/* vol",sizeof(vol),vol), vol)){
		{t_ct_("\n608 \t\t{"); t_ct_("\n609 \t\t\tprintf(\"Planet #%3d Range=%3d \",i,prange(base,i));"); 
			printf("Planet #%3d Range=%3d ",(u_ct_("\t\t\t/* i",sizeof(i),i), i),prange((u_ct_("\t\t\t/* base",sizeof(base),base), base),(u_ct_("\t\t\t/* i",sizeof(i),i), i)));t_ct_("\n610 \t\t\tif (s->owner[i] == (-1))"); 
			if ((s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i]) == (-1)){t_ct_("\n611 \t\t\t\tprintf(\"Own= *Neutral*\\n\");"); 
				printf("Own= *Neutral*\n");}
			else{t_ct_("\n612 \t\t\telse"); t_ct_("\n613 \t\t\t\tprintf(\"Own = %s]\\n\",s->names[s->owner[i]]);"); 
				printf("Own = %s]\n",(s->names[(s->owner[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])], u_ct_("\t\t\t\t/* s->names[s->owner[i]]",sizeof(s->names[s->owner[i]]),s->names[s->owner[i]]), s->names[s->owner[i]]));}t_ct_("\n614 \t\t}"); 
		}}}t_ct_("\n    /* return */"); 
}


prandom(range)
int range;
{char *_ct;t_ct_("\n618 prandom(range)"); t_ct_("\n621 \treturn(irand(range)+1);"); 
	return(irand((u_ct_("\t/* range",sizeof(range),range), range))+1);
}

update()
{
	long now;
	int i,j,k;char *_ct;t_ct_("\n624 update()"); t_ct_("\n628 \tnow=time(0);"); 
	(now=time(0), u_ct_("\t/* now",sizeof(now),now), now);t_ct_("\n629 \ts->alive[mynum]=now;"); 
	(s->alive[(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)]=(now, u_ct_("\t/* now",sizeof(now),now), now), u_ct_("\t/* s->alive[mynum]",sizeof(s->alive[mynum]),s->alive[mynum]), s->alive[mynum]);t_ct_("\n630 \tfor(i=0;i<8;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<8;t_ct_("\n630 \tfor(i=0;i<8;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n631 \t\tif(s->alive[i]<(now-420L) && s->alive[i]!=0 && s->names[i][0]!='\\0')"); 
		if((s->alive[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->alive[i]",sizeof(s->alive[i]),s->alive[i]), s->alive[i])<((u_ct_("\t\t/* now",sizeof(now),now), now)-420L) && (s->alive[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->alive[i]",sizeof(s->alive[i]),s->alive[i]), s->alive[i])!=0 && (s->names[(u_ct_("\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0])!='\0'){{t_ct_("{"); t_ct_("\n632 \t\t\ts->alive[i]=0;"); 
			(s->alive[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=0, u_ct_("\t\t\t/* s->alive[i]",sizeof(s->alive[i]),s->alive[i]), s->alive[i]);t_ct_("\n633 \t\t\tfor(j=0;j<39;j++)"); 
			for((j=0, u_ct_("\t\t\t/* j",sizeof(j),j), j);(u_ct_("\t\t\t/* j",sizeof(j),j), j)<39;t_ct_("\n633 \t\t\tfor(j=0;j<39;j++)"),(j++, u_ct_("\t\t\t/* j",sizeof(j),j), j-1)){t_ct_(" if(s->owner[j]==i)");  if((s->owner[(u_ct_("\t\t\t/* j",sizeof(j),j), j)], u_ct_("\t\t\t/* s->owner[j]",sizeof(s->owner[j]),s->owner[j]), s->owner[j])==(u_ct_("\t\t\t/* i",sizeof(i),i), i)){t_ct_(" s->owner[j] = -1;");  (s->owner[(u_ct_("\t\t\t/* j",sizeof(j),j), j)] = -1, u_ct_("\t\t\t/* s->owner[j]",sizeof(s->owner[j]),s->owner[j]), s->owner[j]);}}t_ct_("\n634 \t\t\tlock();"); 
			lock();t_ct_("\n635 \t\t\tk=s->mesnum+1;"); 
			(k=(u_ct_("\t\t\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)+1, u_ct_("\t\t\t/* k",sizeof(k),k), k);t_ct_("\n636 \t\t\tif(k>4)"); 
			if((u_ct_("\t\t\t/* k",sizeof(k),k), k)>4){t_ct_(" k=0;");  (k=0, u_ct_("\t\t\t/* k",sizeof(k),k), k);}t_ct_("\n637 \t\t\tsprintf(s->public[k],\"  %s] has been cleared.\\0\",s->names[i]);"); 
			sprintf((s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t/* s->public[k]",sizeof(s->public[k]),s->public[k]), s->public[k]),"  %s] has been cleared.\0",(s->names[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->names[i]",sizeof(s->names[i]),s->names[i]), s->names[i]));t_ct_("\n638 \t\t\ts->public[k][1]='E';"); 
			(s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)][1]='E', u_ct_("\t\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]);t_ct_("\n639 \t\t\ts->public[k][0]=8;"); 
			(s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)][0]=8, u_ct_("\t\t\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0]);t_ct_("\n640 \t\t\ts->mesnum=k;"); 
			(s->mesnum=(k, u_ct_("\t\t\t/* k",sizeof(k),k), k), u_ct_("\t\t\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n641 \t\t\ts->meslok--;"); 
			(s->meslok--, u_ct_("\t\t\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n642 \t\t\ts->names[i][0]='\\0';"); 
			(s->names[(u_ct_("\t\t\t/* i",sizeof(i),i), i)][0]='\0', u_ct_("\t\t\t/* s->names[i][0]",sizeof(s->names[i][0]),s->names[i][0]), s->names[i][0]);t_ct_("\n643 \t\t\ts->game--;"); 
			(s->game--, u_ct_("\t\t\t/* s->game",sizeof(s->game),s->game), s->game+1);t_ct_("\n644 \t\t}"); 
		}}t_ct_("\n645 \t}"); 
	}}
t_ct_("\n647 \tfor (i=0;i<40;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n647 \tfor (i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n648 \t\tif( s->lstup[i]<(now-45L) )"); 
		if( (s->lstup[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->lstup[i]",sizeof(s->lstup[i]),s->lstup[i]), s->lstup[i])<((u_ct_("\t\t/* now",sizeof(now),now), now)-45L) ){
		{t_ct_("\n649 \t\t{"); t_ct_("\n650 \t\t\ts->lstup[i]=now;"); 
			(s->lstup[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=(now, u_ct_("\t\t\t/* now",sizeof(now),now), now), u_ct_("\t\t\t/* s->lstup[i]",sizeof(s->lstup[i]),s->lstup[i]), s->lstup[i]);t_ct_("\n651 \t\t\ts->ships[i]=s->ships[i]+s->produ[i]*1;"); 
			(s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=(s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i])+(s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i])*1, u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]);t_ct_("\n652 \t\t\tif(s->owner[i]==-1 && s->ships[i]>250)"); 
			if((s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==-1 && (s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i])>250){t_ct_(" s->ships[i]=250;");  (s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=250, u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]);}t_ct_("\n653 \t\t\tif(s->ships[i]>50*s->produ[i] && s->produ[i]<10)"); 
			if((s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i])>50*(s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]) && (s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i])<10){t_ct_(" s->produ[i]++;");  (s->produ[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]++, u_ct_("\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]-1);}t_ct_("\n654 \t\t\tif(s->ships[i]>1500)"); 
			if((s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i])>1500){t_ct_(" s->ships[i]=1500;");  (s->ships[(u_ct_("\t\t\t/* i",sizeof(i),i), i)]=1500, u_ct_("\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]);}t_ct_("\n655 \t\t}"); 
		}}}t_ct_("\n    /* return */"); 
}

galaxy()
{
	int i;
	extern char *ctime();char *_ct;t_ct_("\n658 galaxy()"); t_ct_("\n662 \tprintf(\"\\nGalaxy status at %s\\n\\n\",sdate(0L));"); 
	printf("\nGalaxy status at %s\n\n",sdate(0L));t_ct_("\n663 \tfor (i=0;i<40;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n663 \tfor (i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){
	{t_ct_("\n664 \t{"); t_ct_("\n665 \t\tprintf(\"#%2d (%2d,%2d) \",i, s->xpos[i], s->ypos[i]);"); 
		printf("#%2d (%2d,%2d) ",(u_ct_("\t\t/* i",sizeof(i),i), i), (s->xpos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->xpos[i]",sizeof(s->xpos[i]),s->xpos[i]), s->xpos[i]), (s->ypos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->ypos[i]",sizeof(s->ypos[i]),s->ypos[i]), s->ypos[i]));t_ct_("\n666 \t\tif (s->owner[i] == (-1))"); 
		if ((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i]) == (-1)){t_ct_("\n667 \t\t\tprintf(\"*Neutral*\\n\");"); 
			printf("*Neutral*\n");}
		else{t_ct_("\n668 \t\telse"); t_ct_("\n669 \t\t\tprintf(\"%s]\\n\",s->names[s->owner[i]]);"); 
			printf("%s]\n",(s->names[(s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])], u_ct_("\t\t\t/* s->names[s->owner[i]]",sizeof(s->names[s->owner[i]]),s->names[s->owner[i]]), s->names[s->owner[i]]));}t_ct_("\n670 \t}"); 
	}}t_ct_("\n671 \tprintf(\"\\n\");"); 
	printf("\n");t_ct_("\n    /* return */"); 
}

oneplanet()
{
	int i;char *_ct;t_ct_("\n674 oneplanet()"); t_ct_("\n677 \ti = -1;"); 
	(i = -1, u_ct_("\t/* i",sizeof(i),i), i);
	while(t_ct_("\n678 \twhile(i<0 || i>39)"),(u_ct_("\t/* i",sizeof(i),i), i)<0 || (u_ct_("\t/* i",sizeof(i),i), i)>39){
	{t_ct_("\n679 \t{"); t_ct_("\n680 \t\tprintf(\"\\nWhich Planet: \");"); 
		printf("\nWhich Planet: ");t_ct_("\n681 \t\tgetnum(&i);"); 
		getnum(&i);t_ct_("\n682 \t}"); 
	}}t_ct_("\n683 \tif (s->owner[i] == (-1))"); 
	if ((s->owner[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i]) == (-1)){t_ct_("\n684 \t\tprintf(\"#%2d Owner= *Neutral*\\n\",i);"); 
		printf("#%2d Owner= *Neutral*\n",(u_ct_("\t\t/* i",sizeof(i),i), i));}
	else{t_ct_("\n685 \telse"); t_ct_("\n686 \t\tprintf(\"#%2d Owner= %s] \",i,s->names[s->owner[i]]);"); 
		printf("#%2d Owner= %s] ",(u_ct_("\t\t/* i",sizeof(i),i), i),(s->names[(s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])], u_ct_("\t\t/* s->names[s->owner[i]]",sizeof(s->names[s->owner[i]]),s->names[s->owner[i]]), s->names[s->owner[i]]));}t_ct_("\n687 \tif(s->owner[i]==mynum)"); 
	if((s->owner[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_("\n688 \t\tprintf(\" Prod:%3d Ships:%5d\\n\",s->produ[i],s->ships[i]);"); 
		printf(" Prod:%3d Ships:%5d\n",(s->produ[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]),(s->ships[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]));}
	else{t_ct_("\n689 \telse"); t_ct_("\n690 \t\tprintf(\"\\n\");"); 
		printf("\n");}t_ct_("\n    /* return */"); 
}

fleets()
{
	int i,j,k,l,m,flg;
	int despla,desown,desarm,myarm;
	long now;char *_ct;t_ct_("\n693 fleets()"); t_ct_("\n698 \tnow=time(0);"); 
	(now=time(0), u_ct_("\t/* now",sizeof(now),now), now);t_ct_("\n699 \tif(pirtim!=0L && pirtim<now)"); 
	if((u_ct_("\t/* pirtim",sizeof(pirtim),pirtim), pirtim)!=0L && (u_ct_("\t/* pirtim",sizeof(pirtim),pirtim), pirtim)<(u_ct_("\t/* now",sizeof(now),now), now)){{t_ct_("{"); t_ct_("\n700 \t\tpirtim=0L;"); 
		(pirtim=0L, u_ct_("\t\t/* pirtim",sizeof(pirtim),pirtim), pirtim);t_ct_("\n701 \t\tif(vtmode)"); 
		if((u_ct_("\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(24,1);");  gotoxy(24,1);}t_ct_("\n702 \t\tprintf(\"\\nYour fleet #%d has been attacked by pirates!\\n\",pirflt);"); 
		printf("\nYour fleet #%d has been attacked by pirates!\n",(u_ct_("\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt));t_ct_("\n703 \t\tprintf(\"Your fleet has: %d\\n\",stack[pirflt][0]);"); 
		printf("Your fleet has: %d\n",(stack[(u_ct_("\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)][0], u_ct_("\t\t/* stack[pirflt][0]",sizeof(stack[pirflt][0]),stack[pirflt][0]), stack[pirflt][0]));t_ct_("\n704 \t\tprintf(\"Pirate's fleet: %d\\n\",pirsiz);"); 
		printf("Pirate's fleet: %d\n",(u_ct_("\t\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz));t_ct_("\n705 \t\tif(pirsiz>stack[pirflt][0])"); 
		if((u_ct_("\t\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz)>(stack[(u_ct_("\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)][0], u_ct_("\t\t/* stack[pirflt][0]",sizeof(stack[pirflt][0]),stack[pirflt][0]), stack[pirflt][0])){{t_ct_("{"); t_ct_("\n706 \t\t\tprintf(\"Your fleet has been destroyed!\\n\");"); 
			printf("Your fleet has been destroyed!\n");t_ct_("\n707 \t\t\tevents[pirflt]=0L;"); 
			(events[(u_ct_("\t\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)]=0L, u_ct_("\t\t\t/* events[pirflt]",sizeof(events[pirflt]),events[pirflt]), events[pirflt]);t_ct_("\n708 \t\t}"); 
		}}
		else{t_ct_("\n709 \t\telse");  {t_ct_(" {"); t_ct_("\n710 \t\t\tstack[pirflt][0]=stack[pirflt][0]-(pirsiz/3);"); 
			(stack[(u_ct_("\t\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)][0]=(stack[(u_ct_("\t\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)][0], u_ct_("\t\t\t/* stack[pirflt][0]",sizeof(stack[pirflt][0]),stack[pirflt][0]), stack[pirflt][0])-((u_ct_("\t\t\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz)/3), u_ct_("\t\t\t/* stack[pirflt][0]",sizeof(stack[pirflt][0]),stack[pirflt][0]), stack[pirflt][0]);t_ct_("\n711 \t\t\tprintf(\"%d ships escaped from the pirates!\\n\",stack[pirflt][0]);"); 
			printf("%d ships escaped from the pirates!\n",(stack[(u_ct_("\t\t\t/* pirflt",sizeof(pirflt),pirflt), pirflt)][0], u_ct_("\t\t\t/* stack[pirflt][0]",sizeof(stack[pirflt][0]),stack[pirflt][0]), stack[pirflt][0]));t_ct_("\n712 \t\t}"); 
		}}t_ct_("\n713 \t\tpirsiz=0;"); 
		(pirsiz=0, u_ct_("\t\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz);t_ct_("\n714 \t\tif(vtmode)"); 
		if((u_ct_("\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" prompt();");  prompt();}t_ct_("\n715 \t}"); 
	}}t_ct_("\n716 \tfor (i=0;i<99;i++)"); 
	for ((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<99;t_ct_("\n716 \tfor (i=0;i<99;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n717 \t\tif (events[i]!=0L)"); 
		if ((events[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])!=0L){t_ct_("\n718 \t\t\tif (events[i]<now)"); 
			if ((events[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])<(u_ct_("\t\t\t/* now",sizeof(now),now), now)){
			{t_ct_("\n719 \t\t\t{"); t_ct_("\n720 \t\t\t\tmyarm=stack[i][0];"); 
				(myarm=(stack[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t\t\t/* stack[i][0]",sizeof(stack[i][0]),stack[i][0]), stack[i][0]), u_ct_("\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm);t_ct_("\n721 \t\t\t\tdespla=stack[i][1];"); 
				(despla=(stack[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)][1], u_ct_("\t\t\t\t/* stack[i][1]",sizeof(stack[i][1]),stack[i][1]), stack[i][1]), u_ct_("\t\t\t\t/* despla",sizeof(despla),despla), despla);t_ct_("\n722 \t\t\t\tdesown=s->owner[despla];"); 
				(desown=(s->owner[(u_ct_("\t\t\t\t/* despla",sizeof(despla),despla), despla)], u_ct_("\t\t\t\t/* s->owner[despla]",sizeof(s->owner[despla]),s->owner[despla]), s->owner[despla]), u_ct_("\t\t\t\t/* desown",sizeof(desown),desown), desown);t_ct_("\n723 \t\t\t\tdesarm=s->ships[despla];"); 
				(desarm=(s->ships[(u_ct_("\t\t\t\t/* despla",sizeof(despla),despla), despla)], u_ct_("\t\t\t\t/* s->ships[despla]",sizeof(s->ships[despla]),s->ships[despla]), s->ships[despla]), u_ct_("\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm);t_ct_("\n724 \t\t\t\tif(mynum==desown)"); 
				if((u_ct_("\t\t\t\t/* mynum",sizeof(mynum),mynum), mynum)==(u_ct_("\t\t\t\t/* desown",sizeof(desown),desown), desown)){
				{t_ct_("\n725 \t\t\t\t{"); t_ct_("\n726 \t\t\t\t\tif(vtmode)"); 
					if((u_ct_("\t\t\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(24,1);");  gotoxy(24,1);}t_ct_("\n727 \t\t\t\t\tprintf(\"\\nPlanet %d recieves reinforcements\\n\",\n\t\t\t\t\t    despla);"); 
					printf("\nPlanet %d recieves reinforcements\n",
					    (u_ct_("\t\t\t\t\t/* despla",sizeof(despla),despla), despla));t_ct_("\n729 \t\t\t\t\ts->ships[despla]=desarm+myarm;"); 
					(s->ships[(u_ct_("\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=(u_ct_("\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm)+(myarm, u_ct_("\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm), u_ct_("\t\t\t\t\t/* s->ships[despla]",sizeof(s->ships[despla]),s->ships[despla]), s->ships[despla]);t_ct_("\n730 \t\t\t\t}"); 
				}}
				else{t_ct_("\n731 \t\t\t\telse"); 
				{t_ct_("\n732 \t\t\t\t{"); t_ct_("\n733 \t\t\t\t\tif(vtmode)"); 
					if((u_ct_("\t\t\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(24,1);");  gotoxy(24,1);}t_ct_("\n734 \t\t\t\t\tprintf(\"\\nInvasion alert! Your fleet arrived at %d\",despla);"); 
					printf("\nInvasion alert! Your fleet arrived at %d",(u_ct_("\t\t\t\t\t/* despla",sizeof(despla),despla), despla));t_ct_("\n735 \t\t\t\t\tprintf(\"\\nYour fleet has: %d\\nDefending fleet:%d\\n\",myarm,desarm);"); 
					printf("\nYour fleet has: %d\nDefending fleet:%d\n",(u_ct_("\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm),(u_ct_("\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm));t_ct_("\n736 \t\t\t\t\tflg=0;"); 
					(flg=0, u_ct_("\t\t\t\t\t/* flg",sizeof(flg),flg), flg);t_ct_("\n737 \t\t\t\t\tlock();"); 
					lock();
					while(t_ct_("\n738 \t\t\t\t\twhile(myarm>0 && desarm>0)"),(u_ct_("\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm)>0 && (u_ct_("\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm)>0){
					{t_ct_("\n739 \t\t\t\t\t{"); t_ct_("\n740 \t\t\t\t\t\tl=10;"); 
						(l=10, u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l);t_ct_("\n741 \t\t\t\t\t\tl=l*myarm/desarm;"); 
						(l=(u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l)*(u_ct_("\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm)/(desarm, u_ct_("\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm), u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l);t_ct_("\n742 \t\t\t\t\t\tif(l>18)"); 
						if((u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l)>18){t_ct_(" l=18;");  (l=18, u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l);}t_ct_("\n743 \t\t\t\t\t\tif(l<3)"); 
						if((u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l)<3){t_ct_(" l=3;");  (l=3, u_ct_("\t\t\t\t\t\t/* l",sizeof(l),l), l);}t_ct_("\n744 \t\t\t\t\t\tif(flg=0)"); 
						if((flg=0, u_ct_("\t\t\t\t\t\t/* flg",sizeof(flg),flg), flg)){
						{t_ct_("\n745 \t\t\t\t\t\t{"); t_ct_("\n746 \t\t\t\t\t\t\tflg=irand(5)+1;"); 
							(flg=irand(5)+1, u_ct_("\t\t\t\t\t\t\t/* flg",sizeof(flg),flg), flg);t_ct_("\n747 \t\t\t\t\t\t\tif(flg<=3)"); 
							if((u_ct_("\t\t\t\t\t\t\t/* flg",sizeof(flg),flg), flg)<=3){t_ct_(" goto PREPARED;");  goto PREPARED;}t_ct_("\n748 \t\t\t\t\t\t}"); 
						}}
						while(t_ct_("\n749 \t\t\t\t\t\twhile( (m=irand(myarm)+1) < myarm/2)"),((m=irand((u_ct_("\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm))+1, u_ct_("\t\t\t\t\t\t/* m",sizeof(m),m), m)) < (u_ct_("\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm)/2){t_ct_(";"); ;}t_ct_("\n750 \t\t\t\t\t\tfor(j=0;j<m;j++)"); 
						for((j=0, u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j);(u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j)<(u_ct_("\t\t\t\t\t\t/* m",sizeof(m),m), m);t_ct_("\n750 \t\t\t\t\t\tfor(j=0;j<m;j++)"),(j++, u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j-1)){t_ct_("\n751 \t\t\t\t\t\t\tif(desarm>0)"); 
							if((u_ct_("\t\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm)>0){t_ct_("\n752 \t\t\t\t\t\t\t\tif(irand(20)<=l)"); 
								if(irand(20)<=(u_ct_("\t\t\t\t\t\t\t\t/* l",sizeof(l),l), l)){t_ct_(" desarm--;");  (desarm--, u_ct_("\t\t\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm+1);}}}
PREPARED:t_ct_("\n753 PREPARED:"); t_ct_("\n754 \t\t\t\t\t\tif(desarm>0)"); 
						if((u_ct_("\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm)>0){
						{t_ct_("\n755 \t\t\t\t\t\t{"); 
							while(t_ct_("\n756 \t\t\t\t\t\t\twhile( (m=irand(desarm)+1) < desarm/2)"),((m=irand((u_ct_("\t\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm))+1, u_ct_("\t\t\t\t\t\t\t/* m",sizeof(m),m), m)) < (u_ct_("\t\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm)/2){t_ct_(";"); ;}t_ct_("\n757 \t\t\t\t\t\t\tfor(j=0;j<m;j++)"); 
							for((j=0, u_ct_("\t\t\t\t\t\t\t/* j",sizeof(j),j), j);(u_ct_("\t\t\t\t\t\t\t/* j",sizeof(j),j), j)<(u_ct_("\t\t\t\t\t\t\t/* m",sizeof(m),m), m);t_ct_("\n757 \t\t\t\t\t\t\tfor(j=0;j<m;j++)"),(j++, u_ct_("\t\t\t\t\t\t\t/* j",sizeof(j),j), j-1)){t_ct_("\n758 \t\t\t\t\t\t\t\tif(myarm>0)"); 
								if((u_ct_("\t\t\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm)>0){t_ct_("\n759 \t\t\t\t\t\t\t\t\tif(irand(20)>=l)"); 
									if(irand(20)>=(u_ct_("\t\t\t\t\t\t\t\t\t/* l",sizeof(l),l), l)){t_ct_(" myarm--;");  (myarm--, u_ct_("\t\t\t\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm+1);}
}}t_ct_("\n761 \t\t\t\t\t\t}"); 
						}}t_ct_("\n762 \t\t\t\t\t}"); 
					}}t_ct_("\n763 \t\t\t\t\tif(myarm>0)"); 
					if((u_ct_("\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm)>0){
					{t_ct_("\n764 \t\t\t\t\t{"); t_ct_("\n765 \t\t\t\t\t\tprintf(\"You WON! %d ships hold planet #%d\\n\",myarm,despla);"); 
						printf("You WON! %d ships hold planet #%d\n",(u_ct_("\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm),(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla));t_ct_("\n766 \t\t\t\t\t\ts->owner[despla]=mynum;"); 
						(s->owner[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=(mynum, u_ct_("\t\t\t\t\t\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t\t\t\t\t\t/* s->owner[despla]",sizeof(s->owner[despla]),s->owner[despla]), s->owner[despla]);t_ct_("\n767 \t\t\t\t\t\ts->ships[despla]=myarm;"); 
						(s->ships[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=(myarm, u_ct_("\t\t\t\t\t\t/* myarm",sizeof(myarm),myarm), myarm), u_ct_("\t\t\t\t\t\t/* s->ships[despla]",sizeof(s->ships[despla]),s->ships[despla]), s->ships[despla]);t_ct_("\n768 \t\t\t\t\t\ts->produ[despla]=s->produ[despla]-irand(4);"); 
						(s->produ[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=(s->produ[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)], u_ct_("\t\t\t\t\t\t/* s->produ[despla]",sizeof(s->produ[despla]),s->produ[despla]), s->produ[despla])-irand(4), u_ct_("\t\t\t\t\t\t/* s->produ[despla]",sizeof(s->produ[despla]),s->produ[despla]), s->produ[despla]);t_ct_("\n769 \t\t\t\t\t\tif (s->produ[despla]<1)"); 
						if ((s->produ[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)], u_ct_("\t\t\t\t\t\t/* s->produ[despla]",sizeof(s->produ[despla]),s->produ[despla]), s->produ[despla])<1){t_ct_(" s->produ[despla]=1;");  (s->produ[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=1, u_ct_("\t\t\t\t\t\t/* s->produ[despla]",sizeof(s->produ[despla]),s->produ[despla]), s->produ[despla]);}t_ct_("\n770 \t\t\t\t\t}"); 
					}}
					else{t_ct_("\n771 \t\t\t\t\telse"); 
					{t_ct_("\n772 \t\t\t\t\t{"); t_ct_("\n773 \t\t\t\t\t\tprintf(\"You LOST! %d enemy ships hold planet #%d\\n\",desarm,despla);"); 
						printf("You LOST! %d enemy ships hold planet #%d\n",(u_ct_("\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm),(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla));t_ct_("\n774 \t\t\t\t\t\ts->ships[despla]=desarm;"); 
						(s->ships[(u_ct_("\t\t\t\t\t\t/* despla",sizeof(despla),despla), despla)]=(desarm, u_ct_("\t\t\t\t\t\t/* desarm",sizeof(desarm),desarm), desarm), u_ct_("\t\t\t\t\t\t/* s->ships[despla]",sizeof(s->ships[despla]),s->ships[despla]), s->ships[despla]);t_ct_("\n775 \t\t\t\t\t}"); 
					}}t_ct_("\n776 \t\t\t\t\tk=s->comnum;"); 
					(k=(s->comnum, u_ct_("\t\t\t\t\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum), u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k);t_ct_("\n777 \t\t\t\t\tk++;"); 
					(k++, u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k-1);t_ct_("\n778 \t\t\t\t\tif(k>9)"); 
					if((u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)>9){t_ct_(" k=0;");  (k=0, u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k);}t_ct_("\n779 \t\t\t\t\ts->combat[k][0]=mynum;"); 
					(s->combat[(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)][0]=(mynum, u_ct_("\t\t\t\t\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t\t\t\t\t/* s->combat[k][0]",sizeof(s->combat[k][0]),s->combat[k][0]), s->combat[k][0]);t_ct_("\n780 \t\t\t\t\ts->combat[k][1]=despla;"); 
					(s->combat[(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)][1]=(despla, u_ct_("\t\t\t\t\t/* despla",sizeof(despla),despla), despla), u_ct_("\t\t\t\t\t/* s->combat[k][1]",sizeof(s->combat[k][1]),s->combat[k][1]), s->combat[k][1]);t_ct_("\n781 \t\t\t\t\ts->comnum=k;"); 
					(s->comnum=(k, u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k), u_ct_("\t\t\t\t\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum);t_ct_("\n782 \t\t\t\t\ts->meslok--;"); 
					(s->meslok--, u_ct_("\t\t\t\t\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n783 \t\t\t\t}"); 
				}}t_ct_("\n784 \t\t\t\tevents[i]=0L;"); 
				(events[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)]=0L, u_ct_("\t\t\t\t/* events[i]",sizeof(events[i]),events[i]), events[i]);
t_ct_("\n786 \t\t\t}"); 
			}}}}t_ct_("\n    /* return */"); 
}

lock()
{
	int i;char *_ct;t_ct_("\n789 lock()"); t_ct_("\n792 \ti=0;"); 
	(i=0, u_ct_("\t/* i",sizeof(i),i), i);t_ct_("\n793 \tif(s->meslok<0 || s->meslok>20)"); 
	if((u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok)<0 || (u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok)>20){t_ct_(" s->meslok=0;");  (s->meslok=0, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok);}
	while(t_ct_("\n794 \twhile(s->meslok>0 && i<5)"),(u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok)>0 && (u_ct_("\t/* i",sizeof(i),i), i)<5){
	{t_ct_("\n795 \t{"); t_ct_("\n796 \t\ti++;"); 
		(i++, u_ct_("\t\t/* i",sizeof(i),i), i-1);t_ct_("\n797 \t\tsleep(2);"); 
		sleep(2);t_ct_("\n798 \t}"); 
	}}t_ct_("\n799 \ts->meslok=0;"); 
	(s->meslok=0, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok);t_ct_("\n800 \ts->meslok++;"); 
	(s->meslok++, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok-1);t_ct_("\n    /* return */"); 
}

getpub()
{
	char *ptr;
	int k,type,i;char *_ct;t_ct_("\n803 getpub()"); t_ct_("\n807 \ti=0;"); 
	(i=0, u_ct_("\t/* i",sizeof(i),i), i);
t_ct_("\n809 \tk=mymes;"); 
	(k=(mymes, u_ct_("\t/* mymes",sizeof(mymes),mymes), mymes), u_ct_("\t/* k",sizeof(k),k), k);
	while(t_ct_("\n810 \twhile(k!=s->mesnum)"),(u_ct_("\t/* k",sizeof(k),k), k)!=(u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)){
	{t_ct_("\n811 \t{"); t_ct_("\n812 \t\tk++;"); 
		(k++, u_ct_("\t\t/* k",sizeof(k),k), k-1);t_ct_("\n813 \t\tif(k > 4)"); 
		if((u_ct_("\t\t/* k",sizeof(k),k), k) > 4){t_ct_("\n814 \t\t\tk = 0;"); 
			(k = 0, u_ct_("\t\t\t/* k",sizeof(k),k), k);}t_ct_("\n815 \t\ti=1;"); 
		(i=1, u_ct_("\t\t/* i",sizeof(i),i), i);t_ct_("\n816 \t\tif(s->public[k][0] != mynum)"); 
		if((s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)][0], u_ct_("\t\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0]) != (u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_("\n817 \t\t\tif( s->public[k][1] == 'P' || s->public[k][1] == mynum || s->public[k][1]=='E')"); 
			if( (s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]) == 'P' || (s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]) == (u_ct_("\t\t\t/* mynum",sizeof(mynum),mynum), mynum) || (s->public[(u_ct_("\t\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1])=='E'){
			{t_ct_("\n818 \t\t\t{"); t_ct_("\n819 \t\t\t\ttype=s->public[k][1];"); 
				(type=(s->public[(u_ct_("\t\t\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t\t\t/* s->public[k][1]",sizeof(s->public[k][1]),s->public[k][1]), s->public[k][1]), u_ct_("\t\t\t\t/* type",sizeof(type),type), type);t_ct_("\n820 \t\t\t\tif(type !='P' && type !='C' && type!='E')"); 
				if((u_ct_("\t\t\t\t/* type",sizeof(type),type), type) !='P' && (u_ct_("\t\t\t\t/* type",sizeof(type),type), type) !='C' && (u_ct_("\t\t\t\t/* type",sizeof(type),type), type)!='E'){t_ct_(" type = 'I';");  (type = 'I', u_ct_("\t\t\t\t/* type",sizeof(type),type), type);}t_ct_("\n821 \t\t\t\tptr = &s->public[k][2];"); 
				(ptr = &s->public[(u_ct_("\t\t\t\t/* k",sizeof(k),k), k)][2], u_ct_("\t\t\t\t/* ptr",sizeof(ptr),ptr), ptr);t_ct_("\n822 \t\t\t\tif(vtmode)"); 
				if((u_ct_("\t\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(24,1);");  gotoxy(24,1);}t_ct_("\n823 \t\t\t\tif (type == 'I')"); 
				if ((u_ct_("\t\t\t\t/* type",sizeof(type),type), type) == 'I'){t_ct_("\n824 \t\t\t\t\tprintf(\"\\nPrivate message from %s]\\n\",s->names[s->public[k][0]]);"); 
					printf("\nPrivate message from %s]\n",(s->names[(s->public[(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)][0], u_ct_("\t\t\t\t\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0])], u_ct_("\t\t\t\t\t/* s->names[s->public[k][0]]",sizeof(s->names[s->public[k][0]]),s->names[s->public[k][0]]), s->names[s->public[k][0]]));}
				else{t_ct_("\n825 \t\t\t\telse"); t_ct_(" if(type=='P')");  if((u_ct_("\t\t\t\t/* type",sizeof(type),type), type)=='P'){t_ct_("\n826 \t\t\t\t\tprintf(\"\\nPublic message by %s]\\n\",s->names[s->public[k][0]]);"); 
					printf("\nPublic message by %s]\n",(s->names[(s->public[(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)][0], u_ct_("\t\t\t\t\t/* s->public[k][0]",sizeof(s->public[k][0]),s->public[k][0]), s->public[k][0])], u_ct_("\t\t\t\t\t/* s->names[s->public[k][0]]",sizeof(s->names[s->public[k][0]]),s->names[s->public[k][0]]), s->names[s->public[k][0]]));}
				else{t_ct_("\n827 \t\t\t\telse"); t_ct_(" printf(\"\\n\");");  printf("\n");}}t_ct_("\n828 \t\t\t\tprintf(\"%s\\n\\n\",ptr);"); 
				printf("%s\n\n",(u_ct_("\t\t\t\t/* ptr",sizeof(ptr),ptr), ptr));t_ct_("\n829 \t\t\t}"); 
			}}}t_ct_("\n830 \t}"); 
	}}t_ct_("\n831 \tmymes=k;"); 
	(mymes=(k, u_ct_("\t/* k",sizeof(k),k), k), u_ct_("\t/* mymes",sizeof(mymes),mymes), mymes);t_ct_("\n832 \tif(i && vtmode)"); 
	if((u_ct_("\t/* i",sizeof(i),i), i) && (u_ct_("\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" prompt();");  prompt();}t_ct_("\n    /* return */"); 
}

getcom()
{
	int k;
	int defpl,attak,own;char *_ct;t_ct_("\n835 getcom()"); t_ct_("\n839 \tk=mycom;"); 
	(k=(mycom, u_ct_("\t/* mycom",sizeof(mycom),mycom), mycom), u_ct_("\t/* k",sizeof(k),k), k);
	while(t_ct_("\n840 \twhile(k!=s->comnum)"),(u_ct_("\t/* k",sizeof(k),k), k)!=(u_ct_("\t/* s->comnum",sizeof(s->comnum),s->comnum), s->comnum)){
	{t_ct_("\n841 \t{"); t_ct_("\n842 \t\tk++;"); 
		(k++, u_ct_("\t\t/* k",sizeof(k),k), k-1);t_ct_("\n843 \t\tif(k>9)"); 
		if((u_ct_("\t\t/* k",sizeof(k),k), k)>9){t_ct_(" k=0;");  (k=0, u_ct_("\t\t/* k",sizeof(k),k), k);}t_ct_("\n844 \t\tattak=s->combat[k][0];"); 
		(attak=(s->combat[(u_ct_("\t\t/* k",sizeof(k),k), k)][0], u_ct_("\t\t/* s->combat[k][0]",sizeof(s->combat[k][0]),s->combat[k][0]), s->combat[k][0]), u_ct_("\t\t/* attak",sizeof(attak),attak), attak);t_ct_("\n845 \t\town=s->owner[s->combat[k][1]];"); 
		(own=(s->owner[(s->combat[(u_ct_("\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t/* s->combat[k][1]",sizeof(s->combat[k][1]),s->combat[k][1]), s->combat[k][1])], u_ct_("\t\t/* s->owner[s->combat[k][1]]",sizeof(s->owner[s->combat[k][1]]),s->owner[s->combat[k][1]]), s->owner[s->combat[k][1]]), u_ct_("\t\t/* own",sizeof(own),own), own);t_ct_("\n846 \t\tdefpl=s->combat[k][1];"); 
		(defpl=(s->combat[(u_ct_("\t\t/* k",sizeof(k),k), k)][1], u_ct_("\t\t/* s->combat[k][1]",sizeof(s->combat[k][1]),s->combat[k][1]), s->combat[k][1]), u_ct_("\t\t/* defpl",sizeof(defpl),defpl), defpl);t_ct_("\n847 \t\tif(attak!=mynum)"); 
		if((u_ct_("\t\t/* attak",sizeof(attak),attak), attak)!=(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){
		{t_ct_("\n848 \t\t{"); t_ct_("\n849 \t\t\tif(vtmode)"); 
			if((u_ct_("\t\t\t/* vtmode",sizeof(vtmode),vtmode), vtmode)){t_ct_(" gotoxy(24,1);");  gotoxy(24,1);}t_ct_("\n850 \t\t\tprintf(\"\\nInvasion attempt!\\nAttacker:%s]\\nPlanet #%d\\n\",\n\t\t\t    s->names[attak],defpl);"); 
			printf("\nInvasion attempt!\nAttacker:%s]\nPlanet #%d\n",
			    (s->names[(u_ct_("\t\t\t/* attak",sizeof(attak),attak), attak)], u_ct_("\t\t\t/* s->names[attak]",sizeof(s->names[attak]),s->names[attak]), s->names[attak]),(u_ct_("\t\t\t/* defpl",sizeof(defpl),defpl), defpl));t_ct_("\n852 \t\t\tif(own==-1)"); 
			if((u_ct_("\t\t\t/* own",sizeof(own),own), own)==-1){t_ct_(" printf(\"It remains Neutral\\n\");");  printf("It remains Neutral\n");}t_ct_("\n853 \t\t\tif(own==attak)"); 
			if((u_ct_("\t\t\t/* own",sizeof(own),own), own)==(u_ct_("\t\t\t/* attak",sizeof(attak),attak), attak)){t_ct_(" printf(\"%s] has gained the planet.\\n\",s->names[own]);");  printf("%s] has gained the planet.\n",(s->names[(u_ct_("\t\t\t/* own",sizeof(own),own), own)], u_ct_("\t\t\t/* s->names[own]",sizeof(s->names[own]),s->names[own]), s->names[own]));}t_ct_("\n854 \t\t\tif(own!=attak && own!=-1)"); 
			if((u_ct_("\t\t\t/* own",sizeof(own),own), own)!=(u_ct_("\t\t\t/* attak",sizeof(attak),attak), attak) && (u_ct_("\t\t\t/* own",sizeof(own),own), own)!=-1){t_ct_(" printf(\"%s] retains control.\\n\",s->names[own]);");  printf("%s] retains control.\n",(s->names[(u_ct_("\t\t\t/* own",sizeof(own),own), own)], u_ct_("\t\t\t/* s->names[own]",sizeof(s->names[own]),s->names[own]), s->names[own]));}t_ct_("\n855 \t\t\tif(own==mynum)"); 
			if((u_ct_("\t\t\t/* own",sizeof(own),own), own)==(u_ct_("\t\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_(" printf(\"%d ships still hold your planet %d\\n\",\n\t\t\t    s->ships[defpl],defpl);");  printf("%d ships still hold your planet %d\n",
			    (s->ships[(u_ct_("\t\t\t/* defpl",sizeof(defpl),defpl), defpl)], u_ct_("\t\t\t/* s->ships[defpl]",sizeof(s->ships[defpl]),s->ships[defpl]), s->ships[defpl]),(u_ct_("\t\t\t/* defpl",sizeof(defpl),defpl), defpl));}t_ct_("\n857 \t\t}"); 
		}}t_ct_("\n858 \t}"); 
	}}t_ct_("\n859 \tmycom=k;"); 
	(mycom=(k, u_ct_("\t/* k",sizeof(k),k), k), u_ct_("\t/* mycom",sizeof(mycom),mycom), mycom);t_ct_("\n860 \tprompt();"); 
	prompt();t_ct_("\n    /* return */"); 
}


prange(i,j)
int i,j;
{
	float root();
	float truerange;
	long tmp1,tmp2,tmp3;
	int test;
char *_ct;t_ct_("\n864 prange(i,j)"); t_ct_("\n872 \ttmp1= ( s->xpos[i]-s->xpos[j]);"); 
	(tmp1= ( (s->xpos[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->xpos[i]",sizeof(s->xpos[i]),s->xpos[i]), s->xpos[i])-(s->xpos[(u_ct_("\t/* j",sizeof(j),j), j)], u_ct_("\t/* s->xpos[j]",sizeof(s->xpos[j]),s->xpos[j]), s->xpos[j])), u_ct_("\t/* tmp1",sizeof(tmp1),tmp1), tmp1);t_ct_("\n873 \ttmp1=tmp1*tmp1;"); 
	(tmp1=(u_ct_("\t/* tmp1",sizeof(tmp1),tmp1), tmp1)*(tmp1, u_ct_("\t/* tmp1",sizeof(tmp1),tmp1), tmp1), u_ct_("\t/* tmp1",sizeof(tmp1),tmp1), tmp1);
t_ct_("\n875 \ttmp2= ( s->ypos[i]-s->ypos[j]);"); 
	(tmp2= ( (s->ypos[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->ypos[i]",sizeof(s->ypos[i]),s->ypos[i]), s->ypos[i])-(s->ypos[(u_ct_("\t/* j",sizeof(j),j), j)], u_ct_("\t/* s->ypos[j]",sizeof(s->ypos[j]),s->ypos[j]), s->ypos[j])), u_ct_("\t/* tmp2",sizeof(tmp2),tmp2), tmp2);t_ct_("\n876 \ttmp2= ( tmp2*tmp2);"); 
	(tmp2= ( (u_ct_("\t/* tmp2",sizeof(tmp2),tmp2), tmp2)*(u_ct_("\t/* tmp2",sizeof(tmp2),tmp2), tmp2)), u_ct_("\t/* tmp2",sizeof(tmp2),tmp2), tmp2);
t_ct_("\n878 \ttmp3= ( s->zpos[i]-s->zpos[j]);"); 
	(tmp3= ( (s->zpos[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* s->zpos[i]",sizeof(s->zpos[i]),s->zpos[i]), s->zpos[i])-(s->zpos[(u_ct_("\t/* j",sizeof(j),j), j)], u_ct_("\t/* s->zpos[j]",sizeof(s->zpos[j]),s->zpos[j]), s->zpos[j])), u_ct_("\t/* tmp3",sizeof(tmp3),tmp3), tmp3);t_ct_("\n879 \ttmp3=tmp3*tmp3;"); 
	(tmp3=(u_ct_("\t/* tmp3",sizeof(tmp3),tmp3), tmp3)*(tmp3, u_ct_("\t/* tmp3",sizeof(tmp3),tmp3), tmp3), u_ct_("\t/* tmp3",sizeof(tmp3),tmp3), tmp3);
t_ct_("\n881 \ttruerange=root(tmp1+tmp2+tmp3);"); 
	(truerange=root((u_ct_("\t/* tmp1",sizeof(tmp1),tmp1), tmp1)+(u_ct_("\t/* tmp2",sizeof(tmp2),tmp2), tmp2)+(u_ct_("\t/* tmp3",sizeof(tmp3),tmp3), tmp3)), u_ct_("\t/* truerange",sizeof(truerange),truerange), truerange);t_ct_("\n882 \ttest=truerange;"); 
	(test=(truerange, u_ct_("\t/* truerange",sizeof(truerange),truerange), truerange), u_ct_("\t/* test",sizeof(test),test), test);t_ct_("\n883 \tif(test<truerange)"); 
	if((u_ct_("\t/* test",sizeof(test),test), test)<(u_ct_("\t/* truerange",sizeof(truerange),truerange), truerange)){t_ct_(" test++;");  (test++, u_ct_("\t/* test",sizeof(test),test), test-1);}t_ct_("\n884 \treturn(test);"); 
	return(u_ct_("\t/* (test)",sizeof((test)),(test)), (test));
}


compute()
{
	int k;
	int i,j;
char *_ct;t_ct_("\n888 compute()"); t_ct_("\n893 \tprintf(\"\\nPlanet #:\");"); 
	printf("\nPlanet #:");t_ct_("\n894 \tk=getnum(&i);"); 
	(k=getnum(&i), u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n895 \tprintf(\"\\nPlanet #:\");"); 
	printf("\nPlanet #:");t_ct_("\n896 \tk=getnum(&j);"); 
	(k=getnum(&j), u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n897 \tk=prange(i,j);"); 
	(k=prange((u_ct_("\t/* i",sizeof(i),i), i),(u_ct_("\t/* j",sizeof(j),j), j)), u_ct_("\t/* k",sizeof(k),k), k);
t_ct_("\n899 \tprintf(\"Range from %d to %d = %d\\n\",i,j,k);"); 
	printf("Range from %d to %d = %d\n",(u_ct_("\t/* i",sizeof(i),i), i),(u_ct_("\t/* j",sizeof(j),j), j),(u_ct_("\t/* k",sizeof(k),k), k));t_ct_("\n    /* return */"); 
}
float root(x)
long x;
{
	int z;
	float e;
	double y;char *_ct;t_ct_("\n901 root(x)"); t_ct_("\n907 \tif(x==0)"); 
	if((u_ct_("\t/* x",sizeof(x),x), x)==0){t_ct_(" return(0.0);");  return(0.0);}t_ct_("\n908 \ty=x/2.0;"); 
	(y=(u_ct_("\t/* x",sizeof(x),x), x)/2.0, u_ct_("\t/* y",sizeof(y),y), y);t_ct_("\n909 \tz=0;"); 
	(z=0, u_ct_("\t/* z",sizeof(z),z), z);t_ct_("\n910 \te = 1.0;"); 
	(e = 1.0, u_ct_("\t/* e",sizeof(e),e), e);
	while(t_ct_("\n911 \twhile((y*y != x) && e>0.001)"),((u_ct_("\t/* y",sizeof(y),y), y)*(u_ct_("\t/* y",sizeof(y),y), y) != (u_ct_("\t/* x",sizeof(x),x), x)) && (u_ct_("\t/* e",sizeof(e),e), e)>0.001){
	{t_ct_("\n912 \t{"); t_ct_("\n913 \t\tz++;"); 
		(z++, u_ct_("\t\t/* z",sizeof(z),z), z-1);t_ct_("\n914 \t\ty=(y+x/y)/2;"); 
		(y=((u_ct_("\t\t/* y",sizeof(y),y), y)+(u_ct_("\t\t/* x",sizeof(x),x), x)/(u_ct_("\t\t/* y",sizeof(y),y), y))/2, u_ct_("\t\t/* y",sizeof(y),y), y);t_ct_("\n915 \t\te= y*y - x;"); 
		(e= (u_ct_("\t\t/* y",sizeof(y),y), y)*(u_ct_("\t\t/* y",sizeof(y),y), y) - (x, u_ct_("\t\t/* x",sizeof(x),x), x), u_ct_("\t\t/* e",sizeof(e),e), e);t_ct_("\n916 \t\tif(e < 0.0)"); 
		if((u_ct_("\t\t/* e",sizeof(e),e), e) < 0.0){t_ct_(" e = -e;");  (e = -(e, u_ct_("\t\t/* e",sizeof(e),e), e), u_ct_("\t\t/* e",sizeof(e),e), e);}t_ct_("\n917 \t}"); 
	}}
t_ct_("\n919 \treturn(y);"); 
	return(u_ct_("\t/* (y)",sizeof((y)),(y)), (y));
}

flts()
{
	int i, fltflag;
	extern char *ctime();
char *_ct;t_ct_("\n922 flts()"); t_ct_("\n927 \tfltflag = 0;"); 
	(fltflag = 0, u_ct_("\t/* fltflag",sizeof(fltflag),fltflag), fltflag);t_ct_("\n928 \tprintf(\"\\nFleet  Dest  Size  Arrival\\n\");"); 
	printf("\nFleet  Dest  Size  Arrival\n");t_ct_("\n929 \tfor(i=0;i<99;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<99;t_ct_("\n929 \tfor(i=0;i<99;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){t_ct_("\n930 \t\tif(events[i]!=0L)"); 
		if((events[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])!=0L){
		{t_ct_("\n931 \t\t{"); t_ct_("\n932 \t\t\tfltflag = 1;"); 
			(fltflag = 1, u_ct_("\t\t\t/* fltflag",sizeof(fltflag),fltflag), fltflag);t_ct_("\n933 \t\t\tprintf(\"  %2d    %2d   %4d  %5s\\n\",\n\t\t\t    i,stack[i][1],stack[i][0],sdate(events[i]));"); 
			printf("  %2d    %2d   %4d  %5s\n",
			    (u_ct_("\t\t\t/* i",sizeof(i),i), i),(stack[(u_ct_("\t\t\t/* i",sizeof(i),i), i)][1], u_ct_("\t\t\t/* stack[i][1]",sizeof(stack[i][1]),stack[i][1]), stack[i][1]),(stack[(u_ct_("\t\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t\t/* stack[i][0]",sizeof(stack[i][0]),stack[i][0]), stack[i][0]),sdate((events[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])));t_ct_("\n935 \t\t}"); 
		}}}t_ct_("\n936 \tif (fltflag == 0)"); 
	if ((u_ct_("\t/* fltflag",sizeof(fltflag),fltflag), fltflag) == 0){t_ct_("\n937 \t\tprintf(\"\\nNo Fleets currently in flight.\\n\");"); 
		printf("\nNo Fleets currently in flight.\n");}t_ct_("\n    /* return */"); 
}

broadcast()
{
	char tmp[100];
	int ptr,k;char *_ct;t_ct_("\n940 broadcast()"); t_ct_("\n944 \tptr=2;"); 
	(ptr=2, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr);t_ct_("\n945 \tprintf(\"\\nEnter Broadcast Message:\\n>  \");"); 
	printf("\nEnter Broadcast Message:\n>  ");
	while(t_ct_("\n946 \twhile( (k=getchar())=='\\n')"),((k=getchar(), u_ct_("\t/* k",sizeof(k),k), k))=='\n'){t_ct_(";"); ;}
	while(t_ct_("\n947 \twhile(ptr<100 && k!='\\n')"),(u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr)<100 && (u_ct_("\t/* k",sizeof(k),k), k)!='\n'){
	{t_ct_("\n948 \t{"); t_ct_("\n949 \t\ttmp[ptr]=k;"); 
		(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]=(k, u_ct_("\t\t/* k",sizeof(k),k), k), u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]);t_ct_("\n950 \t\tptr++;"); 
		(ptr++, u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr-1);t_ct_("\n951 \t\ttmp[ptr]='\\0';"); 
		(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]='\0', u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]);t_ct_("\n952 \t\tk=getchar();"); 
		(k=getchar(), u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n953 \t}"); 
	}}t_ct_("\n954 \ttmp[1]='P';"); 
	(tmp[1]='P', u_ct_("\t/* tmp[1]",sizeof(tmp[1]),tmp[1]), tmp[1]);t_ct_("\n955 \ttmp[0]=mynum;"); 
	(tmp[0]=(mynum, u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t/* tmp[0]",sizeof(tmp[0]),tmp[0]), tmp[0]);
t_ct_("\n957 \tlock();"); 
	lock();t_ct_("\n958 \tk=s->mesnum+1;"); 
	(k=(u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)+1, u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n959 \tif(k>4)"); 
	if((u_ct_("\t/* k",sizeof(k),k), k)>4){t_ct_("k=0;"); (k=0, u_ct_("\t/* k",sizeof(k),k), k);}t_ct_("\n960 \tfor(ptr=0;ptr<100;ptr++)"); 
	for((ptr=0, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr);(u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr)<100;t_ct_("\n960 \tfor(ptr=0;ptr<100;ptr++)"),(ptr++, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr-1)){t_ct_("\n961 \t\ts->public[k][ptr]=tmp[ptr];"); 
		(s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)][(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]=(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)], u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]), u_ct_("\t\t/* s->public[k][ptr]",sizeof(s->public[k][ptr]),s->public[k][ptr]), s->public[k][ptr]);}t_ct_("\n962 \ts->mesnum=k;"); 
	(s->mesnum=(k, u_ct_("\t/* k",sizeof(k),k), k), u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n963 \ts->meslok--;"); 
	(s->meslok--, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n964 \tprintf(\"Message Sent.\\n\");"); 
	printf("Message Sent.\n");t_ct_("\n    /* return */"); 
}


prvmsg()
{
	char tmp[100];
	int ptr,k, player;
char *_ct;t_ct_("\n968 prvmsg()"); t_ct_("\n973 \tptr=2;"); 
	(ptr=2, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr);t_ct_("\n974 \tprintf(\"\\nEnter Private Message:\\n>  \");"); 
	printf("\nEnter Private Message:\n>  ");
	while(t_ct_("\n975 \twhile( (k=getchar())=='\\n')"),((k=getchar(), u_ct_("\t/* k",sizeof(k),k), k))=='\n'){t_ct_(";"); ;}
	while(t_ct_("\n976 \twhile(ptr<100 && k!='\\n')"),(u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr)<100 && (u_ct_("\t/* k",sizeof(k),k), k)!='\n'){
	{t_ct_("\n977 \t{"); t_ct_("\n978 \t\ttmp[ptr]=k;"); 
		(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]=(k, u_ct_("\t\t/* k",sizeof(k),k), k), u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]);t_ct_("\n979 \t\tptr++;"); 
		(ptr++, u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr-1);t_ct_("\n980 \t\ttmp[ptr]='\\0';"); 
		(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]='\0', u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]);t_ct_("\n981 \t\tk=getchar();"); 
		(k=getchar(), u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n982 \t}"); 
	}}t_ct_("\n983 \tif(toggle>-1)"); 
	if((u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle)>-1){t_ct_(" player=toggle;");  (player=(toggle, u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle), u_ct_("\t/* player",sizeof(player),player), player);}
	else{t_ct_("\n984 \telse");  {t_ct_(" {"); t_ct_("\n985 \t\tprintf(\"\\nMessage to (player NUMBER):\");"); 
		printf("\nMessage to (player NUMBER):");t_ct_("\n986 \t\tk=getnum(&ptr);"); 
		(k=getnum(&ptr), u_ct_("\t\t/* k",sizeof(k),k), k);t_ct_("\n987 \t\tplayer = ptr;"); 
		(player = (ptr, u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr), u_ct_("\t\t/* player",sizeof(player),player), player);t_ct_("\n988 \t}"); 
	}}t_ct_("\n989 \tif ((player < 0) || (player > 10) || (s->names[player] == '\\0'))"); 
	if (((u_ct_("\t/* player",sizeof(player),player), player) < 0) || ((u_ct_("\t/* player",sizeof(player),player), player) > 10) || ((s->names[(u_ct_("\t/* player",sizeof(player),player), player)], u_ct_("\t/* s->names[player]",sizeof(s->names[player]),s->names[player]), s->names[player]) == '\0')){
	{t_ct_("\n990 \t{"); t_ct_("\n991 \t\tprintf(\"Illegal player number.\\n\");"); 
		printf("Illegal player number.\n");t_ct_("\n992 \t\treturn(0);"); 
		return(0);
	}}t_ct_("\n994 \ttmp[1]=player;"); 
	(tmp[1]=(player, u_ct_("\t/* player",sizeof(player),player), player), u_ct_("\t/* tmp[1]",sizeof(tmp[1]),tmp[1]), tmp[1]);t_ct_("\n995 \ttmp[0]=mynum;"); 
	(tmp[0]=(mynum, u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum), u_ct_("\t/* tmp[0]",sizeof(tmp[0]),tmp[0]), tmp[0]);
t_ct_("\n997 \tlock();"); 
	lock();t_ct_("\n998 \tk=s->mesnum+1;"); 
	(k=(u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum)+1, u_ct_("\t/* k",sizeof(k),k), k);t_ct_("\n999 \tif(k>4)"); 
	if((u_ct_("\t/* k",sizeof(k),k), k)>4){t_ct_("k=0;"); (k=0, u_ct_("\t/* k",sizeof(k),k), k);}t_ct_("\n1000 \tfor(ptr=0;ptr<100;ptr++)"); 
	for((ptr=0, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr);(u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr)<100;t_ct_("\n1000 \tfor(ptr=0;ptr<100;ptr++)"),(ptr++, u_ct_("\t/* ptr",sizeof(ptr),ptr), ptr-1)){t_ct_("\n1001 \t\ts->public[k][ptr]=tmp[ptr];"); 
		(s->public[(u_ct_("\t\t/* k",sizeof(k),k), k)][(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)]=(tmp[(u_ct_("\t\t/* ptr",sizeof(ptr),ptr), ptr)], u_ct_("\t\t/* tmp[ptr]",sizeof(tmp[ptr]),tmp[ptr]), tmp[ptr]), u_ct_("\t\t/* s->public[k][ptr]",sizeof(s->public[k][ptr]),s->public[k][ptr]), s->public[k][ptr]);}t_ct_("\n1002 \ts->mesnum=k;"); 
	(s->mesnum=(k, u_ct_("\t/* k",sizeof(k),k), k), u_ct_("\t/* s->mesnum",sizeof(s->mesnum),s->mesnum), s->mesnum);t_ct_("\n1003 \ts->meslok--;"); 
	(s->meslok--, u_ct_("\t/* s->meslok",sizeof(s->meslok),s->meslok), s->meslok+1);t_ct_("\n1004 \tprintf(\"\\nMessage Sent to %s.\\n\",s->names[player]);"); 
	printf("\nMessage Sent to %s.\n",(s->names[(u_ct_("\t/* player",sizeof(player),player), player)], u_ct_("\t/* s->names[player]",sizeof(s->names[player]),s->names[player]), s->names[player]));t_ct_("\n    /* return */"); 
}

send()
{
	extern char *ctime();
	int i,j,k;
	int from,to;
char *_ct;t_ct_("\n1007 send()"); t_ct_("\n1013 \ti = -1;"); 
	(i = -1, u_ct_("\t/* i",sizeof(i),i), i);
	while(t_ct_("\n1014 \twhile(i==-1)"),(u_ct_("\t/* i",sizeof(i),i), i)==-1){
	{t_ct_("\n1015 \t{"); t_ct_("\n1016 \t\tprintf(\"\\nFrom world:\");"); 
		printf("\nFrom world:");t_ct_("\n1017 \t\ti=getnum(&from);"); 
		(i=getnum(&from), u_ct_("\t\t/* i",sizeof(i),i), i);t_ct_("\n1018 \t\tif(from <0 || from >39)"); 
		if((u_ct_("\t\t/* from",sizeof(from),from), from) <0 || (u_ct_("\t\t/* from",sizeof(from),from), from) >39){t_ct_(" i = -1;");  (i = -1, u_ct_("\t\t/* i",sizeof(i),i), i);}t_ct_("\n1019 \t}"); 
	}}t_ct_("\n1020 \tif (s->owner[from]!=mynum)"); 
	if ((s->owner[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->owner[from]",sizeof(s->owner[from]),s->owner[from]), s->owner[from])!=(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){
	{t_ct_("\n1021 \t{"); t_ct_("\n1022 \t\tprintf(\"\\nYou do not own that world!\\n\");"); 
		printf("\nYou do not own that world!\n");t_ct_("\n1023 \t\treturn(0);"); 
		return(0);
	}}t_ct_("\n1025 \tj = -1;"); 
	(j = -1, u_ct_("\t/* j",sizeof(j),j), j);
	while(t_ct_("\n1026 \twhile(j==-1)"),(u_ct_("\t/* j",sizeof(j),j), j)==-1){
	{t_ct_("\n1027 \t{"); t_ct_("\n1028 \t\tprintf(\"To world:\");"); 
		printf("To world:");t_ct_("\n1029 \t\tj = getnum(&to);"); 
		(j = getnum(&to), u_ct_("\t\t/* j",sizeof(j),j), j);t_ct_("\n1030 \t\tif(to<0 || to>39)"); 
		if((u_ct_("\t\t/* to",sizeof(to),to), to)<0 || (u_ct_("\t\t/* to",sizeof(to),to), to)>39){t_ct_(" j = -1;");  (j = -1, u_ct_("\t\t/* j",sizeof(j),j), j);}t_ct_("\n1031 \t}"); 
	}}
t_ct_("\n1033 \tfor (k=0;events[k]!=0L && k<100; k++)"); 
	for ((k=0, u_ct_("\t/* k",sizeof(k),k), k);(events[(u_ct_("\t/* k",sizeof(k),k), k)], u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k])!=0L && (u_ct_("\t/* k",sizeof(k),k), k)<100;t_ct_("\n1033 \tfor (k=0;events[k]!=0L && k<100; k++)"),(k++, u_ct_("\t/* k",sizeof(k),k), k-1)){t_ct_(";"); ;}
t_ct_("\n1035 \tif(k==100)"); 
	if((u_ct_("\t/* k",sizeof(k),k), k)==100){
	{t_ct_("\n1036 \t{"); t_ct_("\n1037 \t\tprintf(\"\\nSorry, too many fleets in the air!\\n\");"); 
		printf("\nSorry, too many fleets in the air!\n");t_ct_("\n1038 \t\treturn(0);"); 
		return(0);
	}}
t_ct_("\n1041 \tprintf(\"Number to send (%d) :\",s->ships[from]);"); 
	printf("Number to send (%d) :",(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]));t_ct_("\n1042 \ti=getnum(&j);"); 
	(i=getnum(&j), u_ct_("\t/* i",sizeof(i),i), i);
t_ct_("\n1044 \tif(j>s->ships[from] || j<1)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)>(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]) || (u_ct_("\t/* j",sizeof(j),j), j)<1){
	{t_ct_("\n1045 \t{"); t_ct_("\n1046 \t\tprintf(\"Invalid fleet size!\");"); 
		printf("Invalid fleet size!");t_ct_("\n1047 \t\treturn(0);"); 
		return(0);
	}}
t_ct_("\n1050 \tif(s->owner[from]!=mynum)"); 
	if((s->owner[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->owner[from]",sizeof(s->owner[from]),s->owner[from]), s->owner[from])!=(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){
	{t_ct_("\n1051 \t{"); t_ct_("\n1052 \t\tprintf(\"\\nYou have lost control of that world!!!\\n\");"); 
		printf("\nYou have lost control of that world!!!\n");t_ct_("\n1053 \t\treturn(0);"); 
		return(0);

	}}
t_ct_("\n1057 \tstack[k][0]=j;"); 
	(stack[(u_ct_("\t/* k",sizeof(k),k), k)][0]=(j, u_ct_("\t/* j",sizeof(j),j), j), u_ct_("\t/* stack[k][0]",sizeof(stack[k][0]),stack[k][0]), stack[k][0]);t_ct_("\n1058 \ts->ships[from]=s->ships[from]-j;"); 
	(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)]=(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from])-(j, u_ct_("\t/* j",sizeof(j),j), j), u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]);t_ct_("\n1059 \tstack[k][1]=to;"); 
	(stack[(u_ct_("\t/* k",sizeof(k),k), k)][1]=(to, u_ct_("\t/* to",sizeof(to),to), to), u_ct_("\t/* stack[k][1]",sizeof(stack[k][1]),stack[k][1]), stack[k][1]);
t_ct_("\n1061 \tevents[k]=time(0)+prange(from,to)*trav_time;"); 
	(events[(u_ct_("\t/* k",sizeof(k),k), k)]=time(0)+prange((u_ct_("\t/* from",sizeof(from),from), from),(u_ct_("\t/* to",sizeof(to),to), to))*(trav_time, u_ct_("\t/* trav_time",sizeof(trav_time),trav_time), trav_time), u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k]);t_ct_("\n1062 \tprintf(\"Fleet will arrive at %s\\n\",sdate(events[k]));"); 
	printf("Fleet will arrive at %s\n",sdate((events[(u_ct_("\t/* k",sizeof(k),k), k)], u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k])));t_ct_("\n1063 \tpirate(k);"); 
	pirate((u_ct_("\t/* k",sizeof(k),k), k));t_ct_("\n    /* return */"); 
}

/*
 * kwicklist command
 */
kwick()
{
	int idx;
char *_ct;t_ct_("\n1069 kwick()"); t_ct_("\n1073 \tprintf(\"Kwick list:\\n\\n\");"); 
	printf("Kwick list:\n\n");t_ct_("\n1074 \tfor (idx = 0; idx <= 39; idx++)"); 
	for ((idx = 0, u_ct_("\t/* idx",sizeof(idx),idx), idx); (u_ct_("\t/* idx",sizeof(idx),idx), idx) <= 39;t_ct_("\n1074 \tfor (idx = 0; idx <= 39; idx++)"),(idx++, u_ct_("\t/* idx",sizeof(idx),idx), idx-1)){
	{t_ct_("\n1075 \t{"); t_ct_("\n1076 \t\tif (s->owner[idx] == mynum)"); 
		if ((s->owner[(u_ct_("\t\t/* idx",sizeof(idx),idx), idx)], u_ct_("\t\t/* s->owner[idx]",sizeof(s->owner[idx]),s->owner[idx]), s->owner[idx]) == (u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_("\n1077 \t\t\tprintf(\"#%2d Prod:%3d Ships:%5d\\n\",idx,s->produ[idx],s->ships[idx]);"); 
			printf("#%2d Prod:%3d Ships:%5d\n",(u_ct_("\t\t\t/* idx",sizeof(idx),idx), idx),(s->produ[(u_ct_("\t\t\t/* idx",sizeof(idx),idx), idx)], u_ct_("\t\t\t/* s->produ[idx]",sizeof(s->produ[idx]),s->produ[idx]), s->produ[idx]),(s->ships[(u_ct_("\t\t\t/* idx",sizeof(idx),idx), idx)], u_ct_("\t\t\t/* s->ships[idx]",sizeof(s->ships[idx]),s->ships[idx]), s->ships[idx]));}t_ct_("\n1078 \t}"); 
	}}t_ct_("\n    /* return */"); 
}

/*
 * fetch a number from the keyboard
 */

getnum(x)

int *x;
{
	char buffer[99];
	int flg,k,j;
char *_ct;t_ct_("\n1085 getnum(x)"); t_ct_("\n1092 \tfflush(stdin);"); 
	fflush(stdin);t_ct_("\n1093 \tflg = -1;"); 
	(flg = -1, u_ct_("\t/* flg",sizeof(flg),flg), flg);
	while(t_ct_("\n1094 \twhile(flg==-1)"),(u_ct_("\t/* flg",sizeof(flg),flg), flg)==-1){
	{t_ct_("\n1095 \t{"); t_ct_("\n1096 \t\tflg = 0;"); 
		(flg = 0, u_ct_("\t\t/* flg",sizeof(flg),flg), flg);t_ct_("\n1097 \t\tj = 0;"); 
		(j = 0, u_ct_("\t\t/* j",sizeof(j),j), j);
		while(t_ct_("\n1098 \t\twhile( (k = getchar())!='\\n' )"),((k = getchar(), u_ct_("\t\t/* k",sizeof(k),k), k))!='\n' ){
		{t_ct_("\n1099 \t\t{"); t_ct_("\n1100 \t\t\tbuffer[j]=k;"); 
			(buffer[(u_ct_("\t\t\t/* j",sizeof(j),j), j)]=(k, u_ct_("\t\t\t/* k",sizeof(k),k), k), u_ct_("\t\t\t/* buffer[j]",sizeof(buffer[j]),buffer[j]), buffer[j]);t_ct_("\n1101 \t\t\tif (k<'0' || k>'9')"); 
			if ((u_ct_("\t\t\t/* k",sizeof(k),k), k)<'0' || (u_ct_("\t\t\t/* k",sizeof(k),k), k)>'9'){t_ct_(" flg = -1;");  (flg = -1, u_ct_("\t\t\t/* flg",sizeof(flg),flg), flg);}t_ct_("\n1102 \t\t\tj++;"); 
			(j++, u_ct_("\t\t\t/* j",sizeof(j),j), j-1);t_ct_("\n1103 \t\t}"); 
		}}t_ct_("\n1104 \t\tif(j==0)"); 
		if((u_ct_("\t\t/* j",sizeof(j),j), j)==0){t_ct_(" flg = -1;");  (flg = -1, u_ct_("\t\t/* flg",sizeof(flg),flg), flg);}t_ct_("\n1105 \t\tif(flg==-1)"); 
		if((u_ct_("\t\t/* flg",sizeof(flg),flg), flg)==-1){t_ct_(" printf(\"\\nA number is expected: \");");  printf("\nA number is expected: ");}t_ct_("\n1106 \t}"); 
	}}t_ct_("\n1107 \tbuffer[j]='\\0';"); 
	(buffer[(u_ct_("\t/* j",sizeof(j),j), j)]='\0', u_ct_("\t/* buffer[j]",sizeof(buffer[j]),buffer[j]), buffer[j]);t_ct_("\n1108 \tj=sscanf(buffer,\"%d\",x);"); 
	(j=sscanf((u_ct_("\t/* buffer",sizeof(buffer),buffer), buffer),"%d",(u_ct_("\t/* x",sizeof(x),x), x)), u_ct_("\t/* j",sizeof(j),j), j);t_ct_("\n    /* return */"); 
}

/*
 * getppn() -- fetch the user's PPN number 
 */
getppn(xx,yy)

int *xx, *yy;
{char *_ct;t_ct_("\n1114 getppn(xx,yy)"); t_ct_("\n1118 \t*xx = getgid();"); 
	(*xx = getgid(), u_ct_("\t/* *xx",sizeof(*xx),*xx), *xx);t_ct_("\n1119 \t*yy = getuid();"); 
	(*yy = getuid(), u_ct_("\t/* *yy",sizeof(*yy),*yy), *yy);t_ct_("\n    /* return */"); 
}

clrgam()
{
	char text[100];
char *_ct;t_ct_("\n1122 clrgam()"); t_ct_("\n1126 \tif (myp==3 && mypn==0 || myp==102 && mypn==80)"); 
	if ((u_ct_("\t/* myp",sizeof(myp),myp), myp)==3 && (u_ct_("\t/* mypn",sizeof(mypn),mypn), mypn)==0 || (u_ct_("\t/* myp",sizeof(myp),myp), myp)==102 && (u_ct_("\t/* mypn",sizeof(mypn),mypn), mypn)==80){{t_ct_("{"); t_ct_("\n1127 \t\tprintf(\"\\n(Cleargame): Are you sure:N\\010\");"); 
		printf("\n(Cleargame): Are you sure:N\010");t_ct_("\n1128 \t\tgets(text);"); 
		(_ct = gets(text), s_ct_("\t\t/* text",text), _ct);t_ct_("\n1129 \t\tif(!strcmp(text,\"YES\"))"); 
		if(!(strcmp((s_ct_("\t\t/* text",text), text),"YES"), u_ct_("\t\t/* strcmp(text,\"YES\")",sizeof(strcmp(text,"YES")),strcmp(text,"YES")), strcmp(text,"YES"))){{t_ct_("{"); t_ct_("\n1130 \t\t\ts->start = 0;"); 
			(s->start = 0, u_ct_("\t\t\t/* s->start",sizeof(s->start),s->start), s->start);t_ct_("\n1131 \t\t\tsetup();"); 
			setup();t_ct_("\n1132 \t\t}"); 
		}}t_ct_("\n1133 \t}"); 
	}}t_ct_("\n    /* return */"); 
}
/*
 * scan() - Displays map in nonvt mode.
 */
scan()
{
	int x,y;char *_ct;t_ct_("\n1138 scan()"); t_ct_("\n1141 \tfor(y=1;y<15;y++)"); 
	for((y=1, u_ct_("\t/* y",sizeof(y),y), y);(u_ct_("\t/* y",sizeof(y),y), y)<15;t_ct_("\n1141 \tfor(y=1;y<15;y++)"),(y++, u_ct_("\t/* y",sizeof(y),y), y-1)){{t_ct_("{"); t_ct_("\n1142 \t\tfor(x=1;x<16;x++)"); 
		for((x=1, u_ct_("\t\t/* x",sizeof(x),x), x);(u_ct_("\t\t/* x",sizeof(x),x), x)<16;t_ct_("\n1142 \t\tfor(x=1;x<16;x++)"),(x++, u_ct_("\t\t/* x",sizeof(x),x), x-1)){{t_ct_("{"); t_ct_("\n1143 \t\t\tputchar(' ');"); 
			putchar(' ');t_ct_("\n1144 \t\t\tif(s->scaner[x][y]<0)"); 
			if((s->scaner[(u_ct_("\t\t\t/* x",sizeof(x),x), x)][(u_ct_("\t\t\t/* y",sizeof(y),y), y)], u_ct_("\t\t\t/* s->scaner[x][y]",sizeof(s->scaner[x][y]),s->scaner[x][y]), s->scaner[x][y])<0){t_ct_(" printf(\". \");");  printf(". ");}
			else{t_ct_("\n1145 \t\t\telse"); t_ct_(" printf(\"%2d\",s->scaner[x][y]);");  printf("%2d",(s->scaner[(u_ct_("\t\t\t/* x",sizeof(x),x), x)][(u_ct_("\t\t\t/* y",sizeof(y),y), y)], u_ct_("\t\t\t/* s->scaner[x][y]",sizeof(s->scaner[x][y]),s->scaner[x][y]), s->scaner[x][y]));}t_ct_("\n1146 \t\t}"); 
		}}t_ct_("\n1147 \t\tputchar('\\n');"); 
		putchar('\n');t_ct_("\n1148 \t}"); 
	}}t_ct_("\n    /* return */"); 
}
/*
 * setit() - VTMODE Screen Setup.
 */
setit()
{
	int i;char *_ct;t_ct_("\n1153 setit()"); t_ct_("\n1156 \tcls();"); 
	cls();t_ct_("\n1157 \tfor(i=1;i<15;i++)"); 
	for((i=1, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<15;t_ct_("\n1157 \tfor(i=1;i<15;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1158 \t\tgotoxy(i,36);"); 
		gotoxy((u_ct_("\t\t/* i",sizeof(i),i), i),36);t_ct_("\n1159 \t\tprintf(\".  .  .  .  .  .  .  .  .  .  .  .  .  .  .\");"); 
		printf(".  .  .  .  .  .  .  .  .  .  .  .  .  .  .");t_ct_("\n1160 \t}"); 
	}}t_ct_("\n1161 \tgotoxy(14,1);"); 
	gotoxy(14,1);t_ct_("\n1162 \tprintf(\"===Command- =========[     ]=======\");"); 
	printf("===Command- =========[     ]=======");t_ct_("\n    /* return */"); 
}
/*
 * updtim() - Update gametime in VTMODE.
 */
updtim()
{char *_ct;t_ct_("\n1167 updtim()"); t_ct_("\n1169 \tgotoxy(14,23);"); 
	gotoxy(14,23);t_ct_("\n1170 \tprintf(\"%s\",sdate(0L));"); 
	printf("%s",sdate(0L));t_ct_("\n    /* return */"); 
}
cls()
{char *_ct;t_ct_("\n1172 cls()"); t_ct_("\n1174 \tgotoxy(1,1);"); 
	gotoxy(1,1);t_ct_("\n1175 \tprintf(\"\\033[J\");"); 
	printf("\033[J");t_ct_("\n    /* return */"); 
}
gotoxy(x,y)
char x,y;
{char *_ct;t_ct_("\n1177 gotoxy(x,y)"); t_ct_("\n1180 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1181 \tprintf(\"[%d;%dH\",x,y);"); 
	printf("[%d;%dH",(u_ct_("\t/* x",sizeof(x),x), x),(u_ct_("\t/* y",sizeof(y),y), y));t_ct_("\n1182 \treturn(0);"); 
	return(0);
}
setscr()
{char *_ct;t_ct_("\n1184 setscr()"); t_ct_("\n1186 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1187 \tprintf(\"[15;24r\");"); 
	printf("[15;24r");t_ct_("\n    /* return */"); 
}
nrmscr()
{char *_ct;t_ct_("\n1189 nrmscr()"); t_ct_("\n1191 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1192 \tprintf(\"[0;24r\");"); 
	printf("[0;24r");t_ct_("\n    /* return */"); 
}
/*
 * autosd() - Sends multiple fleets from all planets to a certain
 *            planet.
 */
autosd()
{
	int k,i,to,j,howmch;char *_ct;t_ct_("\n1198 autosd()"); t_ct_("\n1201 \ti = -1;"); 
	(i = -1, u_ct_("\t/* i",sizeof(i),i), i);
	while(t_ct_("\n1202 \twhile(i==-1)"),(u_ct_("\t/* i",sizeof(i),i), i)==-1){{t_ct_("{"); t_ct_("\n1203 \t\tprintf(\"Autosend to which planet: \");"); 
		printf("Autosend to which planet: ");t_ct_("\n1204 \t\ti=getnum(&to);"); 
		(i=getnum(&to), u_ct_("\t\t/* i",sizeof(i),i), i);t_ct_("\n1205 \t\tif(to<0 || to>39)"); 
		if((u_ct_("\t\t/* to",sizeof(to),to), to)<0 || (u_ct_("\t\t/* to",sizeof(to),to), to)>39){t_ct_(" i = -1;");  (i = -1, u_ct_("\t\t/* i",sizeof(i),i), i);}t_ct_("\n1206 \t}"); 
	}}t_ct_("\n1207 \tfor(k=0;k<40;k++)"); 
	for((k=0, u_ct_("\t/* k",sizeof(k),k), k);(u_ct_("\t/* k",sizeof(k),k), k)<40;t_ct_("\n1207 \tfor(k=0;k<40;k++)"),(k++, u_ct_("\t/* k",sizeof(k),k), k-1)){{t_ct_("{"); t_ct_("\n1208 \t\tif(s->owner[k]==mynum && k!=to)"); 
		if((s->owner[(u_ct_("\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t/* s->owner[k]",sizeof(s->owner[k]),s->owner[k]), s->owner[k])==(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum) && (u_ct_("\t\t/* k",sizeof(k),k), k)!=(u_ct_("\t\t/* to",sizeof(to),to), to)){{t_ct_("{"); t_ct_("\n1209 \t\t\ti = -1;"); 
			(i = -1, u_ct_("\t\t\t/* i",sizeof(i),i), i);
			while(t_ct_("\n1210 \t\t\twhile(i==-1)"),(u_ct_("\t\t\t/* i",sizeof(i),i), i)==-1){{t_ct_("{"); t_ct_("\n1211 \t\t\t\tprintf(\"#%2d - %4d: \",k,s->ships[k]);"); 
				printf("#%2d - %4d: ",(u_ct_("\t\t\t\t/* k",sizeof(k),k), k),(s->ships[(u_ct_("\t\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k]));t_ct_("\n1212 \t\t\t\ti = getnum(&howmch);"); 
				(i = getnum(&howmch), u_ct_("\t\t\t\t/* i",sizeof(i),i), i);t_ct_("\n1213 \t\t\t\tif(howmch<0 || howmch>s->ships[k])"); 
				if((u_ct_("\t\t\t\t/* howmch",sizeof(howmch),howmch), howmch)<0 || (u_ct_("\t\t\t\t/* howmch",sizeof(howmch),howmch), howmch)>(s->ships[(u_ct_("\t\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k])){t_ct_(" i = -1;");  (i = -1, u_ct_("\t\t\t\t/* i",sizeof(i),i), i);}
				else{t_ct_("\n1214 \t\t\t\telse"); t_ct_(" if(howmch>0)");  if((u_ct_("\t\t\t\t/* howmch",sizeof(howmch),howmch), howmch)>0){{t_ct_("{"); t_ct_("\n1215 \t\t\t\t\tfor (j=0;events[j]!=0L && j<100; j++)"); 
					for ((j=0, u_ct_("\t\t\t\t\t/* j",sizeof(j),j), j);(events[(u_ct_("\t\t\t\t\t/* j",sizeof(j),j), j)], u_ct_("\t\t\t\t\t/* events[j]",sizeof(events[j]),events[j]), events[j])!=0L && (u_ct_("\t\t\t\t\t/* j",sizeof(j),j), j)<100;t_ct_("\n1215 \t\t\t\t\tfor (j=0;events[j]!=0L && j<100; j++)"),(j++, u_ct_("\t\t\t\t\t/* j",sizeof(j),j), j-1)){t_ct_(";"); ;}t_ct_("\n1216 \t\t\t\t\tif(j==100)"); 
					if((u_ct_("\t\t\t\t\t/* j",sizeof(j),j), j)==100){{t_ct_("{"); t_ct_("\n1217 \t\t\t\t\t\tprintf(\"\\nSorry, too many fleets in the air!\\n\");"); 
						printf("\nSorry, too many fleets in the air!\n");t_ct_("\n1218 \t\t\t\t\t\treturn(0);"); 
						return(0);
					}}t_ct_("\n1220 \t\t\t\t\tif(s->owner[k]!=mynum)"); 
					if((s->owner[(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t\t\t/* s->owner[k]",sizeof(s->owner[k]),s->owner[k]), s->owner[k])!=(u_ct_("\t\t\t\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_(" printf(\"\\nYou have lost control of world #%d\\n\",k);");  printf("\nYou have lost control of world #%d\n",(u_ct_("\t\t\t\t\t/* k",sizeof(k),k), k));}
					else{t_ct_("\n1221 \t\t\t\t\telse");  {t_ct_(" {"); t_ct_("\n1222 \t\t\t\t\t\tstack[j][0]=howmch;"); 
						(stack[(u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j)][0]=(howmch, u_ct_("\t\t\t\t\t\t/* howmch",sizeof(howmch),howmch), howmch), u_ct_("\t\t\t\t\t\t/* stack[j][0]",sizeof(stack[j][0]),stack[j][0]), stack[j][0]);t_ct_("\n1223 \t\t\t\t\t\ts->ships[k]=s->ships[k]-howmch;"); 
						(s->ships[(u_ct_("\t\t\t\t\t\t/* k",sizeof(k),k), k)]=(s->ships[(u_ct_("\t\t\t\t\t\t/* k",sizeof(k),k), k)], u_ct_("\t\t\t\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k])-(howmch, u_ct_("\t\t\t\t\t\t/* howmch",sizeof(howmch),howmch), howmch), u_ct_("\t\t\t\t\t\t/* s->ships[k]",sizeof(s->ships[k]),s->ships[k]), s->ships[k]);t_ct_("\n1224 \t\t\t\t\t\tstack[j][1]=to;"); 
						(stack[(u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j)][1]=(to, u_ct_("\t\t\t\t\t\t/* to",sizeof(to),to), to), u_ct_("\t\t\t\t\t\t/* stack[j][1]",sizeof(stack[j][1]),stack[j][1]), stack[j][1]);t_ct_("\n1225 \t\t\t\t\t\tevents[j]=time(0)+prange(k,to)*trav_time;"); 
						(events[(u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j)]=time(0)+prange((u_ct_("\t\t\t\t\t\t/* k",sizeof(k),k), k),(u_ct_("\t\t\t\t\t\t/* to",sizeof(to),to), to))*(trav_time, u_ct_("\t\t\t\t\t\t/* trav_time",sizeof(trav_time),trav_time), trav_time), u_ct_("\t\t\t\t\t\t/* events[j]",sizeof(events[j]),events[j]), events[j]);t_ct_("\n1226 \t\t\t\t\t\tprintf(\"Fleet will arrive at %s\\n\",sdate(events[j]));"); 
						printf("Fleet will arrive at %s\n",sdate((events[(u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j)], u_ct_("\t\t\t\t\t\t/* events[j]",sizeof(events[j]),events[j]), events[j])));t_ct_("\n1227 \t\t\t\t\t\tpirate(j);"); 
						pirate((u_ct_("\t\t\t\t\t\t/* j",sizeof(j),j), j));t_ct_("\n1228 \t\t\t\t\t}"); 
					}}t_ct_("\n1229 \t\t\t\t}"); 
				}}}t_ct_("\n1230 \t\t\t}"); 
			}}t_ct_("\n1231 \t\t}"); 
		}}t_ct_("\n1232 \t}"); 
	}}t_ct_("\n    /* return */"); 
}
blink()
{char *_ct;t_ct_("\n1234 blink()"); t_ct_("\n1236 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1237 \tprintf(\"[5m\");"); 
	printf("[5m");t_ct_("\n1238 \treturn(0);"); 
	return(0);
}
dim()
{char *_ct;t_ct_("\n1240 dim()"); t_ct_("\n1242 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1243 \tprintf(\"[2m\");"); 
	printf("[2m");t_ct_("\n1244 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1245 \tprintf(\"[8m\");"); 
	printf("[8m");t_ct_("\n    /* return */"); 
}
normal()
{char *_ct;t_ct_("\n1247 normal()"); t_ct_("\n1249 \tputchar(ESC);"); 
	putchar((u_ct_("\t/* ESC",sizeof(ESC),ESC), ESC)); t_ct_("\n1250 \tprintf(\"[0m\");"); 
	printf("[0m");t_ct_("\n1251 \treturn(0);"); 
	return(0);
}
/*
 * prvtog() - Toggles who to send private messages to.
 */
prvtog()
{char *_ct;t_ct_("\n1256 prvtog()"); t_ct_("\n1258 \tif(toggle>-1)"); 
	if((u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle)>-1){{t_ct_("{"); t_ct_("\n1259 \t\ttoggle = -1;"); 
		(toggle = -1, u_ct_("\t\t/* toggle",sizeof(toggle),toggle), toggle);t_ct_("\n1260 \t\tprintf(\"Private message toggle is off.\\n\");"); 
		printf("Private message toggle is off.\n");t_ct_("\n1261 \t\treturn(0);"); 
		return(0);
	}}
	do{t_ct_("\n1263 \tdo");  {t_ct_(" {"); t_ct_("\n1264 \t\tprintf(\"Enter user number to private msg to: \");"); 
		printf("Enter user number to private msg to: ");t_ct_("\n1265 \t\tgetnum(&toggle);"); 
		getnum(&toggle);t_ct_("\n1266 \t\tif(toggle<0 || toggle>7)"); 
		if((u_ct_("\t\t/* toggle",sizeof(toggle),toggle), toggle)<0 || (u_ct_("\t\t/* toggle",sizeof(toggle),toggle), toggle)>7){t_ct_(" toggle = -1;");  (toggle = -1, u_ct_("\t\t/* toggle",sizeof(toggle),toggle), toggle);}t_ct_("\n1267 \t}"); 
	}t_ct_(" while(toggle<0);"); } while((u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle)<0);t_ct_("\n1268 \tprintf(\"Toggle set to send to %s]\",s->names[toggle]);"); 
	printf("Toggle set to send to %s]",(s->names[(u_ct_("\t/* toggle",sizeof(toggle),toggle), toggle)], u_ct_("\t/* s->names[toggle]",sizeof(s->names[toggle]),s->names[toggle]), s->names[toggle]));t_ct_("\n    /* return */"); 
}
/*
 * warp() - Send warp fleets. Arrive 1/2 the time a normal fleet
 *          takes, and uses 10% of the fleet.
 */
warp()
{
	extern char *ctime();
	int i,j,k;
	int from,to;
char *_ct;t_ct_("\n1274 warp()"); t_ct_("\n1280 \ti = -1;"); 
	(i = -1, u_ct_("\t/* i",sizeof(i),i), i);
	while(t_ct_("\n1281 \twhile(i==-1)"),(u_ct_("\t/* i",sizeof(i),i), i)==-1){
	{t_ct_("\n1282 \t{"); t_ct_("\n1283 \t\tprintf(\"\\nWarp from world:\");"); 
		printf("\nWarp from world:");t_ct_("\n1284 \t\ti=getnum(&from);"); 
		(i=getnum(&from), u_ct_("\t\t/* i",sizeof(i),i), i);t_ct_("\n1285 \t\tif(from <0 || from >39)"); 
		if((u_ct_("\t\t/* from",sizeof(from),from), from) <0 || (u_ct_("\t\t/* from",sizeof(from),from), from) >39){t_ct_(" i = -1;");  (i = -1, u_ct_("\t\t/* i",sizeof(i),i), i);}t_ct_("\n1286 \t}"); 
	}}t_ct_("\n1287 \tif (s->owner[from]!=mynum)"); 
	if ((s->owner[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->owner[from]",sizeof(s->owner[from]),s->owner[from]), s->owner[from])!=(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){
	{t_ct_("\n1288 \t{"); t_ct_("\n1289 \t\tprintf(\"\\nYou do not own that world!\\n\");"); 
		printf("\nYou do not own that world!\n");t_ct_("\n1290 \t\treturn(0);"); 
		return(0);
	}}t_ct_("\n1292 \tj = -1;"); 
	(j = -1, u_ct_("\t/* j",sizeof(j),j), j);
	while(t_ct_("\n1293 \twhile(j==-1)"),(u_ct_("\t/* j",sizeof(j),j), j)==-1){
	{t_ct_("\n1294 \t{"); t_ct_("\n1295 \t\tprintf(\"Warp to world:\");"); 
		printf("Warp to world:");t_ct_("\n1296 \t\tj=getnum(&to);"); 
		(j=getnum(&to), u_ct_("\t\t/* j",sizeof(j),j), j);t_ct_("\n1297 \t\tif(to<0 || to>39)"); 
		if((u_ct_("\t\t/* to",sizeof(to),to), to)<0 || (u_ct_("\t\t/* to",sizeof(to),to), to)>39){t_ct_(" j = -1;");  (j = -1, u_ct_("\t\t/* j",sizeof(j),j), j);}t_ct_("\n1298 \t}"); 
	}}
t_ct_("\n1300 \tfor (k=0;events[k]!=0L && k<100; k++)"); 
	for ((k=0, u_ct_("\t/* k",sizeof(k),k), k);(events[(u_ct_("\t/* k",sizeof(k),k), k)], u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k])!=0L && (u_ct_("\t/* k",sizeof(k),k), k)<100;t_ct_("\n1300 \tfor (k=0;events[k]!=0L && k<100; k++)"),(k++, u_ct_("\t/* k",sizeof(k),k), k-1)){t_ct_(";"); ;}
t_ct_("\n1302 \tif(k==100)"); 
	if((u_ct_("\t/* k",sizeof(k),k), k)==100){
	{t_ct_("\n1303 \t{"); t_ct_("\n1304 \t\tprintf(\"\\nSorry, too many fleets in the air!\\n\");"); 
		printf("\nSorry, too many fleets in the air!\n");t_ct_("\n1305 \t\treturn(0);"); 
		return(0);
	}}
t_ct_("\n1308 \tprintf(\"Number to send (%d) :\",s->ships[from]);"); 
	printf("Number to send (%d) :",(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]));t_ct_("\n1309 \ti=getnum(&j);"); 
	(i=getnum(&j), u_ct_("\t/* i",sizeof(i),i), i);
t_ct_("\n1311 \tif(j>s->ships[from] || j<100)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)>(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]) || (u_ct_("\t/* j",sizeof(j),j), j)<100){
	{t_ct_("\n1312 \t{"); t_ct_("\n1313 \t\tprintf(\"Invalid fleet size!\");"); 
		printf("Invalid fleet size!");t_ct_("\n1314 \t\treturn(0);"); 
		return(0);
	}}
t_ct_("\n1317 \tif(s->owner[from]!=mynum)"); 
	if((s->owner[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->owner[from]",sizeof(s->owner[from]),s->owner[from]), s->owner[from])!=(u_ct_("\t/* mynum",sizeof(mynum),mynum), mynum)){
	{t_ct_("\n1318 \t{"); t_ct_("\n1319 \t\tprintf(\"\\nYou have lost control of that world!!!\\n\");"); 
		printf("\nYou have lost control of that world!!!\n");t_ct_("\n1320 \t\treturn(0);"); 
		return(0);

	}}
t_ct_("\n1324 \tstack[k][0]=j-j/10;"); 
	(stack[(u_ct_("\t/* k",sizeof(k),k), k)][0]=(u_ct_("\t/* j",sizeof(j),j), j)-(u_ct_("\t/* j",sizeof(j),j), j)/10, u_ct_("\t/* stack[k][0]",sizeof(stack[k][0]),stack[k][0]), stack[k][0]);t_ct_("\n1325 \ts->ships[from]=s->ships[from]-j;"); 
	(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)]=(s->ships[(u_ct_("\t/* from",sizeof(from),from), from)], u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from])-(j, u_ct_("\t/* j",sizeof(j),j), j), u_ct_("\t/* s->ships[from]",sizeof(s->ships[from]),s->ships[from]), s->ships[from]);t_ct_("\n1326 \tstack[k][1]=to;"); 
	(stack[(u_ct_("\t/* k",sizeof(k),k), k)][1]=(to, u_ct_("\t/* to",sizeof(to),to), to), u_ct_("\t/* stack[k][1]",sizeof(stack[k][1]),stack[k][1]), stack[k][1]);
t_ct_("\n1328 \tevents[k]=time(0)+prange(from,to)*(trav_time/2);"); 
	(events[(u_ct_("\t/* k",sizeof(k),k), k)]=time(0)+prange((u_ct_("\t/* from",sizeof(from),from), from),(u_ct_("\t/* to",sizeof(to),to), to))*((u_ct_("\t/* trav_time",sizeof(trav_time),trav_time), trav_time)/2), u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k]);t_ct_("\n1329 \tprintf(\"Warp fleet will arrive at %s carrying %d ships\\n\",sdate(events[k]),j-j/10);"); 
	printf("Warp fleet will arrive at %s carrying %d ships\n",sdate((events[(u_ct_("\t/* k",sizeof(k),k), k)], u_ct_("\t/* events[k]",sizeof(events[k]),events[k]), events[k])),(u_ct_("\t/* j",sizeof(j),j), j)-(u_ct_("\t/* j",sizeof(j),j), j)/10);t_ct_("\n    /* return */"); 
}
/*
 * plupd() - Lists player's planets in VTMODE
 */
plupd()
{
	int i,j,col,row;char *_ct;t_ct_("\n1334 plupd()"); t_ct_("\n1337 \tclrjnk();"); 
	clrjnk();t_ct_("\n1338 \tgotoxy(1,1);"); 
	gotoxy(1,1);t_ct_("\n1339 \tprintf(\"Pl.  Pro  Shps   Pl.  Pro  Shps\");"); 
	printf("Pl.  Pro  Shps   Pl.  Pro  Shps");t_ct_("\n1340 \tcol=1;"); 
	(col=1, u_ct_("\t/* col",sizeof(col),col), col);t_ct_("\n1341 \trow=1;"); 
	(row=1, u_ct_("\t/* row",sizeof(row),row), row);t_ct_("\n1342 \tj=0;"); 
	(j=0, u_ct_("\t/* j",sizeof(j),j), j);t_ct_("\n1343 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n1343 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1344 \t\tif(s->owner[i]==mynum)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){{t_ct_("{"); t_ct_("\n1345 \t\t\tj++;"); 
			(j++, u_ct_("\t\t\t/* j",sizeof(j),j), j-1);t_ct_("\n1346 \t\t\tif(j<25)"); 
			if((u_ct_("\t\t\t/* j",sizeof(j),j), j)<25){{t_ct_("{"); t_ct_("\n1347 \t\t\t\trow++;"); 
				(row++, u_ct_("\t\t\t\t/* row",sizeof(row),row), row-1);t_ct_("\n1348 \t\t\t\tif(row>13)"); 
				if((u_ct_("\t\t\t\t/* row",sizeof(row),row), row)>13){{t_ct_("{"); t_ct_("\n1349 \t\t\t\t\trow=2;"); 
					(row=2, u_ct_("\t\t\t\t\t/* row",sizeof(row),row), row);t_ct_("\n1350 \t\t\t\t\tcol=col+17;"); 
					(col=(u_ct_("\t\t\t\t\t/* col",sizeof(col),col), col)+17, u_ct_("\t\t\t\t\t/* col",sizeof(col),col), col);t_ct_("\n1351 \t\t\t\t}"); 
				}}t_ct_("\n1352 \t\t\t\tgotoxy(row,col);"); 
				gotoxy((u_ct_("\t\t\t\t/* row",sizeof(row),row), row),(u_ct_("\t\t\t\t/* col",sizeof(col),col), col));t_ct_("\n1353 \t\t\t\tprintf(\"#%2d   %2d  %4d\",i,s->produ[i],s->ships[i]);"); 
				printf("#%2d   %2d  %4d",(u_ct_("\t\t\t\t/* i",sizeof(i),i), i),(s->produ[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t\t/* s->produ[i]",sizeof(s->produ[i]),s->produ[i]), s->produ[i]),(s->ships[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t\t/* s->ships[i]",sizeof(s->ships[i]),s->ships[i]), s->ships[i]));t_ct_("\n1354 \t\t\t}"); 
			}}t_ct_("\n1355 \t\t}"); 
		}}t_ct_("\n1356 \t}"); 
	}}t_ct_("\n1357 \tgotoxy(14,34);"); 
	gotoxy(14,34);t_ct_("\n1358 \tif(j>24)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)>24){{t_ct_("{"); t_ct_("\n1359 \t\tblink();"); 
		blink();t_ct_("\n1360 \t\tputchar('*');"); 
		putchar('*');t_ct_("\n1361 \t\tnormal();"); 
		normal();t_ct_("\n1362 \t}"); 
	}}
	else{t_ct_("\n1363 \telse"); t_ct_(" putchar('=');");  putchar('=');}t_ct_("\n1364 \tprompt();"); 
	prompt();t_ct_("\n    /* return */"); 
}
/*
 * clrjnk() - Clear out the left side box in VTMODE
 */
clrjnk()
{
	int i;char *_ct;t_ct_("\n1369 clrjnk()"); t_ct_("\n1372 \tfor(i=0;i<14;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<14;t_ct_("\n1372 \tfor(i=0;i<14;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1373 \t\tgotoxy(i,35);"); 
		gotoxy((u_ct_("\t\t/* i",sizeof(i),i), i),35);t_ct_("\n1374 \t\tputchar(ESC);"); 
		putchar((u_ct_("\t\t/* ESC",sizeof(ESC),ESC), ESC));t_ct_("\n1375 \t\tprintf(\"[1K\");"); 
		printf("[1K");t_ct_("\n1376 \t}"); 
	}}t_ct_("\n    /* return */"); 
}
/*
 * vtscan() - Shows the map in VTMODE
 */
vtscan()
{
	char x;
	int i;char *_ct;t_ct_("\n1381 vtscan()"); t_ct_("\n1385 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n1385 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1386 \t\tgotoxy(s->ypos[i],36+(3*s->xpos[i])-3);"); 
		gotoxy((s->ypos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->ypos[i]",sizeof(s->ypos[i]),s->ypos[i]), s->ypos[i]),36+(3*(s->xpos[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->xpos[i]",sizeof(s->xpos[i]),s->xpos[i]), s->xpos[i]))-3);t_ct_("\n1387 \t\tif(s->owner[i]==mynum)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_(" normal();");  normal();}
		else{t_ct_("\n1388 \t\telse"); t_ct_(" if(s->owner[i]>-1)");  if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])>-1){t_ct_(" blink();");  blink();}}t_ct_("\n1389 \t\tprintf(\"%2d\",i);"); 
		printf("%2d",(u_ct_("\t\t/* i",sizeof(i),i), i));t_ct_("\n1390 \t\tif(s->owner[i]==mynum || s->owner[i]>-1)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* mynum",sizeof(mynum),mynum), mynum) || (s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])>-1){{t_ct_("{"); t_ct_("\n1391 \t\t\tif(s->owner[i]!=mynum)"); 
			if((s->owner[(u_ct_("\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])!=(u_ct_("\t\t\t/* mynum",sizeof(mynum),mynum), mynum)){t_ct_(" normal();");  normal();}
			else{t_ct_("\n1392 \t\t\telse"); t_ct_(" dim();");  dim();}t_ct_("\n1393 \t\t}"); 
		}}t_ct_("\n1394 \t}"); 
	}}t_ct_("\n1395 \tnormal();"); 
	normal();t_ct_("\n1396 \tprompt();"); 
	prompt();t_ct_("\n    /* return */"); 
}
/*
 * vtflts() - Shows the fleets in the left side box in VTMODE.
 */
vtflts()
{
	int i,j,col,row;char *_ct;t_ct_("\n1401 vtflts()"); t_ct_("\n1404 \tclrjnk();"); 
	clrjnk();t_ct_("\n1405 \tgotoxy(1,1);"); 
	gotoxy(1,1);t_ct_("\n1406 \tprintf(\"Dst  Size  Arriv  Dst  Size  Arriv\");"); 
	printf("Dst  Size  Arriv  Dst  Size  Arriv");t_ct_("\n1407 \tcol=1;"); 
	(col=1, u_ct_("\t/* col",sizeof(col),col), col);t_ct_("\n1408 \trow=1;"); 
	(row=1, u_ct_("\t/* row",sizeof(row),row), row);t_ct_("\n1409 \tj=0;"); 
	(j=0, u_ct_("\t/* j",sizeof(j),j), j);t_ct_("\n1410 \tfor(i=0;i<99;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<99;t_ct_("\n1410 \tfor(i=0;i<99;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1411 \t\tif(events[i]!=0L)"); 
		if((events[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])!=0L){{t_ct_("{"); t_ct_("\n1412 \t\t\tj++;"); 
			(j++, u_ct_("\t\t\t/* j",sizeof(j),j), j-1);t_ct_("\n1413 \t\t\tif(j<25)"); 
			if((u_ct_("\t\t\t/* j",sizeof(j),j), j)<25){{t_ct_("{"); t_ct_("\n1414 \t\t\t\trow++;"); 
				(row++, u_ct_("\t\t\t\t/* row",sizeof(row),row), row-1);t_ct_("\n1415 \t\t\t\tif(row>13)"); 
				if((u_ct_("\t\t\t\t/* row",sizeof(row),row), row)>13){{t_ct_("{"); t_ct_("\n1416 \t\t\t\t\trow=2;"); 
					(row=2, u_ct_("\t\t\t\t\t/* row",sizeof(row),row), row);t_ct_("\n1417 \t\t\t\t\tcol=col+18;"); 
					(col=(u_ct_("\t\t\t\t\t/* col",sizeof(col),col), col)+18, u_ct_("\t\t\t\t\t/* col",sizeof(col),col), col);t_ct_("\n1418 \t\t\t\t}"); 
				}}t_ct_("\n1419 \t\t\t\tgotoxy(row,col);"); 
				gotoxy((u_ct_("\t\t\t\t/* row",sizeof(row),row), row),(u_ct_("\t\t\t\t/* col",sizeof(col),col), col));t_ct_("\n1420 \t\t\t\tprintf(\" %2d  %4d  %s\",stack[i][1],stack[i][0],sdate(events[i]));"); 
				printf(" %2d  %4d  %s",(stack[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)][1], u_ct_("\t\t\t\t/* stack[i][1]",sizeof(stack[i][1]),stack[i][1]), stack[i][1]),(stack[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)][0], u_ct_("\t\t\t\t/* stack[i][0]",sizeof(stack[i][0]),stack[i][0]), stack[i][0]),sdate((events[(u_ct_("\t\t\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t\t\t/* events[i]",sizeof(events[i]),events[i]), events[i])));t_ct_("\n1421 \t\t\t}"); 
			}}t_ct_("\n1422 \t\t}"); 
		}}t_ct_("\n1423 \t}"); 
	}}t_ct_("\n1424 \tgotoxy(14,34);"); 
	gotoxy(14,34);t_ct_("\n1425 \tif(j>24)"); 
	if((u_ct_("\t/* j",sizeof(j),j), j)>24){{t_ct_("{"); t_ct_("\n1426 \t\tblink();"); 
		blink();t_ct_("\n1427 \t\tputchar('*');"); 
		putchar('*');t_ct_("\n1428 \t\tnormal();"); 
		normal();t_ct_("\n1429 \t}"); 
	}}
	else{t_ct_("\n1430 \telse"); t_ct_(" putchar('=');");  putchar('=');}t_ct_("\n1431 \tprompt();"); 
	prompt();t_ct_("\n    /* return */"); 
}
/*
 * shtscn() - Short range map for 40 col users.
 */
shtscn()
{
	int pl,xstrt,xend,ystrt,yend,x,y;char *_ct;t_ct_("\n1436 shtscn()"); t_ct_("\n1439 \tpl = -1;"); 
	(pl = -1, u_ct_("\t/* pl",sizeof(pl),pl), pl);
	while(t_ct_("\n1440 \twhile(pl==-1)"),(u_ct_("\t/* pl",sizeof(pl),pl), pl)==-1){{t_ct_("{"); t_ct_("\n1441 \t\tprintf(\"Enter planet: \");"); 
		printf("Enter planet: ");t_ct_("\n1442 \t\tgetnum(&pl);"); 
		getnum(&pl);t_ct_("\n1443 \t\tif(pl<0 || pl>39)"); 
		if((u_ct_("\t\t/* pl",sizeof(pl),pl), pl)<0 || (u_ct_("\t\t/* pl",sizeof(pl),pl), pl)>39){t_ct_(" pl = -1;");  (pl = -1, u_ct_("\t\t/* pl",sizeof(pl),pl), pl);}t_ct_("\n1444 \t}"); 
	}}t_ct_("\n1445 \txstrt=s->xpos[pl]-5;"); 
	(xstrt=(s->xpos[(u_ct_("\t/* pl",sizeof(pl),pl), pl)], u_ct_("\t/* s->xpos[pl]",sizeof(s->xpos[pl]),s->xpos[pl]), s->xpos[pl])-5, u_ct_("\t/* xstrt",sizeof(xstrt),xstrt), xstrt);t_ct_("\n1446 \txend=s->xpos[pl]+5;"); 
	(xend=(s->xpos[(u_ct_("\t/* pl",sizeof(pl),pl), pl)], u_ct_("\t/* s->xpos[pl]",sizeof(s->xpos[pl]),s->xpos[pl]), s->xpos[pl])+5, u_ct_("\t/* xend",sizeof(xend),xend), xend);t_ct_("\n1447 \tif(xstrt<1)"); 
	if((u_ct_("\t/* xstrt",sizeof(xstrt),xstrt), xstrt)<1){ {t_ct_(" {"); t_ct_("\n1448 \t\txend=xend+(1-xstrt);"); 
		(xend=(u_ct_("\t\t/* xend",sizeof(xend),xend), xend)+(1-(u_ct_("\t\t/* xstrt",sizeof(xstrt),xstrt), xstrt)), u_ct_("\t\t/* xend",sizeof(xend),xend), xend);t_ct_("\n1449 \t\txstrt=1;"); 
		(xstrt=1, u_ct_("\t\t/* xstrt",sizeof(xstrt),xstrt), xstrt);t_ct_("\n1450 \t}"); 
	}}t_ct_("\n1451 \tif(xend>15)"); 
	if((u_ct_("\t/* xend",sizeof(xend),xend), xend)>15){ {t_ct_(" {"); t_ct_("\n1452 \t\txstrt=xstrt-(xend-15);"); 
		(xstrt=(u_ct_("\t\t/* xstrt",sizeof(xstrt),xstrt), xstrt)-((u_ct_("\t\t/* xend",sizeof(xend),xend), xend)-15), u_ct_("\t\t/* xstrt",sizeof(xstrt),xstrt), xstrt);t_ct_("\n1453 \t\txend=15;"); 
		(xend=15, u_ct_("\t\t/* xend",sizeof(xend),xend), xend);t_ct_("\n1454 \t}"); 
	}}t_ct_("\n1455 \tystrt=s->ypos[pl]-5;"); 
	(ystrt=(s->ypos[(u_ct_("\t/* pl",sizeof(pl),pl), pl)], u_ct_("\t/* s->ypos[pl]",sizeof(s->ypos[pl]),s->ypos[pl]), s->ypos[pl])-5, u_ct_("\t/* ystrt",sizeof(ystrt),ystrt), ystrt);t_ct_("\n1456 \tyend=s->ypos[pl]+5;"); 
	(yend=(s->ypos[(u_ct_("\t/* pl",sizeof(pl),pl), pl)], u_ct_("\t/* s->ypos[pl]",sizeof(s->ypos[pl]),s->ypos[pl]), s->ypos[pl])+5, u_ct_("\t/* yend",sizeof(yend),yend), yend);t_ct_("\n1457 \tif(ystrt<1)"); 
	if((u_ct_("\t/* ystrt",sizeof(ystrt),ystrt), ystrt)<1){{t_ct_("{"); t_ct_("\n1458 \t\tyend=yend+(1-ystrt);"); 
		(yend=(u_ct_("\t\t/* yend",sizeof(yend),yend), yend)+(1-(u_ct_("\t\t/* ystrt",sizeof(ystrt),ystrt), ystrt)), u_ct_("\t\t/* yend",sizeof(yend),yend), yend);t_ct_("\n1459 \t\tystrt=1;"); 
		(ystrt=1, u_ct_("\t\t/* ystrt",sizeof(ystrt),ystrt), ystrt);t_ct_("\n1460 \t}"); 
	}}t_ct_("\n1461 \tif(yend>14)"); 
	if((u_ct_("\t/* yend",sizeof(yend),yend), yend)>14){{t_ct_("{"); t_ct_("\n1462 \t\tystrt=ystrt-(yend-14);"); 
		(ystrt=(u_ct_("\t\t/* ystrt",sizeof(ystrt),ystrt), ystrt)-((u_ct_("\t\t/* yend",sizeof(yend),yend), yend)-14), u_ct_("\t\t/* ystrt",sizeof(ystrt),ystrt), ystrt);t_ct_("\n1463 \t\tyend=14;"); 
		(yend=14, u_ct_("\t\t/* yend",sizeof(yend),yend), yend);t_ct_("\n1464 \t}"); 
	}}t_ct_("\n1465 \tfor(y=ystrt;y<yend+1;y++)"); 
	for((y=(ystrt, u_ct_("\t/* ystrt",sizeof(ystrt),ystrt), ystrt), u_ct_("\t/* y",sizeof(y),y), y);(u_ct_("\t/* y",sizeof(y),y), y)<(u_ct_("\t/* yend",sizeof(yend),yend), yend)+1;t_ct_("\n1465 \tfor(y=ystrt;y<yend+1;y++)"),(y++, u_ct_("\t/* y",sizeof(y),y), y-1)){{t_ct_("{"); t_ct_("\n1466 \t\tfor(x=xstrt;x<xend+1;x++)"); 
		for((x=(xstrt, u_ct_("\t\t/* xstrt",sizeof(xstrt),xstrt), xstrt), u_ct_("\t\t/* x",sizeof(x),x), x);(u_ct_("\t\t/* x",sizeof(x),x), x)<(u_ct_("\t\t/* xend",sizeof(xend),xend), xend)+1;t_ct_("\n1466 \t\tfor(x=xstrt;x<xend+1;x++)"),(x++, u_ct_("\t\t/* x",sizeof(x),x), x-1)){{t_ct_("{"); t_ct_("\n1467 \t\t\tputchar(' ');"); 
			putchar(' ');t_ct_("\n1468 \t\t\tif(s->scaner[x][y]==-1)"); 
			if((s->scaner[(u_ct_("\t\t\t/* x",sizeof(x),x), x)][(u_ct_("\t\t\t/* y",sizeof(y),y), y)], u_ct_("\t\t\t/* s->scaner[x][y]",sizeof(s->scaner[x][y]),s->scaner[x][y]), s->scaner[x][y])==-1){t_ct_(" printf(\". \");");  printf(". ");}
			else{t_ct_("\n1469 \t\t\telse"); t_ct_(" printf(\"%2d\",s->scaner[x][y]);");  printf("%2d",(s->scaner[(u_ct_("\t\t\t/* x",sizeof(x),x), x)][(u_ct_("\t\t\t/* y",sizeof(y),y), y)], u_ct_("\t\t\t/* s->scaner[x][y]",sizeof(s->scaner[x][y]),s->scaner[x][y]), s->scaner[x][y]));}t_ct_("\n1470 \t\t}"); 
		}}t_ct_("\n1471 \t\tputchar('\\n');"); 
		putchar('\n');t_ct_("\n1472 \t}"); 
	}}t_ct_("\n    /* return */"); 
}
/*
 * vtindiv() - Lists a player's planets in VTMODE.
 */
vtindiv()
{
	int i,j,b,col,row;char *_ct;t_ct_("\n1477 vtindiv()"); t_ct_("\n1480 \tj = -1;"); 
	(j = -1, u_ct_("\t/* j",sizeof(j),j), j);
	while(t_ct_("\n1481 \twhile(j==-1)"),(u_ct_("\t/* j",sizeof(j),j), j)==-1){{t_ct_("{"); t_ct_("\n1482 \t\tprintf(\"Summary of player: \");"); 
		printf("Summary of player: ");t_ct_("\n1483 \t\tgetnum(&j);"); 
		getnum(&j);t_ct_("\n1484 \t\tif(j<0 || j>7)"); 
		if((u_ct_("\t\t/* j",sizeof(j),j), j)<0 || (u_ct_("\t\t/* j",sizeof(j),j), j)>7){t_ct_(" j = -1;");  (j = -1, u_ct_("\t\t/* j",sizeof(j),j), j);}t_ct_("\n1485 \t}"); 
	}}t_ct_("\n1486 \tclrjnk();"); 
	clrjnk();t_ct_("\n1487 \tgotoxy(1,1);"); 
	gotoxy(1,1);t_ct_("\n1488 \tprintf(\"Summary of %s]\\n\\n\",s->names[j]);"); 
	printf("Summary of %s]\n\n",(s->names[(u_ct_("\t/* j",sizeof(j),j), j)], u_ct_("\t/* s->names[j]",sizeof(s->names[j]),s->names[j]), s->names[j]));t_ct_("\n1489 \tprintf(\"Planet  Planet  Planet  Planet\");"); 
	printf("Planet  Planet  Planet  Planet");t_ct_("\n1490 \tcol=3;"); 
	(col=3, u_ct_("\t/* col",sizeof(col),col), col);t_ct_("\n1491 \trow=3;"); 
	(row=3, u_ct_("\t/* row",sizeof(row),row), row);t_ct_("\n1492 \tb=0;"); 
	(b=0, u_ct_("\t/* b",sizeof(b),b), b);t_ct_("\n1493 \tfor(i=0;i<40;i++)"); 
	for((i=0, u_ct_("\t/* i",sizeof(i),i), i);(u_ct_("\t/* i",sizeof(i),i), i)<40;t_ct_("\n1493 \tfor(i=0;i<40;i++)"),(i++, u_ct_("\t/* i",sizeof(i),i), i-1)){{t_ct_("{"); t_ct_("\n1494 \t\tif(s->owner[i]==j)"); 
		if((s->owner[(u_ct_("\t\t/* i",sizeof(i),i), i)], u_ct_("\t\t/* s->owner[i]",sizeof(s->owner[i]),s->owner[i]), s->owner[i])==(u_ct_("\t\t/* j",sizeof(j),j), j)){{t_ct_("{"); t_ct_("\n1495 \t\t\tb++;"); 
			(b++, u_ct_("\t\t\t/* b",sizeof(b),b), b-1);t_ct_("\n1496 \t\t\tif(b>10)"); 
			if((u_ct_("\t\t\t/* b",sizeof(b),b), b)>10){{t_ct_("{"); t_ct_("\n1497 \t\t\t\tb=1;"); 
				(b=1, u_ct_("\t\t\t\t/* b",sizeof(b),b), b);t_ct_("\n1498 \t\t\t\trow=3;"); 
				(row=3, u_ct_("\t\t\t\t/* row",sizeof(row),row), row);t_ct_("\n1499 \t\t\t\tcol=col+8;"); 
				(col=(u_ct_("\t\t\t\t/* col",sizeof(col),col), col)+8, u_ct_("\t\t\t\t/* col",sizeof(col),col), col);t_ct_("\n1500 \t\t\t}"); 
			}}t_ct_("\n1501 \t\t\trow++;"); 
			(row++, u_ct_("\t\t\t/* row",sizeof(row),row), row-1);t_ct_("\n1502 \t\t\tgotoxy(row,col);"); 
			gotoxy((u_ct_("\t\t\t/* row",sizeof(row),row), row),(u_ct_("\t\t\t/* col",sizeof(col),col), col));t_ct_("\n1503 \t\t\tprintf(\"#%2d\",i);"); 
			printf("#%2d",(u_ct_("\t\t\t/* i",sizeof(i),i), i));t_ct_("\n1504 \t\t}"); 
		}}t_ct_("\n1505 \t}"); 
	}}t_ct_("\n1506 \tprompt();"); 
	prompt();t_ct_("\n    /* return */"); 
}
/*
 * pirate() - Random pirate fleets.
 */
pirate(i)
int i;
{
	int ispir;char *_ct;t_ct_("\n1511 pirate(i)"); t_ct_("\n1515 \tispir=irand(40);"); 
	(ispir=irand(40), u_ct_("\t/* ispir",sizeof(ispir),ispir), ispir);t_ct_("\n1516 \tif(pirtim!=0L)"); 
	if((u_ct_("\t/* pirtim",sizeof(pirtim),pirtim), pirtim)!=0L){t_ct_(" return(0);");  return(0);}t_ct_("\n1517 \tif(ispir!=25)"); 
	if((u_ct_("\t/* ispir",sizeof(ispir),ispir), ispir)!=25){t_ct_(" return(0);");  return(0);}
	while(t_ct_("\n1518 \twhile(pirsiz<20)"),(u_ct_("\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz)<20){t_ct_(" pirsiz=irand(80);");  (pirsiz=irand(80), u_ct_("\t/* pirsiz",sizeof(pirsiz),pirsiz), pirsiz);}t_ct_("\n1519 \tpirflt=i;"); 
	(pirflt=(i, u_ct_("\t/* i",sizeof(i),i), i), u_ct_("\t/* pirflt",sizeof(pirflt),pirflt), pirflt);t_ct_("\n1520 \tpirtim=events[i]-20L;"); 
	(pirtim=(events[(u_ct_("\t/* i",sizeof(i),i), i)], u_ct_("\t/* events[i]",sizeof(events[i]),events[i]), events[i])-20L, u_ct_("\t/* pirtim",sizeof(pirtim),pirtim), pirtim);t_ct_("\n    /* return */"); 
}
/*
 * Exit the game
*/
leave()
{char *_ct;t_ct_("\n1525 leave()"); t_ct_("\n1527 \tfcntl(0, F_SETFL, flags & ~O_NDELAY);"); 
	fcntl(0, (u_ct_("\t/* F_SETFL",sizeof(F_SETFL),F_SETFL), F_SETFL), (u_ct_("\t/* flags",sizeof(flags),flags), flags) & ~(u_ct_("\t/* O_NDELAY",sizeof(O_NDELAY),O_NDELAY), O_NDELAY));t_ct_("\n1528 \tioctl(0, TCSETAF, &tsave);"); 
	ioctl(0, (u_ct_("\t/* TCSETAF",sizeof(TCSETAF),TCSETAF), TCSETAF), &tsave);t_ct_("\n1529 \tprintf(\"Exiting\\n\");"); 
	printf("Exiting\n");t_ct_("\n1530 \texit(0);"); 
	exit(0);t_ct_("\n    /* return */"); 
}
#include <stdio.h>
#include <setjmp.h>
#define VM_CT_ 10
#define PF_CT_ printf(
#define LM_CT_ 20
#line 1 "/usr/lib/ctrace/runtime.c"
/*	Copyright (c) 1984 AT&T	*/
/*	  All Rights Reserved  	*/

/*	THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	*/
/*	The copyright notice above does not evidence any   	*/
/*	actual or intended publication of such source code.	*/

#ident	"@(#)ctrace:runtime.c	1.3"
/*	ctrace - C program debugging tool
 *
 *	run-time package of trace functions
 *
 */

#ifndef B_CT_

/* signal catching function used by u_ct_ */
static jmp_buf sj_ct_;
static
f_ct_()
{
	longjmp(sj_ct_, 1);
}
#endif

#ifdef LM_CT_
#define I_CT_(x)	((x + LM_CT_) % LM_CT_)

/* global data used by loop detection code */
static	int	ts_ct_;	/* trace state */
#endif

/* global data used by duplicate variable trace code */
static	int	vc_ct_;	/* var trace count within statement */

static	struct	{	/* var values within statement */
	char	*vn_ct_;	/* var name */
	int	vt_ct_;		/* var type (0 is string, > 0 is size) */
	union	{		/* var value */
		char	*vs_ct_;/* string */
		int	vi_ct_;	/* int */
		long	vl_ct_;	/* long */
		double	vd_ct_;	/* double */
	} vv_ct_;
} v_ct_[VM_CT_];

/* trace on/off control */
static	int	tr_ct_ = 1;

static
ctron()
{
	tr_ct_ = 1;
	PF_CT_" \b\n    /* trace on */ \b");
}

static
ctroff()
{
	tr_ct_ = 0;
	PF_CT_" \b\n    /* trace off */ \b");
}
/* print the statement text */

static
t_ct_(text)
register char	*text;
{
#ifdef LM_CT_
	static	int	loop_start, next_stmt;
	static	char	*stmt[LM_CT_];
	static	long	loops;
	register int	i;
	register char	*s;
	register char	c;
#endif

	/* return if tracing is off */
	if (!tr_ct_) {
		return;
	}
#ifdef LM_CT_
	if (ts_ct_ == 2)	/* if not tracing */
		if (strcmp(text, stmt[next_stmt]) == 0) {
			if (strcmp(text, stmt[loop_start]) == 0) {
				++loops;
				if (loops % 1000 == 0)
					PF_CT_" \b\n    /* still repeating after %ld times */ \b", loops);
				next_stmt = loop_start;
			}
			next_stmt = I_CT_(next_stmt + 1);
			vc_ct_ = 0;	/* reset the var count */
			return;
		}
		else {	/* doesn't match next statement */
			if (loops == 0)
				PF_CT_" \b\n    /* repeated < 1 time */ \b");
			else
				PF_CT_" \b\n    /* repeated %ld times */ \b", loops);
			loops = 0;
			PF_CT_" \b%s \b", stmt[I_CT_(next_stmt - 1)]); /* print last statement */
			ts_ct_ = 4;	/* force var printing */
			for (i = 0; i < vc_ct_; ++i) /* print its vars */
				if (v_ct_[i].vt_ct_ == 0) /* string? */
					s_ct_(v_ct_[i].vn_ct_, v_ct_[i].vv_ct_.vs_ct_); /* yes */
				else if (v_ct_[i].vt_ct_ == sizeof(int))
					u_ct_(v_ct_[i].vn_ct_, v_ct_[i].vt_ct_, v_ct_[i].vv_ct_.vi_ct_);
				else if (v_ct_[i].vt_ct_ == sizeof(long))
					u_ct_(v_ct_[i].vn_ct_, v_ct_[i].vt_ct_, v_ct_[i].vv_ct_.vl_ct_);
				else	/* double */
					u_ct_(v_ct_[i].vn_ct_, v_ct_[i].vt_ct_, v_ct_[i].vv_ct_.vd_ct_);
			ts_ct_ = 0;	/* start tracing */
		}
#endif
	vc_ct_ = 0;	/* reset the var count */

#ifdef LM_CT_
	if (ts_ct_ == 0) {	/* if looking for the start of a loop */
		/* if statement in list */
		for (i = I_CT_(next_stmt - 2); i != I_CT_(next_stmt - 1); i = I_CT_(i - 1))
			if ((s = stmt[i]) != 0 &&	/* saved text could be null */
			    strcmp(text, s) == 0 &&	/* if text matches */
			    (c = s[strlen(s) - 1]) != '{' && c != '}') { /* and is not a brace */
				ts_ct_ = 1;	/* look for the loop end */
				loop_start = i;
				next_stmt = I_CT_(loop_start + 1);
				goto print_stmt;
			}
	}
	else	/* if looking for the loop end */
		if (strcmp(text, stmt[loop_start]) == 0) { /* if start stmt */
			ts_ct_ = 2;			/* stop tracing */
			PF_CT_" \b\n    /* repeating */ \b");
			stmt[next_stmt] = text;	/* save as end marker */
			next_stmt = I_CT_(loop_start + 1);
			return;
		}
		else if (strcmp(text, stmt[next_stmt]) != 0) /* if not next stmt */
			ts_ct_ = 0; /* look for the start of a loop */
	stmt[next_stmt] = text;	/* save this statement */
	next_stmt = I_CT_(next_stmt + 1); /* inc the pointer */
print_stmt:
#endif
	PF_CT_" \b%s \b", text); /* print this statement */
#ifndef B_CT_
	fflush(stdout);		/* flush the output buffer */
#endif
}
/* dump a string variable */

static
s_ct_(name, value)
register char	*name;
register char	*value;
{
	/* return if tracing is off */
	if (!tr_ct_) {
		return;
	}
#ifdef LM_CT_
	/* save the var name and value */
	if (ts_ct_ != 4) {	/* if not forcing var printing */
		v_ct_[vc_ct_].vn_ct_ = name;
		v_ct_[vc_ct_].vt_ct_ = 0;	/* var type is string */
		v_ct_[vc_ct_].vv_ct_.vs_ct_ = value;
		++vc_ct_;
	}

	if (ts_ct_ == 2)	/* if not tracing */
		return;
#endif

	PF_CT_" \b\n    %s == \"", name);
	/* flush before printing the string because it may cause an
	   abort if it is not null terminated */
#ifndef B_CT_
	fflush(stdout);
#endif
	PF_CT_"%s\" */ \b", value);
#ifndef B_CT_
	fflush(stdout);
#endif
}
/* dump a variable of an unknown type */

static
u_ct_(name, _size, value)	/* size is a macro in <macros.h> */
char	*name;
register int	_size;
union {
	char	*p;
	int	i;
	long	l;
	double	d;
} value;
{
#ifndef isprint
#include <ctype.h>
#endif
	register int	i;
	register char	*s;
	register char	c;

	/* return if tracing is off */
	if (!tr_ct_) {
		return;
	}
	/* normalize the size (pointer and float are the same size as either int or long) */
	if (_size == sizeof(char) || _size == sizeof(short))
		_size = sizeof(int);
	else if (_size != sizeof(int) && _size != sizeof(long) && _size != sizeof(double))
		/* this is an extern pointer (size=0), or array or struct address */
		_size = sizeof(char *);
#ifdef LM_CT_
	if (ts_ct_ != 4) {	/* if not forcing var printing */
#endif
		/* don't dump the variable if its value is the same */
		for (i = 0; i < vc_ct_; ++i)
			if (_size == v_ct_[i].vt_ct_ && strcmp(name, v_ct_[i].vn_ct_) == 0)
				if (_size == sizeof(int)) {
					if (value.i == v_ct_[i].vv_ct_.vi_ct_)
						return;
				}
				else if (_size == sizeof(long)) {
					if (value.l == v_ct_[i].vv_ct_.vl_ct_)
						return;
				}
				else /* double */
					if (value.d == v_ct_[i].vv_ct_.vd_ct_)
						return;
	
		/* save the var name and value */
		v_ct_[vc_ct_].vn_ct_ = name;
		v_ct_[vc_ct_].vt_ct_ = _size;
		if (_size == sizeof(int)) {
			v_ct_[vc_ct_].vv_ct_.vi_ct_ = value.i;
		}
		else if (_size == sizeof(long)) {
			v_ct_[vc_ct_].vv_ct_.vl_ct_ = value.l;
		}
		else /* double */
			v_ct_[vc_ct_].vv_ct_.vd_ct_ = value.d;
		++vc_ct_;
#ifdef LM_CT_
	}

	if (ts_ct_ == 2)	/* if not tracing */
		return;
#endif
	/* determine the variable type and print it */
	PF_CT_" \b\n    %s == ", name);
	if (_size == sizeof(int)) {
		PF_CT_"%d", value.i);	/* decimal */
#ifdef O_CT_
		if ((unsigned) value.i > 7)	/* octal */
			PF_CT_" or 0%o", value.i);
#endif
#ifdef X_CT_
		if ((unsigned) value.i > 9)	/* hexadecimal */
			PF_CT_" or 0x%x", value.i);
#endif
#ifdef U_CT_
		if (value.i < 0)		/* unsigned */
			PF_CT_" or %u", value.i);
#endif
#ifdef E_CT_
		if (_size == sizeof(float))	/* float */
			PF_CT_" or %e", value.i);
#endif
		if ((unsigned) value.i <= 255) /* character */
			if (isprint(value.i))
				PF_CT_" or '%c'", value.i);
			else if (iscntrl(value.i)) {
				switch (value.i) {
					case '\n': c = 'n'; break;
					case '\t': c = 't'; break;
					case '\b': c = 'b'; break;
					case '\r': c = 'r'; break;
					case '\f': c = 'f'; break;
					case '\v': c = 'v'; break;
					default:    c = '\0';
				}
				if (c != '\0')
					PF_CT_" or '\\%c'", c);
			}
	}
	else if (_size == sizeof(long)) {
		PF_CT_"%ld", value.l);	/* decimal */
#ifdef O_CT_
		if ((unsigned) value.l > 7)	/* octal */
			PF_CT_" or 0%lo", value.l);
#endif
#ifdef X_CT_
	if ((unsigned) value.l > 9)	/* hexadecimal */
			PF_CT_" or 0x%lx", value.l);
#endif
#ifdef U_CT_
		if (value.l < 0)		/* unsigned */
			PF_CT_" or %lu", value.l);
#endif
#ifdef E_CT_
		if (_size == sizeof(float))	/* float */
			PF_CT_" or %e", value.l);
#endif
	}
	else if (_size == sizeof(double))	/* double */
		PF_CT_"%e", value.d);
#ifndef B_CT_
	/* check for a possible non-null pointer */
	if (_size == sizeof(char *) && value.p != 0) {
		void	(*sigbus)(), (*sigsegv)();

		/* see if this is a non-null string */
		if (setjmp(sj_ct_) == 0) {
			sigbus = signal(SIGBUS, f_ct_);
			sigsegv = signal(SIGSEGV, f_ct_);
			if (*value.p != '\0')
				for (s = value.p; ; ++s) {
					if ((c = *s) == '\0') {
						PF_CT_" or \"%s\"", value.p);
						break;
					}
					/* don't use isspace(3) because \v and others will not print properly */
					if (!isprint(c) && c != '\t' && c != '\n')
						break;	/* not string */
				}
		}
		signal(SIGBUS, sigbus);
		signal(SIGSEGV, sigsegv);
	}
#endif
	PF_CT_" */ \b");
#ifndef B_CT_
	fflush(stdout);
#endif
}
