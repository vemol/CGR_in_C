/*
 * cgr.h
 *
 *  Created on: Dec 30, 2019
 *      Author: vemol
 */

#include "loadPlan.h"

#ifndef CGR_H_
#define CGR_H_



#define CGRDEBUG 0
#define FALSE 0
#define TRUE 1
#define NO 0
#define YES 1
#define INF 5000000
//#define NULL 0


typedef struct prx_node{
	node node;
	time_t arrivalTime;
	unsigned int hopCount;
	char *id;
}prxnode;

struct prxnode_list {

	prxnode prxmNode;
	struct prxnode_list *next;

};

typedef struct prxnode_list prxnodeList;



 struct contactList{

	 Contact *contact;
	 struct contactList *prev;
	 struct contactList *next;

 };


struct routeStr{

	 Contact *ctcList;
	 node firstHop;
	 nodeList *hops;
	 unsigned int hopCount;
	 time_t toTime;
	 time_t arrivalTime;
	 unsigned int capacity;

 };


struct routeList{

	struct routeStr *route;
	//struct routeList *prev;
	struct routeList *next;
};

 typedef struct {

	 char *data;
	 unsigned int bitlength;

	 time_t startTime;
	 time_t deadline;
	 node startnode;
	 node destination;	// no confundir destino de contacto con destino de bundle

	 // metricas para tomar futuras decisiones
	 int critical;

 }Bundle;



 nodeList *addToNodeList(nodeList *, node);
 prxnodeList *addToPrxNodeList(prxnodeList *,prxnode);
 struct routeList *appendRoute(struct routeList *, struct routeStr *);

 void cgrForward(Bundle *bundle, Contact *cp, nodeList *En);
 struct routeList *cgrAllPathsAnchor(node, node, time_t, Contact *);
 void clearWorkAreas(Contact *);
 void clearWorkArea(Contact *);
 Bundle *create_bundle(node, node);
 struct contactList *createNeighborList(Contact *cp);
 struct routeStr *dijkstra(Contact *, node , Contact *);
 //void dijkstra(Contact *, node , Contact *);
 struct routeList *first_depleted(node, node, time_t , Contact *, int);

 unsigned int isInNodeList(nodeList *, node);
 unsigned int isInPrxNodeList(prxnodeList *, node);
 prxnodeList *identifyProximateNodes(Bundle *, Contact *, nodeList *);
 time_t min(time_t, time_t);
 Contact *nextContact(node, Contact *cp);
 //para determinar el proximo contacto del destino con la siguiente fuente
 //destino = siguiente fuente (la misma ruta pero siguiente salto)
 Contact *nextNeighbor(Contact *cp);
 //para determinar los contactos alternativos o vecinos
 //fuente de contacto = fuente del suiguiente contacto, (ruta alternativa)


 struct routeList *loadRouteList(Bundle *, Contact *);

 void printRoute(struct routeStr *);
 void printRoutes(struct routeList *);
 void printCtc(Contact *);
 void printCp(Contact *);
 void memCount(size_t);
 void memFreedCount(size_t);

 //total de memoria liberada
 size_t freedMem;
 //memoria dinamica maxima utilizada
 size_t totalMem;








#endif /* CGR_H_ */
