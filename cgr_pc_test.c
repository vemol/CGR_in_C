/*
 ============================================================================
 Name        : cgr_pc_test.c
 Author      : Vega, Blas
 Date		 : 10/10/19
 Version     :
 Copyright   : Free for all
 Description : C, Ansi-style. Simple CGR implementation for testing and future developtments
 ============================================================================
 */



#include "cgr.h"

int main (void) {
	clock_t start, stop;

	Contact *cp, *cpaux;
	Bundle *bundle;

	nodeList *En=NULL;

	//creo el supuesto dato con todos sus parametros
	bundle=create_bundle(20,65);
	//agrego los nodos que debe excluir
	node exNode = 3;
	En = addToNodeList(En, exNode);
	int cantMaxNodos = 1500;
	for(int i = 600; i<= cantMaxNodos; i+=50 ){
		printf("En %d nodos: ",i);
		// cargo el plan
		cp=load_plan(i);	printf("Plan cargado\n");
		//start counter
		start = clock();
		//start CGR
		cgrForward(bundle, cp, En);
		//stop counter
		stop = clock();

		printf("Tiempo de ejecución:\t%lu\n",stop-start);

		fflush(stdin);

		//liberar toda la memoria

		cpaux = cp;
		for( ;cpaux != NULL; ){
			cp = cpaux;
			cpaux=cpaux->next;

			//destroy routes

			if(!cp->sucessor) free(cp->sucessor);
			if(!cp->predecessor) free(cp->predecessor);
			if(!cp->visitedNodes) free(cp->visitedNodes);

			free(cp);
		}
	}


}


nodeList *addToNodeList(nodeList *nl,  node nodo){

	nodeList *nw;
	nw = malloc(sizeof(nodeList));
	if(nw == NULL){
		printf("malloc fail");

	}
	nw->nInL = nodo;
	nw->next=nl;
	return nw;
}

prxnodeList *addToPrxNodeList(prxnodeList *pnl, prxnode nodo){

	prxnodeList *nw;
	nw = malloc(sizeof(prxnodeList));
	if(nw == NULL){
			printf("malloc fail");

	}
	nw->prxmNode = nodo;
	nw->next=pnl;
	return nw;

}
struct routeList *appendRoute(struct routeList *rts, struct routeStr *Rt){

	//if(!rts) printf("Recibo ruta nula\n");
	struct routeList *nwRt;
	nwRt = malloc(sizeof(struct routeList));
	if(nwRt == NULL) printf("malloc in appendRoute fail");
	struct routeStr *rt = malloc(sizeof(struct routeStr));
	if(rt == NULL) printf("malloc in appendRoute fail");

	nwRt->route = rt;
	nwRt->next = NULL;

	Contact *ctcHead,*ctc, *rtx;
	ctc = malloc(sizeof(Contact));
	if(ctc == NULL) printf("malloc in appendRoute fail");
	ctcHead = ctc;

	for(rtx = Rt->ctcList ;rtx !=NULL; rtx = rtx->sucessor){

		ctc->source = rtx->source;
		ctc->destination = rtx->destination;
		ctc->start = rtx->start;
		ctc->end = rtx->end;
		ctc->rate = rtx->rate;

		//work area
		ctc->arrivalTime = rtx->arrivalTime;
		ctc->capacity = rtx->capacity;
		ctc->residualCapacity = rtx->residualCapacity;
		ctc->confidence = rtx->confidence;
		ctc->visited = rtx->visited;		//boolean
		ctc->suppressed = rtx->suppressed;
		ctc->visitedNodes = rtx->visitedNodes;
		ctc->sucessor = NULL;
		//ctc->predecessor = rtx->ctcList->predecessor;	//para guardar el contacto anterior
		if(!rtx->sucessor) break;
		ctc->sucessor = malloc(sizeof(Contact));
		ctc = ctc->sucessor; //para guardar el contacto siguiente

	}

	nwRt->route->ctcList = ctcHead;
	nwRt->route->arrivalTime = Rt->arrivalTime;
	nwRt->route->capacity	= Rt->capacity;
	nwRt->route->firstHop	= Rt->firstHop;
	nwRt->route->hopCount	= Rt->hopCount;
	nwRt->route->hops		= Rt->hops;
	nwRt->route->toTime		= Rt->toTime;

	//nwRt->route = rt;
	//nwRt->route->ctcList = ctcHead;

	nwRt->next  = rts;

	return nwRt;
}

void clearWorkAreas(Contact *cp) {

		for ( ; cp!=NULL; cp=cp->next){

			cp->arrivalTime = LONG_MAX;
			cp->capacity	= 0;
			cp->predecessor = NULL;
			cp->visited 	= FALSE;
			cp->residualCapacity = cp->capacity;
			cp->visitedNodes=NULL;

		}
#if CGRDEBUG
		//printf("WorkAreas Cleared\n");
#endif
}
void clearWorkArea(Contact *cp) {

			cp->arrivalTime = LONG_MAX;
			cp->predecessor = NULL;
			cp->visited 	= FALSE;
			cp->residualCapacity = cp->capacity;
			cp->visitedNodes=NULL;
}

Bundle *create_bundle(node source, node dest){

	Bundle *bundle;

	char data[] = "CGR_IS_THE_FUTURE";

	bundle=malloc(sizeof(Bundle));
	bundle->data		= malloc(sizeof(data));
	if(bundle->data == NULL){
		printf("malloc in createBundle fail");

	}
	bundle->data		= data;
	bundle->startnode	= source;
	bundle->destination	= dest;
	bundle->startTime	= 100;
	bundle->deadline	= 10000;
	bundle->bitlength 	= sizeof(bundle->data)*8;

	bundle->critical = 0;

	return bundle;// envio solo la direccion de memoria
}

struct contactList *createNeighborList(Contact *cp){

	struct contactList *head,*neighborList;

	Contact *currCtc, *prevCtc; // para recorrer el contact plan
	currCtc = prevCtc = NULL;
	unsigned int minNode, maxNode;
	maxNode = 0;	minNode= UINT_MAX;

	neighborList = malloc(sizeof(struct contactList));
	if(neighborList == NULL){
						printf("malloc in createNeighborList fail");

					}

	head=neighborList;

	// busco el mayor y el menor nodo.
	for(currCtc = cp; currCtc != NULL ; currCtc=currCtc->next){
		if (currCtc->source < minNode) minNode = currCtc->source;
		if (currCtc->source > maxNode) maxNode = currCtc->source;
	}
	for(currCtc = cp; currCtc->source != minNode ; currCtc=currCtc->next)
		;
	prevCtc = currCtc;
	while(TRUE){

		for(currCtc = cp ; currCtc != NULL ; currCtc= currCtc->next){
			if(currCtc->source == minNode ){

				neighborList->contact = currCtc;
/*
				printf("%u,\t%u->%u\n",neighborList->contact->NodeNbr,
						neighborList->contact->source,
						neighborList->contact->destination);
*/
				neighborList->next=malloc(sizeof(struct contactList));
				if(neighborList->next == NULL){
					printf("malloc in createNeighborList fail");

				}
				neighborList->next->prev=neighborList;
				neighborList=neighborList->next;

			}
		}

		//avanzo
		if(minNode == maxNode) break;
		if(currCtc == NULL){
			currCtc = cp;
			minNode++;

		}



	}
	neighborList->prev->next=NULL;

	return head;
}

struct routeList *cgrAllPathsAnchor(node src, node dest, time_t currTime, Contact *cp){
//Not well implemented, better use first_depleted
	struct routeStr *Rt=NULL;
	struct routeList *routes=NULL;
	Contact *curCtc, *limitCtc, *anchorCtc, *rootCtc, *firstContact;
	curCtc = limitCtc = anchorCtc = rootCtc = firstContact = NULL;



	// create rootcontact
	rootCtc = malloc(sizeof(Contact));
	if(rootCtc == NULL){
		printf("malloc in cgrAllPathsAnchor fail");

	}
	rootCtc->source = src;
	rootCtc->destination = src;
	rootCtc->predecessor = NULL;
	rootCtc->sucessor	 = NULL;
	rootCtc->next		 = NULL;
	rootCtc->arrivalTime = currTime;

	// reset suppresed contacts
	curCtc=cp;
	while(curCtc!=NULL){
		clearWorkArea(curCtc);
		curCtc->suppressed = FALSE;
		curCtc=curCtc->next;
	}

	while (TRUE){

		Rt = dijkstra(rootCtc, dest, cp);

		if (!Rt) break;

		firstContact = Rt->ctcList;

		if(anchorCtc && anchorCtc != firstContact){

			for(curCtc=cp; curCtc!=NULL ; curCtc=curCtc->next){
				clearWorkArea(curCtc);
				if(curCtc->source != src)
					curCtc->suppressed = TRUE;
			}

			anchorCtc->suppressed  = TRUE;
			anchorCtc = NULL;
			continue;
		}

		routes = appendRoute(routes, Rt);
		//Busco el limiting contact
		if( Rt->toTime == firstContact->end){
			limitCtc = firstContact;
		}else{
			//the first is not a limiting contact, start anchor search
			anchorCtc = firstContact;

			for(curCtc = Rt->ctcList; curCtc!= NULL ; curCtc = curCtc->sucessor){

				if(curCtc->end == Rt->toTime){
					limitCtc = curCtc;
					break;
				}
			}
		}
		limitCtc->suppressed = TRUE;
		for(curCtc=cp;curCtc!=NULL;curCtc=curCtc->next)
			clearWorkArea(curCtc);

	}//end while

	printRoutes(routes);
	return routes;
}

void cgrForward(Bundle *bundle, Contact *cp, nodeList *En){

	prxnode nextHop;
	prxnodeList *Pn, *Pnaux;
	Pn=identifyProximateNodes(bundle, cp, En);

	printf("Proximate nodes: ");
	for(prxnodeList *prxn = Pn; prxn!=NULL;prxn=prxn->next)
	{
		printf(" %u,",prxn->prxmNode.node);

	}
	printf("\n");

	if(bundle->critical) //enqueue a copy of B to each node in Pn

		return ;

	nextHop.node = 0;
	nextHop.hopCount =0;
	nextHop.arrivalTime = ULONG_MAX;
	nextHop.id = 0;

	for(prxnodeList *prxn = Pn; prxn!=NULL;prxn=prxn->next)
	{
		if (nextHop.node == 0) {
			nextHop = prxn->prxmNode;

		}else if(prxn->prxmNode.arrivalTime < nextHop.arrivalTime) {

			nextHop = prxn->prxmNode;

		}else if(prxn->prxmNode.arrivalTime > nextHop.arrivalTime)

			continue;

		 else if(prxn->prxmNode.hopCount < nextHop.hopCount) {

			 nextHop = prxn->prxmNode;

		}else if(prxn->prxmNode.hopCount > nextHop.hopCount)

			continue;

		else if(prxn->prxmNode.id < nextHop.id){

			nextHop = prxn->prxmNode;
		}

	}

	if (nextHop.node != 0) {

		printf("Next hop : %u,",nextHop.node);
	}
	printf("\n");
	fflush(stdin);

	Pnaux = Pn;
	for(;Pnaux != NULL; ){
		Pn = Pnaux;
		Pnaux = Pn->next;
		free(Pn);
	}

}
struct routeStr *dijkstra(Contact *root, node dest, Contact *plan) {
	#if CGRDEBUG
		printf(" Comienza Dijkstra\n");
	#endif

	time_t bestArrTime, bestFinArrTime, arrTime;
	time_t rtToTime=LONG_MAX;
	Contact *finalContact, *current, *next, *ctc;
	finalContact = current = next = ctc = NULL;
	struct routeStr *Route = NULL;
	struct contactList *neighborList,*nlist ;
	neighborList = nlist = createNeighborList(plan);
	nodeList *nl;
	bestFinArrTime=LONG_MAX;
	current = root;

	if (!isInNodeList(root->visitedNodes,root->destination)){
		root->visitedNodes = addToNodeList(root->visitedNodes, root->destination);
	}

	#if CGRDEBUG
		printf(" Start Loop\n");
	#endif
	while(TRUE){

		for(neighborList=nlist;neighborList!=NULL;neighborList=neighborList->next){

			if(current->destination == neighborList->contact->source){

				//ignore supressed, visited contacts
				//if(neighborList->contact->sucessor && neighborList->contact->suppressed)
				//	continue;
				if(neighborList->contact->visited || neighborList->contact->suppressed)
					continue;
				if(neighborList->contact->end <= current->arrivalTime)
					continue;
				//ignore contacts that leads to a previously visited nodes
				if(isInNodeList(current->visitedNodes,neighborList->contact->destination))
					continue;
				//actualizo la capacidad de envio de datos
				if(neighborList->contact->capacity==0)
					neighborList->contact->capacity = neighborList->contact->rate*(neighborList->contact->end - neighborList->contact->start);
				//calculo el costo (arrival time)
				if(neighborList->contact->start < current->arrivalTime){

					arrTime	= current->arrivalTime ;

				}else
					arrTime = neighborList->contact->start;

				//actualizo el costo si hay uno mejor o igual

				if(arrTime <= neighborList->contact->arrivalTime){

					neighborList->contact->arrivalTime = arrTime;
					neighborList->contact->predecessor = current;

					nl = neighborList->contact->visitedNodes;
					for( ;nl != NULL; ){
						neighborList->contact->visitedNodes = nl;
						nl=nl->next;
						free(neighborList->contact->visitedNodes);
					}
					neighborList->contact->visitedNodes = NULL;

					for(nodeList *nl = current->visitedNodes;nl!=NULL;nl=nl->next)
						neighborList->contact->visitedNodes=addToNodeList(neighborList->contact->visitedNodes, nl->nInL);

					neighborList->contact->visitedNodes = addToNodeList(neighborList->contact->visitedNodes, neighborList->contact->destination);

					// marcar si se llego a destino
					if (neighborList->contact->destination == dest
							&& neighborList->contact->arrivalTime < bestFinArrTime){

						bestFinArrTime = neighborList->contact->arrivalTime;
						finalContact = neighborList->contact;

					}

				}

			}

		}

		current->visited = 1;

		//determinar el mejor proximo contacto en todo el plan
		bestArrTime = LONG_MAX;
		next = NULL;

		for(ctc=plan;ctc!=NULL;ctc=ctc->next){

			if(ctc->suppressed || ctc->visited)
				continue;
			if(ctc->arrivalTime > bestFinArrTime)
				continue;
			if(ctc->arrivalTime < bestArrTime)
			{
				bestArrTime = ctc->arrivalTime;
				next = ctc;
			}

		}

		if(next==NULL) break;
		current = next;
	#if CGRDEBUG
		printf(" next != NULL\n");
	#endif

	}

	if(finalContact != NULL){

		ctc=finalContact;
		while(ctc->predecessor != root && ctc->predecessor != NULL)
					ctc=ctc->predecessor;
		ctc->predecessor = NULL;
		finalContact->sucessor = NULL;

		for(current=finalContact;current!=NULL;current=current->predecessor){
			if(current->predecessor != NULL) {
				ctc=current->predecessor;
				ctc->sucessor = current;
			}
			if(current->end < rtToTime) rtToTime = current->end;
		}

		//ahora anoto parametros relevantes de la ruta
		Route = (struct routeStr *)malloc(sizeof(struct routeStr));
		if(Route == NULL){
			printf("malloc in Route dijkstra fail");

		}

		Route->ctcList	= ctc;
		Route->firstHop	=ctc->destination;
		Route->hopCount =0;
		Route->toTime	=rtToTime;
		Route->arrivalTime= ctc->arrivalTime;
		Route->capacity =  UINT_MAX;
		for(current=ctc; current!=NULL;current=current->sucessor){

			Route->hops = addToNodeList(Route->hops,current->destination);
			Route->hopCount++;
			if(current->capacity <= Route->capacity) Route->capacity = current->capacity;
		}
	}

	nlist = neighborList;
	for(;nlist != NULL;){
		neighborList = nlist;
		nlist=nlist->next;
		free(neighborList);

	}
	//printRoute(Route);
	return Route;
 }

struct routeList *first_depleted(node src, node dest, time_t currTime, Contact *cp, int keep_residual_capacity){
	printf("first_depleted\n");
	int residualCapacity;
	struct routeStr *Rt=NULL;
	struct routeList *routes=NULL;
	Contact *curCtc, *rootCtc;
	curCtc = rootCtc = NULL;
	// create rootcontact
	rootCtc = malloc(sizeof(Contact));
	if(rootCtc == NULL){
		printf("malloc in first_depleted fail");
	}
	rootCtc->source = src;
	rootCtc->destination = src;
	rootCtc->start = 0;
	rootCtc->end = LONG_MAX;
	rootCtc->confidence =1;
	rootCtc->rate = 100;
	rootCtc->capacity = rootCtc->rate*(rootCtc->start - rootCtc->end);
	rootCtc->predecessor = NULL;
	rootCtc->sucessor	 = NULL;
	rootCtc->next		 = NULL;
	rootCtc->arrivalTime = currTime;
	rootCtc->visitedNodes= NULL;

	//reset suppressed contacts
	curCtc=cp;
	while(curCtc!=NULL){
		curCtc->residualCapacity= curCtc->capacity;
		residualCapacity = curCtc->residualCapacity;
		clearWorkArea(curCtc);
		curCtc->suppressed = FALSE;

		if(keep_residual_capacity){
			curCtc->residualCapacity = residualCapacity;
		}
		curCtc=curCtc->next;
	}

	while(TRUE){

		Rt = dijkstra(rootCtc, dest, cp);

		if(!Rt) break;
		//consume volume in all hops and supress limiting hop
		for(curCtc = Rt->ctcList;curCtc != NULL; curCtc = curCtc->next){
			curCtc->residualCapacity -= Rt->capacity;

			if(curCtc->residualCapacity == 0){
				//printf(" \n %u \n", curCtc->residualCapacity);
				curCtc->suppressed = TRUE;

			}
		}

		routes = appendRoute(routes, Rt);

		free(Rt);
	}

	free(rootCtc);
	return routes;

}

prxnodeList *identifyProximateNodes(Bundle *bundle, Contact *cp, nodeList *En){

	prxnodeList *Pn=NULL;	//Proximate node list
	prxnode pn;
	prxnodeList *nl;
	struct routeList*rtl, *Rl=NULL;

	time_t currTime = 80;

	if(!Rl) Rl=loadRouteList(bundle,cp);
	printRoutes(Rl);


	for(rtl=Rl;rtl!=NULL;rtl=rtl->next){
		//ignore past route
		if(rtl->route->toTime <= currTime)					continue;
		//route arrives late
		if(rtl->route->arrivalTime >= bundle->deadline) 	continue;
		//not enough capacity
		if(rtl->route->capacity < bundle->bitlength)		continue;
		//next hop is excluded
		if( isInNodeList(En, rtl->route->ctcList->destination))	 		continue;
		//outbound queue depleted
		if((rtl->route->capacity) < bundle->bitlength)					continue;
		//printRoute(rtl->route);
		for(nl=Pn;(nl!= NULL);nl=nl->next){

			if(nl->prxmNode.node == rtl->route->ctcList->destination){


				if(nl->prxmNode.arrivalTime > rtl->route->arrivalTime){
					nl->prxmNode.node = rtl->route->ctcList->destination;
					nl->prxmNode.arrivalTime = rtl->route->ctcList->arrivalTime;
				}
				//previous route was better
				else if(nl->prxmNode.arrivalTime<rtl->route->arrivalTime){
					continue;
				}

				else if(nl->prxmNode.hopCount > rtl->route->hopCount){
					//previous route was better
					continue;
				}

				else if(nl->prxmNode.hopCount < rtl->route->hopCount){
					continue;
					//previous route was better
				}
				break;
			}
		}
		//if route.nextHop does not belong to Pn
		if(!isInPrxNodeList(Pn,rtl->route->ctcList->destination)){
			//printf("%u,",Pn->prxmNode.node);
			pn.node = rtl->route->ctcList->destination;
			pn.arrivalTime = rtl->route->ctcList->arrivalTime;
			pn.hopCount = rtl->route->hopCount;
			Pn = addToPrxNodeList(Pn, pn);

		}

	}


	//clear memory


	rtl = Rl;
	for(;rtl != NULL ;){
		Rl=rtl;
		rtl = Rl->next;
		free(Rl->route->hops);
		free(Rl);

	}
	return Pn;
}



 unsigned int isInNodeList(nodeList *n, node nd){
	 nodeList *nl;
	for (nl=n;nl!=NULL;nl=nl->next)
		 if (nd == nl->nInL){
			return 1;
		 }
	return 0;
 }
unsigned int isInPrxNodeList(prxnodeList *n, node nd){
	for (prxnodeList *nl=n; nl!=NULL; nl=nl->next){
	 if (nd == nl->prxmNode.node){
			return 1;
		 }
	}
	return 0;
 }



 struct routeList *loadRouteList(Bundle *bundle, Contact *cp){

 	struct routeList *Rl;
 	int keep_residual_capacity =1;
 						//node, node, time_t, Contact *
 	//Rl = cgrAllPathsAnchor(20, 65,80, cp);
 	Rl = first_depleted(bundle->startnode, bundle->destination, bundle->startTime, cp, keep_residual_capacity);

 	return Rl;
 }

 void memCount(size_t xWantedSize){
	 totalMem += xWantedSize;
 }

 void memFreedCount(size_t pv){
	 totalMem -= pv;
	 freedMem += pv;

 }

 Contact *nextNeighbor(Contact *cp){
 	Contact *aux;

 	for(aux=cp;aux!=NULL;aux=aux->next)
 		if(cp->source == aux->source){
 			//printf("El siguiente vecino de %c es en %d\n", cp->source,aux->NodeNbr);
 			return aux;
 		}
     //printf(" No hay vecino entonces retorno NULL\n");
 	return NULL;

 }

 void printRoute(struct routeStr *rt){
	if (!rt) return;
	Contact *p;
 	printf("Desde 	hasta	range\n");
 	for(p=rt->ctcList; p!=NULL; p=p->sucessor)
 	printf("%u\t%u\t%ld-%ld\n",p->source,p->destination,p->start,p->end);

 }
 void printRoutes(struct routeList *routes){
	if (!routes)	return;
	 struct routeList *rts;
	 //Contact *p;
	 //taskENTER_CRITICAL(); //for FreeRTOS
	 printf("\n****Impresion de rutas****\n");


	 for(rts=routes;rts!=NULL;rts= rts->next){

		printRoute(rts->route);
		if(rts->next) printf("next Route\n");
	 }
	 printf("**************************\n");
	 //taskEXIT_CRITICAL(); //for FreeRTOS
 }
 void printCtc(Contact *p){

	 //if(!p) return ;
	 //printf("Desde 	hasta	start	end	rate\n");
	 printf("%u->%u(%ld-%ld)\t%u\n",
	  				p->source, p->destination,
	 				p->start, p->end,
	 				p->rate);
	 //printf(" art, cpt, vtd, spd\n");
	 //printf("%ld, %u, %u, %u\n",p->arrivalTime,
	//		 p->capacity, p->visited, p->suppressed);

 }

 void printCp(Contact *cp){
	Contact *p;
 	printf("Desde 	hasta	start	end	rate\n");
 	for(p=cp; p!=NULL; p=p->next)
 		printf("%u\t%u\t%ld\t%ld\t%u\n",
 				p->source, p->destination,
				p->start, p->end,
				p->rate);

 }

