/*
    *   Sensors requirement:
    *       Accelerometer:
    *           rate: 10 ms
    *       Gyroscope:
    *           rate: 10 ms
    *       Magnetic Field Sensor:
    *           rate: 20 ms
    *
    *   Trigger source:
    *       Gyroscope (fusion, game rotation vector)
    *       Accelerometer (geomagnetic rotation vector)
    *
    *   Sensors type:
    *       continuous
    *
    *   Low power mode support:
    *       No
    *
    *   Output:
    *       size: float[5] * 3 + float[3] * 3
    *
    *   Output report format:
    *       rot_vec[0] = rotation vector[0] (unit vector)
    *       rot_vec[1] = rotation vector[1] (unit vector)
    *       rot_vec[2] = rotation vector[2] (unit vector)
    *       rot_vec[3] = rotation vector[3] (unit vector)
    *       rot_vec[4] = estimated heading accuracy (radian)
    *       game_rot_vec[0] = game rotation vector[0] (unit vector)
    *       game_rot_vec[1] = game rotation vector[1] (unit vector)
    *       game_rot_vec[2] = game rotation vector[2] (unit vector)
    *       game_rot_vec[3] = game rotation vector[3] (unit vector)
    *       game_rot_vec[4] = 0 (reserved)
    *       geo_rot_vec[0] = geomagnetic rotation vector[0] (unit vector)
    *       geo_rot_vec[1] = geomagnetic rotation vector[1] (unit vector)
    *       geo_rot_vec[2] = geomagnetic rotation vector[2] (unit vector)
    *       geo_rot_vec[3] = geomagnetic rotation vector[3] (unit vector)
    *       geo_rot_vec[4] = estimated heading accuracy (radian)
    *       orien[0] = azimuth of Android axis (degree)
    *       orien[1] = pitch of Android axis (degree)
    *       orien[2] = roll of Android axis (degree)
    *       la[0] = x-axis data of linear acceleration (m/s^2)
    *       la[1] = y-axis data of linear acceleration (m/s^2)
    *       la[2] = z-axis data of linear acceleration (m/s^2)
    *       grav[0] = x-axis data of gravity (m/s^2)
    *       grav[1] = y-axis data of gravity (m/s^2)
    *       grav[2] = z-axis data of gravity (m/s^2)
    *
    *   Algo introduction:
    *       An orientation sensor reports the attitude of the device.
    *       A gravity sensor reports the direction and magnitude of gravity in the device's
    *       coordinates.
    *       A linear acceleration sensor reports the linear acceleration of the device in the
    *       sensor frame, not including gravity.
    *       A rotation vector sensor reports the orientation of the device relative to the
    *       East-North-Up coordinates frame.
    *       A game rotation vector sensor is similar to a rotation vector sensor but not using the
    *       geomagnetic field.
    *       A geomagnetic rotation vector is similar to a rotation vector sensor but using a
    *       magnetometer and no gyroscope.
    *
    *   Algo test scenario:
    *       1. Calibrate gyroscope. Put the device on the table.
    *          The data should near 0, not over 3 place behind the decimal point.
    *       2. Calibrate magnetic sensor. The accuracy should be 3. The data should be 10 ~ 60.
    *       3. Orientation
    *          A. Y-axis points to North, Z-axis points to Sky   : [0] =   0, [1] =     0, [2] =   0
    *          B. Y-axis points to East , Z-axis points to Sky   : [0] =  90, [1] =     0, [2] =   0
    *          C. Y-axis points to South, Z-axis points to Sky   : [0] = 180, [1] =     0, [2] =   0
    *          D. Y-axis points to West , Z-axis points to Sky   : [0] = 270, [1] =     0, [2] =   0
    *          E. X-axis points to East , Y-axis points to Sky   : [0] =   0, [1] =   -90, [2] =   0
    *          F. X-axis points to East , Y-axis points to South : [0] =   0, [1] = +-180, [2] =   0
    *          G. X-axis points to East , Y-axis points to Ground: [0] =   0, [1] =    90, [2] =   0
    *          H. Y-axis points to North, X-axis points to Sky   : [0] =   0, [1] =     0, [2] =  90
    *          I. Y-axis points to North, X-axis points to West  : [0] = 180, [1] =   180, [2] =   0
    *          J. Y-axis points to North, X-axis points to Ground: [0] =   0, [1] =     0, [2] = -90
    *       4. Linear acceleration
    *          A. Move along positive X-axis fast: [0] = positive, [1] = 0, [2] = 0
    *          B. Move along negative X-axis fast: [0] = negative, [1] = 0, [2] = 0
    *          C. Move along positive Y-axis fast: [0] = 0, [1] = positive, [2] = 0
    *          D. Move along negative Y-axis fast: [0] = 0, [1] = negative, [2] = 0
    *          E. Move along positive Z-axis fast: [0] = 0, [1] = 0, [2] = positive
    *          F. Move along negative Z-axis fast: [0] = 0, [1] = 0, [2] = negative
    *       5. Gravity
    *          A. X-axis points to Sky   : [0] =  9.8, [1] =    0, [2] =    0
    *          B. X-axis points to Ground: [0] = -9.8, [1] =    0, [2] =    0
    *          C. Y-axis points to Sky   : [0] =    0, [1] =  9.8, [2] =    0
    *          D. Y-axis points to Ground: [0] =    0, [1] = -9.8, [2] =    0
    *          E. Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  9.8
    *          F. Z-axis points to Ground: [0] =    0, [1] =    0, [2] = -9.8
    *       6. Rotation vector
    *          A. Y-axis points to North, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =    0
    *          B. Y-axis points to East , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] = -0.7
    *          C. Y-axis points to South, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  +-1
    *          D. Y-axis points to West , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  0.7
    *          E. X-axis points to East , Y-axis points to Sky   : [0] =  0.7, [1] =    0, [2] =    0
    *          F. X-axis points to East , Y-axis points to South : [0] =  +-1, [1] =    0, [2] =    0
    *          G. X-axis points to East , Y-axis points to Ground: [0] = -0.7, [1] =    0, [2] =    0
    *          H. Y-axis points to North, X-axis points to Sky   : [0] =    0, [1] = -0.7, [2] =    0
    *          I. Y-axis points to North, X-axis points to West  : [0] =    0, [1] =  +-1, [2] =    0
    *          J. Y-axis points to North, X-axis points to Ground: [0] =    0, [1] =  0.7, [2] =    0
    *       7. Game rotation vector
    *          *. Start running this algorithm when device Y-axis points to North, Z-axis points to Sky.
    *          A. Y-axis points to North, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =    0
    *          B. Y-axis points to East , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] = -0.7
    *          C. Y-axis points to South, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  +-1
    *          D. Y-axis points to West , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  0.7
    *          E. X-axis points to East , Y-axis points to Sky   : [0] =  0.7, [1] =    0, [2] =    0
    *          F. X-axis points to East , Y-axis points to South : [0] =  +-1, [1] =    0, [2] =    0
    *          G. X-axis points to East , Y-axis points to Ground: [0] = -0.7, [1] =    0, [2] =    0
    *          H. Y-axis points to North, X-axis points to Sky   : [0] =    0, [1] = -0.7, [2] =    0
    *          I. Y-axis points to North, X-axis points to West  : [0] =    0, [1] =  +-1, [2] =    0
    *          J. Y-axis points to North, X-axis points to Ground: [0] =    0, [1] =  0.7, [2] =    0
    *       8. Geomagnetic rotation vector
    *          A. Y-axis points to North, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =    0
    *          B. Y-axis points to East , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] = -0.7
    *          C. Y-axis points to South, Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  +-1
    *          D. Y-axis points to West , Z-axis points to Sky   : [0] =    0, [1] =    0, [2] =  0.7
    *          E. X-axis points to East , Y-axis points to Sky   : [0] =  0.7, [1] =    0, [2] =    0
    *          F. X-axis points to East , Y-axis points to South : [0] =  +-1, [1] =    0, [2] =    0
    *          G. X-axis points to East , Y-axis points to Ground: [0] = -0.7, [1] =    0, [2] =    0
    *          H. Y-axis points to North, X-axis points to Sky   : [0] =    0, [1] = -0.7, [2] =    0
    *          I. Y-axis points to North, X-axis points to West  : [0] =    0, [1] =  +-1, [2] =    0
    *          J. Y-axis points to North, X-axis points to Ground: [0] =    0, [1] =  0.7, [2] =    0
    *
    *   Algo history list:
    *       V01.00.01_20150706: Change to new framework.
    *                           SHA-1: BB4ABC7E73F3340799DA49D4AA64114D9A1F3FE6
    *       V01.01.01_20150804: Fix duplicate definitions for "state_to_matrix".
    *                           SHA-1: A50A859C19FD245E6922684F06F156B8C99CA66F
    *       V01.02.02_20150813: Return 0 when static over 3 seconds.
    *         (Edit: Yu-Kuen)   SHA-1: 50085CBF8C5EAB5708ED789CAAD9E061712BC1B7
    *       V01.03.01_20150817: Reset timer when static for slowly moving situation.
    *         (Edit: Yu-Kuen)   SHA-1: 71BDA60101E97A14A533B901F52C7DA52F5E9015
    *       V01.04.01_20150820: Fine tune the static and correct timing.
    *         (Edit: Yu-Kuen)   SHA-1: F6856B1A02A82CE35A0F4467937BCBC6BD853868
    *       V01.04.02_20150821: Remove unused code.
    *         (Edit: Yu-Kuen)   SHA-1: 71BEDF74CF8C9307C5C87FE8E8D503CEDCE42829
    *       V01.05.01_20150826: Add outputs in rotation vector.
    *         (Edit: Yu-Kuen)   SHA-1: EF01F35371624B0AC24C3D39D736CBF7F13D0BD4
    *       V01.06.01_20150827: Fine tune the parameter.
    *         (Edit: Yu-Kuen)   SHA-1: 843469F8DD6174B11427D342DB65FADAAAC63D7B
    *       V01.07.06_20150915: Add long low-pass filter in geomagnetic rotation vector to reduce
    *                           the shake. Fine tune the static time of no calculation. Minus bias
    *                           when static in linear acceleration.
    *         (Edit: Yu-Kuen)   SHA-1: AA73AB55ACEA19E95A812B19101DE3FFFEE8944B
    *       V01.08.01_20151001: Seperate variables of rotation vector and game rotation vector.
    *                           Modify average code. Add gyro threshold in static detection.
    *                           Fine tune the parameter of correction time. Remove unused code.
    *         (Edit: Yu-Kuen)   SHA-1: A1512C40506A2FBE9AE06BCA3ED2D5D113937316
    *       V01.09.02_20151016: Add API for switching static filter ON/OFF.
    *                           Fine tune the parameter of static filter.
    *                           Add dt limit in geomagnetic rotation vector.
    *         (Edit: Yu-Kuen)   SHA-1: E667D87879CC3F5E8F7B2C86B621B387637F3357
    *       V01.10.02_20151027: Change linear acceleration filter to Alan's filter.
    *         (Edit: Yu-Kuen)   SHA-1: D2C941F557A34FC6D4D715D35B99E9D51019A939
    *       V01.10.03_20151124: Edit initial function type.
    *         (Edit: Yu-Kuen)   SHA-1: 117646F8B5B74253026B5A5F7563FE0010723DE5
    *       V01.10.04_20151125: Edit some text.
    *         (Edit: Yu-Kuen)   SHA-1: 06582BFBB8693323952D457EADAD26B0303F8EFD
    *       V01.11.01_20151126: Add Alan's filter for magnetic correct.
    *         (Edit: Yu-Kuen)   SHA-1: 55DDFDCF8A75CEB63213ADC2F07A5C53AFCD4241
    *       V01.12.02_20151211: Add Alan's filter for magnetic interference.
    *         (Edit: Yu-Kuen)   SHA-1: F7749D07FBA7A2788E2307470B6CB5089A14AB9A
    *       V01.13.01_20151224: Fix bug of merging Alan's filter for magnetic interference.
    *         (Edit: Yu-Kuen)   SHA-1: CCECB29671EE1DC8879DF305969C73875D1B6A30
    *       V01.14.01_20151225: Add DSP instruction set.
    *         (Edit: Yu-Kuen)   SHA-1: C51CEFB21F79FF93677F034414B211E499A67489
    *       V01.15.01_20160121: Game rotation vector initial point to north.
    *         (Edit: Yu-Kuen)   SHA-1: 9F2ED0CFCAC56B5E7511723F1DCDE0B67B4DD4B8
    *       V01.15.02_20160201: Edit PI definition.
    *         (Edit: Yu-Kuen)   SHA-1: 2F5E3EB8561DBB006E7C6B65511FAB897E7C4E31
    *       V01.15.03_20160226: Fix a bug of game rotation vector initial pointing to north.
    *         (Edit: Yu-Kuen)   SHA-1: B1F9EEABB1C0F44A446F972D9DF1B9CC5E84EA31
    *       V01.15.04_20160226: Set initial pose of geomagnetic rotation vector to first sensor data.
    *                           SHA-1: FF087F16EC71A295A57C80BA1F356B4E117E2492
    *       V01.15.05_20160302: Fix a bug of game rotation vector initial pointing to north.
    *         (Edit: Yu-Kuen)   SHA-1: 5802F7EA19303DD84F46964F87347167A34B62B8
    *       V01.16.01_20160317: Edit some typing. Add Cadence library.
    *         (Edit: Yu-Kuen)   SHA-1: E7C3B1156C72DA16C166D41BBEA9EB8779B2CDDE
    *       V01.17.01_20160321: Edit some typing. Add Cadence library.
    *         (Edit: Yu-Kuen)   SHA-1: 4FCF41F639DAE967B3888D85C628ACD0AD0484BA
    *       V01.17.02_20160322: Edit some typing.
    *         (Edit: Yu-Kuen)   SHA-1: 31B7F70A287E74C276ACFA8346B0D700DA4765BD
    *       V01.17.03_20160323: Add Cadence library.
    *         (Edit: Yu-Kuen)   SHA-1: 9EA983FD53DFB6A7E576C5BD4A26CAC40EDD9EEE
    *       V02.01.01_20160328: Change to new format.
    *         (Edit: Yu-Kuen)   SHA-1: 5FF0494E3535EF7AE820DB32F01D1CBEF54E8006
    *       V02.02.01_20160330: Add set config function.
    *         (Edit: Yu-Kuen)   SHA-1: 07B16AB52EC49762D45E2A389A88EEF9EA32FF29
    *       V02.02.02_20160330: Fix a bug of function name.
    *         (Edit: Yu-Kuen)   SHA-1: 90C5805F677415D90482A6C6C26548B486120724
    *       V02.03.01_20160407: Add parameters in set config function. Remove output of mouse orientation.
    *         (Edit: Yu-Kuen)   SHA-1: 2DF60C259D85643237C946222C60F148196AF8AD (IAR)
    *                                  91576879EB28253BE04F596AA8002A0FB6B00C0D (ADS5)
    *       V02.03.02_20160413: Add return output for set config.
    *         (Edit: Yu-Kuen)   SHA-1: B64FFF26AF6A75095528ABF82057DCA11923D506 (IAR)
    *                                  E2E8F0669FB1CDF09518E06731EE58F25625C843 (ADS5)
    *       V02.03.03_20160413: Fix a bug of size of setting config.
    *         (Edit: Yu-Kuen)   SHA-1: BFAF68A23891A63EFBF3FFB7662E52A023AB16C3 (IAR)
    *                                  4F4F56B33E00E6FA645C8ABCAD40C6F1A3D9B717 (ADS5)
    *       V02.03.04_20160426: Fix warnings.
    *         (Edit: Yu-Kuen)   SHA-1: 462E64AF64D00472CCC381D73CFCB75E4D1C3EA5 (GCC)
    *       V02.04.02_20160531: Change standard math library to float version. Fix warnings.
    *                           Add new Cadence library. Separate outputs of fusion.
    *                           Optimize the time of matrix calculation and average filter.
    *                           Rename function names of matrix calculations and sub-functions.
    *         (Edit: Yu-Kuen)   SHA-1: 5BFD939F7D5638A33B5F9088C8546F34E33C943B (IAR)
    *       V02.04.03_20160601: Fix a bug of average filter.
    *         (Edit: Yu-Kuen)   SHA-1: CDB873DC60E80C8E6319742CD9FA7CBFEA228301 (IAR)
    *       V02.05.01_20160602: Separate rotation matrix function.
    *         (Edit: Yu-Kuen)   SHA-1: 06D96BAFA50C95CF84AFC465438395893818B0BA (IAR)
    *                                  0AADF8D1EB1C9A3C90250BCF645663D01315A34D (XPLR)
    *       V02.05.02_20160727: Fix warnings of same names of variables.
    *         (Edit: Yu-Kuen)   SHA-1: 59A33A2F2D6737D166C34901457C240B84034B3F (IAR)
    *       V02.05.03_20161011: Check if initial function has called. Prevent divided by 0.
    *                           Change the limit value of sensors.
    *         (Edit: Yu-Kuen)   SHA-1: 0EDB2DDF7614E372F05F7F50D079DC813107DF9A (IAR)
    *       V02.05.05_20161012: Fix a bug of memory out of range. Prevent triangular function error.
    *         (Edit: Yu-Kuen)   SHA-1: AB2346679C5C7FA01B7E321B2586F6617A829518 (IAR)
    *       V02.05.06_20161013: Move triangular function error prevention to definition.
    *         (Edit: Yu-Kuen)   SHA-1: 512A1F587F3189E052F5116042D93FAD609BF3B9 (IAR)
    *       V02.06.01_20161116: Fix a bug of fourth element of rotation vector.
    *                           Add gyro threshold in acc/mag correct timing.
    *         (Edit: Yu-Kuen)   SHA-1: DEA7E3F829877DF7D30694F12BC81F5299843635 (IAR)
    *       V02.06.02_20161116: Fix a bug of fourth element of game rotation vector.
    *         (Edit: Yu-Kuen)   SHA-1: 79A6F5AEB809D364F4F8B71ACEE11C1EEE8F2FD7 (IAR)
    *       V02.06.03_20161122: Add comments. Add ST nanohub math definition.
    *         (Edit: Yu-Kuen)   SHA-1: 74557E675839A81FB546D1924680365EDE89D044 (IAR)
    * ----- External Shared Memory -----
    *       V02.06.04_20161124: Change to external memory framework.
    *       V03.00.01_20161124:
    *         (Edit: Yu-Kuen)   SHA-1: 49739128E71BEFE0305096F8D4C474AD6572BE62 (Qualcomm)
    *       V03.00.02_20161206: Based on Omar's definition, modify the prototype to support external memory framework.
    *         (Edit: SC.Chang)  SHA-1: BAD7CCFAD206D7E90F42682F5C08214FE999027F / MD5: F0FA78AB9CC30E544A93F0AEA36BDBF6 (IAR)
    *       V03.01.03_20170123: 1. Support APIs of cwm_get_mouse_orien().
    *         (Edit: SC.Chang)  2. Remove the common code for ARM_DSP_LIB platform.
    *                           3. Remove the common code for CADENCE_LIB and default platform.
    *                           SHA-1: 8E346458D17ECC7942465C61814628A7BF5F9D46 / MD5: 9B90DD6AB1513E39E469FD0AEA13603A (IAR)
    *       V03.02.04_20170414: 1. Add APIs: cwm_get_spherical_point() & cwm_get_spherical_coord() for point on spherical surface (x, y, z) & spherical coordinate system (r, theta, phi).
    *         (Edit: SC.Chang)  2. Support Anti-Magnetic feature by ANTI_MAG_CTRL.
    *       V03.03.05_20170512: 1. Modify the API for point on sphere surface & spherical coordinate system.
    *         (Edit: SC.Chang)  2. In "Air Mouse mode", suppress the suddenly jitter when stop moving.
    *                           3. Modify the variable name of timestamp, add unit to make it be more clear.
    *       V03.04.06_20170521: 1. Support the feature of anti-magnetic.
    *         (Edit: SC.Chang)  2. Keep and separate the anti-mag state with original state for output compare with each other.
    *       V03.04.07_20170525: 1. Trust the evaluation result of Anti-Magnetic module when magnetic update.
    *       V03.05.08_20170609: 1. Add Struct_Init_CWM_EKF_FUSION_T() to initial the structure of CWM_EKF_FUSION_T.
    *         (Edit: SC.Chang)  2. Add enum definition CWM_FUSION_OP_MODE_E for different operation mode.
    *                           3. In "Air Mouse mode", support magnetometer and resolve some bugs.
    *                           4. Support "VDR Operation Mode".
    *                           5. Separate the API of cwm_get_non_anti_mag_rot_vec(), and roll back the prototype of cwm_fusion().
    *                           6. Disable the flag of ANTI_MAG_CTRL as default configuration.
    *                           7. Define structure of NDK_MAGNETIC_INFO2 and it should sync with NDK_MAGNETIC_INFO.
    *       V03.06.09_20170619: 1. Integrate the variable of init_mag_duration into structure CWM_FUSION_DLYUPD_T.
    *         (Edit: SC.Chang)  2. Integrate the variable of nMagAccuracy into structure CWM_FUSION_DLYUPD_T.
    *                           3. In Air Mouse mode, refine the behavior for slow motion.
    *                           4. For ALAN_MAGINTER, integrate some variables into the structure of CWM_FUSION_ALAN_MAGINTER_T.
    *                           5. Reduce the magnetic update level from 3 to >= 2.
    *                           6. Resolve the issue about state update for OP_MODE_NORMAL / OP_MODE_VDR.
    *                           7. Integrate the update timing constraint into detect_fusion_update_immediately().
    *       V03.07.10_20170703: 1. Resolve the jitter of rotation vector when static.
    *         (Edit: SC.Chang)  2. Define and integrate some hard code value into FUSION_DLY_UPD_KE_TOLERANCE.
    *                           3. In Air Mouse mode, roll back the gyro constraint about (+/-1.5 dps).
    *                           4. Add mag constraint to avoid the abnormal value {0, 0, 0}.
    *                           5. Resolve the bug about accumulation error.
    *                           6. Integrate and separate the state update process into some types.
    *                           7. When leave the acc_correct range, keep on updating the state during first N seconds.
    *       V03.08.11_20170725: 1. Refine the performance for static, slow motion & Swing Left and Right.
    *         (Edit: SC.Chang)  2. Add cwm_get_orien2() to calculate euler like information by spherical coordinate system.
    *                           3. Rotation vector format transform for release aar.
    *                           4. Support the factory test procedure.
    *       V03.09.12_20170728: 1. Change some API name to be "cwm_fusion_factory_performance_check".
    *         (Edit: SC.Chang)  2. Add NULL pointer check in each API.
    *                           3. Separate the tollerance of factory performance check into 3 parts for each Euler angle.
    *                           4. Calculate the total rotation angle, avoid some operation mistakes.
    *                           5. Support the performance check status output information.
    *                           6. Support the feature of "RETRY", reset some information when START command is received.
    *                           7. Support the call back function for LOG print out.
    *       V03.10.13_20170729: 1. In factory fusion performance check, use timeout replace the "STOP" from user.
    *         (Edit: SC.Chang)  2. Stop the log print out, when the process of fusion performance check is done.
    *       V03.11.00_20170803: 1. For manufacture, update the content of log immediately, to avoid lost some information.
    *         (Edit: SC.Chang)  2. Fix the bug about the constraint of "Device is stable", to avoid the centrifugal force effect the posture.
    *       V03.12.00_20170807: 1. Avoid the route of motion tracking become huge incorrect.
    *         (Edit: SC.Chang)  2. For Air mouse mode, support the huge force detect, and update the posture immediately when it happened.
    *                           3. For Air mouse mode, remove the constraint P-1 as default.
    *       V03.12.01_20170809: 1. For Air mouse mode, before resolve the issue about suddenly jitter when stop moving,
    *         (Edit: SC.Chang)     just release all constraints for temporary use.
    *       V03.12.02_20170809: 1. Add constraint of N-1) When the accumulated error under the assigned threshold.
    *         (Edit: SC.Chang)  2. Speed up the constraint of P-1) When just static and enter into the acc_correct range.
    *       V03.13.00_20170811: 1. Switch to new Gyro Integral method.
    *         (Edit: SC.Chang)  2. Avoid the over correction
    *       V03.13.01_20170814: 1. Remove set_cwm_fusion_factory_CtrlFlag().
    *         (Edit: SC.Chang)
    *       V03.14.00_20170800: 1. Support the tolerance for sensor's Sensitivity Error.
    *         (Edit: SC.Chang)  2. During the first N seconds, the constraints will all be released, and try to calculate the roughly posture ASAP.
    *                           3. In cwm_get_orien2(), avoid the Gimbal Lock.
    *                           4. Add support of adapt to different ODR for static detect.
    *       V03.14.01_20170907: 1. Support dynamic accelerometer update, not process in static only.
    *         (Edit: SC.Chang)  2. Little update for FUSION_ADAPTIVE_ODR.
    *                           3. For FUSION_DYN_ACC_UPD, fine tune some parameters and update constraints.
    *                           4. Support the posture update smoothly, but disable it as default.
    *                           5. Remove memsize_cwm_fusion(), and change some prototype of API which include "void *hFusionMem".
    *       V03.14.02_20170911: 1. Auto calib for variance of accelerometer length.
    *         (Edit: SC.Chang)  2. In GetDynamicAccUpdateInfo(), add constraint for gyroscope length.
    *                           3. Add cwm_get_pos_update_info() & FUSION_POS_UPD_INFO_T for tracking debug.
    *                           4. Resolve the bug of P-1 constraint in Detect_fusion_update_type().
    *       V03.14.03_20170915: 1. Tunimg the dynamic accelerometer update process, and resolve the problem about posture error.
    *         (Edit: SC.Chang)  2. In dynamic acc upodate process, change the target signal to real one, to improve the latency.
    *                           3. In dynamic acc upodate process, release the one of constraint about gyro length.
    *       V03.14.04_20170921: 1. In dynamic acc upodate process, support multiple level to run the process.
    *         (Edit: SC.Chang)  2. In dynamic acc upodate process, it should be controlled by operation mode (nOpMode).
    *                           3. Avoid the vibration when initial the posture state.
    *                           4. Modify the constraints of dynamic acc upodate process.
    *                           5. Add quaternion common utilities for some special posture requirement.
    *                           6. For the purpose to avoid the explicit update process be shown on UI, need to shift a lot of update process into "near static" region.
    *       V03.14.05_20170922: 1. Refine the parameters for dynamic accelerometer update process.
    *         (Edit: SC.Chang)
    *       V03.14.06_20170925: 1. Avoid the problem about initial posture has big jump.
    *         (Edit: SC.Chang)  2. For the requirement of 3GVR02 customer, set FUSION_UPD_STATIC_SEC to be 0.050f, avoid the posture with suddenly drift when stop moving.
    *                           3. In cwm_rotvec_transform()::FORMAT_TRANSFORM_GROUND_TO_PRJ_3GVR02, resolve the problem about quaternion rotation.
    *       V03.14.07_20170926: 1. Refine the quaternion transformation for project 3GVR02.
    *         (Edit: SC.Chang)
    *       V03.14.08_20170927: 1. Resolve the bug in quaternion transformation for project 3GVR02.
    *         (Edit: SC.Chang)
    *       V03.14.09_20170929: 1. Refine the dynamic acc update to reduce the vibration for project 3GVR02.
    *         (Edit: SC.Chang)  2. In fusion_update(), return 0 when state initialize process is running.
    *       V03.15.00_20171011: 1. When device start up, separate to 2 phases, after all process is done then return the rotation vector.
    *         (Edit: SC.Chang)  2. In acc_update(), change the way by quaternion rotation and avoid the yaw angle be effected during process.
    *       V03.15.01_20171020: 1. Disable the APIs about factory manufacturing by FUSION_FACTORY_TEST.
    *         (Edit: SC.Chang)  2. Refine the parameters for acc_update(), to avoid the vibration in VR headset.
    *       V03.15.02_20171026: 1. Avoid the vibration caused by acc_update().
    *         (Edit: SC.Chang)
    *       V03.15.03_20171031: 1. In mag_update(), change the way by quaternion rotation and avoid the pitch / roll angle be effected during process.
    *         (Edit: SC.Chang)  2. Avoid the vibration caused by mag_update().
    *                           3. Support dynamic magnetometer update, not process in static only.
    *       V03.15.04_20171102: 1. In Daydream, the initial duration should not too long, otherwise Daydream will use {0,0,0,1} as default rotation vector.
    *         (Edit: SC.Chang)  2. In acc_update(), fix the bug about Gimbal Lock in roll.
    *                           3. When just static, after the time duration, stop the mag_update() to avoid be interfered by environment.
    *                           4. Disable the feature of dynamic accelerometer update temporarily.
    *                           5. In acc_update() / mag_update(), add parameter to control the update speed.
    *       V03.15.05_20171106: 1. Support the hard static detect.
    *         (Edit: SC.Chang)  2. Support simple Anti-Magnetometer interference.
    *                           3. Refine the parameters for 9-axes fusion.
    *
    *   Verification checksum:
    *
    *     V03.15.05_20171106
    *
    *     SHA-1: DEBE08BAF0F26AC87D139D5BC87DC8837BC369F4 / MD5: 512ED68C602794E389A6926B8A6A0C76 (ADS5)
    *     SHA-1: 026AA7FA7A5CC7AD42F9B59EC5EB7FBF2FC2F269 / MD5: A21CEAE7FF8EED313D895DC9066803A7 (AGCC)
    *     SHA-1: CECFD95B8EB4F9EDACFE7F93BBBA0CD769075A40 / MD5: E72C75526FDC04C4195497FCF5D7B5EF (HEXAGON)
    *     SHA-1: 4143FF3CE245AD95DFED8FC21310C88B90B54AB7 / MD5: 6B3762E2123B6FDCD6D0527DFFC70167 (IAR)
    *     SHA-1: 356FCEC70B1196E9B121873667B61B05751A80F7 / MD5: 32EEFD5E2C11B48CD400C2033904DC87 (KEIL M0)
    *     SHA-1: 940FE7F4C063BBBDDE3D36A721C66D6869DBC95C / MD5: 395829ADCA398A1C329E9B70F2D8FABF (KEIL M4)
    *     SHA-1: 420D10BFDE308F0350471920E1701964C648A321 / MD5: 5ADEE82E7CB3126D001820DA0C57F894 (XPLR NANOHUB)
    *     SHA-1: 3689FC16A0B3E816B7835D472332064FA2D16949 / MD5: 8779EECCF4A9B14EF1A60C61B068FEE1 (XPLR)
    *     SHA-1: 9B7860D05E24434BB6AE0753668905D16C47C568 / MD5: 8CFADB171A53ADB7D543EDDB3CDCC05D (LINUX_PIXEL)
    *     SHA-1: D03020DA92B7D1136E408397B08268858DFC0E0F / MD5: C12EDAD3ECC9E0F0DC154FF1CDA02218 (LINUX_SPRD)
    *     SHA-1: DC651F14B463CF6BDDC092CC5A0EF7CCF8483215 / MD5: DC9F6590173B0F16745739EAA927F006 (LINUX_SPRD)
*/

#ifndef __CwmFusion_H__
#define __CwmFusion_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cwm_algo_callback.h"

// For function switching issue, this structure should sync with NDK_MAGNETIC_INFO
typedef struct {
    float strength;
    float mag_ref[3];       // {x,y,z} data of reference magnetic field sensor (uT)
    float mag_estimate[3];  // {x,y,z} data of estimate magnetic field sensor (uT)
    char  trustMag;         // Note: use char replace bool.
} NDK_MAGNETIC_INFO2;

typedef enum {
    SPHERE_POINTER_X_AXIS = 0,    // 0
    SPHERE_POINTER_Y_AXIS,        // 1
    SPHERE_POINTER_Z_AXIS,        // 2
    SPHERE_POINTER_NEG_X_AXIS,    // 3
    SPHERE_POINTER_NEG_Y_AXIS,    // 4
    SPHERE_POINTER_NEG_Z_AXIS,    // 5
} FUSION_CFG_SPHERE_POINTER_AXIS_E;

typedef enum {
    SPHERE_PHI_FROM_X_AXIS,
    SPHERE_PHI_FROM_Y_AXIS,
} FUSION_CFG_SPHERECOORD_PHI_FROM_AXIS_E;

void version_cwm_fusion(int *version);
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

int memsize_cwm_rot_vec(void);
int memsize_cwm_game_rot_vec(void);
int memsize_cwm_geo_rot_vec(void);
/*
    *
    *   Get shared memory size for assigned structure.
    *
*/


void init_cwm_fusion(void *hRotMem);
void init_cwm_game_rot_vec(void *hGameMem);
void init_cwm_geo_rot_vec(void *hGeoMem);
/*
    *
    *   Initialize variables
    *
*/

void enable_cwm_fusion(void *hRotMem);
void enable_cwm_game_rot_vec(void *hGameMem);
void enable_cwm_geo_rot_vec(void *hGeoMem);
/*
    *
    *   Call this function when enable this sensor
    *
*/

void disable_cwm_fusion(void *hRotMem);
void disable_cwm_game_rot_vec(void *hGameMem);
void disable_cwm_geo_rot_vec(void *hGeoMem);
/*
    *
    *   Call this function when disable this sensor
    *
*/

void set_config_cwm_fusion(void *hRotMem, void *data, int size);
void set_config_cwm_game_rot_vec(void *hGameMem, void *data, int size);
/*
    *
    *   Set configurations and parameters of algorithm
    *
    *   Default: char data[5] = {1, 15, 3, 3, 0};
    *   data[0] = static filter ON / OFF < 1 ~ 2 >
    *             (1: DO NOT run algorithm when static for some time <-> 2: always run algorithm)
    *   data[1] = time threshold of static filter < 1 ~ 30 >
    *             (1: static for 1 second <-> 30: static for 30 second)
    *   data[2] = level threshold of static filter < 1 ~ 5 >
    *             (1: allow small movement <-> 5: need to be more static)
    *   data[3] = level threshold of acc/mag correction < 1 ~ 5 >
    *             (1: smooth but slow <-> 5: fast but sharp)
    *   data[4] = Operation Mode
    *             (0: Normal mode, 1: Air mouse mode, 2: VDR mode)
    *
*/

void set_config_cwm_geo_rot_vec(void *hGeoMem, void *data, int size);
/*
    *
    *   Set configurations and parameters of algorithm
    *
    *   Default: char data[4] = {1, 15, 3, 3};
    *   data[0] = static filter ON / OFF < 1 ~ 2 >
    *             (1: DO NOT run algorithm when static for some time <-> 2: always run algorithm)
    *   data[1] = time threshold of static filter < 1 ~ 30 >
    *             (1: static for 1 second <-> 30: static for 30 second)
    *   data[2] = level threshold of static filter < 1 ~ 5 >
    *             (1: allow small movement <-> 5: need to be more static)
    *   data[3] = level threshold of smooth filter when static < 1 ~ 5 >
    *             (1: smooth but slow <-> 5: fast but sharp)
    *
*/

void cwm_trust_magnetic_field(void *hRotMem, int nYES_NO);
/*
    *
    *   For the purpose of anti-magnetic interference, may I trust the current magnetic field?
    *
    *   Input:
    *       nYES_NO = 0: NO, 1: YES
    *
*/

int cwm_get_magnetic_field_info(void *hRotMem, NDK_MAGNETIC_INFO2 *pMagInfo);
/*
    *
    *   Get the current information about magnetic field.
    *
    *   Output:
    *       *pMagInfo = memory pointer to the structure of NDK_MAGNETIC_INFO in cwm_anti_magnetic.
    *
    *   Return value:
    *       0 / 1 = whether the current magnetic field can be trusted or not.
    *
*/

int cwm_fusion(void *hRotMem, float *acc, float *gyro, float *mag, float *rot_vec, float dt_us);
int cwm_game_rot_vec(void *hGameMem, float *acc, float *gyro, float *game_rot_vec, float dt_us);
int cwm_geo_rot_vec(void *hGeoMem, float *acc, float *mag, float *geo_rot_vec, float dt_us);
/*
    *
    *   Run algorithm
    *
    *   Input:
    *       acc[0] = x-axis data of accelerometer (m/s^2)
    *       acc[1] = y-axis data of accelerometer (m/s^2)
    *       acc[2] = z-axis data of accelerometer (m/s^2)
    *       gyro[0] = x-axis data of gyroscope (rad/s)
    *       gyro[1] = y-axis data of gyroscope (rad/s)
    *       gyro[2] = z-axis data of gyroscope (rad/s)
    *       mag[0] = x-axis data of magnetic field sensor (uT)
    *       mag[1] = y-axis data of magnetic field sensor (uT)
    *       mag[2] = z-axis data of magnetic field sensor (uT)
    *       mag[3] = the accuracy of magnetic field
    *       dt_us = time difference between each algorithm call (us)
    *
    *   Output:
    *       rot_vec[0] = rotation vector[0] (unit vector)
    *       rot_vec[1] = rotation vector[1] (unit vector)
    *       rot_vec[2] = rotation vector[2] (unit vector)
    *       rot_vec[3] = rotation vector[3] (unit vector)
    *       rot_vec[4] = estimated heading accuracy (radian)
    *       game_rot_vec[0] = game rotation vector[0] (unit vector)
    *       game_rot_vec[1] = game rotation vector[1] (unit vector)
    *       game_rot_vec[2] = game rotation vector[2] (unit vector)
    *       game_rot_vec[3] = game rotation vector[3] (unit vector)
    *       game_rot_vec[4] = 0 (reserved)
    *       geo_rot_vec[0] = geomagnetic rotation vector[0] (unit vector)
    *       geo_rot_vec[1] = geomagnetic rotation vector[1] (unit vector)
    *       geo_rot_vec[2] = geomagnetic rotation vector[2] (unit vector)
    *       geo_rot_vec[3] = rotation vector[3] (unit vector)
    *       geo_rot_vec[4] = estimated heading accuracy (radian)
    *
    *   Return value:
    *       -9 = output set config at first run
    *            (rot_vec[0] = data[0] + data[1] * 100, rot_vec[1] = data[2] + data[3] * 100)
    *       -8 = set config error
    *       -4 = data error (acc == 0 || acc > 128 || acc < -128 || gyro > 50 || gyro < -50)
    *       -3 = dt error (dt <= 0 || dt > 1000000)
    *       -2 = not initialzed
    *       -1 = error
    *        0 = nothing to do
    *        1 = report event
    *
*/

void cwm_get_rot_mat(float *rot_vec, float *rot_mat);
/*
    *
    *   Get Rotation matrix
    *
    *   Input:
    *       rot_vec[5] = rotation vector
    *
    *   Output:
    *       rot_mat[9] = rotation matrix
    *
*/

void cwm_get_orien(float *rot_mat, float *orien);
void cwm_get_orien2(float *rot_mat, float *orien);
void cwm_get_mouse_orien(float *rot_mat, float *mouse_orien);
void cwm_get_linear_acc(void *hRotMem, float *rot_mat, float *la);
void cwm_get_gravity(float *rot_mat, float *grav);
void cwm_get_point_on_spheresurface(float *rot_mat, float *axis_dir, float *ref_point, float *tar_PtSphereSurface);
void cwm_get_point_on_spheresurface_axis(float *rot_mat, FUSION_CFG_SPHERE_POINTER_AXIS_E eumCfgPointerAxis, float *tar_PtSphereSurface);
void cwm_get_spherical_coord(float *rot_mat, float *axis_dir, float *ref_point, float *tar_Spherical_r_theta_phi, FUSION_CFG_SPHERECOORD_PHI_FROM_AXIS_E eumCfgBasePhiAxis);
void cwm_get_spherical_coord_axis(float *rot_mat, float *tar_Spherical_r_theta_phi, FUSION_CFG_SPHERE_POINTER_AXIS_E eumCfgPointerAxis, FUSION_CFG_SPHERECOORD_PHI_FROM_AXIS_E eumCfgBasePhiAxis);
/*
    *
    *   Get fusion results
    *
    *   Input:
    *       rot_mat[9]   = rotation matrix
    *       axis_dir[3]  = axis direction. Ex (-1, 1, 1) or NULL
    *       ref_point[3] = reference point. Ex: (0, 1, 0)
    *       eumCfgPointerAxis = The index of pointer axis, which should be one of value in FUSION_CFG_SPHERECOORD_PHI_FROM_AXIS_E
    *       eumCfgBasePhiAxis = The index of phi angle strat from, which should be one of value in FUSION_CFG_SPHERECOORD_PHI_FROM_AXIS_E
    *
    *   Output:
    *       orien[0] = azimuth of Android axis (0 ~ 360 degree)
    *       orien[1] = pitch of Android axis (-180 ~ 180 degree)
    *       orien[2] = roll of Android axis (-90 ~ 90 degree)
    *       mouse_orien[0] = azimuth (-180 ~ 180 degree)
    *       mouse_orien[1] = pitch (-90 ~ 90 degree)
    *       mouse_orien[2] = roll (-180 ~ 180 degree)
    *       la[0] = x-axis data of linear acceleration (m/s^2)
    *       la[1] = y-axis data of linear acceleration (m/s^2)
    *       la[2] = z-axis data of linear acceleration (m/s^2)
    *       grav[0] = x-axis data of gravity (m/s^2)
    *       grav[1] = y-axis data of gravity (m/s^2)
    *       grav[2] = z-axis data of gravity (m/s^2)
    *       tar_PtSphereSurface[3] = target point (x, y, z) which rotated from ref_point[].
    *       tar_Spherical_r_theta_phi[0] = radial distance r of target point in spherical coordinate system.
    *       tar_Spherical_r_theta_phi[1] = polar angle theta (degree) of target point in spherical coordinate system. PS: start from +Z
    *       tar_Spherical_r_theta_phi[2] = azimuthal angle phi (degree) of target point in spherical coordinate system. PS: start from +X
    *
*/

int cwm_get_non_anti_mag_rot_vec(void *hRotMem, float *non_anti_mag_rot_vec);
/*
    *   For Demo purpose only, need special library version:
    *       when both of Anti-Mag / Non-Anti-Mag rotation vector are enabled.
    *
    *   Output:
    *       non_anti_mag_rot_vec[0] = rotation vector[0] with Non-Anti-Magnetic support (unit vector)
    *       non_anti_mag_rot_vec[1] = rotation vector[1] with Non-Anti-Magnetic support (unit vector)
    *       non_anti_mag_rot_vec[2] = rotation vector[2] with Non-Anti-Magnetic support (unit vector)
    *       non_anti_mag_rot_vec[3] = rotation vector[3] with Non-Anti-Magnetic support (unit vector)
    *       non_anti_mag_rot_vec[4] = estimated heading accuracy with Non-Anti-Magnetic support (radian)
 */


typedef enum {
    FORMAT_TRANSFORM_BY_PASS              = 0,
    FORMAT_TRANSFORM_ANDROID_TO_DAYDREAM  = 1,
    FORMAT_TRANSFORM_DAYDREAM_TO_ANDROID  = 2,
    FORMAT_TRANSFORM_GROUND_TO_PRJ_3GVR02 = 3,
} FUSION_CFG_FORMAT_TRANSFORM_E;

int cwm_rotvec_transform(float *rot_vec_xyzw_in, float *rot_vec_xyzw_out, FUSION_CFG_FORMAT_TRANSFORM_E eumCfgFormatType, float fScale, float *rot_matrix_4x4);
/*
    *   Transform rotation vector to some normalized format.
    *
    *   Input:
    *       rot_vec_xyzw_in[4]  = rotation vector input with {X, Y, Z, W} order.
    *       eumCfgFormatType    = format transform index, which should be one of value in FUSION_CFG_FORMAT_TRANSFORM_E
    *       fScale              = some format need or is scaled, if want to skip this, please input 1.0f
    *       rot_matrix_4x4[16]  = rotation matrix (4x4) for axis.
    *                             If NULL, will be handled will internally as default setting.
    *
    *   Output:
    *       rot_vec_xyzw_out[4] = rotation vector output with {X, Y, Z, W} order.
    *
    *   Return:
    *       0: Success, -1: Error
 */


/*
 * Quaternion common utilities
 * ========================================== */
typedef enum _FUSION_CFG_QUATERNION_TYPE_E {
    QUATERNION_TYPE_WXYZ,       // 0
    QUATERNION_TYPE_XYZW,       // 1
} FUSION_CFG_QUATERNION_TYPE_E;

// Convert quaternion type between XYZW <==> WXYZ
void Convert_Quaternion_Type(float fQuaternion[], FUSION_CFG_QUATERNION_TYPE_E eumCfgFromQuatType, FUSION_CFG_QUATERNION_TYPE_E eumCfgToQuatType, float fQuaternionTo[]);
void Get_Quaternion_Conjugate(float fQuaternion[], FUSION_CFG_QUATERNION_TYPE_E eumCfgFromQuatType, FUSION_CFG_QUATERNION_TYPE_E eumCfgToQuatType, float fQuaternionsConj[]);
void Multiple_Quaternion_PQ(float fQuaternion_P[], float fQuaternion_Q[], FUSION_CFG_QUATERNION_TYPE_E eumCfgQuatType, float fQuaternion_PQ[]);
void Get_Rebased_Quaternion(float fQuaternion[], float fQuaternion_Base[], FUSION_CFG_QUATERNION_TYPE_E eumCfgQuatType, float fQuaternion_Rebased[]);

/*
    *   Quaternion common utilities.
    *
    *   Input:
    *       fQuaternion[4]      = input rotation vector, with {X, Y, Z, W} or {W, X, Y, Z} order.
    *       eumCfgFromQuatType  = input quaternion type.
    *       eumCfgToQuatType    = output quaternion type.
    *       eumCfgQuatType      = input / output quaternion type.
    *
    *   Output:
    *       fQuaternionTo[4]    = rotation vector output with eumCfgToQuatType order.
    *       fQuaternionsConj[4] = Conjugate rotation vector output with eumCfgToQuatType order.
    *       fQuaternion_PQ[4]   = rotation vector output PQ = P * Q
    *
    *   Return:
    *       N/A
 */


/*
 * For Tracking Debug
 * ========================================== */
typedef struct {
    float fVarianceG[3];
    int   nCorrectFlag[4];
} FUSION_POS_UPD_INFO_T;

int cwm_get_pos_update_info(void *hRotMem, FUSION_POS_UPD_INFO_T* ptPosUpdInfo);


/*
 * For Factory Performance Test
 * ========================================== */
typedef struct {
    float fTolPercentage[3];
} FUSION_FACTORY_CONFIG_T;

typedef enum {
    FACTORYRC_TYPE_NO_OUTPUT        = 0,
    FACTORYRC_TYPE_PASS             = 1,
    FACTORYRC_TYPE_FAIL             = -1,
    FACTORYRC_TYPE_MEMORY_ERROR     = -2,
    FACTORYRC_TYPE_PARAMETER_ERROR  = -3,
    FACTORYRC_TYPE_INIT_ERROR       = -4,
} FUSION_FACTORY_RC_TYPE_E;

int memsize_cwm_fusion_factory_performance_check(void);
int init_cwm_fusion_factory_performance_check(void *hFactoryMem);
int set_cwm_fusion_factory_cb_register(void *hFactoryMem, pCWM_ALGO_CB_Handle_t ptAlgo_cb);
int set_config_cwm_fusion_factory_performance_check(void *hFactoryMem, FUSION_FACTORY_CONFIG_T *ptCfgParm);
int set_cwm_fusion_factory_JIG_Ready_Flag(void *hFactoryMem);
/*
    *   Run the fusion factory performance test.
    *
    *   Input:
    *       ptCfgParm     = the structure point of FUSION_FACTORY_CONFIG_T.
    *                       Ex: FUSION_FACTORY_CONFIG_T tFusionPerformanceCheckCfgParm = { 0.5f, 0.1f, 0.1f };
    *       nStartOrStop  = 0:STOP, 1:START
    *
    *   Return:
    *       0: No output, <0: Error (Ref: FUSION_FACTORY_RC_TYPE_E)
 */

int  cwm_fusion_factory_performance_check(void *hFactoryMem, void *hRotMem, float rot_mat[], int *nStatus_out, float dt_us);
/*
    *   Fusion factory performance test.
    *
    *   Input:
    *       rot_mat[9]    = rotation matrix
    *
    *   Output:
    *       nStatus_out   = return status
    *
    *   Return:
    *       0: No output, 1: PASS, -1: FAIL (Ref: FUSION_FACTORY_RC_TYPE_E)
 */

// For debug used.
int get_cwm_fusion_factory_performance_check_info(void *hFactoryMem, void *hRotMem, char *szLogBuf);

#ifdef __cplusplus
}
#endif

#endif /* __CwmFusion_H__ */
