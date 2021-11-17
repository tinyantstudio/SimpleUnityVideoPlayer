using System.Collections;
using System.Collections.Generic;
using UnityEngine;


// 
// video player 
// 

public class BaseVideoPlayer
{
    public virtual void InitVideoPlayer()
    {
    }

    public virtual void PrepreVideo(string filepath)
    {
    }

    public virtual void StartVideo()
    {
    }

    public virtual void Pause()
    {
    }

    public virtual void Resume()
    {
    }


    public virtual void Stop()
    {
    }
    
    public virtual void ShutDownVideoPlayer()
    {
    }
}

public class TinyVideoPlayer : BaseVideoPlayer
{
    
    
}