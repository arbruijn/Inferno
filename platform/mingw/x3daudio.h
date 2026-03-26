/*
 * Copyright (c) 2015 Andrew Eikum for CodeWeavers
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#ifndef _X3DAUDIO_H
#define _X3DAUDIO_H

/*
typedef struct X3DAUDIO_VECTOR {
    float x, y, z;
} X3DAUDIO_VECTOR;
*/
#include <DirectXMath.h>
typedef DirectX::XMFLOAT3 X3DAUDIO_VECTOR;

typedef struct X3DAUDIO_CONE {
    float InnerAngle;
    float OuterAngle;
    float InnerVolume;
    float OuterVolume;
    float InnerLPF;
    float OuterLPF;
    float InnerReverb;
    float OuterReverb;
} X3DAUDIO_CONE;

typedef struct X3DAUDIO_DISTANCE_CURVE_POINT {
    float Distance;
    float DSPSetting;
} X3DAUDIO_DISTANCE_CURVE_POINT;

typedef struct X3DAUDIO_DISTANCE_CURVE {
    X3DAUDIO_DISTANCE_CURVE_POINT *pPoints;
    UINT32 PointCount;
} X3DAUDIO_DISTANCE_CURVE;

typedef struct X3DAUDIO_LISTENER {
    X3DAUDIO_VECTOR OrientFront;
    X3DAUDIO_VECTOR OrientTop;
    X3DAUDIO_VECTOR Position;
    X3DAUDIO_VECTOR Velocity;
    X3DAUDIO_CONE *pCone;
} X3DAUDIO_LISTENER;

typedef struct X3DAUDIO_EMITTER {
    X3DAUDIO_CONE *pCone;
    X3DAUDIO_VECTOR OrientFront;
    X3DAUDIO_VECTOR OrientTop;
    X3DAUDIO_VECTOR Position;
    X3DAUDIO_VECTOR Velocity;
    float InnerRadius;
    float InnerRadiusAngle;
    UINT32 ChannelCount;
    float ChannelRadius;
    float *pChannelAzimuths;
    X3DAUDIO_DISTANCE_CURVE *pVolumeCurve;
    X3DAUDIO_DISTANCE_CURVE *pLFECurve;
    X3DAUDIO_DISTANCE_CURVE *pLPFDirectCurve;
    X3DAUDIO_DISTANCE_CURVE *pLPFReverbCurve;
    X3DAUDIO_DISTANCE_CURVE *pReverbCurve;
    float CurveDistanceScaler;
    float DopplerScaler;
} X3DAUDIO_EMITTER;

typedef struct X3DAUDIO_DSP_SETTINGS {
    float *pMatrixCoefficients;
    float *pDelayTimes;
    UINT32 SrcChannelCount;
    UINT32 DstChannelCount;
    float LPFDirectCoefficient;
    float LPFReverbCoefficient;
    float ReverbLevel;
    float DopplerFactor;
    float EmitterToListenerAngle;
    float EmitterToListenerDistance;
    float EmitterVelocityComponent;
    float ListenerVelocityComponent;
} X3DAUDIO_DSP_SETTINGS;

#define X3DAUDIO_CALCULATE_MATRIX           0x00000001
#define X3DAUDIO_CALCULATE_DELAY            0x00000002
#define X3DAUDIO_CALCULATE_LPF_DIRECT       0x00000004
#define X3DAUDIO_CALCULATE_LPF_REVERB       0x00000008
#define X3DAUDIO_CALCULATE_REVERB           0x00000010
#define X3DAUDIO_CALCULATE_DOPPLER          0x00000020
#define X3DAUDIO_CALCULATE_EMITTER_ANGLE    0x00000040
#define X3DAUDIO_CALCULATE_ZEROCENTER       0x00010000
#define X3DAUDIO_CALCULATE_REDIRECT_TO_LFE  0x00020000

#ifndef _SPEAKER_POSITIONS_
#define _SPEAKER_POSITIONS_
#define SPEAKER_FRONT_LEFT                  0x00000001
#define SPEAKER_FRONT_RIGHT                 0x00000002
#define SPEAKER_FRONT_CENTER                0x00000004
#define SPEAKER_LOW_FREQUENCY               0x00000008
#define SPEAKER_BACK_LEFT                   0x00000010
#define SPEAKER_BACK_RIGHT                  0x00000020
#define SPEAKER_FRONT_LEFT_OF_CENTER        0x00000040
#define SPEAKER_FRONT_RIGHT_OF_CENTER       0x00000080
#define SPEAKER_BACK_CENTER                 0x00000100
#define SPEAKER_SIDE_LEFT                   0x00000200
#define SPEAKER_SIDE_RIGHT                  0x00000400
#define SPEAKER_TOP_CENTER                  0x00000800
#define SPEAKER_TOP_FRONT_LEFT              0x00001000
#define SPEAKER_TOP_FRONT_CENTER            0x00002000
#define SPEAKER_TOP_FRONT_RIGHT             0x00004000
#define SPEAKER_TOP_BACK_LEFT               0x00008000
#define SPEAKER_TOP_BACK_CENTER             0x00010000
#define SPEAKER_TOP_BACK_RIGHT              0x00020000
#define SPEAKER_RESERVED                    0x7ffc0000
#define SPEAKER_ALL                         0x80000000
#endif

#ifndef SPEAKER_MONO
#define SPEAKER_MONO                    SPEAKER_FRONT_CENTER
#define SPEAKER_STEREO                  (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT)
#define SPEAKER_2POINT1                 (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY)
#define SPEAKER_SURROUND                (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_BACK_CENTER)
#define SPEAKER_QUAD                    (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
#define SPEAKER_4POINT1                 (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_LOW_FREQUENCY | SPEAKER_BACK_LEFT | \
                                         SPEAKER_BACK_RIGHT)
#define SPEAKER_5POINT1                 (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT)
#define SPEAKER_7POINT1                 (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_FRONT_LEFT_OF_CENTER | \
                                         SPEAKER_FRONT_RIGHT_OF_CENTER)
#define SPEAKER_5POINT1_SURROUND        (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_SIDE_LEFT  | SPEAKER_SIDE_RIGHT)
#define SPEAKER_7POINT1_SURROUND        (SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT | SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY | \
                                         SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT | SPEAKER_SIDE_LEFT  | SPEAKER_SIDE_RIGHT)
#endif

#define X3DAUDIO_SPEED_OF_SOUND             343.5f

#define X3DAUDIO_HANDLE_BYTESIZE 20
typedef BYTE X3DAUDIO_HANDLE[X3DAUDIO_HANDLE_BYTESIZE];

#define X3DAUDIO_API_(type) EXTERN_C __declspec(dllimport) type STDAPIVCALLTYPE
X3DAUDIO_API_(HRESULT) X3DAudioInitialize(UINT32, float, X3DAUDIO_HANDLE);
X3DAUDIO_API_(void) X3DAudioCalculate(const X3DAUDIO_HANDLE, const X3DAUDIO_LISTENER *,
        const X3DAUDIO_EMITTER *, UINT32, X3DAUDIO_DSP_SETTINGS *);

typedef struct XAUDIO2FX_REVERB_I3DL2_PARAMETERS {
  float WetDryMix;
  INT32 Room;
  INT32 RoomHF;
  float RoomRolloffFactor;
  float DecayTime;
  float DecayHFRatio;
  INT32 Reflections;
  float ReflectionsDelay;
  INT32 Reverb;
  float ReverbDelay;
  float Diffusion;
  float Density;
  float HFReference;
} XAUDIO2FX_REVERB_I3DL2_PARAMETERS;

#define X3DAUDIO_PI 3.141592654f
#define X3DAUDIO_2PI 6.283185307f

typedef struct XAUDIO2FX_REVERB_PARAMETERS {
  float  WetDryMix;
  UINT32 ReflectionsDelay;
  BYTE   ReverbDelay;
  BYTE   RearDelay;
  BYTE   SideDelay;
  BYTE   PositionLeft;
  BYTE   PositionRight;
  BYTE   PositionMatrixLeft;
  BYTE   PositionMatrixRight;
  BYTE   EarlyDiffusion;
  BYTE   LateDiffusion;
  BYTE   LowEQGain;
  BYTE   LowEQCutoff;
  BYTE   HighEQGain;
  BYTE   HighEQCutoff;
  float  RoomFilterFreq;
  float  RoomFilterMain;
  float  RoomFilterHF;
  float  ReflectionsGain;
  float  ReverbGain;
  float  DecayTime;
  float  Density;
  float  RoomSize;
  BOOL   DisableLateField;
} XAUDIO2FX_REVERB_PARAMETERS;

#define XAUDIO2FX_I3DL2_PRESET_DEFAULT         {100,-10000,    0,0.0f, 1.00f,0.50f,-10000,0.020f,-10000,0.040f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_GENERIC         {100, -1000, -100,0.0f, 1.49f,0.83f, -2602,0.007f,   200,0.011f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_PADDEDCELL      {100, -1000,-6000,0.0f, 0.17f,0.10f, -1204,0.001f,   207,0.002f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_ROOM            {100, -1000, -454,0.0f, 0.40f,0.83f, -1646,0.002f,    53,0.003f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_BATHROOM        {100, -1000,-1200,0.0f, 1.49f,0.54f,  -370,0.007f,  1030,0.011f,100.0f, 60.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_LIVINGROOM      {100, -1000,-6000,0.0f, 0.50f,0.10f, -1376,0.003f, -1104,0.004f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_STONEROOM       {100, -1000, -300,0.0f, 2.31f,0.64f,  -711,0.012f,    83,0.017f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_AUDITORIUM      {100, -1000, -476,0.0f, 4.32f,0.59f,  -789,0.020f,  -289,0.030f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_CONCERTHALL     {100, -1000, -500,0.0f, 3.92f,0.70f, -1230,0.020f,    -2,0.029f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_CAVE            {100, -1000,    0,0.0f, 2.91f,1.30f,  -602,0.015f,  -302,0.022f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_ARENA           {100, -1000, -698,0.0f, 7.24f,0.33f, -1166,0.020f,    16,0.030f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_HANGAR          {100, -1000,-1000,0.0f,10.05f,0.23f,  -602,0.020f,   198,0.030f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_CARPETEDHALLWAY {100, -1000,-4000,0.0f, 0.30f,0.10f, -1831,0.002f, -1630,0.030f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_HALLWAY         {100, -1000, -300,0.0f, 1.49f,0.59f, -1219,0.007f,   441,0.011f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_STONECORRIDOR   {100, -1000, -237,0.0f, 2.70f,0.79f, -1214,0.013f,   395,0.020f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_ALLEY           {100, -1000, -270,0.0f, 1.49f,0.86f, -1204,0.007f,    -4,0.011f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_FOREST          {100, -1000,-3300,0.0f, 1.49f,0.54f, -2560,0.162f,  -613,0.088f, 79.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_CITY            {100, -1000, -800,0.0f, 1.49f,0.67f, -2273,0.007f, -2217,0.011f, 50.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_MOUNTAINS       {100, -1000,-2500,0.0f, 1.49f,0.21f, -2780,0.300f, -2014,0.100f, 27.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_QUARRY          {100, -1000,-1000,0.0f, 1.49f,0.83f,-10000,0.061f,   500,0.025f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_PLAIN           {100, -1000,-2000,0.0f, 1.49f,0.50f, -2466,0.179f, -2514,0.100f, 21.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_PARKINGLOT      {100, -1000,    0,0.0f, 1.65f,1.50f, -1363,0.008f, -1153,0.012f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_SEWERPIPE       {100, -1000,-1000,0.0f, 2.81f,0.14f,   429,0.014f,   648,0.021f, 80.0f, 60.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_UNDERWATER      {100, -1000,-4000,0.0f, 1.49f,0.10f,  -449,0.007f,  1700,0.011f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_SMALLROOM       {100, -1000, -600,0.0f, 1.10f,0.83f,  -400,0.005f,   500,0.010f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_MEDIUMROOM      {100, -1000, -600,0.0f, 1.30f,0.83f, -1000,0.010f,  -200,0.020f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_LARGEROOM       {100, -1000, -600,0.0f, 1.50f,0.83f, -1600,0.020f, -1000,0.040f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_MEDIUMHALL      {100, -1000, -600,0.0f, 1.80f,0.70f, -1300,0.015f,  -800,0.030f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_LARGEHALL       {100, -1000, -600,0.0f, 1.80f,0.70f, -2000,0.030f, -1400,0.060f,100.0f,100.0f,5000.0f}
#define XAUDIO2FX_I3DL2_PRESET_PLATE           {100, -1000, -200,0.0f, 1.30f,0.90f,     0,0.002f,     0,0.010f,100.0f, 75.0f,5000.0f}

#include <xaudio2.h>
#if 0
HRESULT XAudio2Create(
  IXAudio2          **ppXAudio2,
  UINT32            Flags,
  XAUDIO2_PROCESSOR XAudio2Processor
);
#endif

#define XAUDIO2_LOG_ERRORS     0x0001   // For handled errors with serious effects.
#define XAUDIO2_LOG_WARNINGS   0x0002   // For handled errors that may be recoverable.
#define XAUDIO2_LOG_INFO       0x0004   // Informational chit-chat (e.g. state changes).
#define XAUDIO2_LOG_DETAIL     0x0008   // More detailed chit-chat.
#define XAUDIO2_LOG_API_CALLS  0x0010   // Public API function entries and exits.
#define XAUDIO2_LOG_FUNC_CALLS 0x0020   // Internal function entries and exits.
#define XAUDIO2_LOG_TIMING     0x0040   // Delays detected and other timing data.
#define XAUDIO2_LOG_LOCKS      0x0080   // Usage of critical sections and mutexes.
#define XAUDIO2_LOG_MEMORY     0x0100   // Memory heap usage information.
#define XAUDIO2_LOG_STREAMING  0x1000   // Audio streaming information.

#define FXMASTERINGLIMITER_MIN_RELEASE     1
#define FXMASTERINGLIMITER_MAX_RELEASE     20
#define FXMASTERINGLIMITER_DEFAULT_RELEASE 6

#define FXMASTERINGLIMITER_MIN_LOUDNESS     1
#define FXMASTERINGLIMITER_MAX_LOUDNESS     1800
#define FXMASTERINGLIMITER_DEFAULT_LOUDNESS 1000

#define XAUDIO2FX_REVERB_MIN_WET_DRY_MIX            0.0f
#define XAUDIO2FX_REVERB_MIN_REFLECTIONS_DELAY      0
#define XAUDIO2FX_REVERB_MIN_REVERB_DELAY           0
#define XAUDIO2FX_REVERB_MIN_REAR_DELAY             0
#define XAUDIO2FX_REVERB_MIN_7POINT1_SIDE_DELAY     0
#define XAUDIO2FX_REVERB_MIN_7POINT1_REAR_DELAY     0
#define XAUDIO2FX_REVERB_MIN_POSITION               0
#define XAUDIO2FX_REVERB_MIN_DIFFUSION              0
#define XAUDIO2FX_REVERB_MIN_LOW_EQ_GAIN            0
#define XAUDIO2FX_REVERB_MIN_LOW_EQ_CUTOFF          0
#define XAUDIO2FX_REVERB_MIN_HIGH_EQ_GAIN           0
#define XAUDIO2FX_REVERB_MIN_HIGH_EQ_CUTOFF         0
#define XAUDIO2FX_REVERB_MIN_ROOM_FILTER_FREQ       20.0f
#define XAUDIO2FX_REVERB_MIN_ROOM_FILTER_MAIN       -100.0f
#define XAUDIO2FX_REVERB_MIN_ROOM_FILTER_HF         -100.0f
#define XAUDIO2FX_REVERB_MIN_REFLECTIONS_GAIN       -100.0f
#define XAUDIO2FX_REVERB_MIN_REVERB_GAIN            -100.0f
#define XAUDIO2FX_REVERB_MIN_DECAY_TIME             0.1f
#define XAUDIO2FX_REVERB_MIN_DENSITY                0.0f
#define XAUDIO2FX_REVERB_MIN_ROOM_SIZE              0.0f

#define XAUDIO2FX_REVERB_MAX_WET_DRY_MIX            100.0f
#define XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY      300
#define XAUDIO2FX_REVERB_MAX_REVERB_DELAY           85
#define XAUDIO2FX_REVERB_MAX_REAR_DELAY             5
#define XAUDIO2FX_REVERB_MAX_7POINT1_SIDE_DELAY     5
#define XAUDIO2FX_REVERB_MAX_7POINT1_REAR_DELAY     20
#define XAUDIO2FX_REVERB_MAX_POSITION               30
#define XAUDIO2FX_REVERB_MAX_DIFFUSION              15
#define XAUDIO2FX_REVERB_MAX_LOW_EQ_GAIN            12
#define XAUDIO2FX_REVERB_MAX_LOW_EQ_CUTOFF          9
#define XAUDIO2FX_REVERB_MAX_HIGH_EQ_GAIN           8
#define XAUDIO2FX_REVERB_MAX_HIGH_EQ_CUTOFF         14
#define XAUDIO2FX_REVERB_MAX_ROOM_FILTER_FREQ       20000.0f
#define XAUDIO2FX_REVERB_MAX_ROOM_FILTER_MAIN       0.0f
#define XAUDIO2FX_REVERB_MAX_ROOM_FILTER_HF         0.0f
#define XAUDIO2FX_REVERB_MAX_REFLECTIONS_GAIN       20.0f
#define XAUDIO2FX_REVERB_MAX_REVERB_GAIN            20.0f
#define XAUDIO2FX_REVERB_MAX_DENSITY                100.0f
#define XAUDIO2FX_REVERB_MAX_ROOM_SIZE              100.0f

#define XAUDIO2FX_REVERB_DEFAULT_WET_DRY_MIX        100.0f
#define XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_DELAY  5
#define XAUDIO2FX_REVERB_DEFAULT_REVERB_DELAY       5
#define XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY         5
#define XAUDIO2FX_REVERB_DEFAULT_7POINT1_SIDE_DELAY 5
#define XAUDIO2FX_REVERB_DEFAULT_7POINT1_REAR_DELAY 20
#define XAUDIO2FX_REVERB_DEFAULT_POSITION           6
#define XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX    27
#define XAUDIO2FX_REVERB_DEFAULT_EARLY_DIFFUSION    8
#define XAUDIO2FX_REVERB_DEFAULT_LATE_DIFFUSION     8
#define XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_GAIN        8
#define XAUDIO2FX_REVERB_DEFAULT_LOW_EQ_CUTOFF      4
#define XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_GAIN       8
#define XAUDIO2FX_REVERB_DEFAULT_HIGH_EQ_CUTOFF     4
#define XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_FREQ   5000.0f
#define XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_MAIN   0.0f
#define XAUDIO2FX_REVERB_DEFAULT_ROOM_FILTER_HF     0.0f
#define XAUDIO2FX_REVERB_DEFAULT_REFLECTIONS_GAIN   0.0f
#define XAUDIO2FX_REVERB_DEFAULT_REVERB_GAIN        0.0f
#define XAUDIO2FX_REVERB_DEFAULT_DECAY_TIME         1.0f
#define XAUDIO2FX_REVERB_DEFAULT_DENSITY            100.0f
#define XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE          100.0f
#define XAUDIO2FX_REVERB_DEFAULT_DISABLE_LATE_FIELD FALSE

    typedef struct FXMASTERINGLIMITER_PARAMETERS {
        UINT32 Release;  // release time (tuning factor with no specific units)
        UINT32 Loudness; // loudness target (threshold)
    } FXMASTERINGLIMITER_PARAMETERS;

__inline HRESULT XAudio2CreateVolumeMeter(_Outptr_ IUnknown** ppApo, UINT32 Flags = 0)
{
    UNREFERENCED_PARAMETER(Flags);
    return CreateAudioVolumeMeter(ppApo);
}

__inline HRESULT XAudio2CreateReverb(_Outptr_ IUnknown** ppApo, UINT32 Flags = 0)
{
    UNREFERENCED_PARAMETER(Flags);
    return CreateAudioReverb(ppApo);
}

  #define FX_API_(type) STDAPIV_(type)
 FX_API_(HRESULT) CreateFX (REFCLSID clsid, IUnknown** pEffect, const void* pInitDat = NULL, UINT32 InitDataByteSize = 0);


__inline void ReverbConvertI3DL2ToNative
(
    const XAUDIO2FX_REVERB_I3DL2_PARAMETERS* pI3DL2,
    XAUDIO2FX_REVERB_PARAMETERS* pNative
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
    ,  BOOL sevenDotOneReverb =TRUE
#endif
)
{
    float reflectionsDelay;
    float reverbDelay;

    // RoomRolloffFactor is ignored

    // These parameters have no equivalent in I3DL2
#if(_WIN32_WINNT >= _WIN32_WINNT_WIN10)
    if(sevenDotOneReverb)
    {
        pNative->RearDelay = XAUDIO2FX_REVERB_DEFAULT_7POINT1_REAR_DELAY; // 20
    }
    else
    {
        pNative->RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
    }
    pNative->SideDelay = XAUDIO2FX_REVERB_DEFAULT_7POINT1_SIDE_DELAY; // 5
#else
    pNative->RearDelay = XAUDIO2FX_REVERB_DEFAULT_REAR_DELAY; // 5
#endif
    pNative->PositionLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionRight = XAUDIO2FX_REVERB_DEFAULT_POSITION; // 6
    pNative->PositionMatrixLeft = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->PositionMatrixRight = XAUDIO2FX_REVERB_DEFAULT_POSITION_MATRIX; // 27
    pNative->RoomSize = XAUDIO2FX_REVERB_DEFAULT_ROOM_SIZE; // 100
    pNative->LowEQCutoff = 4;
    pNative->HighEQCutoff = 6;

    // The rest of the I3DL2 parameters map to the native property set
    pNative->RoomFilterMain = (float)pI3DL2->Room / 100.0f;
    pNative->RoomFilterHF = (float)pI3DL2->RoomHF / 100.0f;

    if (pI3DL2->DecayHFRatio >= 1.0f)
    {
        INT32 index = (INT32)(-4.0 * log10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->HighEQGain = 8;
        pNative->DecayTime = pI3DL2->DecayTime * pI3DL2->DecayHFRatio;
    }
    else
    {
        INT32 index = (INT32)(4.0 * log10(pI3DL2->DecayHFRatio));
        if (index < -8) index = -8;
        pNative->LowEQGain = 8;
        pNative->HighEQGain = (BYTE)((index < 0) ? index + 8 : 8);
        pNative->DecayTime = pI3DL2->DecayTime;
    }

    reflectionsDelay = pI3DL2->ReflectionsDelay * 1000.0f;
    if (reflectionsDelay >= XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY) // 300
    {
        reflectionsDelay = (float)(XAUDIO2FX_REVERB_MAX_REFLECTIONS_DELAY - 1);
    }
    else if (reflectionsDelay <= 1)
    {
        reflectionsDelay = 1;
    }
    pNative->ReflectionsDelay = (UINT32)reflectionsDelay;

    reverbDelay = pI3DL2->ReverbDelay * 1000.0f;
    if (reverbDelay >= XAUDIO2FX_REVERB_MAX_REVERB_DELAY) // 85
    {
        reverbDelay = (float)(XAUDIO2FX_REVERB_MAX_REVERB_DELAY - 1);
    }
    pNative->ReverbDelay = (BYTE)reverbDelay;

    pNative->ReflectionsGain = (float)pI3DL2->Reflections / 100.0f;
    pNative->ReverbGain = (float)pI3DL2->Reverb / 100.0f;
    pNative->EarlyDiffusion = (BYTE)(15.0f * pI3DL2->Diffusion / 100.0f);
    pNative->LateDiffusion = pNative->EarlyDiffusion;
    pNative->Density = pI3DL2->Density;
    pNative->RoomFilterFreq = pI3DL2->HFReference;

    pNative->WetDryMix = pI3DL2->WetDryMix;
    pNative->DisableLateField = FALSE;
}

class FXMasteringLimiter;
template<> inline auto __mingw_uuidof<FXMasteringLimiter>() -> GUID const&
{
    static constexpr GUID the_uuid = { 0xC4137916, 0x2BE1, 0x46FD, 0x85, 0x99, 0x44, 0x15, 0x36, 0xF4, 0x98, 0x56 };
    return the_uuid;
}

#endif
