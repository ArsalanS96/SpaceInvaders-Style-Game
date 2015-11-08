/*
 * game.c
 *
 *  Created on: 8 Nov 2015
 *      Author: Arsalan Syed
 */
#include <stdio.h>
#include <stdint.h>   /* Declarations of uint_32 and the like */
//#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */


int topBound=0;
int botBound=0;

int score=0;
int level=1;

int enemies[10]={};
int lazers[10]={};


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
move(){
	*posP+=speed;
}

changeSpeed(int incr){
	*speedP+=incr;
}

loseLife(){
	*livesP--;	
}

//Turns the shield off or on
toggleShield(){
	*hasShieldP=~hasShield; //invert the bit
}


toggleShieldReady(){
	*shieldReadyP=~shieldReady;
}



void initIO( void )
{
  volatile int* trise = (volatile int*) 0xbf886100;
  *trise = *trise & 0xff00; //only concerned with lower 16 bits. Set lower 8 to value = 0.
  
  volatile int* porte = (volatile int*) 0xbf886110; //pointer for PORTE
  *porte=0; //initialise to zero (all lights off).	
  
  TRISD = TRISD & 0xffc0; // ---- 1111 1110 0000 set bits 11 to 5 with value 1 (input).
  
  return;
}


main(){
	
}

