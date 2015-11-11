/*
 * game.c
 *
 *  Created on: 8 Nov 2015
 *      Author: Arsalan Syed
 */
#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */


done=0;

int powerPoints=0;
int *ppP=&powerPoints;

//the allowed height difference between an enemy and a bullet
int diffTol=1;


int shotDefPos=100;

int topBound=0;
int botBound=0;

int score=0;
int level=1;

//positions of enemies
int enemyPos[10]={};


//an array containing the position of a shot
int shots[10]={};


int pos=0;
int *posP=&pos;

int lives=3;
int *livesP=&lives;

int hasShield=0; //boolean
int *hasShieldP=&hasShield;

int shieldReady=0; //bool that determines if the shield can be deployed (1 -> shield can be used)
int *shieldReadyP=&shieldReady;

int speed=0;
int *speedP=&speed;

int damageOutput=10; //per shot
int fireRate=5; //per second

//Moves the position of the player. speed>0 moves up, speed<0 moves down
moveUp(){
	*posP+=speed;
}

moveDown(){
	*posP-=speed;
}


changeSpeed(int incr){
	*speedP+=incr;
}

loseLife(){
	*livesP--;	
	PORTE--; //turn off a light
}


deployShield(){
	if(shieldReady){
		shieldOn();
	}
}

//Turns the shield on
shieldOn(){
	*hasShieldP=1;
}


toggleShieldReady(){
	*shieldReadyP=~shieldReady;
}


arrayLength(int[] arr){
	return sizeof arr / sizeof *arr;
}

//when storing bullets, we want a FIFO list. Assuming all bullets travel
//at the same speed, the first bullet to be shot will be the first one to
//be removed(by hitting a target or going off the display).
fire(){		
	int i;
	if(shots[arrayLength(shots)]>0){ //if array is not full (last element is a non zero value
		for(i=0;i<arrayLength(shots);i++){
			shots[i+1]=shots[i];		
			shots[0]=shotDefPos; 
		}
	}
	else{
		//no more bullets can be shot until ther'es space
	}
}


lastBullet(){
	return shots[arrayLength(shots)];
}


hitsEnemy(){
	int i;
	int j;
	for(i=0;i<arrayLength(enemyPos);i++){
		if(abs(enemyPos[i]-lastBullet())<diffTol{ //hits an enemy
			for(j=1;j<arrayLength(shots);j++){
				shots[j-1]=shots[j];  //shift all bullets down.
			}		
		}
	}	
}

//similar method to hisEnemy where bullet goes off the screen




//Initializes input/output ports.
void initIO(){
	TRISE=TRISE&0xff00;
	PORTE=0;	
	TRISD=TRISD&0xff0; //buttons are on bit 7 to 4, switches are on bits 11 to 8. Set all of those to input.  
  	return;
}




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


/* 
 * Returns status of toggle-switches on the board. Has no paramaters.
 * return: four lsb's of this value contain info of switches 4,3,2,1.
 * all other values are zero. 
 * SW4 to SW1 are bits 11 to 8 on PORTD. 
 */
int getsw(void){
	return swVals = (PORTD >> 8) & 0xf; 
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
	return butVals=(PORTD >> 4) & 0xf; //returns 4 bits, BTN 4,3,2,1
}

main(){
	while(!done){
		checkInput();
		gameOver();
	}
	dispScore();
}




gameOver(){
	if(lives==0){		
		done=0;
	}
}



