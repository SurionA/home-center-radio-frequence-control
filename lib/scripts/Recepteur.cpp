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

void sigint_handler(int sig_no)
{
	stop = 1;
	printf("CTRL-C pressed\n");
	perror("CTRL-C pressed\n");
	cerr<<q.size()<<endl;


	sigaction(SIGINT, &old_action, NULL);
	//    kill(0, SIGINT);
}


//Programme principal
int main (int argc, char** argv)
{
	struct timeval tn;
	struct sigaction action;
	memset(&action, 0, sizeof(action));
	action.sa_handler = &sigint_handler;
	sigaction(SIGINT, &action, &old_action);

	log("Demarrage du programme");

	string csvFilePath = argv[2];
	cout<<"Fichier de sortie"<<csvFilePath<<endl;

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
	pinMode(pin, INPUT);
	log("Pin GPIO configure en entree");

	//On passe en temps réel
	scheduler_realtime();

	int i = 0;
	do
	{
		//log("Nouvelle mesure");

		gettimeofday(&tn, NULL);

		timePinVal tpv;

		//log("Récupération valeur mesure");

		tpv.pinValue = digitalRead(pin);

		//log("Recopie temps");

		tpv.tv_usec = tn.tv_usec;
		tpv.index = i;

		//log("Avant log");

		//log(tpv.tv_usec + " " + tpv.pinValue);

		//log("Avant ajout queue");

		q.push(tpv);

		++i;

		//delay(1); //1ms
		delayMicroseconds(100);

	} while (!stop);

	scheduler_standard();

	//ofstream myfile;
	//myfile.open ("example.txt");

	FILE* fout = fopen(csvFilePath.c_str(), "w");

	//log("Dépilage");
	do
	{
		//cout<<q.size()<<endl;
		timePinVal tpv = q.front();
		q.pop();
		//cout << tpv.index << ";" << tpv.tv_usec << ";" << tpv.pinValue << endl;
		//myfile << tpv.index << ";" << tpv.tv_usec << ";" << tpv.pinValue << '\n';
		fprintf(fout, "%u;%u\n", tpv.index, tpv.pinValue);

	} while(!q.empty());

	//myfile.close();

	fclose(fout);

	return 0;
}


