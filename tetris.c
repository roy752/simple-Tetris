#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
	
	Root=(treepointer)malloc(sizeof(treetree));

	createRankList();
	while(!exit){
		clear();
		switch(menu()){
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_RECOMMEND: recommendedPlay(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	return 0;
}
void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			Root->field[j][i]=field[j][i]=0;
	Root->link=NULL;
	Root->lv=0;
	Root->path=0;
	Root->r=0;
	Root->x=0;
	Root->y=0;
	Root->Maxscore=0;
	Root->Nowscore=0;

	nextBlock[0]=rand()%7;
	nextBlock[1]=rand()%7;
	nextBlock[2]=rand()%7;
	nextBlock[3]=rand()%7;
	nextBlock[4]=rand()%7;
	blockRotate=0;
	TreeNumber = 0;
	lv=0;
	recommend(&Root);
//	recommend(field,0,0);
	if (DoRecom == 0)
	{
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
	}
	else
	{
		blockY = -1;
		blockRotate = maxblockRotate;
		blockX = maxblockX;
		blockY = maxblockY;
	}
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY, blockX, nextBlock[0], blockRotate);
//	DrawBlock(blockY,blockX,nextBlock[0],blockRotate,' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	DrawBox(10,WIDTH+10,4,8); //

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(17,WIDTH+10);
	printw("SCORE");
	DrawBox(18,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(19,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}

		move(11+i,WIDTH+13);
		for ( j = 0 ; j < 4 ; j++ ) {
		  if ( block[nextBlock[2]][0][i][j] == 1 ){
			attron(A_REVERSE);
			printw(" ");
			attroff(A_REVERSE);
		  }
		  else printw(" ");
		}
	}
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){
  DrawField();
  DrawShadow(y, x, blockID, blockRotate);
  DrawBlock(Root->link->y,Root->link->x,blockID,Root->link->r,'R');
  DrawRecommend(y, x, blockID, blockRotate);
  DrawBlock(y, x, blockID, blockRotate, ' ');
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	time_t start, stop;
	start = time(NULL);
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");

	double duration;
	printw("The total memory for the list : ");
	printw("%ld Bytes\n", ss);
	stop = time(NULL);
	duration = (double)(stop - start);
	printw("The Total time for tetris : %lf seconds\n", duration);
	printw("space efficiency = %lf\n", (double)score / ss);
	printw("time efficiency = %lf", (double)score / duration);
	refresh();
	getch();
	newRank(score);
}
char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j, ret = 1;
	for(i = 0; i < 4; i++){
	  for(j = 0; j < 4; j++){
		if(block[currentBlock][blockRotate][i][j] == 1){
		  if(blockY + i >= HEIGHT)
			return 0;
		  if(blockX + j < 0)
			return 0;
		  if(blockX + j >= WIDTH)
			return 0;
		  if (f[blockY + i][blockX + j])
			  return 0;
		}
	  }
	}
	return 1;
}
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int blk = currentBlock, rot = blockRotate, y = blockY, x = blockX;
	switch(command){
	  case KEY_UP:
		rot = (rot + 3) % 4;
		break;
	  case KEY_DOWN:
		y--;
		break;
	  case KEY_LEFT:
		x++;
		break;
	  case KEY_RIGHT:
		x--;
		break;
	}

	for(i = 0; i < 4; i++)
	  for(j = 0; j < 4; j++){
		if(block[blk][rot][i][j] == 1){
		  if(i + y >= 0){
			move(i + y + 1, j + x + 1);
			printw(".");
		  }
		}
	  }
//	DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
	move(HEIGHT, WIDTH + 10);
}

void BlockDown(int sig){
  if(CheckToMove(field, nextBlock[0], blockRotate, blockY+1, blockX)){
	blockY++;
	DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	timed_out = 0;
  }
  else  if(blockY == -1){
	DrawField();
	gameOver = 1;
	DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	timed_out = 0;
  }
  else{
	score += AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
	score += DeleteLine(field);
	PrintScore(score);
	nextBlock[0] = nextBlock[1];
	nextBlock[1] = nextBlock[2];
	nextBlock[2] = nextBlock[3];
	nextBlock[3] = nextBlock[4];
	nextBlock[4]=rand()%7;
	DrawNextBlock(nextBlock);
	
	TreeNumber = 0;
	lv=0;
	int i,j;
	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			Root->field[j][i]=field[j][i];
	recommend(&Root);
	if (DoRecom == 0)
	{
		blockRotate = 0;
		blockY = -1;
		blockX = WIDTH / 2 - 2;
	}
	else
	{
		blockY = -1;
		blockRotate= maxblockRotate;
		blockX= maxblockX;
		blockY = maxblockY;
	}
	DrawField();
	DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
	timed_out = 0;
  }
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	int i, j;
	int touched = 0;
	for(i = 0; i < 4; i++){
	  for(j = 0; j < 4; j++){
		if(block[currentBlock][blockRotate][i][j] == 1){
			f[blockY + i][blockX + j] = 1;
			if(f[blockY + i + 1][blockX + j] == 1)
			  touched ++;
			else if(blockY + i + 1 == HEIGHT)
			  touched ++;
		}
	  }
	}
	return touched * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
  int i, j, k;
  int fillFlag;
  int count = 0;
  for(j = 1; j < HEIGHT; j++){   ////j=1
	fillFlag = 1;
	for(i = 0; i < WIDTH; i++){
	  if(f[j][i] == 0){
		fillFlag = 0;
		break;
	  }
	}
	if(fillFlag == 1){
	  count++;
	  for(k = 0; k < WIDTH; k++)
		f[j][k] = 0;
	  for(i = j; i > 0 ; i--)
		for(k = 0; k < WIDTH; k++)
		  f[i][k] = f[i - 1][k];
	}
  }
  return 100 * count * count;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
  while(y < HEIGHT && CheckToMove(field, nextBlock[0], blockRotate, y+1, x))
	y++;
  DrawBlock(y, x, blockID, blockRotate, '/');
}

void createRankList(){
	// user code
	FILE *in = fopen("rank.txt","r");
	if(in == NULL)
		return;
   fscanf(in, "%d", &ranks);

   int i;
   Node *newNode = NULL;
   for(i=0;i<ranks;i++)
   {
	if(newNode){
	  newNode -> link = (Node *) malloc (sizeof(Node));
	  newNode = newNode -> link;
	  newNode -> link = NULL;
	  fscanf(in, "%s %d", newNode -> name, &(newNode -> score));
	}
	else{
	  newNode = (Node *) malloc (sizeof(Node));
	  newNode -> link = NULL;
	  fscanf(in, "%s %d", newNode -> name, &(newNode -> score));
	  root = newNode;
	}
  }
}

void rank(){
	// user code
	Range r;
  char name[NAMELEN];
  //int del;
  Node *ptr;
  int count;

  r.from = 0;
  r.to = 0;
  clear();
  printw("1. list ranks from X to Y\n");
  printw("2. list ranks by a specific name\n");
  printw("3. delete a specific rank\n");
  switch(wgetch(stdscr)){
	case '1' :
	  echo();
	  printw("X: ");
	  scanw("%d", &r.from);
	  printw("Y: ");
	  scanw("%d", &r.to);
	  noecho();
	  printw("       name       |   score\n");
	  printw("------------------------------\n");


	  if(r.from == 0)r.from = 1;
	  if(r.to == 0)r.to = ranks;
	  if(r.from > r.to || r.from < 1 || r.to > ranks){
		printw("search failure: no rank in the list\n");
		break;
	  }
  
	  count = 0;
	  for(ptr = root; ptr; ptr = ptr -> link){
		count ++;
		if(count >= r.from && count <= r.to)
		  printw(" %-17s| %d\n", ptr -> name, ptr -> score);
		else if(count > r.to)
		  break;
	  }
	  break;
	case '2' :
		clear();
		echo();
		printw("Your Name: ");
		scanw("%s", Name);
		noecho();
		printw("       name       |   score\n");
		printw("------------------------------\n");
		count = 0;
		for (ptr = root; ptr; ptr = ptr->link) {
			if (strcmp(ptr->name, Name) == 0)
			{
				printw(" %-17s| %d\n", ptr->name, ptr->score);
				count++;
			}
		}
		if (!count)
			printw("\nsearch failure: no name in the list\n");
	  break;
	case '3' :
		echo();
		printw("Input the Rank: ");
		scanw("%d", &deletenumber);
		noecho();
		if (deletenumber <= 0 || deletenumber > ranks)
		{
			printw("\nsearch failure: the rank not in the list\n");
			break;
		}
		count = 1;
		if (deletenumber == 1)
			root = root->link;
		else
		{
			for (ptr = root; ptr; ptr = ptr->link)
			{
				count++;
				if (count == deletenumber)
				{
					ptr->link = ptr->link->link;
					ranks--;
					break;
				}
			}
		}
		ranks--;
		printw("result : the rank deleted\n");
		writeRankFile();
	  break;
	default : break;
  }

  getch();
}

void writeRankFile(){
	// user code
	Node *ptr;
	FILE *out = fopen("rank.txt","w");
	fprintf(out, "%d\n", ranks);
	for(ptr = root; ptr; ptr = ptr -> link)
	  fprintf(out, "%s %d\n", ptr -> name, ptr -> score);
	fclose(out);
}

void newRank(int score){
  Node *neww, *prev, *current;
  clear();
  neww = (Node *) malloc (sizeof(Node));
  neww -> score = score;
  printw("your name: ");
  echo();
  getstr(neww -> name);
  noecho();
  ranks++;
  //add(ne);

  if(root == NULL){
	root = neww;
	neww -> link = NULL;
	return;
  }
  
  for(prev = root, current = root; current; current = current -> link){
	if(current -> score <= score){
	  if(current == root){
		neww -> link = current;
		root = neww;
	  }
	  else{
		prev -> link = neww;
		neww -> link = current;
	  }
	  break;
	}
	prev = current;
  }
  if(current == NULL){
	prev -> link = neww;
	neww -> link = NULL;
  }
  writeRankFile();
}

void DrawRecommend(int y, int x, int blockID,int blockRotate){
	// user code
	int i,j;
}
void modified_recommend()
{
	recommend(&Root);
}
int recommend(treepointer *roots){	//lv는 VISIBLE_BLOCKS
	int r,x,y,max=0,temm=0,temmax=0;
	treepointer Root=*roots;
	int i,j;
	if(Root->lv==VISIBLE_BLOCK)
		return 0;
	// user code
	if (Root->lv == VISIBLE_BLOCK - 1)
	{
		for (x = 0; x < 10; x++)
			if (Root->field[x][0])
				return -100;
	}
	for(r=0;r<4;r++)		//rotate
	{
		for(x=0;x<10;x++)			//where
		{
			y=-1;
			if(CheckToMove(Root->field,nextBlock[Root->lv],r,0,x)==0)
				continue;
			while(CheckToMove(Root->field, nextBlock[Root->lv], r, y+1, x))
				y++;

			treepointer ptr = NULL;
			ptr = (treepointer)malloc(sizeof(treetree));
			ss += sizeof(ptr);
			for(i=0;i<HEIGHT;i++)
				for(j=0;j<WIDTH;j++)
					ptr->field[i][j]=Root->field[i][j];
			ptr->r=r;
			ptr->x=x;
			ptr->y=y;
			ptr->lv=(Root->lv)+1;
			ptr->Nowscore = AddBlockToField(ptr->field,nextBlock[Root->lv],r,y,x);
			ptr->Nowscore = AddBlockToField(ptr->field,nextBlock[Root->lv],r,y,x);
			temm = DeleteLine(ptr->field);
			ptr->Nowscore += temm;
			ptr->Nowscore+=recommend(&ptr);
			
			if(temm >=200 && temm > temmax)
			{
				temmax = temm;
				max = ptr->Nowscore;
				Root->link = ptr;
				if (Root->lv == 0)
				{
					maxblockRotate = r;
					maxblockX = x;
					maxblockY = y;
				}
			}
			else if (max < ptr->Nowscore)
			{
				max = ptr->Nowscore;
				Root->link = ptr;
				if (Root->lv == 0)
				{
					maxblockRotate = r;
					maxblockX = x;
					maxblockY = y;
				}
			}
		}
	}
	if (Root->lv == 0 && max <= 0)
		gameOver = 1;
	return max;
}
void DrawRecom()
{
	
}
void recommendedPlay(){
	// user code
	DoRecom = 1;
	play();
}

//meaningless typo
//meaningless typoCheck