/*
 * load_plan.h
 *
 *  Created on: Dec 30, 2019
 *      Author: vemol
 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include <time.h>

#ifndef LOADPLAN_H_
#define LOADPLAN_H_




typedef unsigned int node;

struct node_list {

	node nInL;
	struct node_list *next;

};


typedef struct node_list nodeList;

 struct contact{
  	// parametros minimos para UN contacto ISL

  	node source;
  	node destination;
  	time_t start;
  	time_t end;
  	unsigned int rate;

  	//work area
	time_t arrivalTime;
	unsigned int capacity;
	unsigned int residualCapacity;
	unsigned int confidence;
	int visited;		//boolean
	int suppressed;
	struct contact *predecessor;	//para guardar el contacto anterior
	struct contact *sucessor;	//para guardar el contacto siguiente
	nodeList *visitedNodes;

	struct contact *next;
  	struct contact *prev;

  };

 typedef struct contact Contact;

Contact *load_plan(int );

#endif /* LOADPLAN_H_ */
