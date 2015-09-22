//
//  FFmpegH264Source.h
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#ifndef MESAI_FFMPEGH264_SOURCE_HH
#define MESAI_FFMPEGH264_SOURCE_HH


#include <functional>
#include <FramedSource.hh>
#include <UsageEnvironment.hh>
#include <Groupsock.hh>
#include "FFmpegH264Encoder.h"

namespace MESAI
{
    
  class FFmpegH264Source : public FramedSource {
  public:
    static FFmpegH264Source* createNew(UsageEnvironment& env, FFmpegH264Encoder * E_Source);
    FFmpegH264Source(UsageEnvironment& env, FFmpegH264Encoder *  E_Source);
    ~FFmpegH264Source();

  private:
    static void deliverFrameStub(void* clientData) {((FFmpegH264Source*) clientData)->deliverFrame();};
    virtual void doGetNextFrame();
    void deliverFrame();
    virtual void doStopGettingFrames();
    void onFrame();

    
  private:
    FFmpegH264Encoder * Encoding_Source;
    EventTriggerId m_eventTriggerId;

  };

}
#endif
