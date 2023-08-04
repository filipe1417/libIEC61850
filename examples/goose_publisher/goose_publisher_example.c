/*
 * goose_publisher_example.c
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

#include "mms_value.h"
#include "goose_publisher.h"
#include "hal_thread.h"

/* has to be executed as root! */

//void eventoOcorreu(){

//}

int
main(int argc, char **argv)
{
    char *interface;

    if (argc > 1)
        interface = argv[1];
    else
        interface = "eth0";

    printf("Using interface %s\n", interface);

    LinkedList dataSetValues = LinkedList_create();
    LinkedList dataSetValuesAuxiliar = LinkedList_create();


    LinkedList_add(dataSetValues, MmsValue_newIntegerFromInt32(1234));
    LinkedList_add(dataSetValues, MmsValue_newBinaryTime(false));
    LinkedList_add(dataSetValues, MmsValue_newIntegerFromInt32(5678));

    dataSetValuesAuxiliar = dataSetValues;
    bool eventoOcorreu = false;


    CommParameters gooseCommParameters;

    gooseCommParameters.appId = 1000;
    gooseCommParameters.dstAddress[0] = 0x01;
    gooseCommParameters.dstAddress[1] = 0x0c;
    gooseCommParameters.dstAddress[2] = 0xcd;
    gooseCommParameters.dstAddress[3] = 0x01;
    gooseCommParameters.dstAddress[4] = 0x00;
    gooseCommParameters.dstAddress[5] = 0x01;
    gooseCommParameters.vlanId = 0;
    gooseCommParameters.vlanPriority = 4;

    /*
     * Create a new GOOSE publisher instance. As the second parameter the interface
     * name can be provided (e.g. "eth0" on a Linux system). If the second parameter
     * is NULL the interface name as defined with CONFIG_ETHERNET_INTERFACE_ID in
     * stack_config.h is used.
     */
    GoosePublisher publisher = GoosePublisher_create(&gooseCommParameters, interface);

    if (publisher) {
        GoosePublisher_setGoCbRef(publisher, "simpleIOGenericIO/LLN0$GO$gcbAnalogValues");
        GoosePublisher_setConfRev(publisher, 1);
        GoosePublisher_setDataSetRef(publisher, "simpleIOGenericIO/LLN0$AnalogValues");
        GoosePublisher_setTimeAllowedToLive(publisher, 500);



        int i = 0;
        int tempoDeEspera = 2000;
        int tempoAuxiliar = tempoDeEspera;

        while (dataSetValuesAuxiliar == dataSetValues){
        
            if (eventoOcorreu){
                tempoAuxiliar = tempoDeEspera / 8;
                while (tempoAuxiliar < tempoDeEspera) {
                    if (GoosePublisher_publish(publisher, dataSetValues) == -1) {
                        printf("Error sending message!\n"); 
                    }
                    Thread_sleep(tempoAuxiliar);
                    tempoAuxiliar *= 2;
                }
                eventoOcorreu = false;
            }

            if (GoosePublisher_publish(publisher, dataSetValues) == -1) {
                printf("Error sending message!\n"); 
            }
            Thread_sleep(tempoDeEspera);
            i ++;

            if (i == 4){
                eventoOcorreu = true;
            }
            
            
            
            
            /*if (GoosePublisher_publish(publisher, dataSetValues) == -1) {
                printf("Error sending message!\n"); 
            }

            if (i == 4){
                printf("Evento aconteceu!!");
                tempoDeEspera /= 16;
            }

            else if (i == 8){
                tempoDeEspera *= 4;
            }

            else if (i == 10){
                tempoDeEspera *= 4;
            }

            
            Thread_sleep(tempoDeEspera);
            */
        }
        
        

            GoosePublisher_destroy(publisher);
    }
    else {
        printf("Failed to create GOOSE publisher. Reason can be that the Ethernet interface doesn't exist or root permission are required.\n");
    }

    LinkedList_destroyDeep(dataSetValues, (LinkedListValueDeleteFunction) MmsValue_delete);

    return 0;
}




