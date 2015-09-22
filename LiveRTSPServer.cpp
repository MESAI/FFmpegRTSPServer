//
//  LiveRTSPServer.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "LiveRTSPServer.h"

namespace MESAI
{
	LiveRTSPServer::LiveRTSPServer( FFmpegH264Encoder * a_Encoder, int port, int httpPort )
		: m_Encoder (a_Encoder), portNumber(port), httpTunnelingPort(httpPort)
	{
		quit = 0;
	}

	LiveRTSPServer::~LiveRTSPServer()
	{

	}

	void LiveRTSPServer::run()
	{
		TaskScheduler    *scheduler;
		UsageEnvironment *env ;
		char RTSP_Address[1024];
		RTSP_Address[0]=0x00;

        scheduler = BasicTaskScheduler::createNew();
        env = BasicUsageEnvironment::createNew(*scheduler);
        
        UserAuthenticationDatabase* authDB = NULL;
        
        // if (m_Enable_Pass){
        // 	authDB = new UserAuthenticationDatabase;
        // 	authDB->addUserRecord(UserN, PassW);
        // }
        
        OutPacketBuffer::maxSize = 2000000;
        RTSPServer* rtspServer = RTSPServer::createNew(*env, portNumber, authDB);
        
        if (rtspServer == NULL)
        {
            *env <<"LIVE555: Failed to create RTSP server: %s\n", env->getResultMsg();
        }
        else {
            
            
            if(httpTunnelingPort)
            {
                rtspServer->setUpTunnelingOverHTTP(httpTunnelingPort);
            }
            
            char const* descriptionString = "MESAI Streaming Session";
            
            FFmpegH264Source * source = FFmpegH264Source::createNew(*env,m_Encoder);
            StreamReplicator * inputDevice = StreamReplicator::createNew(*env, source, false);
            
            ServerMediaSession* sms = ServerMediaSession::createNew(*env, RTSP_Address, RTSP_Address, descriptionString);
            sms->addSubsession(MESAI::LiveServerMediaSubsession::createNew(*env, inputDevice));
            rtspServer->addServerMediaSession(sms);
            
            char* url = rtspServer->rtspURL(sms);
            *env << "Play this stream using the URL \"" << url << "\"\n";
            delete [] url;
            
            //signal(SIGNIT,sighandler);
            env->taskScheduler().doEventLoop(&quit); // does not return
            
            Medium::close(rtspServer);
            Medium::close(inputDevice);
        }
        
        env->reclaim();
        delete scheduler;
	}
}