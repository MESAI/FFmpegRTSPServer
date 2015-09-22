//
//  LiveRTSPServer.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include <UsageEnvironment.hh>
#include <BasicUsageEnvironment.hh>
#include <GroupsockHelper.hh>
#include <liveMedia.hh>
#include "LiveServerMediaSubsession.h"
#include "FFmpegH264Source.h"
#include "FFmpegH264Encoder.h"

namespace MESAI {

	class LiveRTSPServer
	{
	public:

		LiveRTSPServer(FFmpegH264Encoder  * a_Encoder, int port, int httpPort );
		~LiveRTSPServer();
		void run();

	private:
		int portNumber;
		int httpTunnelingPort;
		FFmpegH264Encoder * m_Encoder;
		char quit;

	};
}