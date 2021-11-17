using System.Collections;
using System.Collections.Generic;
using System.Diagnostics;
using UnityEngine;

public static class SimpleDebuger
{
    private const string UnityLogPrefix = "YDUnity";

    [Conditional("VERBOSE_LOG")]
    public static void LogVerbose(string tag, string message)
    {
        UnityEngine.Debug.Log(string.Format("[{0}][VERB][{1}]{2}", UnityLogPrefix, tag, message));
    }

    [Conditional("DEBUG")]
    public static void LogDebug(string tag, string message)
    {
        UnityEngine.Debug.Log(string.Format("[{0}][DEBUG][{1}]{2}", UnityLogPrefix, tag, message));
    }
    
    public static void LogInfo(string tag, string message)
    {
        UnityEngine.Debug.Log(string.Format("[{0}][INFO][{1}]{2}", UnityLogPrefix, tag, message));
    }

    public static void LogWarn(string tag, string message)
    {
        UnityEngine.Debug.LogWarning(string.Format("[{0}][WARN][{1}]{2}", UnityLogPrefix, tag, message));
    }

    public static void LogError(string tag, string message)
    {
        UnityEngine.Debug.LogError(string.Format("[{0}][ERRO][{1}]{2}", UnityLogPrefix, tag, message));
    }
}