/*
    *   Sensors requirement:
    *       Rotation Vector:
    *           rate: 10 ms
    *
    *   Trigger source:
    *       fusion, any kind of rotation vector
    *
    *   Sensors type:
    *       continuous
    *
    *   Low power mode support:
    *       No
    *
    *   Output:
    *       size: float[2] * 1
    *
    *   Output report format:
    *
    *   Algo introduction:
    *
    *   Algo test scenario:
    *
    *   Algo history list:
    *       V00.01.00_20171108: 1. First release for HTC's demo.
    *         (Edit: SC.Chang)
    *
    *   Verification checksum:
    *
    *     V00.01.00_20171108    Ex-Shared memory size: 36(0x24) bytes.
    *
    *     SHA-1: 79B2EC884D386439C3D5E4C4161B028F03D156E8 / MD5: CAE35428797EF329AF09C8A2F0D08BE0 (ADS5)
    *     SHA-1: D4E4EE887FC7D1C41384D4DF25A1E16323D93645 / MD5: FCDFAD22A048E4BFCD203C212E7F0822 (AGCC)
    *     SHA-1: C7D81CF6D4701A5A87149B373E60088005703C82 / MD5: AC16231914FA99B6D5FB5AB8B22BBB3A (HEXAGON)
    *     SHA-1: C505A86CE6A547FF9B473853E4375EB36A7115B6 / MD5: A9284B70D7D9984C87C25FC53E89F005 (IAR)
    *     SHA-1: 913BE5424DF1537F258B7E38A454F1523B9350B3 / MD5: 9F46A48EB1BFA8777A94A0A2134243B7 (KEIL M0)
    *     SHA-1: 95FE4E75BC82B27BE8E26C60F35EB57FE39AA94E / MD5: 44B3F67A8A7BED2BC550AA01213AE489 (KEIL M4)
    *     SHA-1: D45A49566133EC5A11E937FECD5F4643DBF54B15 / MD5: 0B8B0D889A831DE8783ADE0AB2297669 (XPLR NANOHUB)
    *     SHA-1: 34A2869F83A9593F84B3D8117C8A1CF4A62DA98F / MD5: AD8E48E3C7D34D7406D5203D0CD7EAC2 (XPLR)
    *     SHA-1: 88F1BE4421588B05EDF0BD0455828D158002CE50 / MD5: BD9A9A00F040225EF015A3AC7562796B (LINUX_PIXEL)
    *     SHA-1: D7BFE313CD6107CD0E74992DD88A75EE5EBE8965 / MD5: 87B1A40538D0775D6CF5056FB313B361 (LINUX_SPRD)
    *     SHA-1: EA500EEDB4CE845253FECE972026BCAB21C77F4C / MD5: F56D7EB0F38AE45CBD39E3EF390CA2A9 (LINUX_SPRD)
*/

#ifndef __CWM_AIR_MOUSE_H__
#define __CWM_AIR_MOUSE_H__

#ifdef __cplusplus
extern "C" {
#endif

/*
    *
    *   Get version
    *
    *   Output:
    *       version[0] = date (yyyymmdd)
    *       version[1] = version
    *       version[2] = subversion
    *       version[3] = build
    *
    *
*/
void version_cwm_air_mouse(int *version);

/*
    *
    *   Get shared memory size for assigned structure.
    *
*/
int memsize_cwm_air_mouse(void);

/*
    *
    *   Initialize variables
    *
*/
void init_cwm_air_mouse(void *hAirMouseMem);

/*
    *
    *   Call this function when enable this sensor
    *
*/
void enable_cwm_air_mouse(void *hAirMouseMem);

/*
    *
    *   Call this function when disable this sensor
    *
*/
void disable_cwm_air_mouse(void *hAirMouseMem);

/*
    *
    *   Set configurations and parameters of algorithm
    *
    *   Default: int data[3] = {1920, 1080, 60};
    *   data[0] = Screen Resolution - Width
    *   data[1] = Screen Resolution - Height
    *   data[2] = Operation Front angle range
    *
*/
void set_config_cwm_air_mouse(void *hAirMouseMem, void *data, int size);

/*
    *
    *   Run algorithm
    *
    *   Input:
    *       fRot_vec[0] = rotation vector[0] (X)
    *       fRot_vec[1] = rotation vector[1] (Y)
    *       fRot_vec[2] = rotation vector[2] (Z)
    *       fRot_vec[3] = rotation vector[3] (W)
    *       fRot_vec[4] = estimated heading accuracy (radian)
    *
    *   Output:
    *       fMovement[0] = movement on horizontal (dx)
    *       fMovement[1] = movement on vertical   (dy)
    *
    *   Return value:
    *       -9 = output set config at first run
    *       -8 = set config error
    *       -4 = data error
    *       -3 = dt error (dt <= 0 || dt > 1000000)
    *       -2 = not initialzed
    *       -1 = error
    *        0 = nothing to do
    *        1 = report event
    *
*/
int cwm_air_mouse(void *hAirMouseMem, float *fRot_vec, float *fMovement, float dt_us);

#ifdef __cplusplus
}
#endif

#endif /* __CWM_AIR_MOUSE_H__ */
