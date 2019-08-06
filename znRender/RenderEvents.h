#pragma once

#include "common.h"

// FORWARD BEGIN
class Camera;
class PipelineState;
class Viewport;
// FORWARD END

class OW_ENGINE_API UpdateEventArgs : public EventArgs
{
	typedef EventArgs base;
public:
	UpdateEventArgs
    (
        const Object& caller, 
        float DeltaTime, 
        float TotalTime, 
        uint64_t FrameCounter
    )
		: base(caller)
		, ElapsedTime(DeltaTime)
		, TotalTime(TotalTime)
        , FrameCounter(FrameCounter)
	{}

	float                                           ElapsedTime;
	float                                           TotalTime;
    int64_t                                         FrameCounter;
};
typedef Delegate<UpdateEventArgs> UpdateEvent;



class OW_ENGINE_API RenderEventArgs : public EventArgs
{
	typedef EventArgs base;
public:
	RenderEventArgs
    (
        const Object& Caller, 
        float DeltaTime, 
        float TotalTime, 
        uint64_t FrameCounter,
        const Camera* Camera,
        const PipelineState* PipelineState,
        const Object* Node
    )
		: base(Caller)
		, ElapsedTime(DeltaTime)
		, TotalTime(TotalTime)
		, FrameCounter(FrameCounter)

        , Camera(Camera)
        , PipelineState(PipelineState)
        , Node(Node)
	{}

	float                                           ElapsedTime;
	float                                           TotalTime;
	int64_t                                         FrameCounter;

    const Camera*                                   Camera;
    const PipelineState*                            PipelineState;
    const Object*                                   Node;
};
typedef Delegate<RenderEventArgs> RenderEvent;
