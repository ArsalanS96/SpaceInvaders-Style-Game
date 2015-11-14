/*
 * game.c
 *
 *  Created on: 8 Nov 2015
 *      Author: Arsalan Syed and Martin Nilsson
 */
#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
//#include <pic32mx.h>  /* Declarations of system-specific addresses etc */

#define MAX_ENEMIES 10
#define MAX_BULLETS 10

#define TOP_BOUND 0
#define BOTTOM_BOUND 100
#define RIGHT_BOUND 100
#define LEFT_BOUND 0

#define ENEMY_HEALTH 20
#define BULLET_MARGIN 5
#define DIFF_TOL_X 1
#define DIFF_TOL_Y 1
#define PLAYER_X 0
#define BULLET_SPEED 5
#define SCORE_PER_KILL 5

struct Enemy {
	int posX;
	int posY;
	int health;
};


struct Bullet {
	int posX;
	int posY;
	int damage;	
};

//For having 3,2,1,0 lives, the lights must have values 7,3,1,0. This 
//var is difference between 7,3 and then 3,1 and so on.
int lightDecr = 4;

//Indicates when the game is done 
int done=0;

//the amount of lives, once they are zero, the game is over
int lives=3;
int *livesP=&lives;

//1 if the shield is on, 0 if it is off
int hasShield=0; 
int *hasShieldP=&hasShield;

//determines if the shield can be deployed (1 -> shield can be used)
int shieldReady=0; 
int *shieldReadyP=&shieldReady;

//the speed of the player
int speed=0;
int *speedP=&speed;

//the damage each shot by the player produces
int damageOutput=10; //per shot

//the amount of shots the player can make per second
int fireRate=1;

//total score
int score=0;

//an accumulated amount of points that can be used for power ups.
int powerPoints=0;
int *ppP=&powerPoints;

//the player can only move along the Y-axis.
int playerY=0;


struct Enemy enemies[MAX_ENEMIES]= {};
int enemyArrayIndex=0;

struct Bullet bullets[MAX_BULLETS]= {};
int bulletArrayIndex=0;


/**
 * Runs the main game loop until the game is over
 */
main(){
	initIO();
	while(!done){
		checkInput();
		gameOver();
	}
	dispScore();
}

/**
 * Checks button and switch input and performs the corresponding action
 */
checkInput(){
	int butVal=getbtns();
	int swVal = getsw();
	
	if(butVal&1){ //button 1 is pressed down
		deployShield();
	}
	else if(butVal&2){ //button 2 is pressed down
		moveDown();
	}
	else if(butVal&4){ //button 3 is pressed down
		fire();
	}
	else if(butVal&8){ //button 4 is pressed down
		moveUp();
	}
	
	if(swVal&1){ //switch 1 is pressed down
		incFireRate();
	}
	else if(swVal&2){ //switch 2 is pressed down
		incDamage();
	}
	else if(swVal&4){ //switch 3 is pressed down
		freeze();
	}
	else if(swVal&8){ //switch 4 is pressed down
		ult();
	}
}


/**
 * Initializes input/output ports.
 */
 
initIO(){
	TRISE=TRISE&0xff00;
	PORTE=0;	
	TRISD=TRISD&0xff0; //buttons are on bit 7 to 4, switches are on bits 11 to 8. Set all of those to input.  
	PORTD=0;
  	return;
}


/*
 * Moves the player up
 */
moveUp(){
	playerY+=speed;
}


/*
 * Moves the player down
 */
moveDown(){
	playerY-=speed;
}



/*when storing bullets, we want a FIFO list. Assuming all bullets travel
 * at the same speed, the first bullet to be shot will be the first one to
 * be removed(by hitting a target or going off the display).
 */
fire(){		
	//if the bullet array is not empty, we can add a bullet to it
	if(hasSpace(bullets)){
		bullets[bulletArrayIndex]=spawnBullet();
		bulletArrayIndex++;
	}	
}

moveBullets(){
	int i;
	for(i=0;i<arrayLength(bullets);i++){
		bullets[i].posX+=BULLET_SPEED;
	}
}




//Determines what to do if an enemy is hit. TODO
hitsEnemy(){
	int i;
	int j;
	int xDiff;
	int yDiff;
	
	for(i=0;i<arrayLength(bullets);i++){
		for(j=0;j<arrayLength(enemies);j++){
			if(enemies[i]!=NULL)
			xDiff = abs(enemies[j].posX-bullets[i].posX);
			yDiff = abs(enemies[j].posY-bullets[i].posY);
			if(xDiff<DIFF_TOL_X&&yDiff<DIFF_TOL_Y){ //hits an enemy
				bullets[i]=NULL;
				enemies[j]=NULL;
				score+=SCORE_PER_KILL;
			}
		}
	}
}

/*
Creates an enemy and gives it an x and y position. The x position can be anywhere from
the middle of the screen the far right. The y position is between the top and the bottom of the screen.
NOTE: This needs to be done periodically and when there's space for enemies to spawn
*/

spawnEnemy(){
	struct Enemy enemy;
	enemy.posX=(rand()%(RIGHT_BOUND-RIGHT_BOUND/2))+RIGHT_BOUND/2; 
	enemy.posY=(rand()%(BOTTOM_BOUND-TOP_BOUND))+TOP_BOUND;
	enemy.health=ENEMY_HEALTH;
	enemies[enemyArrayIndex]=enemy;
	enemyArrayIndex++;
}


struct Bullet spawnBullet(){
	struct Bullet bullet;
	bullet.posX=PLAYER_X+BULLET_MARGIN; 
	bullet.posY=playerY;
	return bullet;
}


//similar method to hitsEnemy where bullet goes off the screen
offScreen(){
		
}


//TODO
//binary can have pattern 101 but we want lights to go 111 -> 011 -> 001 -> 000 instead of 111 -> 110 -> 101 -> ....
loseLife(){
	*livesP--;	
	PORTE-=lightDecr; //turn off a light  
	lightDecr=lightDecr/2;
}


changeSpeed(int incr){
	*speedP+=incr;
}



//returns the last bullet from the bullets array
lastBullet(){
	return bullets[arrayLength(bullets)];
}




//finds the length of an array
arrayLength(int arr[]){
	return (sizeof arr) / (sizeof *arr);
}









/* 
 * Returns status of toggle-switches on the board. Has no paramaters.
 * return: four lsb's of this value contain info of switches 4,3,2,1.
 * all other values are zero. 
 * SW4 to SW1 are bits 11 to 8 on PORTD. 
 */
int getsw(void){
	return (PORTD >> 8) & 0xf; 
}

/*
*Function prototype: int getbtns(void);
Parameter: none.
Return value: The 3 least significant bits of the return value must contain current data from push
buttons BTN4, BTN3, and BTN2. BTN2 corresponds to the least significant bit. All other bits of
the return value must be zero.
Notes: The function getbtns will never be called before Port D has been correctly initialized. The
buttons BTN4, BTN3, and BTN2, are connected to bits 7, 6 and 5 of Port D.
*/

int getbtns(void){
	return (PORTD >> 4) & 0xf; //returns 4 bits, BTN 4,3,2,1
}





gameOver(){
	if(lives==0){		
		done=0;
	}
}


displayPlayer(){
	
}


displayEnemy(struct Enemy enemy){
	
}

displayEnemies(){
	
}

displayGameOver(){
	
}

displayGameStart(){
	
}













/* 
top left corner: (0,0)
   bottom left corner: (0,botBound)
   top right corner: (rightBound,0)
   bottom right corner: (rightBound,botBound)
*/
