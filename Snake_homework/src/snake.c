#include "snake.h"

snake SnakeInit(snake mysnake){
	for(uint8_t i=0; i<37; i++)
		mysnake.body[i]=0;
	mysnake.body[15]=1; //itt indul a snake
	mysnake.size=1;   //1 m�ret�
	mysnake.dir=rightright; //persze jobbra n�z
	mysnake.isAlive=true; //�s �l
	return mysnake;
}

uint8_t PlaceFood(snake mysnake) {  //random helyre rakja a kaj�t ahol nincs a k�gy�
	uint8_t food;
	do{
		food=rand()%36;
	} while(mysnake.body[food]!=0);
	return food;
}

snake NextDirUART(snake mysnake, char uartdir) {
	if(uartdir=='j') //ha jobbra fordul
		switch(mysnake.dir) {
			case(rightright):
			case(upright):
			case(downright):
				mysnake.dir=rightdown;
				break;
			case(leftleft):
			case(upleft):
			case(downleft):
				mysnake.dir=leftup;
				break;
			case(rightdown):
			case(leftdown):
			case(downdown):
				mysnake.dir=downleft;
				break;
			case(rightup):
			case(leftup):
			case(upup):
				mysnake.dir=upright;
				break;
			default:
				mysnake.size=0;
				mysnake.dir=rightright;
				mysnake.isAlive=false;
				for(uint8_t i; i<36; i++)
					mysnake.body[i]=0;
				break;
		}
	else if(uartdir=='b') //ha balra fordul
		switch(mysnake.dir) {
			case(rightright):
			case(upright):
			case(downright):
				mysnake.dir=rightup;
				break;
			case(leftleft):
			case(upleft):
			case(downleft):
				mysnake.dir=leftdown;
				break;
			case(rightdown):
			case(leftdown):
			case(downdown):
				mysnake.dir=downright;
				break;
			case(rightup):
			case(leftup):
			case(upup):
				mysnake.dir=upleft;
				break;
			default:
				mysnake.size=0;
				mysnake.dir=rightright;
				mysnake.isAlive=false;
				for(uint8_t i; i<36; i++)
					mysnake.body[i]=0;
				break;
		}
	else {
		char kifele[16]="Rossz karakter!";
		for(uint8_t i=0;i<15;i++)
			USART_Tx(UART0, kifele[i]);
	}
	return mysnake;
}

snake NextDirNoUART(snake mysnake) {
		switch(mysnake.dir) {
			case(rightright):
			case(upright):
			case(downright):
				mysnake.dir=rightright;
				break;
			case(leftleft):
			case(upleft):
			case(downleft):
				mysnake.dir=leftleft;
				break;
			case(rightdown):
			case(leftdown):
			case(downdown):
				mysnake.dir=downdown;
				break;
			case(rightup):
			case(leftup):
			case(upup):
				mysnake.dir=upup;
				break;
			default:
				mysnake.size=0;
				mysnake.dir=rightright;
				mysnake.isAlive=false;
				for(uint8_t i; i<36; i++)
					mysnake.body[i]=0;
				break;
		}
	return mysnake;
}

//if(uarthappened) { //valahol TimerIT-ben
//	NextDirUART
//}
//else {
//	NextDirNoUART
//}

snake MoveSnake(snake mysnake, uint8_t *fruit) { //snake-et mozgatja, n�zi a mag�baharap�st �s a gy�m�lcsev�st is, �s lekezeli
	uint8_t head, headforfruit=100; //head lesz az, amit n�zni fog az ir�ny mellett ahhoz, hogy merre l�ptesse a k�gy�t, de a head a l�p�s ut�n el lesz rontva, hogy �tugorja a t�bbi if else-t, a headforfruit megmarad, hogy a gy�m�lcs check-re megmaradjon a fej helyzete
	bool errorbit=true; //ha ez nem fordul false-ba �s nem marad �gy, akkor size!=(legnagyobb mysnake.body[] tagj�val, valami nagyon nem ok�)
	for(uint8_t i=0; i<37; i++)
		if(mysnake.body[i]==mysnake.size){
			head=i;
			errorbit=false; //ha egyezik a size a body egyik tagj�val, akkor vagy egyezik a size a body size-j�val, vagy kisebb, de nem nagyobb legal�bb, nem lesz hiba
		}
	for(uint8_t i=0; i<37; i++)
		if(mysnake.body[i]>mysnake.size)
			errorbit=true; //�gy m�r biztos, hogy a size egyezik a body legnagyobb tagj�val, ha nem akkor error van
	if(errorbit) {//hibakeres�s, mivel t�rjen vissza?
		mysnake.isAlive=false;
		for(uint8_t i=0; i<37; i++)
			mysnake.body[i]=0;
		mysnake.size=0;
		mysnake.dir=rightright;
		return mysnake;
	}
	bool fruiteaten=true;
	if((mysnake.dir==upup)||mysnake.dir==downdown) {  //upup , downdown , leftleft , rightright, upleft, downleft, upright, downright, rightdown, leftdown, rightup, leftup
		if(head>=7&&head<=14) {
			if(mysnake.body[head+15]>1) //magic cucc, az�rt nem j� az 1-sem, mert az mozg�s ut�n a cs�kkentett �rt�ken kellene n�zni, hogy beleharap-e mag�ba, 1-1=0, teh�t m�g j�
				mysnake.isAlive=false;
			mysnake.body[head+15]=mysnake.size+1;
			headforfruit=head+15;
			head=37;
		}
		else if(head>=22&&head<=29) {
			if(mysnake.body[head-15]>1)
				mysnake.isAlive=false;
			mysnake.body[head-15]=mysnake.size+1;
			headforfruit=head-15;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==leftleft) {
		if((head>=1&&head<=6)||(head>=16&&head<=21)||(head>=31&&head<=36)) {
			if(mysnake.body[head-1]>1)
				mysnake.isAlive=false;
			mysnake.body[head-1]=mysnake.size+1;
			headforfruit=head-1;
			head=37;
		}
		else if(head==0||head==15||head==30) {
			if(mysnake.body[head+6]>1)
				mysnake.isAlive=false;
			mysnake.body[head+6]=mysnake.size+1;
			headforfruit=head+6;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==rightright) {
		if((head>=0&&head<=5)||(head>=15&&head<=20)||(head>=30&&head<=35)) {
			if(mysnake.body[head+1]>1)
				mysnake.isAlive=false;
			mysnake.body[head+1]=mysnake.size+1;
			headforfruit=head+1;
			head=37;
		}
		else if(head==6||head==21||head==36) {
			if(mysnake.body[head-6]>1)
				mysnake.isAlive=false;
			mysnake.body[head-6]=mysnake.size+1;
			headforfruit=head-6;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==upleft) {
		if((head>=8&&head<=14)||(head>=23&&head<=29)) {
			if(mysnake.body[head-8]>1)
				mysnake.isAlive=false;
			mysnake.body[head-8]=mysnake.size+1;
			headforfruit=head-8;
			head=37;
		}
		else if(head==7||head==22) {
			if(mysnake.body[head-1]>1)
				mysnake.isAlive=false;
			mysnake.body[head-1]=mysnake.size+1;
			headforfruit=head-1;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==downleft) {
		if((head>=8&&head<=14)||(head>=23&&head<=29)) {
			if(mysnake.body[head+7]>1)
				mysnake.isAlive=false;
			mysnake.body[head+7]=mysnake.size+1;
			headforfruit=head+7;
			head=37;
		}
		else if(head==7||head==22) {
			if(mysnake.body[head+14]>1)
				mysnake.isAlive=false;
			mysnake.body[head+14]=mysnake.size+1;
			headforfruit=head+14;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==upright) {
		if((head>=7&&head<=13)||(head>=22&&head<=28)) {
			if(mysnake.body[head-7]>1)
				mysnake.isAlive=false;
			mysnake.body[head-7]=mysnake.size+1;
			headforfruit=head-7;
			head=37;
		}
		else if(head==14||head==29) {
			if(mysnake.body[head-14]>1)
				mysnake.isAlive=false;
			mysnake.body[head-14]=mysnake.size+1;
			headforfruit=head-14;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==downright) {
		if((head>=7&&head<=13)||(head>=22&&head<=28)) {
			if(mysnake.body[head+8]>1)
				mysnake.isAlive=false;
			mysnake.body[head+8]=mysnake.size+1;
			headforfruit=head+8;
			head=37;
		}
		else if(head==14||head==29) {
			if(mysnake.body[head+1]>1)
				mysnake.isAlive=false;
			mysnake.body[head+1]=mysnake.size+1;
			headforfruit=head+1;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==rightdown) {
		if((head>=0&&head<=6)||(head>=15&&head<=21)) {
			if(mysnake.body[head+8]>1)
				mysnake.isAlive=false;
			mysnake.body[head+8]=mysnake.size+1;
			headforfruit=head+8;
			head=37;
		}
		else if(head>=30&&head<=36) {
			if(mysnake.body[head-22]>1)
				mysnake.isAlive=false;
			mysnake.body[head-22]=mysnake.size+1;
			headforfruit=head-22;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==leftdown) {
		if((head>=0&&head<=6)||(head>=15&&head<=21)) {
			if(mysnake.body[head+7]>1)
				mysnake.isAlive=false;
			mysnake.body[head+7]=mysnake.size+1;
			headforfruit=head+7;
			head=37;
		}
		else if(head>=30&&head<=36) {
			if(mysnake.body[head-23]>1)
				mysnake.isAlive=false;
			mysnake.body[head-23]=mysnake.size+1;
			headforfruit=head-23;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==rightup) {
		if((head>=15&&head<=21)||(head>=30&&head<=36)) {
			if(mysnake.body[head-7]>1)
				mysnake.isAlive=false;
			mysnake.body[head-7]=mysnake.size+1;
			headforfruit=head-7;
			head=37;
		}
		else if(head>=0&&head<=6) {
			if(mysnake.body[head+23]>1)
				mysnake.isAlive=false;
			mysnake.body[head+23]=mysnake.size+1;
			headforfruit=head+23;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	}
	else if(mysnake.dir==leftup) {
		if((head>=15&&head<=21)||(head>=30&&head<=36)) {
			if(mysnake.body[head-8]>1)
				mysnake.isAlive=false;
			mysnake.body[head-8]=mysnake.size+1;
			headforfruit=head-8;
			head=37;
		}
		else if(head>=0&&head<=6) {
			if(mysnake.body[head+22]>1)
				mysnake.isAlive=false;
			mysnake.body[head+22]=mysnake.size+1;
			headforfruit=head+22;
			head=37;
		}
		else {
			errorbit=true;
			head=37;
		}
	} //198 sornyi csoda
	if(errorbit) {//hibakeres�s, mivel t�rjen vissza?
		mysnake.isAlive=false;
		for(uint8_t i=0; i<37; i++)
			mysnake.body[i]=0;
		mysnake.size=0;
		mysnake.dir=rightright;
		return mysnake;
	}
	mysnake.size++; //n�velj�k a size m�ret�t, felt�telezve, hogy megette a gy�m�lcs�t
	if(headforfruit!=*fruit) {//ha a fej helyzete a gy�m�lcsben van, akkor megette, teh�t a k�gy� m�ret�t nem kell cs�kkenteni, am�gy meg igen, �s...
		for(uint8_t i=0; i<37; i++)
			if(mysnake.body[i]!=0)
				mysnake.body[i]--;
		mysnake.size--;   //mivel nem ette meg a gy�m�lcs�t, cs�kkents�k vissza a m�retet
		fruiteaten=false;
	}
	if(fruiteaten) //ha megette a gy�mit, akkor �jat le kell rakni
		*fruit=PlaceFood(mysnake);
	return mysnake;
}

SegmentLCD_LowerCharSegments_TypeDef SnakeandFoodtoLCD(snake thesnake, uint8_t food, SegmentLCD_LowerCharSegments_TypeDef *disp){
    for(uint8_t j=0; j<7; j++) //disp null�z�s
    	disp[j].raw=0;
    thesnake.body[food]=1; //az �telt �gy rajzoljuk ki, hogy berakjuk a snake mez�j�be
	for(uint8_t i=0; i<37; i++) { //snake konvert�l�sa a dispre
		if(thesnake.body[i]!=0) {
			switch(i) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
				disp[i].a=1;
				break;
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				disp[i-7].f=1;
				break;
			case 14:
				disp[6].b=1;
				break;
			case 15:
			case 16:
			case 17:
			case 18:
			case 19:
			case 20:
			case 21:
				disp[i-15].g=1;
				disp[i-15].m=1;
				break;
			case 22:
			case 23:
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
				disp[i-22].e=1;
				break;
			case 29:
				disp[6].c=1;
				break;
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
				disp[i-30].d=1;
				break;
			default:
				break;
			}
		}
	}
	return *disp;
};
