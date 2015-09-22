//
//  FFmpegDecoder.cpp
//  FFmpegRTSPServer
//
//  Created by Mina Saad on 9/22/15.
//  Copyright (c) 2015 Mina Saad. All rights reserved.
//

#include "FFmpegDecoder.h"

namespace MESAI
{
    FFmpegDecoder::FFmpegDecoder(std::string path)
	{
        this->path = path;
	}


	void FFmpegDecoder::intialize()
	{

		// Intialize FFmpeg enviroment
	    av_register_all();
	    avdevice_register_all();
	    avcodec_register_all();
	    avformat_network_init();
	 
	    const char  *filenameSrc = path.c_str();
	 
	    pFormatCtx = avformat_alloc_context();
	 
	    AVCodec * pCodec;
	 
	    if(avformat_open_input(&pFormatCtx,filenameSrc,NULL,NULL) != 0)
	    {
			//exception
			return;
	    } 
	    
	    if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
	    {
	    	//exception
	    	return;
	    }
	    
	    av_dump_format(pFormatCtx, 0, filenameSrc, 0);
	    
	    videoStream = -1;
	    
	    for(int i=0; i < pFormatCtx->nb_streams; i++)
	    {
	        AVStream *st = pFormatCtx->streams[i];
			enum AVMediaType type = st->codec->codec_type;
			if (videoStream == -1)
				if (avformat_match_stream_specifier(pFormatCtx, st, "vst") > 0)
					videoStream = i;
	    }

	   	videoStream = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO,videoStream, -1, NULL, 0);
        
	    if(videoStream == -1) 
	    {
	     	//exception
	    	return;
	    }

	    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
	 
	    pCodec =avcodec_find_decoder(pCodecCtx->codec_id);
	    if(pCodec==NULL)
	    {
	    	//exception
	    	return;
	    }

	    pCodecCtx->codec_id = pCodec->id;
	    pCodecCtx->workaround_bugs   = 1;
	 
	    if(avcodec_open2(pCodecCtx,pCodec,NULL) < 0)
	    {
	    	 //exception
	    	return;
	    }
        
        pFrameRGB = av_frame_alloc();
        AVPixelFormat  pFormat = AV_PIX_FMT_BGR24;
        uint8_t *fbuffer;
        int numBytes;
        numBytes = avpicture_get_size(pFormat,pCodecCtx->width,pCodecCtx->height) ; //AV_PIX_FMT_RGB24
        fbuffer = (uint8_t *) av_malloc(numBytes*sizeof(uint8_t));
        avpicture_fill((AVPicture *) pFrameRGB,fbuffer,pFormat,pCodecCtx->width,pCodecCtx->height);
        
        img_convert_ctx = sws_getCachedContext(NULL,pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,   pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL,NULL);
        
        height = pCodecCtx->height;
        width =  pCodecCtx->width;
        bitrate =pCodecCtx->bit_rate;
        GOP = pCodecCtx->gop_size;
        frameRate = (int )pFormatCtx->streams[videoStream]->avg_frame_rate.num/pFormatCtx->streams[videoStream]->avg_frame_rate.den;
        

    }
    
    void FFmpegDecoder::setOnframeCallbackFunction(std::function<void(uint8_t *)> func)
    {
        onFrame = func;
    }

	
	void FFmpegDecoder::playMedia()
	{
		AVPacket packet;
        AVFrame * pFrame;
		while((av_read_frame(pFormatCtx,&packet)>=0))
    	{
        	if(packet.buf != NULL & packet.stream_index == videoStream)
        	{
                pFrame = av_frame_alloc();
                int frameFinished;
                int decode_ret = avcodec_decode_video2(pCodecCtx,pFrame,&frameFinished,&packet);
                av_free_packet(&packet);
                if(frameFinished)
                {
                    sws_scale(img_convert_ctx, ((AVPicture*)pFrame)->data, ((AVPicture*)pFrame)->linesize, 0, pCodecCtx->height, ((AVPicture *)pFrameRGB)->data, ((AVPicture *)pFrameRGB)->linesize);
                    onFrame(((AVPicture *)pFrameRGB)->data[0]);
                }
                av_frame_unref(pFrame);
                av_free(pFrame);
        	}
            usleep(((double)(1.0/frameRate))*1000000);
       
    	}
    	av_free_packet(&packet);


	}
	
	void FFmpegDecoder::finalize()
	{
        sws_freeContext(img_convert_ctx);
        av_freep(&(pFrameRGB->data[0]));
        av_frame_unref(pFrameRGB);
        av_free(pFrameRGB);
        avcodec_close(pCodecCtx);
        avformat_close_input(&pFormatCtx);
    }

}
