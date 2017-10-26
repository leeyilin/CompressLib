//####################################################################
//  Created:    2010/8/5   9:30
//  Filename:   Declaration.h 
//  Author:     viki[lize]
//********************************************************************
//  Comments:   For API Declaring and Class Declaring.
//
//  UpdateLogs: 
//####################################################################
#ifndef DECLARATION_INCLUDED
#define DECLARATION_INCLUDED

// Dynamic library or Static library.

#if defined(_WIN32) && defined(_DLL)
    #if !defined(LINUXSERVER_DLL) && !defined(LINUXSERVER_STATIC)
        #define LINUXSERVER_DLL
    #endif
#endif

// Windows dll declare.

#if defined(_WIN32) && defined(LINUXSERVER_DLL)
    #if defined(LINUXSERVER_EXPORTS)
        #define LINUXSERVER_API __declspec(dllexport)
    #else
        #define LINUXSERVER_API __declspec(dllimport)   
    #endif
#endif

// Linux does not need dll declare.

#if !defined(LINUXSERVER_API)
    #define LINUXSERVER_API
#endif

#endif
