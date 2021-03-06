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
#include <ctype.h>
#include <signal.h>

#include <stdlib.h>
#include <time.h>

#include <sys/fcntl.h>
#include <termio.h>
#include "share.h"

#define ESC '\033'

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

	shmid1=
	    shmget(SHMKEY1,sizeof(struct s_region),0600|IPC_CREAT|IPC_EXCL);

	if (shmid1<0)
	{
		printf("\nRegion in use or already created..atempting to attach..\n");
		shmid1 = shmget(SHMKEY1,sizeof(struct s_region),0600|IPC_EXCL);
	}

	return((struct s_region *)shmat(shmid1,0,0));
}

remove_shared_region(region)
int region;
{
	printf("Removing shared region\n");
	shmctl(shmid1,IPC_RMID,(struct shmid_ds *)0);
}



int irand(value)
 unsigned int value;
 {
  int i;

  if (value == 0) return(0);  
  i = (int) (value)*drand48();
  return(i);
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

	if (value == 0L)
	{
		timex = time(0) - s->start;
	}
	else
	{
		timex = value - s->start; /* make the number reasonable to look at */
	}
	whole = timex/60L;
	decimal = timex - (whole * 60L);
	sprintf(sdatebfr,"%02ld.%02ld",whole,decimal);
	ptr = sdatebfr;
	return(ptr);
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
	long years;
	toggle = -1;

	printf("\nGALCON - (v4.0) Galactic Conquest\n");
	printf("\n");
	getppn(&myp, &mypn);

	/*remove_shared_region(SHMKEY1);*/

	s = get_shared_region();
	if (s == (struct s_region*) -1) {
		printf("Can't get shared region.\n");
	}

	if (1 == 1)
	{
		game_len = 4500L;  /* changed to desired values and recompile */
		x_siz = 15;
		y_siz = 14;
		z_siz = 1; /* Make Galaxy 2-D */
		prod_time = 45L;
		trav_time = 30L;
	}

	printf("Game length is %ld minutes\n",game_len/60);

	srand48(time(NULL));
	//srand48(clock());
	setrand();
/*
Get the current fcntl flags 
*/
	flags = fcntl(0, F_GETFL, 0);

/*  
Setup terminal for noicanonical, nointerupt, 1 char input every 2/10 seconds
*/
	ioctl(0, TCGETA, &tdes);
	tsave = tdes;
	tdes.c_lflag	&= ~ICANON;
	tdes.c_lflag  &= ~ISIG;
	tdes.c_cc[VMIN] = 1;
	tdes.c_cc[VTIME] = 2;
	ioctl(0, TCSETAF, &tdes);

	if(s->game<0 || s->game>10) s->game=0;
	if(s->meslok<0 || s->meslok>s->game) s->meslok=0;
	lock();
	if(s->game==0 || s->lstup[0]+300L < time(0) || s->lstup[0]>time(0))
	{
		printf("Starting a new game...\n");
		setup();
		s->game=0;
	}
	for (j=0;j<99;j++) events[j]=0L;
	if (s->game>8)
	{
		printf("\nSorry, too many players as it is!\n");
		leave();
	}
	for(i=0;i<20;i++)
		if(s->pppn[i]==myp && s->pnppn[i]==mypn)
		{
			printf("\nSorry, you are locked out for the remainder of the game.\n");
			leave();
		}
	for(i=0;i<15;i++){
		if(s->twicej[i]==myp && s->twiceg[i]==mypn) twice=1;
	}
	mymes=s->mesnum; 
	mycom=s->comnum;
	s->game++;
	home=0;
	mynum=0;
	{
		for(home=0;s->names[home][0]!='\0';home++);
	}
	s->names[home][0]='\n';
	mynum = home;
	home = -1;
	s->meslok--;

	s->alive[mynum]=0;
	k=0;
	s->names[mynum][k]=1;
	while(s->names[mynum][0]==1)
	{
		k=0;
		fcntl(0, F_SETFL, flags & ~O_NDELAY);
		printf("\nEnter your name, please: ");
		gets(scr);
		scr[10] = 0; /* force an EOS */
		sprintf(scr2,"%-10s [%3d,%-3d",scr,myp,mypn);
		for (k = 0; k<20; k++)
			s->names[mynum][k] = scr2[k];
	}
	lock();
	k=s->mesnum+1;
	if(k>4) k=0;
	sprintf(s->public[k],"  #%2d %s] has joined the battle.\0",mynum,s->names[mynum]);
	s->public[k][1]='E';
	s->public[k][0]=mynum;
	s->mesnum=k;
	printf("\n\n");
	home = -1;
	while(home==-1)
	{
		k=irand(40);
		if (s->owner[k]==-1)
		{
			s->owner[k]=mynum;
			if(twice) s->ships[k]=250;
			else s->ships[k]=450;
			if(twice) s->produ[k]=5;
			else s->produ[k]=15;
			home=k;
		}
	}
	printf("\nYour home planet is #%d, located at (%d,%d)\n",
	    home,s->xpos[home],s->ypos[home]);

	s->meslok--;
	prompt();
	inchar=0;
	while(1==1)
	{
		/*
Turn on non-blocking io
*/
		fcntl(0, F_SETFL, flags | O_NDELAY);

		if (inbuf[0]) getchar();
		inbuf[0] = 0;
		inchar = 0;
		inlen = read(0, inbuf, 1);
		if(inlen != 0 )
		{
			printf("\r");
			/*
Turn off non-blocking io
*/
			fcntl(0, F_SETFL, flags & ~O_NDELAY);

			inchar = inbuf[0];
			if (vtmode)gotoxy(24,1);
			inchar = tolower(inchar);
			//if(!vtmode || inchar!='y' && inchar!='u' && inchar!='l') putchar('\n');
		}
		switch(inchar)
		{
		case   0: 
			update();
			sleep(1);
			goto endinc;
		case 'a': 
			area();
			goto endinc;
		case 'b': 
			broadcast();
			goto endinc;
		case 'c': 
			compute();
			goto endinc;
		case 'd': 
			warp();
			goto endinc;
		case 'e': 
			clrgam();
			goto endinc;
		case 'f': 
			flts();
			goto endinc;
		case 'g': 
			galaxy();
			goto endinc;
		case 'i': 
			if(vtmode) vtindiv();
			else individual();
			goto endinc;
		case 'k': 
			kwick();
			goto endinc;
		case 'l': 
			if(vtmode) vtscan();
			else scan();
			goto endinc;
		case 'm': 
			prvtog();
			goto endinc;
		case 'n':  
			if(vtmode){
				vtmode=0;
				nrmscr();
				gotoxy(24,1);
				printf("VT100 Mode Disabled.\n");
			}
			goto endinc;
		case 'o': 
			oneplanet();
			goto endinc;
		case 'p': 
			prvmsg();
			goto endinc;
		case 'q': 
			printf("\n%cAre you sure you want to Quit? ",7);
			k=getchar();
			k=tolower(k);
			while(getchar()!='\n');
			if(k=='y'){
				leave();
			}
			goto endinc;
		case 'r': 
			shtscn();
			goto endinc;
		case 's': 
			send();
			goto endinc;
		case 't': 
			printf("\nCurrent time is %s\n",sdate(0L));
			years= (time(0) - s->start)/60L;
			printf("Years           %ld\n",years);
			goto endinc;
		case 'u': 
			if(vtmode) plupd();
			goto endinc;
		case 'v': 
			if(!vtmode){
				vtmode=1;
				setscr();
				setit();
				vtscan();
				plupd();
				gotoxy(24,1);
				printf("VT100 Mode Activated.\n");
			}
			goto endinc;
		case 'w': 
			who();
			goto endinc;
		case 'x': 
			autosd();
			goto endinc;
		case 'y': 
			if(vtmode) vtflts();
			goto endinc;
endinc: 
			fleets();
			chkend();
			if(mymes!=s->mesnum) getpub();
			if(mycom!=s->comnum) getcom();
			now=time(0);
			if(vtmode && lsttim<=(now-5L)){
				lsttim=now;
				updtim();
				prompt();
			}
		}
		if(inlen != 0) prompt();
	}
}

setrand()
{
	int j;
	long loop;
/*	loop=tod();*/
	loop = 1;
	loop=loop % 3600L;
	while(loop>0)
	{
		loop--;
		j=irand(loop);
	}
	j=irand(irand(irand(j)));
}

chkend()
{
	int i,j,k;
	j=0;
	k=0;
	for(i=0;i<40;i++)
		if(s->owner[i]==mynum) j++;
	for(i=0;i<99;i++)
		if(stack[i][0]!=0) k++;
	if(j==0 && k==0)
	{
		printf("\nYou have lost this round.\n");
		i=0;
		while(s->pppn[i]!=0)i++;
		s->pppn[i]=myp;
		s->pnppn[i]=mypn;
		leave();
	}
	if(j==40)
	{
		printf("\nYou have completely won the war!!\n");
		s->game=0;
		leave();
	}

	if(time(0)<s->start+game_len) return(0);

	if(vtmode){
		nrmscr();
		cls();
	}
	printf("\nThe War is OVER!\nTime limit exceeded");
	printf("\nThe points break down as follows:\n");
	for(i=0;i<10;i++)
	{
		numpl[i]=0; 
		numpr[i]=0; 
		numsh[0]=0;
	}
	for(i=0;i<40;i++)
		if(s->owner[i]!=-1)
		{
			numpl[s->owner[i]]++; 
			numpr[s->owner[i]]=numpr[s->owner[i]]+s->produ[i];
			numsh[s->owner[i]]=numsh[s->owner[i]]+s->ships[i];
		}
	for(i=0;i<10;i++)
		if((s->names[i][0] != '\0') && (s->names[i][0] != 1))
			printf("%20s] Planets:%3d production:%4d Ships:%5d  Score:%6d\n",
			    s->names[i],numpl[i],numpr[i],numsh[i],(numpl[i]*10+numpr[i]*5+numsh[i]));
	s->game--;
	leave();
}

/*
 * who() -- Display the names and PPN's of all player's in the game
 */
who()
{
	int i;

	for(i = 0; i < 8; i++)
	{
		if (s->names[i][0] != '\0')
			printf("Player #%d %20s]\n",i,s->names[i]);
	}
}

prompt()
{
	if(vtmode) gotoxy(14,12);
	else printf("\rCommand: ");
	//fflush(stdout);
}

wrapup()
{
	int i,k;
	if(s->names[mynum][0]!=1){
		lock();
		k=s->mesnum+1;
		if(k>4) k=0;
		sprintf(s->public[k],"  #%2d %s] has left the battle.\0",mynum,s->names[mynum]);
		s->public[k][1]='E';
		s->public[k][0]=mynum;
		s->mesnum=k;
		s->meslok--;
	}
	if(vtmode) {
		nrmscr();
		cls();
	}
	if(!twice && !s->twicej[14]){
		i=0;
		while(s->twicej[i]) i++;
		s->twicej[i]=myp;
		s->twiceg[i]=mypn;
	}
	for (i=0;i<40;i++) if (s->owner[i]==mynum)
	{
		s->owner[i] = -1;
		if(s->produ[i]!=10)
			s->produ[i]=prandom(10);
	}
	s->names[mynum][0]='\0';

	s->game--;
	printf("\nBye Bye\n");
	leave();
}

individual()
{
	int i,j;
	printf("\nSummary of player #:");
	i=getnum(&j);
	printf("\n");
	if(j>0 && j<10) printf("Name: %s]\n",s->names[j]);
	for (i=0;i<40;i++)
		if(s->owner[i]==j)
			printf("Planet #%2d  Distance from #%2d = %4d\n",
			    i,home,prange(home,i));

	if (s->owner[home]!=mynum)
		printf("\nNOTE: You no longer control your home world!\n");
}

/*
 * Initialize the Game Board
 */
setup()
{
	int i;
	int locx,locy,j;
	s->mesnum=s->comnum=s->meslok=0;
	s->start=time(0);
	for(i=0;i<10;i++)s->names[i][0]='\0';
	for(i=0;i<20;i++) s->pppn[i]=0;
	for(i=0;i<15;i++) s->twicej[i]=0;

	/* Clear Galaxy */
	for(i=1;i<16;i++){
		for(j=1;j<16;j++){
			s->scaner[i][j] = -1;
		}
	}
	for (i=0;i<40;i++)
	{
		s->owner[i] = -1;
		s->xpos[i]=prandom(x_siz);
		locx=s->xpos[i];
		s->ypos[i]=prandom(y_siz);
		locy=s->ypos[i];
		s->zpos[i]=prandom(z_siz);
		s->produ[i]=prandom(10);
		s->lstup[i]=time(0);
		s->ships[i]=prandom(s->produ[i]*25);
		s->scaner[locx][locy]=i;
	}
	/* Get 2 Planets RANDOMLY and make their prod to 25 */
	for (i=0;i<2;i++){
		j=prandom(40);
		s->produ[j]=25;
	}
	for(i=0;i<10;i++)
		s->names[i][0]='\0';
}

area()
{
	int base,vol,i;

	printf("\nBase Planet: ");
	i=getnum(&base);
	if(base<0 || base>39)
		return(0);
	printf("Range: ");
	i=getnum(&vol);

	for(i=0;i<40;i++)
		if(prange(base,i)<=vol)
		{
			printf("Planet #%3d Range=%3d ",i,prange(base,i));
			if (s->owner[i] == (-1))
				printf("Own= *Neutral*\n");
			else
				printf("Own = %s]\n",s->names[s->owner[i]]);
		}
}


prandom(range)
int range;
{
	return(irand(range)+1);
}

update()
{
	long now;
	int i,j,k;
	now=time(0);
	s->alive[mynum]=now;
	for(i=0;i<8;i++){
		if(s->alive[i]<(now-420L) && s->alive[i]!=0 && s->names[i][0]!='\0'){
			s->alive[i]=0;
			for(j=0;j<39;j++) if(s->owner[j]==i) s->owner[j] = -1;
			lock();
			k=s->mesnum+1;
			if(k>4) k=0;
			sprintf(s->public[k],"  %s] has been cleared.\0",s->names[i]);
			s->public[k][1]='E';
			s->public[k][0]=8;
			s->mesnum=k;
			s->meslok--;
			s->names[i][0]='\0';
			s->game--;
		}
	}

	for (i=0;i<40;i++)
		if( s->lstup[i]<(now-45L) )
		{
			s->lstup[i]=now;
			s->ships[i]=s->ships[i]+s->produ[i]*1;
			if(s->owner[i]==-1 && s->ships[i]>250) s->ships[i]=250;
			if(s->ships[i]>50*s->produ[i] && s->produ[i]<10) s->produ[i]++;
			if(s->ships[i]>1500) s->ships[i]=1500;
		}
}

galaxy()
{
	int i;
	extern char *ctime();
	printf("\nGalaxy status at %s\n\n",sdate(0L));
	for (i=0;i<40;i++)
	{
		printf("#%2d (%2d,%2d) ",i, s->xpos[i], s->ypos[i]);
		if (s->owner[i] == (-1))
			printf("*Neutral*\n");
		else
			printf("%s]\n",s->names[s->owner[i]]);
	}
	printf("\n");
}

oneplanet()
{
	int i;
	i = -1;
	while(i<0 || i>39)
	{
		printf("\nWhich Planet: ");
		getnum(&i);
	}
	if (s->owner[i] == (-1))
		printf("#%2d Owner= *Neutral*\n",i);
	else
		printf("#%2d Owner= %s] ",i,s->names[s->owner[i]]);
	if(s->owner[i]==mynum)
		printf(" Prod:%3d Ships:%5d\n",s->produ[i],s->ships[i]);
	else
		printf("\n");
}

fleets()
{
	int i,j,k,l,m,flg;
	int despla,desown,desarm,myarm;
	long now;
	now=time(0);
	if(pirtim!=0L && pirtim<now){
		pirtim=0L;
		if(vtmode) gotoxy(24,1);
		printf("\nYour fleet #%d has been attacked by pirates!\n",pirflt);
		printf("Your fleet has: %d\n",stack[pirflt][0]);
		printf("Pirate's fleet: %d\n",pirsiz);
		if(pirsiz>stack[pirflt][0]){
			printf("Your fleet has been destroyed!\n");
			events[pirflt]=0L;
		}
		else {
			stack[pirflt][0]=stack[pirflt][0]-(pirsiz/3);
			printf("%d ships escaped from the pirates!\n",stack[pirflt][0]);
		}
		pirsiz=0;
		if(vtmode) prompt();
	}
	for (i=0;i<99;i++)
		if (events[i]!=0L)
			if (events[i]<now)
			{
				myarm=stack[i][0];
				despla=stack[i][1];
				desown=s->owner[despla];
				desarm=s->ships[despla];
				if(mynum==desown)
				{
					if(vtmode) gotoxy(24,1);
					printf("\nPlanet %d recieves reinforcements\n",
					    despla);
					s->ships[despla]=desarm+myarm;
				}
				else
				{
					if(vtmode) gotoxy(24,1);
					printf("\nInvasion alert! Your fleet arrived at %d",despla);
					printf("\nYour fleet has: %d\nDefending fleet:%d\n",myarm,desarm);
					flg=0;
					lock();
					while(myarm>0 && desarm>0)
					{
						l=10;
						l=l*myarm/desarm;
						if(l>18) l=18;
						if(l<3) l=3;
						if(flg=0)
						{
							flg=irand(5)+1;
							if(flg<=3) goto PREPARED;
						}
						while( (m=irand(myarm)+1) < myarm/2);
						for(j=0;j<m;j++)
							if(desarm>0)
								if(irand(20)<=l) desarm--;
PREPARED:
						if(desarm>0)
						{
							while( (m=irand(desarm)+1) < desarm/2);
							for(j=0;j<m;j++)
								if(myarm>0)
									if(irand(20)>=l) myarm--;

						}
					}
					if(myarm>0)
					{
						printf("You WON! %d ships hold planet #%d\n",myarm,despla);
						s->owner[despla]=mynum;
						s->ships[despla]=myarm;
						s->produ[despla]=s->produ[despla]-irand(4);
						if (s->produ[despla]<1) s->produ[despla]=1;
					}
					else
					{
						printf("You LOST! %d enemy ships hold planet #%d\n",desarm,despla);
						s->ships[despla]=desarm;
					}
					k=s->comnum;
					k++;
					if(k>9) k=0;
					s->combat[k][0]=mynum;
					s->combat[k][1]=despla;
					s->comnum=k;
					s->meslok--;
				}
				events[i]=0L;

			}
}

lock()
{
	int i;
	i=0;
	if(s->meslok<0 || s->meslok>20) s->meslok=0;
	while(s->meslok>0 && i<5)
	{
		i++;
		sleep(2);
	}
	s->meslok=0;
	s->meslok++;
}

getpub()
{
	char *ptr;
	int k,type,i;
	i=0;

	k=mymes;
	while(k!=s->mesnum)
	{
		k++;
		if(k > 4)
			k = 0;
		i=1;
		if(s->public[k][0] != mynum)
			if( s->public[k][1] == 'P' || s->public[k][1] == mynum || s->public[k][1]=='E')
			{
				type=s->public[k][1];
				if(type !='P' && type !='C' && type!='E') type = 'I';
				ptr = &s->public[k][2];
				if(vtmode) gotoxy(24,1);
				if (type == 'I')
					printf("\nPrivate message from %s]\n",s->names[s->public[k][0]]);
				else if(type=='P')
					printf("\nPublic message by %s]\n",s->names[s->public[k][0]]);
				else printf("\n");
				printf("%s\n\n",ptr);
			}
	}
	mymes=k;
	if(i && vtmode) prompt();
}

getcom()
{
	int k;
	int defpl,attak,own;
	k=mycom;
	while(k!=s->comnum)
	{
		k++;
		if(k>9) k=0;
		attak=s->combat[k][0];
		own=s->owner[s->combat[k][1]];
		defpl=s->combat[k][1];
		if(attak!=mynum)
		{
			if(vtmode) gotoxy(24,1);
			printf("\nInvasion attempt!\nAttacker:%s]\nPlanet #%d\n",
			    s->names[attak],defpl);
			if(own==-1) printf("It remains Neutral\n");
			if(own==attak) printf("%s] has gained the planet.\n",s->names[own]);
			if(own!=attak && own!=-1) printf("%s] retains control.\n",s->names[own]);
			if(own==mynum) printf("%d ships still hold your planet %d\n",
			    s->ships[defpl],defpl);
		}
	}
	mycom=k;
	prompt();
}


prange(i,j)
int i,j;
{
	float root();
	float truerange;
	long tmp1,tmp2,tmp3;
	int test;

	tmp1= ( s->xpos[i]-s->xpos[j]);
	tmp1=tmp1*tmp1;

	tmp2= ( s->ypos[i]-s->ypos[j]);
	tmp2= ( tmp2*tmp2);

	tmp3= ( s->zpos[i]-s->zpos[j]);
	tmp3=tmp3*tmp3;

	truerange=root(tmp1+tmp2+tmp3);
	test=truerange;
	if(test<truerange) test++;
	return(test);
}


compute()
{
	int k;
	int i,j;

	printf("\nPlanet #:");
	k=getnum(&i);
	printf("\nPlanet #:");
	k=getnum(&j);
	k=prange(i,j);

	printf("Range from %d to %d = %d\n",i,j,k);
}
float root(x)
long x;
{
	int z;
	float e;
	double y;
	if(x==0) return(0.0);
	y=x/2.0;
	z=0;
	e = 1.0;
	while((y*y != x) && e>0.001)
	{
		z++;
		y=(y+x/y)/2;
		e= y*y - x;
		if(e < 0.0) e = -e;
	}

	return(y);
}

flts()
{
	int i, fltflag;
	extern char *ctime();

	fltflag = 0;
	printf("\nFleet  Dest  Size  Arrival\n");
	for(i=0;i<99;i++)
		if(events[i]!=0L)
		{
			fltflag = 1;
			printf("  %2d    %2d   %4d  %5s\n",
			    i,stack[i][1],stack[i][0],sdate(events[i]));
		}
	if (fltflag == 0)
		printf("\nNo Fleets currently in flight.\n");
}

broadcast()
{
	char tmp[100];
	int ptr,k;
	ptr=2;
	printf("\nEnter Broadcast Message:\n>  ");
	while( (k=getchar())=='\n');
	while(ptr<100 && k!='\n')
	{
		tmp[ptr]=k;
		ptr++;
		tmp[ptr]='\0';
		k=getchar();
	}
	tmp[1]='P';
	tmp[0]=mynum;

	lock();
	k=s->mesnum+1;
	if(k>4)k=0;
	for(ptr=0;ptr<100;ptr++)
		s->public[k][ptr]=tmp[ptr];
	s->mesnum=k;
	s->meslok--;
	printf("Message Sent.\n");
}


prvmsg()
{
	char tmp[100];
	int ptr,k, player;

	ptr=2;
	printf("\nEnter Private Message:\n>  ");
	while( (k=getchar())=='\n');
	while(ptr<100 && k!='\n')
	{
		tmp[ptr]=k;
		ptr++;
		tmp[ptr]='\0';
		k=getchar();
	}
	if(toggle>-1) player=toggle;
	else {
		printf("\nMessage to (player NUMBER):");
		k=getnum(&ptr);
		player = ptr;
	}
	if ((player < 0) || (player > 10) || (s->names[player] == '\0'))
	{
		printf("Illegal player number.\n");
		return(0);
	}
	tmp[1]=player;
	tmp[0]=mynum;

	lock();
	k=s->mesnum+1;
	if(k>4)k=0;
	for(ptr=0;ptr<100;ptr++)
		s->public[k][ptr]=tmp[ptr];
	s->mesnum=k;
	s->meslok--;
	printf("\nMessage Sent to %s.\n",s->names[player]);
}

send()
{
	extern char *ctime();
	int i,j,k;
	int from,to;

	i = -1;
	while(i==-1)
	{
		printf("\nFrom world:");
		i=getnum(&from);
		if(from <0 || from >39) i = -1;
	}
	if (s->owner[from]!=mynum)
	{
		printf("\nYou do not own that world!\n");
		return(0);
	}
	j = -1;
	while(j==-1)
	{
		printf("To world:");
		j = getnum(&to);
		if(to<0 || to>39) j = -1;
	}

	for (k=0;events[k]!=0L && k<100; k++);

	if(k==100)
	{
		printf("\nSorry, too many fleets in the air!\n");
		return(0);
	}

	printf("Number to send (%d) :",s->ships[from]);
	i=getnum(&j);

	if(j>s->ships[from] || j<1)
	{
		printf("Invalid fleet size!");
		return(0);
	}

	if(s->owner[from]!=mynum)
	{
		printf("\nYou have lost control of that world!!!\n");
		return(0);

	}

	stack[k][0]=j;
	s->ships[from]=s->ships[from]-j;
	stack[k][1]=to;

	events[k]=time(0)+prange(from,to)*trav_time;
	printf("Fleet will arrive at %s\n",sdate(events[k]));
	pirate(k);
}

/*
 * kwicklist command
 */
kwick()
{
	int idx;

	printf("Kwick list:\n\n");
	for (idx = 0; idx <= 39; idx++)
	{
		if (s->owner[idx] == mynum)
			printf("#%2d Prod:%3d Ships:%5d\n",idx,s->produ[idx],s->ships[idx]);
	}
}

/*
 * fetch a number from the keyboard
 */

getnum(x)

int *x;
{
	char buffer[99];
	int flg,k,j;

	fflush(stdin);
	flg = -1;
	while(flg==-1)
	{
		flg = 0;
		j = 0;
		while( (k = getchar())!='\n' )
		{
			buffer[j]=k;
			if (k<'0' || k>'9') flg = -1;
			j++;
		}
		if(j==0) flg = -1;
		if(flg==-1) printf("\nA number is expected: ");
	}
	buffer[j]='\0';
	j=sscanf(buffer,"%d",x);
}

/*
 * getppn() -- fetch the user's PPN number 
 */
getppn(xx,yy)

int *xx, *yy;
{
	*xx = getgid();
	*yy = getuid();
}

clrgam()
{
	char text[100];

	if (myp==3 && mypn==0 || myp==102 && mypn==80){
		printf("\n(Cleargame): Are you sure:N\010");
		gets(text);
		if(!strcmp(text,"YES")){
			s->start = 0;
			setup();
		}
	}
}
/*
 * scan() - Displays map in nonvt mode.
 */
scan()
{
	int x,y;
	for(y=1;y<15;y++){
		for(x=1;x<16;x++){
			putchar(' ');
			if(s->scaner[x][y]<0) printf(". ");
			else printf("%2d",s->scaner[x][y]);
		}
		putchar('\n');
	}
}
/*
 * setit() - VTMODE Screen Setup.
 */
setit()
{
	int i;
	cls();
	for(i=1;i<15;i++){
		gotoxy(i,36);
		printf(".  .  .  .  .  .  .  .  .  .  .  .  .  .  .");
	}
	gotoxy(14,1);
	printf("===Command- =========[     ]=======");
}
/*
 * updtim() - Update gametime in VTMODE.
 */
updtim()
{
	gotoxy(14,23);
	printf("%s",sdate(0L));
}
cls()
{
	gotoxy(1,1);
	printf("\033[J");
}
gotoxy(x,y)
char x,y;
{
	putchar(ESC);
	printf("[%d;%dH",x,y);
	return(0);
}
setscr()
{
	putchar(ESC);
	printf("[15;24r");
}
nrmscr()
{
	putchar(ESC);
	printf("[0;24r");
}
/*
 * autosd() - Sends multiple fleets from all planets to a certain
 *            planet.
 */
autosd()
{
	int k,i,to,j,howmch;
	i = -1;
	while(i==-1){
		printf("Autosend to which planet: ");
		i=getnum(&to);
		if(to<0 || to>39) i = -1;
	}
	for(k=0;k<40;k++){
		if(s->owner[k]==mynum && k!=to){
			i = -1;
			while(i==-1){
				printf("#%2d - %4d: ",k,s->ships[k]);
				i = getnum(&howmch);
				if(howmch<0 || howmch>s->ships[k]) i = -1;
				else if(howmch>0){
					for (j=0;events[j]!=0L && j<100; j++);
					if(j==100){
						printf("\nSorry, too many fleets in the air!\n");
						return(0);
					}
					if(s->owner[k]!=mynum) printf("\nYou have lost control of world #%d\n",k);
					else {
						stack[j][0]=howmch;
						s->ships[k]=s->ships[k]-howmch;
						stack[j][1]=to;
						events[j]=time(0)+prange(k,to)*trav_time;
						printf("Fleet will arrive at %s\n",sdate(events[j]));
						pirate(j);
					}
				}
			}
		}
	}
}
blink()
{
	putchar(ESC);
	printf("[5m");
	return(0);
}
dim()
{
	putchar(ESC);
	printf("[2m");
	putchar(ESC);
	printf("[8m");
}
normal()
{
	putchar(ESC); 
	printf("[0m");
	return(0);
}
/*
 * prvtog() - Toggles who to send private messages to.
 */
prvtog()
{
	if(toggle>-1){
		toggle = -1;
		printf("Private message toggle is off.\n");
		return(0);
	}
	do {
		printf("Enter user number to private msg to: ");
		getnum(&toggle);
		if(toggle<0 || toggle>7) toggle = -1;
	} while(toggle<0);
	printf("Toggle set to send to %s]",s->names[toggle]);
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

	i = -1;
	while(i==-1)
	{
		printf("\nWarp from world:");
		i=getnum(&from);
		if(from <0 || from >39) i = -1;
	}
	if (s->owner[from]!=mynum)
	{
		printf("\nYou do not own that world!\n");
		return(0);
	}
	j = -1;
	while(j==-1)
	{
		printf("Warp to world:");
		j=getnum(&to);
		if(to<0 || to>39) j = -1;
	}

	for (k=0;events[k]!=0L && k<100; k++);

	if(k==100)
	{
		printf("\nSorry, too many fleets in the air!\n");
		return(0);
	}

	printf("Number to send (%d) :",s->ships[from]);
	i=getnum(&j);

	if(j>s->ships[from] || j<100)
	{
		printf("Invalid fleet size!");
		return(0);
	}

	if(s->owner[from]!=mynum)
	{
		printf("\nYou have lost control of that world!!!\n");
		return(0);

	}

	stack[k][0]=j-j/10;
	s->ships[from]=s->ships[from]-j;
	stack[k][1]=to;

	events[k]=time(0)+prange(from,to)*(trav_time/2);
	printf("Warp fleet will arrive at %s carrying %d ships\n",sdate(events[k]),j-j/10);
}
/*
 * plupd() - Lists player's planets in VTMODE
 */
plupd()
{
	int i,j,col,row;
	clrjnk();
	gotoxy(1,1);
	printf("Pl.  Pro  Shps   Pl.  Pro  Shps");
	col=1;
	row=1;
	j=0;
	for(i=0;i<40;i++){
		if(s->owner[i]==mynum){
			j++;
			if(j<25){
				row++;
				if(row>13){
					row=2;
					col=col+17;
				}
				gotoxy(row,col);
				printf("#%2d   %2d  %4d",i,s->produ[i],s->ships[i]);
			}
		}
	}
	gotoxy(14,34);
	if(j>24){
		blink();
		putchar('*');
		normal();
	}
	else putchar('=');
	prompt();
}
/*
 * clrjnk() - Clear out the left side box in VTMODE
 */
clrjnk()
{
	int i;
	for(i=0;i<14;i++){
		gotoxy(i,35);
		putchar(ESC);
		printf("[1K");
	}
}
/*
 * vtscan() - Shows the map in VTMODE
 */
vtscan()
{
	char x;
	int i;
	for(i=0;i<40;i++){
		gotoxy(s->ypos[i],36+(3*s->xpos[i])-3);
		if(s->owner[i]==mynum) normal();
		else if(s->owner[i]>-1) blink();
		printf("%2d",i);
		if(s->owner[i]==mynum || s->owner[i]>-1){
			if(s->owner[i]!=mynum) normal();
			else dim();
		}
	}
	normal();
	prompt();
}
/*
 * vtflts() - Shows the fleets in the left side box in VTMODE.
 */
vtflts()
{
	int i,j,col,row;
	clrjnk();
	gotoxy(1,1);
	printf("Dst  Size  Arriv  Dst  Size  Arriv");
	col=1;
	row=1;
	j=0;
	for(i=0;i<99;i++){
		if(events[i]!=0L){
			j++;
			if(j<25){
				row++;
				if(row>13){
					row=2;
					col=col+18;
				}
				gotoxy(row,col);
				printf(" %2d  %4d  %s",stack[i][1],stack[i][0],sdate(events[i]));
			}
		}
	}
	gotoxy(14,34);
	if(j>24){
		blink();
		putchar('*');
		normal();
	}
	else putchar('=');
	prompt();
}
/*
 * shtscn() - Short range map for 40 col users.
 */
shtscn()
{
	int pl,xstrt,xend,ystrt,yend,x,y;
	pl = -1;
	while(pl==-1){
		printf("Enter planet: ");
		getnum(&pl);
		if(pl<0 || pl>39) pl = -1;
	}
	xstrt=s->xpos[pl]-5;
	xend=s->xpos[pl]+5;
	if(xstrt<1) {
		xend=xend+(1-xstrt);
		xstrt=1;
	}
	if(xend>15) {
		xstrt=xstrt-(xend-15);
		xend=15;
	}
	ystrt=s->ypos[pl]-5;
	yend=s->ypos[pl]+5;
	if(ystrt<1){
		yend=yend+(1-ystrt);
		ystrt=1;
	}
	if(yend>14){
		ystrt=ystrt-(yend-14);
		yend=14;
	}
	for(y=ystrt;y<yend+1;y++){
		for(x=xstrt;x<xend+1;x++){
			putchar(' ');
			if(s->scaner[x][y]==-1) printf(". ");
			else printf("%2d",s->scaner[x][y]);
		}
		putchar('\n');
	}
}
/*
 * vtindiv() - Lists a player's planets in VTMODE.
 */
vtindiv()
{
	int i,j,b,col,row;
	j = -1;
	while(j==-1){
		printf("Summary of player: ");
		getnum(&j);
		if(j<0 || j>7) j = -1;
	}
	clrjnk();
	gotoxy(1,1);
	printf("Summary of %s]\n\n",s->names[j]);
	printf("Planet  Planet  Planet  Planet");
	col=3;
	row=3;
	b=0;
	for(i=0;i<40;i++){
		if(s->owner[i]==j){
			b++;
			if(b>10){
				b=1;
				row=3;
				col=col+8;
			}
			row++;
			gotoxy(row,col);
			printf("#%2d",i);
		}
	}
	prompt();
}
/*
 * pirate() - Random pirate fleets.
 */
pirate(i)
int i;
{
	int ispir;
	ispir=irand(40);
	if(pirtim!=0L) return(0);
	if(ispir!=25) return(0);
	while(pirsiz<20) pirsiz=irand(80);
	pirflt=i;
	pirtim=events[i]-20L;
}
/*
 * Exit the game
*/
leave()
{
	fcntl(0, F_SETFL, flags & ~O_NDELAY);
	ioctl(0, TCSETAF, &tsave);
	printf("Exiting\n");
	exit(0);
}
