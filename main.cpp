//
//  main.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "LiveRTSPServer.h"
#include "FFmpegH264Encoder.h"
#include "FFmpegDecoder.h"

MESAI::FFmpegH264Encoder * encoder;
MESAI::LiveRTSPServer * server;
MESAI::FFmpegDecoder * decoder;

int UDPPort;
int HTTPTunnelPort;
pthread_t thread1;
pthread_t thread2;


void * runServer(void * server)
{
    ((MESAI::LiveRTSPServer * ) server)->run();
    pthread_exit(NULL);
}

void * runEncoder(void * encoder)
{
    ((MESAI::FFmpegH264Encoder * ) encoder)->run();
    pthread_exit(NULL);
}

void onFrame(uint8_t * data)
{
    encoder->SendNewFrame(data);
}

int main(int argc, const char * argv[])
{
    if(argc==2)
        decoder = new MESAI::FFmpegDecoder(argv[1]);
    if(argc==3)
        UDPPort = atoi(argv[2]);
    if(argc==4)
        HTTPTunnelPort = atoi(argv[3]);
    decoder->intialize();
    decoder->setOnframeCallbackFunction(onFrame);
    encoder = new MESAI::FFmpegH264Encoder();
    encoder->SetupVideo("dummy.avi",decoder->width,decoder->height,decoder->frameRate,decoder->GOP,decoder->bitrate);
    server = new MESAI::LiveRTSPServer(encoder, UDPPort, HTTPTunnelPort);
    
    pthread_attr_t attr1;
    pthread_attr_init(&attr1);
    pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
    int rc1 = pthread_create(&thread1, &attr1,  runServer, server);
    
    if (rc1){
        //exception
        return -1;
    }
    
    pthread_attr_t attr2;
    pthread_attr_init(&attr2);
    pthread_attr_setdetachstate(&attr2, PTHREAD_CREATE_DETACHED);
    int rc2 = pthread_create(&thread2, &attr2,  runEncoder, encoder);
    
    if (rc2){
        //exception
        return -1;
    }
    
    // Play Media Here
    decoder->playMedia();
    decoder->finalize();
    
}
