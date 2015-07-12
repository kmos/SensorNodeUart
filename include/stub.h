/*
 * stub.h
 *
 *  Created on: 07/lug/2015
 *      Author: ntonjeta
 */

#ifndef STUB_H_
#define STUB_H_



typedef enum{
	Asynchronous = 0, //garantito
	Real_Time, 		 // garantito
	Best_Effort		// non garantito
} ServiceType;

//PROTOTIPI
int initrete();
void send_segment(uint8_t* packet,int bytelen, ServiceType Servizio);

/* Firme livello di rete
 SEND utilizzata dal livello superiore
void send_segment(uint8_t* packet,int bytelen, ServiceType Servizio,segment_port_t srcPort,segment_port_t dstPort,segment_timeout_t timeout);

 callback che si attiva quando si riceve un pacchetto
void on_receive_segment(uint8_t* packet, segment_port_t srcPort,segment_port_t dstPort);
*/


// ****** FUNZIONI *******

void send_segment(uint8_t* packet,int bytelen, ServiceType Servizio){}





#endif /* STUB_H_ */
