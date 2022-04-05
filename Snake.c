//include for keyboard monitoring
#include "windows.h"
#include <ansi_c.h>
#include <ansi_c.h>
#include "toolbox.h"
#include "grandtest.h"
#include <cvirte.h>		
#include <userint.h>
#include "Snake.h"
#include <stdio.h>
#include <stdlib.h>
#include <cvintwrk.h>
#include <utility.h>
#include <stdbool.h>
#define bodyL 25
#define bodyW 25
#define R 12.5
//include for sound
#include "bass.h"

static int panelHandle4,panelHandle3, panelHandle2, panelHandle;

//variables for sound 
HSTREAM Crash_SND;   
HSTREAM MusicM_SND;
HSTREAM MusicG_SND;
HSTREAM Bite_SND;
			

//variables for keyboard monitoring
int Callback_Data;	
int Posting_Handle; 
int Keys_Down[256]={0}; 


//Receives information from windows regarding key presses
int CVICALLBACK Key_DownFunc(int panel, int message, unsigned int* wParam, unsigned int* lParam, void* callbackData)
{
	unsigned int Key = *wParam;
	Keys_Down[Key] = 1;
	return 0;
}

//Receives information from windows regarding key releases
int CVICALLBACK Key_UpFunc(int panel, int message, unsigned int* wParam, unsigned int* lParam, void* callbackData)
{
	unsigned int Key = *wParam;
	Keys_Down[Key] = 0;
	return 0;
}

//asking windows to send keyboard press and release events to our software 
//Specifing 'Key_DownFunc' and 'Key_UpFunc' as the functions to call
void Connect_To_Windows_Events(void)
{
	InstallWinMsgCallback(panelHandle2,WM_KEYDOWN,Key_DownFunc,VAL_MODE_IN_QUEUE,&Callback_Data,&Posting_Handle);
	InstallWinMsgCallback(panelHandle2,WM_KEYUP,Key_UpFunc,VAL_MODE_IN_QUEUE,&Callback_Data,&Posting_Handle);
}					

void InitMP3s(void)
{
	BASS_Init( -1,44100, 0,0,NULL); 
	Crash_SND = BASS_StreamCreateFile(FALSE,"crash.mp3",0,0,0);
	MusicM_SND = BASS_StreamCreateFile(FALSE,"Intro.mp3",0,0,0); 
	MusicG_SND = BASS_StreamCreateFile(FALSE,"happy.mp3",0,0,0); 
	Bite_SND = BASS_StreamCreateFile(FALSE,"bite.wav",0,0,0);  
	
}

void ReleaseMP3s(void)
{
	 BASS_StreamFree(Crash_SND);
	 BASS_StreamFree(MusicM_SND);
	 BASS_StreamFree(MusicG_SND);
     BASS_StreamFree(Bite_SND);
}


typedef struct 
{
	//struct to make structure of position for image 
	int top;	// y location
	int left;   // x location
	int width;  // width of pic
	int height;  // hight of pic
	double offset;   
	double vel;   //velocity var
	int dir // 1 -up , 2- right,3-  down , 4- left clockwise 
} coordinets;


typedef struct 
{
	//struct to make structure for players 
	int score;	
	char name[25];  
	int diff;
	int position;

	
} player;

player players[10];
coordinets MainCanvas,GameCanvas,Head,food,posArr[1000],oldposArr[1000]; 


// Variables assinment
int mainBackg,GameBackg;
int snakeHead_r,snakeHead_l,snakeHead_u,snakeHead_d,crashSmoke,crashBoom,num1,num2,num3;
int foodArr[5];   //peach=1 , cherry =2 , ,strawberry =3, apple =4
int foodtodraw;
double dt;  
int counter = 0,red=237,green=168,blue=119;   //deafult values for colors of snake and counter
int score=0,nextscore,diff,wallsoff=0,growboost=0;
double speed = 0;
int nextfood =0,dead=0;
float vol;
char name[20];
char filename[300]="leadboard.dat";


void initialize()
{  
int i,j;	 
	MainCanvas.top=0;
	MainCanvas.left=0;
	MainCanvas.height=800;
	MainCanvas.width=1380;
	
	GameCanvas.top=0;
	GameCanvas.left=0;
	GameCanvas.height=650;
	GameCanvas.width=1200;
	
	for (i=1;i<1000;i++)
	{
		posArr[i].height=25;
		posArr[i].width=25;
		
		
	}

	
	RecallPanelState (panelHandle3, filename, 0);
	
		//load images to memory
	GetBitmapFromFile ("Pics\\wp2409705.jpg", &mainBackg);
	GetBitmapFromFile ("Pics\\background.jpg", &GameBackg);
	GetBitmapFromFile("Assets\\Head_right.png", &snakeHead_r);	
	GetBitmapFromFile("Assets\\Head_left.png", &snakeHead_l);	    
	GetBitmapFromFile("Assets\\Head_up.png", &snakeHead_u);	    
	GetBitmapFromFile("Assets\\Head_down.png", &snakeHead_d);	    
	GetBitmapFromFile("Assets\\peach.png", &foodArr[0]);	    
	GetBitmapFromFile("Assets\\cherry.png", &foodArr[1]);	    
	GetBitmapFromFile("Assets\\strawberry.png", &foodArr[2]);
	GetBitmapFromFile("Assets\\apple.png", &foodArr[3]);
	GetBitmapFromFile("Assets\\plum.png", &foodArr[4]);
	GetBitmapFromFile("Assets\\BoomText.png", &crashBoom);
	GetBitmapFromFile("Assets\\SmokeImg.png", &crashSmoke);
	
	GetBitmapFromFile("Assets\\3num.png", &num3);
	GetBitmapFromFile("Assets\\2num.png", &num2);
	GetBitmapFromFile("Assets\\1num.png", &num1);
	


	
	//set panel size and position:
	SetPanelSize (panelHandle, 800, 1380); 
	SetPanelSize (panelHandle2, 650, 1330);
	SetPanelSize (panelHandle3, 500, 500);
	SetPanelPos (panelHandle, 0, 0);
	SetPanelPos (panelHandle2, 100, 100);
	SetPanelPos (panelHandle3, 0, 0);       
	
	//  set canvas sizes and position 
	SetCtrlAttribute (panelHandle, PANEL_MAINCANVAS, ATTR_LEFT, 0);
	SetCtrlAttribute (panelHandle, PANEL_MAINCANVAS, ATTR_TOP, 0);
	SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_LEFT, 0);
	SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_TOP, 0);
	
	SetCtrlAttribute (panelHandle, PANEL_MAINCANVAS, ATTR_HEIGHT, 800);
	SetCtrlAttribute (panelHandle, PANEL_MAINCANVAS, ATTR_WIDTH, 1380);
	SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_HEIGHT, 650);
	SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_WIDTH, 1200); 
	
	//drawing main panel background
	CanvasDrawBitmap (panelHandle, PANEL_MAINCANVAS, mainBackg, VAL_ENTIRE_OBJECT, MakeRect (MainCanvas.top, MainCanvas.left, MainCanvas.height, MainCanvas.width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));


	//SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_PEN_COLOR, MakeColor(237, 168, 119));	
	SetCtrlVal (panelHandle, PANEL_NUMERICRED, red);
	SetCtrlAttribute (panelHandle, PANEL_NUMERICRED, ATTR_FILL_COLOR, MakeColor(red,0,0));
	SetCtrlVal (panelHandle, PANEL_NUMERICGRN, green);
	SetCtrlAttribute (panelHandle, PANEL_NUMERICGRN, ATTR_FILL_COLOR, MakeColor(0,green,0));
	SetCtrlVal (panelHandle, PANEL_NUMERICBLU, blue);
	SetCtrlAttribute (panelHandle, PANEL_NUMERICBLU, ATTR_FILL_COLOR, MakeColor(0,0,blue));
	SetCtrlAttribute (panelHandle, PANEL_COLORSCHEM, ATTR_FRAME_COLOR, MakeColor(red,green,blue));
	SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_PEN_FILL_COLOR, MakeColor(red, green, blue));

	



	
	SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_BOLD, 1);
	SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_FONT, VAL_MENU_FONT);
	SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_POINT_SIZE, 15);
	SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_CELL_JUSTIFY,   VAL_CENTER_CENTER_JUSTIFIED);
	SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,1,3), ATTR_TEXT_BGCOLOR,  VAL_DK_GREEN);
	 for (j=2;j<12;j++)
			{
			
				
				SetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint(2,j), players[j-2].name);
				SetTableCellVal (panelHandle3, PANEL_3_TABLE,MakePoint(3,j) , players[j-2].score);
				//players[j-1].diff=diff;
				



			}


}
void terminate()
{
	//Free rotaion image data (should be done once when the image will no longer be used)
	DiscardBitmap(snakeHead_r);
	DiscardBitmap(snakeHead_l);
	DiscardBitmap(snakeHead_u);     
	DiscardBitmap(snakeHead_d); 
	DiscardBitmap(crashSmoke);     
	DiscardBitmap(crashBoom);
	DiscardBitmap(mainBackg);
	DiscardBitmap(GameBackg);
	DiscardBitmap(foodArr[0]);
	DiscardBitmap(foodArr[1]);
	DiscardBitmap(foodArr[2]); 
	DiscardBitmap(foodArr[3]);

	DiscardBitmap(num3);
	DiscardBitmap(num2);
	DiscardBitmap(num1); 



	
	

}




int main (int argc, char *argv[])
{
	if (InitCVIRTE (0, argv, 0) == 0)
		return -1;	/* out of memory */
	if ((panelHandle4 = LoadPanel (0, "Snake.uir", PANEL_4)) < 0)
		return -1;
	if ((panelHandle3 = LoadPanel (0, "Snake.uir", PANEL_3)) < 0)
		return -1;
	if ((panelHandle2 = LoadPanel (0, "Snake.uir", PANEL_2)) < 0)
		return -1;
	if ((panelHandle = LoadPanel (0, "Snake.uir", PANEL)) < 0)
		return -1;
		
	srand (time(0));
	InitMP3s();
	Connect_To_Windows_Events();  //asking windows to send keyboard press and release events to our software
	initialize();

	DisplayPanel (panelHandle);
	BASS_ChannelPlay(MusicM_SND,TRUE);            
	RunUserInterface ();
	terminate();
	BASS_ChannelStop(MusicM_SND);
	ReleaseMP3s();
	DiscardPanel (panelHandle3);
	DiscardPanel (panelHandle2);
	DiscardPanel (panelHandle);
	return 0;
}

void draw_3_2_1()
{

 		
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 240, 170));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			  			Delay(0.7);
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);   
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 200, 150));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			  			Delay(0.1);
			
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 170, 120));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
						
						
						  CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 130, 90));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
						
				CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 100, 70));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 80, 50));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);
			
		
						
						CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
						CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num3, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 50, 30));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			  			Delay(0.10);

						
			//**********************************************			
						

						
						
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 240, 170));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			  			Delay(0.7);
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 200, 150));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);			
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 170, 120));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
						
						
							
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 130, 90));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
							CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
							CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 100, 70));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
						

			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);  
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 80, 50));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);
						
						CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
						CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num2, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 50, 30));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);				
						
						
		//********************************************************************************************				
						
						
						
						
						
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 240, 170));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			  			Delay(0.7);
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS); 
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 200, 150));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);			
			
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 170, 120));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
						
						
							
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 130, 90));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);						
							CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
							CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 100, 70));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);
			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 80, 50));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);
		
						
						CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
						CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
			CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, num1, VAL_ENTIRE_OBJECT, MakeRect (240, 560, 50, 30));
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
Delay(0.1);


}



void draw_start()	  //this function draws the intital snake position for beginning of game
{	

	posArr[0].left=550;
	posArr[0].top=400;
	posArr[0].height=25;
	posArr[0].width=25;
	posArr[0].dir = 2;  //starting right    
	SetCtrlVal (panelHandle2, PANEL_2_SCORENUMERIC, score);
	CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
	CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,snakeHead_r, VAL_ENTIRE_OBJECT, MakeRect (posArr[0].top, posArr[0].left, posArr[0].height,posArr[0].width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));
	CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
}

void grow()		  //fuunction to make snake grow 
{  
	switch (posArr[counter-1].dir)
{
	case 1:
		posArr[counter].top=posArr[counter-1].top + 25; 
		posArr[counter].left=posArr[counter-1].left;
		posArr[counter].dir=1;
		break;
		
		case 3:
		posArr[counter].top=posArr[counter-1].top - 25; 
		posArr[counter].left=posArr[counter-1].left;
		posArr[counter].dir=3;
		break;
		
		case 2:
		posArr[counter].top=posArr[counter-1].top; 
		posArr[counter].left=posArr[counter-1].left-25;
		posArr[counter].dir=2;	
		break;
		
		case 4:
		posArr[counter].top=posArr[counter-1].top; 
		posArr[counter].left=posArr[counter-1].left+25;
		posArr[counter].dir=4;	
		break;
		}

	}
  
void Up()  				   // 'w' pressed

{  int i; 
	
	if (posArr[0].dir == 4)   //from left
	{  
		
		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_u, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width)); 
		for (i=1;i<=counter;i++)
		{
			
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);


		

	}
	
	if (posArr[0].dir == 2)  //from right
	{   
	
	
		
		
	
		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_u, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top, posArr[0].left, posArr[0].height, posArr[0].width));
	
		 for (i=1;i<=counter;i++)
		{
		
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		
		
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
	}
		posArr[0].dir =1;

}

void Right()				 // 'D' pressed
{	
	int i;
	if (posArr[0].dir == 1) 
	{
		
		

		
	
		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_r, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
		
		 for (i=1;i<=counter;i++)
		{   
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);


	}
	
	if (posArr[0].dir == 3)
	{
		
		

		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_r, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
		
	
		 for (i=1;i<=counter;i++)
		{
			
			
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
	}
   		posArr[0].dir = 2;

}
							 
void Left()				   //'A' pressed

{  	 int i;

	if (posArr[0].dir==1)
	{
		
		
	
		
		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_l, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width)); 
		 
		for (i=1;i<=counter;i++)
		{
			
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
		}
		
	if (posArr[0].dir==3)
	{
		
		
		
		
		CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
		CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));

		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_l, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
		

		 for (i=1;i<=counter;i++)
		{
				
			
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
		CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
		CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
		}
		
			posArr[0].dir=4;
	
	
		
}

void Down()					 //'S' presses
{
	int i;
	if (posArr[0].dir == 2)
	{
	
			
	
	
	CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
	CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_d, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
	
	for (i=1;i<=counter;i++)
		{
				
		
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
	
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
	CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
	}
	
	if (posArr[0].dir == 4)
	{

			
		
	
	
	CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
	CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_d, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width)); 
	for (i=1;i<=counter;i++)
		{
				
			
			CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
		}
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));     
	CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);


	}
	posArr[0].dir = 3;

	
}
void clac_snake_pos()		  //calc what button pressed
{
	
	if ((Keys_Down['A']==1) && (Keys_Down['S']==0) && (Keys_Down['W']==0) && (Keys_Down['D']==0) && (posArr[0].dir!=2))
		
		
			{
				Left(); 

			} 
	if ((Keys_Down['W']==1) && (Keys_Down['A']==0) && (Keys_Down['D']==0)&&(Keys_Down['S']==0) && (posArr[0].dir!=3))

			{
				Up();
							
			}  
	if ((Keys_Down['D']==1) && (Keys_Down['W']==0) && (Keys_Down['S']==0)&& (Keys_Down['A']==0)&& (posArr[0].dir!=4))  
			{
				Right();

			}
	if ((Keys_Down['S']==1) && (Keys_Down['A']==0) && (Keys_Down['D']==0)&& (Keys_Down['W']==0) && (posArr[0].dir!=1))  
			{
				Down();

			}   
			
	
	
	
}
		
void move_snake()			   //draw snake

{
	int i;
		for (i=0;i<=counter;i++)
		{
			oldposArr[i].top=posArr[i].top;
			oldposArr[i].left=posArr[i].left;
			oldposArr[i].dir=posArr[i].dir;

		}
	
	CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);            
	CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
	


	switch(posArr[0].dir)
	{
	case 1: //head is pos up
 				 
	posArr[0].top = posArr[0].top - speed;

	
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_u, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
	posArr[0].dir = 1;
	
	break;

	case 2: //head is pos right
	posArr[0].left = posArr[0].left + speed; 


	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_r, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
	posArr[0].dir = 2;

	
	break;
	
	case 3: //head is pos down
	posArr[0].top = posArr[0].top + speed; 
	
	
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_d, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
		posArr[0].dir = 3;

	
	break;
	
	case 4: //head is pos left
	posArr[0].left=posArr[0].left - speed;


	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, snakeHead_l, VAL_ENTIRE_OBJECT, MakeRect(posArr[0].top,posArr[0].left, posArr[0].height, posArr[0].width));
	posArr[0].dir = 4;

	
	break;
	}	
	 
	
	
	
		 if (counter>0)
		 {
		
			for (i=1;i<counter;i++)
		
			{
		
			if (posArr[i].dir == oldposArr[i-1].dir)	 //if the line is in same direction as previous link??
				{
					switch (oldposArr[i-1].dir)
						
					{
						case 1:
								posArr[i].top=oldposArr[i-1].top; 
								posArr[i].left=oldposArr[i-1].left; 
			   					CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
							   break;
							   
						case 2:
							
							posArr[i].left=oldposArr[i-1].left; 
							posArr[i].top=oldposArr[i-1].top; 
			   				CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
							break;
						
							
							
						case 3:
							posArr[i].top=oldposArr[i-1].top; 
							posArr[i].left=oldposArr[i-1].left; 
			   				CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
							break;
							
							
						case 4:
							
								  posArr[i].left=oldposArr[i-1].left; 
								posArr[i].top=oldposArr[i-1].top; 
			   					CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);
							break;
							
				 	//posArr[i].top=oldposArr[i-1].top; 
				//	posArr[i].left=oldposArr[i-1].left; 
			   	//	CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);

				}
				}
						   
			else
				{
				

				posArr[i].top=oldposArr[i-1].top; 
				posArr[i].left=oldposArr[i-1].left; 
				posArr[i].dir=oldposArr[i-1].dir; 
				CanvasDrawRect (panelHandle2, PANEL_2_GAMECANVAS, MakeRect(posArr[i].top,posArr[i].left,posArr[i].height,posArr[i].width), VAL_DRAW_FRAME_AND_INTERIOR);

				}
			}
		 }
			
		
	  


		 
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,foodtodraw, VAL_ENTIRE_OBJECT, MakeRect (food.top, food.left, 25,25));
	CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);


	 }

void calc_food()				  //draw new food


{  	  
	int foodnum;
	
		
			foodnum = rand()%5; 
			foodtodraw = foodArr[foodnum];
			switch(foodnum)
			{
				case 0:
					nextscore = 20;
						break;
					
				case 1:
					nextscore = 10;
						break;
					
				case 2:
					nextscore = 10;
						break;	
					
				case 3:
					nextscore =1; 
						break;
				case 4: 
					nextscore= 25;
					break;
					
			}
			food.top = rand()%600;
			food.left = rand()%1100;
			nextfood=0;
			
			
			
}




void calc_eating()				  //calculates if food is eaten
{
	if (posArr[0].dir==1)
	{
			if (((posArr[0].top-food.top)<25) && (abs(posArr[0].left-food.left)<25))
		
				{
					nextfood =1;
					counter++; 
	
				}
	}
	
		if (posArr[0].dir==3)
	{
			if (((food.top - posArr[0].top)<20) && (abs(posArr[0].left-food.left)<25))

				{
					nextfood =1;
					counter++; 

	
				}
	}
	
	
		if (posArr[0].dir==2)
	{
			if ((abs(posArr[0].top -food.top)<25) && ((food.left-posArr[0].left)<25))

				{
					nextfood =1;
					counter++; 

	
				}
	}
	
		if (posArr[0].dir==4)
	{
			if ((abs(posArr[0].top -food.top)<25) && ((posArr[0].left-food.left)<25))

				{
					nextfood =1;
					counter++; 
	
				}
	}
	
	if ((nextfood == 1) && (foodtodraw==foodArr[2]))
	{
		wallsoff =1;
		SetCtrlVal (panelHandle2, PANEL_2_STRING_Walls, "Yes");
	}
	
	if ((nextfood == 1) && (foodtodraw==foodArr[4]))
	{
		growboost=1;
		counter=counter*2;
		grow();
		
	}
	

		
}



								 //calculates if snake is dead
void is_dead()

{   int i;
	if(wallsoff ==0)
	{
		if (posArr[0].dir == 1)   //direction up
		{
			if (posArr[0].top <= 0 )
				{
				dead = 1;
				}
		
				
			else
			{
				
			for (i=1;i<=counter;i++)
				
			{
				
				if   ((posArr[i].dir!=1)&& (abs(posArr[0].left-posArr[i].left)<25))
					
					
				{
					if ((posArr[0].top<=(posArr[i].top+25))&&(posArr[0].top>posArr[i].top))   
					{
						dead = 1;
					}
				}
			}
			}
		}
		//*********************************************************************************************
		if (posArr[0].dir == 3)     //direction down
		{
		
				 if  ((posArr[0].top + 25) >= 650)
		
				 {
					dead = 1;
				 }
				 
				 else
				 {
		 
				 for (i=1;i<=counter;i++)
					 
				{
				if   ((posArr[i].dir!=3)&& (abs(posArr[0].left-posArr[i].left)<25))           
				{
						if (((posArr[0].top+25)>=posArr[i].top )&&(posArr[0].top<posArr[i].top))   
						{
						dead = 1;
						}
				
			}
		
		 }
		}
		
		}	
		
	//*********************************************************************************************
		if (posArr[0].dir == 2)   //direction right  
		{
				if ((posArr[0].left + 25) >= 1200 )	
				{
		
					dead =1;
				}
				
			   else
			   {
				for (i=1;i<=counter;i++)
					
					
					
				{
					if   ((posArr[i].dir!=2)&& (abs(posArr[0].top-posArr[i].top)<25))    
						
					{
						if (((posArr[0].left+25)>=posArr[i].left )&&(posArr[0].left<posArr[i].left))
						
						{
								dead = 1;
						}
					}
				}
 
		}
	}
		
		//********************************************************************************************
	
		
		
		if (posArr[0].dir == 4)  //direction left                 	
		{
	
			if(posArr[0].left <= 0 )
			
			{
					dead = 1;
			}			   //take the col coordinates
			
			else
			{
	
			 for (i=1;i<=counter;i++)
				
			 {
				 if   ((posArr[i].dir!=4)&& (abs(posArr[0].top-posArr[i].top)<25))    
				 {
						if ((posArr[0].left<=posArr[i].left+25 )&&(posArr[0].left+25>posArr[i].left+25))   
						
							{
									dead = 1;
							}
				 }
			  }  
	
			}
		}
	
	}
	
	if(wallsoff ==1) 
		
			{   
				for (i=0;i<=counter;i++)
				
				{
					if (posArr[i].dir == 1)
					{

							if  ((posArr[0].top<=(posArr[i].top+25))&&(posArr[i].dir!=1)&& ((abs(posArr[0].left-posArr[i].left)<25))&& (i!=0)&&(posArr[0].top>posArr[i].top) )
								{
											dead = 1;
								}
					
					
							if (posArr[i].top <= 0 )
							{
								posArr[i].top = 650;
							}
					}
					
					//************************************
	
			 		if (posArr[i].dir == 3) 
				{
			
							if  ((posArr[i].top + 25) >= 650 )
							{
								posArr[i].top = 0;
							}
							
							if (((posArr[0].top+25)>=posArr[i].top )&&(posArr[i].dir!=3)&& (abs(posArr[0].left-posArr[i].left)<25)&& (i!=0)&&(posArr[0].top<posArr[i].top))
							{
								dead = 1;
							}
				
				
				
				}
			//**************************************************
	
	
			 		if (posArr[i].dir == 2)   
					{
						if((posArr[i].left + 25) >= 1200 )	
						{
								posArr[i].left=0;
				
						}
						
						
						if (((posArr[0].left+25)>=posArr[i].left ) &&(posArr[i].dir!=2)&& (abs(posArr[0].top-posArr[i].top)<25)&& (i!=0)&&(posArr[0].left<posArr[i].left))
						
						{
								dead = 1;
						} 
					
						
						
						
						
					}
	
					
			 	if (posArr[i].dir == 4) 
					
				{
					if  (posArr[i].left <= 0 )
					{
						posArr[i].left = 1200;
					}
				
				
					if ((posArr[0].left<=posArr[i].left+25 )&&(posArr[i].dir!=4)&& (abs(posArr[0].top-posArr[i].top)<25)&& (i!=0)&&(posArr[0].left+25>posArr[i].left+25))
						
							{
									dead = 1;
							}
					
					
				}
		
			}
		
	
}
}

void FinishGame()	  //when snake is dead game finishes with boom graphics
{  	SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);     
	CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
	CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS, GameBackg, VAL_ENTIRE_OBJECT, MakeRect (GameCanvas.top, GameCanvas.left, GameCanvas.height, GameCanvas.width));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,crashSmoke, VAL_ENTIRE_OBJECT, MakeRect (200, 500, 200,200));
	CanvasDrawBitmap (panelHandle2, PANEL_2_GAMECANVAS,crashBoom, VAL_ENTIRE_OBJECT, MakeRect (250, 500, 100,200));

	CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

	
	
}

int CVICALLBACK quit (int panel, int event, void *callbackData,
					  int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			SavePanelState (panelHandle3, filename, 0);                           	

			QuitUserInterface (0);
			break;
	}
	return 0;
}



int CVICALLBACK RunGame (int panel, int control, int event,
						 void *callbackData, int eventData1, int eventData2)
{   char val[100];
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlAttribute (panelHandle, PANEL_NAMESTRING, ATTR_CTRL_VAL, val);
			if	(val[0] == '\0') 			
			{		
				MessagePopup("Note","Enter A Valid Name");
			}
			else
			{
			BASS_ChannelStop(MusicM_SND);

		
			BASS_ChannelPlay(MusicG_SND,TRUE);  
			
			GetCtrlVal (panelHandle, PANEL_RINGSLIDE, &diff);
	
			
			//setting all neccessary to 0 before launch

			dead=0;
			counter=0;
			score=0;
			
			GetCtrlVal (panelHandle, PANEL_NAMESTRING, name);
			switch (diff)  //choosing the difficulty and speed
			{
				case 1:
					speed = 5.75;
				break;
					
				case 2:
					speed = 12.5;
					break;
				case 3:
					speed = 25;
					break;
			}
			HidePanel(panelHandle);
			calc_food();
			GetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_INTERVAL, &dt);  //getting time interval from timer
			DisplayPanel (panelHandle2); 
			draw_3_2_1();
			draw_start();	 //drawing intial batch  
	
		

			

			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 1);	  //timer on
			}
		
			

			break;
	}
	return 0;
}

int CVICALLBACK quitbuttonFunc (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			score = 0;
			SavePanelState (panelHandle3, filename, 0);                           	

			QuitUserInterface (0);
			break;
	}
	return 0;
}

int CVICALLBACK tik (int panel, int control, int event,
					 void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_TIMER_TICK:
		
			clac_snake_pos();
			move_snake();
	
			 
			calc_eating();
	
			if (nextfood == 1)
		{
			BASS_ChannelPlay(Bite_SND,TRUE);
			score = score + nextscore;
			grow();
			
			SetCtrlVal (panelHandle2, PANEL_2_SCORENUMERIC, score);
			
			if (foodtodraw != foodArr[2])
			{
				wallsoff=0;
				SetCtrlVal (panelHandle2, PANEL_2_STRING_Walls, "No");
			}
			
			if (foodtodraw!=foodArr[4])
			{
				growboost=0;
					
			}
			
			
			
			
			calc_food(); 
			
			
		}
		 	is_dead();
			if (dead == 1)
				
		{  	BASS_ChannelStop(MusicG_SND);      
			BASS_ChannelPlay(Crash_SND,TRUE);    
			FinishGame();	
		}
		  

		
			break;
	}
	return 0;
}

int CVICALLBACK FinishGameFunc (int panel, int control, int event,
								void *callbackData, int eventData1, int eventData2)
{  int i,j; 

	

 
	switch (event)
		
	{
		case EVENT_COMMIT:
			BASS_ChannelStop(MusicG_SND);
			BASS_ChannelPlay(MusicM_SND,TRUE);  
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			HidePanel (panelHandle2);
			DisplayPanel (panelHandle); 

			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);

			InsertTableRows (panelHandle3, PANEL_3_TABLE, 12, 1, VAL_CELL_STRING);
			SetTableCellAttribute (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), ATTR_CELL_TYPE, VAL_CELL_NUMERIC);
			SetTableCellAttribute (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), ATTR_DATA_TYPE, VAL_INTEGER);
			SetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint (2, 12), name);
			SetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), score);
			SortTableCells (panelHandle3, PANEL_3_TABLE, MakeRect (2, 2, 11, 2), VAL_COLUMN_MAJOR, 3, 1, 0,0 );
			DeleteTableRows (panelHandle3, PANEL_3_TABLE, 12, 1);
			
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_BOLD, 1);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_FONT, VAL_MENU_FONT);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_POINT_SIZE, 15);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_CELL_JUSTIFY,   VAL_CENTER_CENTER_JUSTIFIED);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,1,3), ATTR_TEXT_BGCOLOR,  VAL_DK_GREEN);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect (3, 1, 9, 3), ATTR_TEXT_BGCOLOR, VAL_WHITE);
			SavePanelState (panelHandle3, filename, 0);                           	

				for (j=0;j<10;j++)
			{
		
				GetTableCellVal (panelHandle3, PANEL_3_TABLE,MakePoint(2,j+2),players[j].name);
				GetTableCellVal (panelHandle3, PANEL_3_TABLE,MakePoint(3,j+2),&players[j].score);
				players[j].diff=diff;
				
		


			}
			
			
			
			for (i=1;i<=counter;i++)
			{
				posArr[i].top=0;
				posArr[i].left=0;
				posArr[i].height=25;
				posArr[i].width=25;
				posArr[i].dir=25;
			}
			score= 0;
			counter= 0;
			wallsoff=0;
			nextscore=0;
			growboost=0;
			SetCtrlVal (panelHandle2, PANEL_2_SCORENUMERIC, 0);
			SetCtrlVal (panelHandle, PANEL_NAMESTRING, "");

			SetCtrlVal (panelHandle2, PANEL_2_STRING_Walls, "No");
			break;


	}
	return 0;
}

int CVICALLBACK MuteMain (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{  int state=0;
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal (panelHandle, PANEL_PICTUREBUTTON_MUTE, &state);
			if (state == 1)
		{
			BASS_ChannelStop(MusicM_SND);
		}
		
		else
		{
			BASS_ChannelPlay(MusicM_SND,TRUE);  
	
		}
			

			break;
	}
	return 0;
}



int CVICALLBACK MuteGame (int panel, int control, int event,
						  void *callbackData, int eventData1, int eventData2)
{ int state=0;
	switch (event)
	{
		case EVENT_COMMIT:
			
			GetCtrlVal (panelHandle2, PANEL_2_PICTUREBUTTON2_MUTE, &state);
			if (state == 1)
		{
			BASS_ChannelStop(MusicG_SND);
		}
		
		else
		{
			BASS_ChannelPlay(MusicG_SND,TRUE);  
	
		}
			

			break;
	}
	return 0;
}

void CVICALLBACK AboutFunc (int menuBar, int menuItem, void *callbackData,
							int panel)
{
		DisplayPanel (panelHandle4);

}



int CVICALLBACK quitAbout (int panel, int event, void *callbackData,
						   int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel (panelHandle4);

			break;
	}
	return 0;
}

void CVICALLBACK DemoFunc (int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	LaunchExecutable ("explorer.exe SnakeGame.mp4");
}

void CVICALLBACK HelpFunc (int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	LaunchExecutable ("explorer.exe Help_doc.pdf");			  //launching document drectly from folder 
									
}

void CVICALLBACK RecordsFunc (int menuBar, int menuItem, void *callbackData,
							  int panel)
{
	RecallPanelState (panelHandle3,filename, 0);  

	DisplayPanel(panelHandle3);
	
}

int CVICALLBACK CloseAbout (int panel, int control, int event,
							void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel (panelHandle4);   

			break;
	}
	return 0;
}

int CVICALLBACK CloseRecords (int panel, int control, int event,
							  void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			HidePanel (panelHandle3);


			break;
	}
	return 0;
}

int CVICALLBACK quitPanel2 (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{ int i,j;
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			   			BASS_ChannelStop(MusicG_SND);
			BASS_ChannelPlay(MusicM_SND,TRUE);  
			SetCtrlAttribute (panelHandle, PANEL_TIMER, ATTR_ENABLED, 0);
			HidePanel (panelHandle2);
			DisplayPanel (panelHandle); 

			CanvasStartBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			CanvasClear (panelHandle2, PANEL_2_GAMECANVAS, VAL_ENTIRE_OBJECT);	 
			CanvasEndBatchDraw (panelHandle2, PANEL_2_GAMECANVAS);
			RecallPanelState (panelHandle3,filename, 0);  

			InsertTableRows (panelHandle3, PANEL_3_TABLE, 12, 1, VAL_CELL_STRING);
			SetTableCellAttribute (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), ATTR_CELL_TYPE, VAL_CELL_NUMERIC);
			SetTableCellAttribute (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), ATTR_DATA_TYPE, VAL_INTEGER);
			SetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint (2, 12), name);
			SetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint (3, 12), score);
			SortTableCells (panelHandle3, PANEL_3_TABLE, MakeRect (2, 2, 11, 2), VAL_COLUMN_MAJOR, 3, 1, 0,0 );
			DeleteTableRows (panelHandle3, PANEL_3_TABLE, 12, 1);
			
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_BOLD, 1);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_FONT, VAL_MENU_FONT);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_TEXT_POINT_SIZE, 15);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,10,3), ATTR_CELL_JUSTIFY,   VAL_CENTER_CENTER_JUSTIFIED);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect(2,1,1,3), ATTR_TEXT_BGCOLOR,  VAL_DK_GREEN);
			SetTableCellRangeAttribute (panelHandle3, PANEL_3_TABLE, MakeRect (3, 1, 9, 3), ATTR_TEXT_BGCOLOR, VAL_WHITE);
			SavePanelState (panelHandle3, filename, 0);                           	

				for (j=0;j<10;j++)
			{
		
				GetTableCellVal (panelHandle3, PANEL_3_TABLE,MakePoint(2,j+2),players[j].name);
				GetTableCellVal (panelHandle3, PANEL_3_TABLE,MakePoint(3,j+2),&players[j].score);
				players[j].diff=diff;
				
		


			}
			
			
			
			for (i=1;i<=counter;i++)
			{
				posArr[i].top=0;
				posArr[i].left=0;
				posArr[i].height=25;
				posArr[i].width=25;
				posArr[i].dir=25;
			}
			score= 0;
			counter= 0;
			wallsoff=0;
			nextscore=0;
			growboost=0;
			SetCtrlVal (panelHandle2, PANEL_2_SCORENUMERIC, 0);
			SetCtrlVal (panelHandle, PANEL_NAMESTRING, "");

			SetCtrlVal (panelHandle2, PANEL_2_STRING_Walls, "No");
			break;


	}
	return 0;
}

int CVICALLBACK quitPanel3 (int panel, int event, void *callbackData,
							int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_GOT_FOCUS:

			break;
		case EVENT_LOST_FOCUS:

			break;
		case EVENT_CLOSE:
			HidePanel (panelHandle3);

			break;
	}
	return 0;
}

int CVICALLBACK VolFunc (int panel, int control, int event,
						 
						 void *callbackData, int eventData1, int eventData2)
{  
	switch (event)
	{
		case EVENT_COMMIT:
		 GetCtrlVal (panelHandle, PANEL_NUMERICSLIDE, &vol);     
 			BASS_SetVolume(vol);

			break;
	}
	return 0;
}

int CVICALLBACK SaveLead (int panel, int control, int event,		//save leadbord
						  void *callbackData, int eventData1, int eventData2)
{
	int Filestatus,pscore,i,status;
	char filetosave[300],pname[100];
	
	FILE *fp;
	
	
	switch (event)
	{
			case EVENT_COMMIT:
			
			Filestatus = FileSelectPopup ("", "*.csv", "", "Please Select a Location", VAL_SAVE_BUTTON, 0, 0, 1, 0, filetosave);

				if (Filestatus>=1)
			{
			
				fp = fopen (filetosave, "w");  
			
			for (i=0;i<10;i++)
			{
			GetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint(2,i+2), pname);
			GetTableCellVal (panelHandle3, PANEL_3_TABLE, MakePoint(3,i+2), &pscore);
			
			fprintf(fp,"%s,%d\n",pname,pscore);

			}
			status = fclose(fp);
			
			
			if (status>=0)	
			{
			MessagePopup("Note", "Table Saved!");
			}
			OpenDocumentInDefaultViewer (filetosave, VAL_NO_ZOOM);       
			}
			break;
	}
	return 0;
}

																	//colorchanging func
int CVICALLBACK Color (int panel, int control, int event,
					   void *callbackData, int eventData1, int eventData2)
{
	switch (event)
	{
		case EVENT_COMMIT:
			
			switch(control)
			{
				case PANEL_NUMERICRED:
					
					GetCtrlVal (panelHandle, PANEL_NUMERICRED, &red);
					SetCtrlAttribute (panelHandle, PANEL_NUMERICRED, ATTR_FILL_COLOR, MakeColor(red,0,0));
					 SetCtrlAttribute (panelHandle, PANEL_COLORSCHEM, ATTR_FRAME_COLOR, MakeColor(red,green,blue));
					 SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_PEN_FILL_COLOR, MakeColor(red, green, blue));
					
					break;
					
					
				case PANEL_NUMERICGRN:
					GetCtrlVal (panelHandle, PANEL_NUMERICGRN, &green);
					 SetCtrlAttribute (panelHandle, PANEL_NUMERICGRN, ATTR_FILL_COLOR, MakeColor(0,green,0));
					  SetCtrlAttribute (panelHandle, PANEL_COLORSCHEM, ATTR_FRAME_COLOR, MakeColor(red,green,blue));
					  SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_PEN_FILL_COLOR, MakeColor(red, green, blue));

					
					break;
											
				case PANEL_NUMERICBLU:
					GetCtrlVal (panelHandle, PANEL_NUMERICBLU, &blue);
					SetCtrlAttribute (panelHandle, PANEL_NUMERICBLU, ATTR_FILL_COLOR, MakeColor(0,0,blue));
					SetCtrlAttribute (panelHandle, PANEL_COLORSCHEM, ATTR_FRAME_COLOR, MakeColor(red,green,blue));
					 					 SetCtrlAttribute (panelHandle2, PANEL_2_GAMECANVAS, ATTR_PEN_FILL_COLOR, MakeColor(red, green, blue));

					
					break;
							
						
						
	
					
			}

			break;
	}
	return 0;
}

void CVICALLBACK ExitMain (int menuBar, int menuItem, void *callbackData,
						   int panel)
{
	
				SavePanelState (panelHandle3, filename, 0);     
							QuitUserInterface (0);


}
