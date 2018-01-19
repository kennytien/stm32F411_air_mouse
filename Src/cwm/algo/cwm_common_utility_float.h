/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CWM_COMMON_UTILITY_FLOAT_H_
#define _CWM_COMMON_UTILITY_FLOAT_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
  ******************************************************************************
  *
  *   Sensors Requirement:
  *       Depends on each API.
  *           rate: Depends on each API.
  *
  *   Trigger source: (which sensors to calculate this algo)
  *       Depends on each API.
  *
  *   Sensors type:
  *       continuous, basic library for other algorithm.
  *
  *   output:
  *       Depends on each API.
  *
  *   output report format:
  *       Depends on each API.
  *
  *   Algo introduction:
  *       Depends on each API.
  *
  *       Trigger Conditions:
  *       1. XXXXXXXXXXXXXXXXXXX
  *
  *   Algo test scenario:
  *       1. XXXXXXXXXXXXXXXXXXX
  *
  *   Algo history list:
  *
  *   v00.01.01_20160421 YK.Tsai      Algorithm library/object first released.
  *   v00.01.03_20160602 YK.Tsai      Edit functions of matrix calculation.
  *   v00.02.04_20160629 SC.Chang     1. Add High-Pass Filter, Low-Pass Filter & supoport the version of vector process.
  *                                   2. Add the API of Vector_SignalBiasRemoved() with shift-register, to remove the signal bias for target vector.
  *                                   SHA-1: 2B70EC4659EA46AA2B9EC56F8BAC612225CF7400 / MD5: A6E1F1F3EA7DEB5F5496442063414344 (IAR)
  *                                   SHA-1: 23709F48B5D041EB238028043AC17241C340848C / MD5: DCD9381AAB135B7153EC6173C96B27F8 (ADS5)
  *                                   SHA-1: 80D1B222504B5984DF5C47FD59C6D8764FBF72BE / MD5: 64B6D76A63B644297533D8FB53B4219E (XPLR)
  *                                   SHA-1:  / MD5:  (KEIL M0)
  *                                   SHA-1:  / MD5:  (KEIL M4)
  *   v00.02.05_20160705 SC.Chang     1. Replace the "sqrt" by "math_sqrt", to resolve the compile warning and be suitable for each platform no matter what the math library is using.
  *                                   2. In StandardDeviation(), now support "Matrix Base" or "Samples Base".
  *                                   SHA-1: 30A99BD3D2C7D9AE60A6305CDE4EB3A5DC6905A9 / MD5: 8CAE874A36F2C83F032CCA58ACD8B7D7 (IAR)
  *                                   SHA-1: 30B2D72C8DBF8DBFF5A85DED6CC461BACAD4AF9F / MD5: D4441BF6D9E1B06E4A3E0E48F1D11802 (ADS5)
  *                                   SHA-1: B5FB97F82B8E02AE083B2CCCF339147E3848297E / MD5: 7A1FF36E2D1B95D080A8C42936AD3940 (XPLR)
  *                                   SHA-1:  / MD5:  (KEIL M0)
  *                                   SHA-1:  / MD5:  (KEIL M4)
  *   v00.03.06_20160721 SC.Chang     Move some source code for CADENCE_LIB from .h to .c
  *                                   SHA-1: 30A99BD3D2C7D9AE60A6305CDE4EB3A5DC6905A9 / MD5: 8CAE874A36F2C83F032CCA58ACD8B7D7 (IAR)
  *                                   SHA-1: 17D6198AF2FF6802977E10E2812FE4AD0572140F / MD5: 47C8C2EF541BEC9C82C159F5E0271A34 (ADS5)
  *                                   SHA-1: 3DF589870746BD59F6E0CD811BD76B035C786063 / MD5: 74FE74DFDE627AABF56EB74F14F5FD0A (XPLR)
  *                                   SHA-1:  / MD5:  (KEIL M0)
  *                                   SHA-1:  / MD5:  (KEIL M4)
  *   v00.04.00_20161214 SC.Chang     Sync. with cwm_fusion V02.07.00_20161214.
  *                                   1. Prevent math function out of range for math_asin()/math_atan2()/math_sqrt().
  *                                   2. Add ST_NANO_MATH to support GNU_GCC compiler.
  *                                   3. For matrix calculation functions, prevent divide by 0.
  *                                   SHA-1: BB1B29B9D59698CB4A48036BF2FAE25603E10D65 / MD5: 480FC4B12E5D18D60B3C2232DE7FCDC5 (ADS5)
  *                                   SHA-1: A3274B624DF1B1799D6EE36791C6CAC338420E1E / MD5: 79109254750C840EE91D92A07B6A40B9 (GNC GCC)
  *                                   SHA-1: 888AAC461370D8E198CFE8DDD843CA118D72CA3E / MD5: FCFA06AC558F266C53E08EB8ECC7156B (HEXAGON)
  *                                   SHA-1: FC780C79757605768493083D3EC57DB2815400DA / MD5: 7F045ACAA16E63A65CB3EFE1BCADAE7E (IAR)
  *                                   SHA-1: 0037AF69B46D7E5E5978A1925FDB8C4C69DC2B0C / MD5: 55E7A180C02892A990EFE6468E14DC40 (XPLR NANOHUB)
  *                                   SHA-1: F6EDE306DD7CDEE9718719CEB7BAA23349DA8125 / MD5: 7EF91609238E8E580593D4378CD0B24B (KEIL M0)
  *                                   SHA-1: 7D13017E5BC07C82CA15F4C68479F7BE362A13B4 / MD5: 597D73240A2FDC5D087D9A9A63406053 (KEIL M4)
  *   v00.04.01_20161215 SC.Chang     For CADENCE_LIB, move some CONST declaration into .c file.
  *                                   SHA-1: FBA0C9FA16041BD2EAE1CF150A56A4B940446816 / MD5: 1B5EBD9842CC92A6698D551BFFF8F506 (ADS5)
  *                                   SHA-1: 58AB4CD246D8EEAF0977DA775982D6C16AFEEC54 / MD5: 5C76BCD151CCFF227EA422B14A872670 (GNC GCC)
  *                                   SHA-1: 888AAC461370D8E198CFE8DDD843CA118D72CA3E / MD5: FCFA06AC558F266C53E08EB8ECC7156B (HEXAGON)
  *                                   SHA-1: FC780C79757605768493083D3EC57DB2815400DA / MD5: 7F045ACAA16E63A65CB3EFE1BCADAE7E (IAR)
  *                                   SHA-1: 5361535F29A857BFDC28CD4CC95DDFA9D8DD3940 / MD5: B4C259799BFE7BE8839B59B778B0B65A (XPLR NANOHUB)
  *                                   SHA-1: 35A665CA9F88E118D514B6B1C5C2E1CD652D243F / MD5: E7E7664B238094ECD47C1B69FCBF00C3 (KEIL M0)
  *                                   SHA-1: EEC17A1AB1AA22DE59099E668C9C0BC61320906A / MD5: F6DC90CFB625A193437ABB3B5D463156 (KEIL M4)
  *   v00.05.02_20170110 SC.Chang     1. Add Discrete_Fourier_Transform_DFT().
  *                                   2. For ARM_DSP_LIB, wrap the arm_sin_f32()/arm_cos_f32() by math_sin()/math_cos(), to avoid some error due to input/output out of range.
  *                                   SHA-1: 2D489F4C07C4EBCD56CA3488C9755435DC2AE28F / MD5: 31559604165847023EA50BB54C4542C6 (ADS5)
  *                                   SHA-1: 0F38B2CDA4AAA0551F63865CA29BA86311F36B50 / MD5: 50804BA592199ADD11DBE8D156F95A4A (GNC GCC)
  *                                   SHA-1: EB8EC72393B04588A68316DB72DB0F5F797EA534 / MD5: 6BCD9B91FF4891291F69BD048F726D7D (HEXAGON)
  *                                   SHA-1: E21C48E68507C8134B220513BA299676026FDA9A / MD5: B1002FB8196964ACA7BE7102FE0366A3 (IAR)
  *                                   SHA-1: 7515BB4789BE69CEBCF0CE3B59BEB54821F50839 / MD5: 099B3B88922140D9E3C91522095C36BE (XPLR NANOHUB)
  *                                   SHA-1: E6CB601E5F4A835B5C94084ED2D23184D8BB35D9 / MD5: 85E05D5807A20562802D6358FEAD4963 (KEIL M0)
  *                                   SHA-1: 45CD24EA3B5502AA283CD345A430FAB38904A451 / MD5: 0CD7374F04C8D7E601842ACC4DB41626 (KEIL M4)
  *   v00.05.03_20170116 SC.Chang     1. For CADENCE_LIB & default platform, wrap to math_sin()/math_cos(), and avoid some error due to input/output out of range.
  *                                   2. In Discrete_Fourier_Transform_DFT(), add bDivided_By_N as input parameter.
  *                                   3. Add header information for each function.
  *                                   4. Add DFT_LOOKUP_TBL_T and InitLUT_Discrete_Fourier_Transform_DFT() to improve the performance.
  *                                   SHA-1: 7292075BDA0FB8C4223D5FD150AC2A0A066E5AEB / MD5: 99BC97723293683A42AAF8DFFAA686F0 (IAR)
  *   V00.06.04_20170220 SC.Chang     1. support math_acos() for each platform.
  *                                   SHA-1: C80E1703E761F8B05DD1D7305199763DEFB605C5 / MD5: 898AA96672CD24DCE5BDD71FEA39D496 (IAR)
  *   V00.07.05_20170414 SC.Chang     1. Add APIs: Vector_High_Pass_Filter2(), Vector_Low_Pass_Filter2().
  *                                   2. Move the platform definition (ARM_DSP_LIB, CADENCE_LIB, ST_NANO_MATH ...) from .h into .c
  *   V00.08.06_20170609 SC.Chang     1. In InitLUT_Discrete_Fourier_Transform_DFT(), add the process to verify input parameters.
  *                                   2. Add API to calculate the Nth item of Fibonacci number.
  *                                   3. Add Common Basic Math API: math_fabs(), math_fmax(), math_fmin().
  *   V00.09.07_20170725 SC.Chang     1. Add Math_Normalize_To_Range().
  *
  *   Verification checksum:
  *
  *       V00.09.07_20170725
  *
  *       SHA-1: D80F4236FA004AEC7C08D86A0407288BE20AF329 / ND5: 6DA57511EF555769A371CF5D75A015FA (ADS5)
  *       SHA-1: 0CA74B2F89FFE12D6893B04B1CFD1FCBC0D39C73 / ND5: 1D8D10223DD4DA7845DC6115EDDB7508 (HEXAGON)
  *       SHA-1: 5C817788190694F993841694641EBD86C8FCEB7B / ND5: A62F82A49313FA405B0FDE50D45CC256 (IAR)
  *       SHA-1: 8A7F06B8C15963FB7BE61A0F09A0BDC8D2112D9A / ND5: 09E48B94B3DA3FBD3938699BD80D80C5 (KEIL M0)
  *       SHA-1: D06C0104309B5D076408E638498D54862705772B / ND5: 05838EE8FC4BA94B57A48EF74BD23D2A (KEIL M4)
  *       SHA-1: E53C83D45FCF417C5A8FC1C6E2CF42FBE7345BAD / ND5: 152214E9BF067ACA5ADB98FF7BD624B2 (XPLR NANOHUB)
  *       SHA-1: A7FC8304275E741A56E0CA0DFE00FB7CDD55E070 / ND5: 64CD03C60CDDAE7AAFD25DB3A3402249 (LINUX_PIXEL)
  *       SHA-1: 99F0F7F88205D2823C3EFD9D3894EA77C8EE36A3 / ND5: 5B629533A13951456FA6093EE3E65238 (LINUX_SPRD)
  *       SHA-1: C7034C2C6B91F6939C7087163880E6CD8A97B03D / ND5: 4D09BD5860AAAD23CEB9931359D046DC (LINUX_SPRD)
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>     // Declaration of uint8_t, uint16_t, uint32_t
#include <stdbool.h>    // Declaration of bool

/* Private define ------------------------------------------------------------*/
/* Coordinate */
#define COORDINATE_X                                    (0)
#define COORDINATE_Y                                    (1)
#define COORDINATE_Z                                    (2)
#define COORDINATE_X_AZIMUTH                            COORDINATE_X
#define COORDINATE_X_YAW                                COORDINATE_X
#define COORDINATE_Y_PITCH                              COORDINATE_Y
#define COORDINATE_Z_ROLL                               COORDINATE_Z

/* Error Code Definition */
#define ERROR_SUCCESS                                   (0x00)  // nothing to do
#define ERROR_REPORT_EVENT                              (0x01)  // report event
#define ERROR_UNKNOWN                                   (-1)    // error
#define ERROR_NOT_INITIALIZED                           (-2)    // not initial
#define ERROR_INVALID_TIMESTAMP                         (-3)    // dt wrong
#define ERROR_INVALID_DATA                              (-4)    // data wrong
#define ERROR_INVALID_CONFIG_PARM                       (-8)    // set config parameter wrong (size, range...)
#define ERROR_INVALID_1ST_CFGPARM_REPORT                (-9)    // return the config parameter at first run after set_config_XXX() be called.
#define ERROR_LICENSE_EXPIRED                           (9999)

/* Constant Definition */
#define CONST_PI                                        (3.14159265358979323846264338327950f)
#define CONST_RAD_TO_DEG                                (57.2957795130823208767981548141050f)
#define CONST_DEG_TO_RAD                                (0.01745329251994329576923690768489f)
#define DEG_TO_RAD                                      (double)((double)CONST_PI / (double)180.0f)
#define RAD_TO_DEG                                      (double)((double)180.0f / (double)CONST_PI)
#define GRAVITATIONAL_ACCEL                             (9.80665f)  //Gravitational acceleration (in meters per square second)
#define GRAVITATIONAL_ACCEL_K                           (9807)      //Gravitational acceleration (in meters per square second) * 1000

/* License Limitation */
#define LICENSE_LIMIT_COUNT                             (5000)

/* Data Type Definition */
#ifndef bool
#define bool                                            unsigned char
#endif  // bool
#ifndef true
#define true                                            (1)
#endif  // true
#ifndef false
#define false                                           (0)
#endif  // false
#ifndef NULL
#define NULL                                            (0x00)
#endif  // NULL

#ifndef uint32_t
#define uint32_t                                        unsigned int
#endif  // uint32_t
#ifndef uint16_t
#define uint16_t                                        unsigned short
#endif  // uint16_t
#ifndef uint8_t
#define uint8_t                                         unsigned char
#endif  // uint8_t


/* Math library define -------------------------------------------------------*/
// Reset the definition for all platform.
#undef math_sin
#undef math_cos

// In compile phase, some definitions for assigned platform are necessary.
// ARM_DSP_LIB:   Include ARM internal DSP library for IAR
//                #include "arm_math.h"
//                #include <math.h>   // sin, cos, asin, atan2, sqrt
//                #define math_sin    arm_sin_f32
//                #define math_cos    arm_cos_f32
//                #define math_asin   asinf
//                #define math_acos   acosf
//                #define math_atan2  atan2f
//
// CADENCE_LIB:   Include Cadence math library for Xplorer
//                #include "NatureDSP_Signal.h"
//                #include "fusion_spl.h"
//                #include "common.h"
//                #include <errno.h>
//                #include <fenv.h>
//                #define math_sin    scl_sinef
//                #define math_cos    scl_cosinef
//                #define math_asin   scl_asinf_inplace
//                #define math_acos   acosf
//                #define math_atan2  scl_atan2f
//                #define math_sqrt   scl_sqrtf_inplace
//                Cadence inplaced API, platform internal used, it should be kept inside the chip, but ...
//                float32_t scl_sqrtf_inplace(float32_t x);
//                float32_t scl_rsqrtf_inplace(float32_t x);
//                float32_t scl_asinf_inplace(float32_t x);
//                void vec_scalef_inplace(float32_t * restrict x, float32_t s, int N);
//
// ST_NANO_MATH:  Include math library for ST Nanohub on GNU GCC
//                #include <nanohub_math.h>   // sin, cos, asin, atan2, sqrt
//                #define math_sin    sinf
//                #define math_cos    cosf
//                #define math_asin   asinf
//                #define math_acos   acosf
//                #define math_atan2  atan2f
//                #define math_sqrt   sqrtf
//
// Default Platform:
//                #include <math.h>           // sin, cos, asin, atan2, sqrt
//                #define math_sin    sinf
//                #define math_cos    cosf
//                #define math_asin   asinf
//                #define math_acos   acosf
//                #define math_atan2  atan2f
//                #define math_sqrt   sqrtf

// Wrapper Functions
float math_sin(float input);
float math_cos(float input);
float math_asin(float input);
float math_acos(float input);
float math_atan2(float input1, float input2);   // atan( input1 / input2 )
float math_sqrt(float input);


/* Private typedef -----------------------------------------------------------*/
//
// Use the 8 can alignment in 4-byte to avoid the alignment issue.
// Please see the following linker to explain that.
// http://blog.csdn.net/jamesf1982/article/details/4375719
// http://www.sf.org.cn/Article/base/200509/260.html
//
#pragma pack(8)

/* Moving Average Configration */
// To avoid the compile error, chage the structure name for integer version with tag of "_INT".
// The content are same with each other, _MOVING_AVG_CFG_T = _MOVING_AVG_CFG_INT_T
typedef struct _MOVING_AVG_CFG_T
{
    /*
     * Basic Concept:
     * 1. nSum     = P1 + P2 + ..... + PN
     *             = nQuotient * nDivisor + nRemainder
     * 2. Sum[N+1] = nQuotient[N] * (nDivisor-1) + nRemainder[N] + Cuttent P[N]
     * 3. Avoid the result of "/" or "%" will be truncated to integer, cause
     *    data not be continuous, so it is necessary multiple a scale before
     *    process, and roll back later.
     */
    int   nQuotient;      // Average Value
    int   nRemainder;
    int   nSum;
} MOVING_AVG_CFG_T;

/* Posture Detect */
typedef struct _POSTURE_DETECT_INFO_T
{
    float fSignalTrend[3];              // The trend of input signal.
    float fDirectionCosine[3];          // The Direction Cosine between input signal and total activated vector. (-1 ~ 1)
    float fDirectionCosine_STD[3];      // The Standard Deviation of Direction Cosine out.
    bool  bIsPostureChanged;            // Is the posture changed?
} POSTURE_DETECT_INFO_T;

/* High / Low Pass Filter */
typedef struct _HLPASS_FILTER_INFO_T
{
    float fX_In_pre[3];                 // Shift-Register of previous input data X
    float fY_Out_pre[3];                // Shift-Register of previous output data Y
    float fCutoff_Freq_Hz;              // Cutoff frequency Fc in Hz
                                        // 1. Set fCutoff_Freq_Hz = 0 to bypass the filter.
    float fSampling_Rate_Hz;            // Data sampling rate in Hz (NOTE: It is fixed for integer version now)
                                        // 1. Set fSampling_Rate_Hz = 0 to bypass the filter.
} HLPASS_FILTER_INFO_T;

typedef struct _BIAS_REMOVED_T
{
    bool  bIsInitialized;               // Is initialized or not?
    HLPASS_FILTER_INFO_T tHPF_Info;
    HLPASS_FILTER_INFO_T tLPF_Info;
} BIAS_REMOVED_SHIFTREG_T;

/* Discrete_Fourier_Transform_DFT */
typedef struct _DFT_LOOKUP_TBL_T
{
    float fSign;                        // value of math_sin()
    float fCosine;                      // value of math_cos()
} DFT_LOOKUP_TBL_T;

/* Coordinate System */
// Spherical Coordinate System
typedef struct _SPHERICAL_COORD_SYS_T {
    float r;                            // radial distance r
    float theta;                        // polar angle (theta), PS: start from +Z
    float phi;                          // azimuthal angle (phi), PS: start from +X
} SPHERICAL_COORD_SYS_T;

#pragma pack()

/* Private function prototypes -----------------------------------------------*/
/* ==============================================
 * Common Basic Math API
 * ============================================== */

float math_fabs(float x);
float math_fmax(float x, float y);
float math_fmin(float x, float y);


/* ==============================================
 * Vector Calculation Functions
 * ============================================== */

/**
  * @brief  Calculate the length of vector.
  * @param  [in]  fVector[]:        Array of vector (X,Y,Z)
  * @retval [out] fVectorStrength:  Vector length
  */

float Vector_Strength(float fVector[]);

/**
  * @brief  Calculate the directional cosine for each axis.
  * @param  [in]  fCurrentVector:     Input signal array.
  *         [out] pfDirectionCosine:  Direction Cosine output array (-1 ~ 1).
  * @retval [out] ERROR_SUCCESS.
  */

int   Vector_Direction_Cosine(float fCurrentVector[], float *pfDirectionCosine);


/* ==============================================
 * Mathematics Calculation Functions
 * ============================================== */

/**
  * @brief  Returns the sign of number.
  * @param  [in]  fNumber:  The number which sign will be separated.
  * @retval [out] +1: When the number is positive.
  *               0 : When the number is zero.
  *               -1: When the number is negative.
  */

int   Math_Sign(float fNumber);

/**
  * @brief  Check the current in range between upper & lower limit.
  * @param  [in]  fUpperLimit:    Upper limit to calculate the overshot count.
  *         [in]  fLowerLimit:    Lower limit to calculate the overshot count.
  *         [in]  fCurrentValue:  Input signal.
  * @retval [out] true:  in range of limitation.
  *               false: out of range.
  */

bool  Math_InRange(float fUpperLimit, float fLowerLimit, float fCurrentValue);

/**
  * @brief  Normalize the input value in asigned range.
  * @param  [in]  value:          Input value.
  *         [in]  min:            Lower limit of asigned range.
  *         [in]  max:            Upper limit of asigned range.
  * @retval [out] the value which converted in range of limitation.
  */
float Math_Normalize_To_Range(float value, float min, float max);

/**
  * @brief  Standard Deviation.
  * @param  [in]  fSampleBuf[]:     The moving buffer for STD calculation.
  *                                 Index 0   is the newest value in buffer.
  *                                 Index N-1 is the oldest value in buffer.
  *         [in]  nSampleBufSize:   The size of fSampleBuf[].
  *         [in]  fCurrentValue:    Input signal.
  *         [out] pfMeanValue:      The mean value calculated by fSampleBuf[] (moving average).
  *         [in]  bRollingBuf:      Enable (true) or Disable (false) the process for moviing buffer.
  *         [in]  bIsMatrixBase:    Switch between Matrix Base (true) or Samples Base (false).
  * @retval [out] Standard Deviation.
  */

float StandardDeviation(float fSampleBuf[], uint8_t nSampleBufSize, float fCurrentValue, float *pfMeanValue, bool bRollingBuf, bool bIsMatrixBase);

/**
  * @brief  Moving Average calculation by SQR method.
  * @param  [in]  tMovAvgCfg[]:     The data structure of MOVING_AVG_CFG_T for vector[x,y,z].
  *         [in]  fVectorIn[]:      The input signal in format of vector[x,y,z].
  *         [in]  nScale:           The scaler for input signal.
  *         [in]  nDivisor:         The base number about moving window.
  *         [out] fVectorAvgOut[]:  The moving average output in format of vector[x,y,z].
  * @retval [out] ERROR_SUCCESS
  */

int   Vector_MovingAvg_SQR(MOVING_AVG_CFG_T tMovAvgCfg[], float fVectorIn[], int nScale, int nDivisor, float fVectorAvgOut[]);

/**
  * @brief  Calculate the Nth item of Fibonacci number.
  * @param  [in]  N:                the Nth item.
  * @retval [out] the Nth item of Fibonacci number.
  * @note   if N >= 48, the number will over the range of uint32_t. In that case, api will
  *         return the value of Fibonacci_number(47) directly.
  */

uint32_t Fibonacci_number(int N);


/* ==============================================
 * Signal Process Functions
 * ============================================== */

/**
  * @brief  Moving Buffer.
  * @param  [in]  fSampleBuf[]:     The moving buffer with internal rotation.
  *         [in]  nSampleBufSize:   The size of fSampleBuf[].
  *         [in]  fCurrentValue:    Input signal.
  *         [in]  bSwitchOrder:     Switch the order of biffer.
  *                                 [bSwitchOrder = false]
  *                                 Index 0   is the newest value in buffer.
  *                                 Index N-1 is the oldest value in buffer.
  *
  *                                 [bSwitchOrder = true]
  *                                 Index 0   is the oldest value in buffer.
  *                                 Index N-1 is the newest value in buffer.
  * @retval [out] None.
  */

void  MovingBuffer(float fSampleBuf[], uint8_t nSampleBufSize, float fCurrentValue, bool bSwitchOrder);

/**
  * @brief  Detect the device posture by accelerometer, for static & translation movement.
  * @param  [in]  tMovAvgCfg_A[]:       Long term SQR Moving Average config A. (Ex: MOVING_AVG_CFG_T tMovAvgCfg_A[3])
  *         [in]  nBufSize_A:           Standard Deviation buffer size A. (Default = 100)
  *         [in]  pfStdDevBuf_B[]:      Standard Deviation buffer B. (Ex: float fStdDevBuf_B[3][CFG_STD_DEVIATION_BUFSIZE])
  *         [in]  nBufSize_B:           Standard Deviation buffer size B. (Default = 20)
  *         [in]  fAccelerometer:       Accelerometer signal in.
  *         [out] *ptPostureDetectInfo: Return the result about the posture information.
  * @retval [out] bIsPostureChanged:    Is the posture changed?
  */

bool  PostureDetect_A(MOVING_AVG_CFG_T tMovAvgCfg_A[], uint8_t nBufSize_A, float *pfStdDevBuf_B[], uint8_t nBufSize_B, float fAccelerometer[], POSTURE_DETECT_INFO_T* ptPostureDetectInfo);

/**
  * @brief  Discrete-time realization High-Pass filter.
  * @param  [in]  fCutoff_Freq_Hz:      Cutoff frequency Fc in Hz
  *                                     1. Set fCutoff_Freq_Hz = 0 to bypass the filter.
  *         [in]  fSampling_Rate_Hz:    Data sampling rate in Hz
  *                                     1. Set fSampling_Rate_Hz = 0 to bypass the filter.
  *         [in]  fX_In:                Current input signal that we want to filter ( X[i] ).
  *         [in]  fX_In_pre:            Previous input signal that we want to filter ( X[i-1] ).
  *         [in]  fY_Out_pre:           Previous filtered output signal ( Y[i-1] ).
  * @retval [out] fY_Out:               Filtered signal output ( Y[i] ).
  */

float High_Pass_Filter(float fCutoff_Freq_Hz, float fSampling_Rate_Hz, float fX_In, float fX_In_pre, float fY_Out_pre);

/**
  * @brief  Discrete-time realization Low-Pass filter.
  * @param  [in]  fCutoff_Freq_Hz:      Cutoff frequency Fc in Hz
  *                                     1. Set fCutoff_Freq_Hz = 0 to bypass the filter.
  *         [in]  fSampling_Rate_Hz:    Data sampling rate in Hz
  *                                     1. Set fSampling_Rate_Hz = 0 to bypass the filter.
  *         [in]  fX_In:                Current input signal that we want to filter ( X[i] ).
  *         [in]  fX_In_pre:            Previous input signal that we want to filter ( X[i-1] ).
  *         [in]  fY_Out_pre:           Previous filtered output signal ( Y[i-1] ).
  * @retval [out] fY_Out:               Filtered signal output ( Y[i] ).
  */

float Low_Pass_Filter(float fCutoff_Freq_Hz, float fSampling_Rate_Hz, float fX_In, float fX_In_pre, float fY_Out_pre);

/**
  * @brief  Discrete-time realization High-Pass filter for Vector.
  * @param  [in]  fCutoff_Freq_Hz:      Cutoff frequency Fc in Hz
  *                                     1. Set fCutoff_Freq_Hz = 0 to bypass the filter.
  *         [in]  fSampling_Rate_Hz:    Data sampling rate in Hz
  *                                     1. Set fSampling_Rate_Hz = 0 to bypass the filter.
  *         [in]  fX_In[]:              Current input signal that we want to filter ( X[i] ).
  *         [in]  fX_In_pre[]:          Previous input signal that we want to filter ( X[i-1] ).
  *         [in]  fY_Out_pre[]:         Previous filtered output signal ( Y[i-1] ).
  *         [out] *pfY_Out[]:           Filtered signal output ( Y[i] ).
  * @retval [out] None
  */

void  Vector_High_Pass_Filter(float fCutoff_Freq_Hz, float fSampling_Rate_Hz, float fX_In[], float fX_In_pre[], float fY_Out_pre[], float* fY_Out);

/**
  * @brief  Discrete-time realization High-Pass filter for Vector.
  * @param  [in]  ptVecHLPF:            Structure pointer of HLPASS_FILTER_INFO_T
  *         [in]  fX_In[]:              Current input signal that we want to filter ( X[i] ).
  *         [out] *pfY_Out:             Filtered signal output ( Y[i] ).
  * @retval [out] None
  */

void Vector_High_Pass_Filter2(HLPASS_FILTER_INFO_T* ptVecHLPF, float fX_In[], float* pfY_Out);

/**
  * @brief  Discrete-time realization Low-Pass filter for Vector.
  * @param  [in]  fCutoff_Freq_Hz:      Cutoff frequency Fc in Hz
  *                                     1. Set fCutoff_Freq_Hz = 0 to bypass the filter.
  *         [in]  fSampling_Rate_Hz:    Data sampling rate in Hz
  *                                     1. Set fSampling_Rate_Hz = 0 to bypass the filter.
  *         [in]  fX_In[]:              Current input signal that we want to filter ( X[i] ).
  *         [in]  fX_In_pre[]:          Previous input signal that we want to filter ( X[i-1] ).
  *         [in]  fY_Out_pre[]:         Previous filtered output signal ( Y[i-1] ).
  *         [out] *pfY_Out[]:           Filtered signal output ( Y[i] ).
  * @retval [out] None
  */

void  Vector_Low_Pass_Filter(float fCutoff_Freq_Hz, float fSampling_Rate_Hz, float fX_In[], float fX_In_pre[], float fY_Out_pre[], float* fY_Out);

/**
  * @brief  Discrete-time realization Low-Pass filter for Vector.
  * @param  [in]  ptVecHLPF:            Structure pointer of HLPASS_FILTER_INFO_T
  *         [in]  fX_In[]:              Current input signal that we want to filter ( X[i] ).
  *         [out] *pfY_Out:             Filtered signal output ( Y[i] ).
  * @retval [out] None
  */

void Vector_Low_Pass_Filter2(HLPASS_FILTER_INFO_T* ptVecHLPF, float fX_In[], float* pfY_Out);

/**
  * @brief  Remove the target signal's bias information.
  * @param  [in]  tBiasRmv_ShiftReg:      The shift register to record all information to operate the API.
  *         [in]  fCurrentVector[]:       The target signal we want to remove it's bias.
  *         [out] pfBiasVector[]:         Return the result of bias.
  *         [out] pfBiasRemovedVector[]:  Return the result witch bias removed.
  * @retval [out] ERROR_SUCCESS
  */

int   Vector_SignalBiasRemoved(BIAS_REMOVED_SHIFTREG_T *ptBiasRmv_ShiftReg, float fCurrentVector[], float *pfBiasVector, float *pfBiasRemovedVector);

/**
  * @brief  Initialize the Lookup Table of Discrete Fourier Transform (DFT)
  * @param  [in]  nArraySize_N:             The array size of x[].
  *         [out] ptDFT_LUT[]:              The array of DFT_LOOKUP_TBL_T, with size of nArraySize_N
  *                                         If NULL then use normal function call, it will enlarge the resource consumption.
  * @retval [out] ERROR_SUCCESS
  */

int InitLUT_Discrete_Fourier_Transform_DFT(int nArraySize_N, DFT_LOOKUP_TBL_T *ptDFT_LUT);

/**
  * @brief  Discrete Fourier Transform (DFT)
  * @param  [in]  fX_In[]:                  Input signal x[].
  *         [in]  nArraySize_N:             The array size of x[].
  *         [in]  ptDFT_LUT[]:              The array of DFT_LOOKUP_TBL_T, with size of nArraySize_N
  *                                         If NULL then use normal function call, it will enlarge the resource consumption.
  *         [out] pfY_DFT_Amplitude_Out[]:  Return the result of DFT power.
  *         [in]  bSwitchOrder:             Switch the order of x[].
  *                                         False:  Oldest x[0]~x[N-1] Newest
  *                                         True:   Newest x[0]~x[N-1] Oldest
  *         [in]  bDivided_By_N:            Whether the value of pfY_DFT_Amplitude_Out[] divided by nArraySize_N or not.
  * @retval [out] ERROR_SUCCESS
  */

int   Discrete_Fourier_Transform_DFT(float fX_In[], int nArraySize_N, DFT_LOOKUP_TBL_T *ptDFT_LUT, float *pfY_DFT_Amplitude_Out, bool bSwitchOrder, bool bDivided_By_N);


/* ==============================================
 * Matrix Calculation
 * ============================================== */
void  matrix_multiply(float *A, float *B, float *C, int AR, int AC, int BC);
void  matrix_multiply_trans(float *A, float *B, float *C, int AR, int AC, int BR);
void  ABAT(float *A, float *B, float *C, int AR, int AC);
void  matrix_inverse(float *A, float *AI);
float matrix_length(float *matrix, int size);
void  matrix_normalize(float *matrix, int size);


/* ==============================================
 * Internal Tester: Avoid the compile warning.
 * ============================================== */
void  cwm_common_utility_float_test(float *acc_g);

#ifdef __cplusplus
}
#endif

#endif /* _CWM_COMMON_UTILITY_FLOAT_H_ */
