#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#include <sched.h>
#include <sstream>
#include <queue>          // std::queue
#include<unistd.h>
#include<signal.h>
#include<string.h>
#include <fstream>
#include <cstdio>

using namespace std;

//initialisation du pin de reception
int pin;

//Fonction de log
void log(string a){
	//Décommenter pour avoir les logs
	cout << a << endl;
}

//Fonction de passage du programme en temps réel (car la reception se joue a la micro seconde près)
void scheduler_realtime() {
	struct sched_param p;
	p.__sched_priority = sched_get_priority_max(SCHED_RR);
	if( sched_setscheduler( 0, SCHED_RR, &p ) == -1 ) {
		perror("Failed to switch to realtime scheduler.");
	}
}

//Fonction de remise du programme en temps standard
void scheduler_standard() {
	struct sched_param p;
	p.__sched_priority = 0;
	if( sched_setscheduler( 0, SCHED_OTHER, &p ) == -1 ) {
		perror("Failed to switch to normal scheduler.");
	}
}

struct timePinVal
{
	int pinValue;
	int index;
	__suseconds_t tv_usec;	/* Microseconds.  */
};

struct sigaction old_action;
queue<timePinVal> q;
volatile bool stop = 0;

//Programme principal
int main (int argc, char** argv)
{
	log("Demarrage du programme");

	string csvFilePath = argv[2];
	cout<<"Fichier de d'entree : "<<csvFilePath<<endl;

	//on récupere l'argument 1, qui est le numéro de Pin GPIO auquel est connecté le recepteur radio
	pin = atoi(argv[1]);
	//Si on ne trouve pas la librairie wiringPI, on arrête l'execution
	if(wiringPiSetup() == -1)
	{
		log("Librairie Wiring PI introuvable, veuillez lier cette librairie...");
		return -1;
	}else{
		log("Librairie WiringPI detectee");
	}
	pinMode(pin, OUTPUT);
	log("Pin GPIO configure en sortie");

	//On passe en temps réel
	scheduler_realtime();

	int index, pinValue;
	queue<timePinVal> q;

	log("Lecture csv");

	FILE *fp;
	fp = fopen(csvFilePath.c_str(), "r");

	while (fscanf(fp, "%u;%u\n", &index, &pinValue) == 2)
	{
		//printf("%u;%u\n", index, pinValue);

		timePinVal tpv;
		tpv.index = index;
		tpv.pinValue = pinValue;
		q.push(tpv);
	}

	fclose(fp);

	log("Emission");

	do
	{
		timePinVal tpv = q.front();
		q.pop();

		digitalWrite(pin, tpv.pinValue);

		delayMicroseconds(100);
	} while(!q.empty());

	scheduler_standard();

	log("Fin");
}

