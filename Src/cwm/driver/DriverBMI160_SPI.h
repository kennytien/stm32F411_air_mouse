#ifndef __DriverBmi160SPI_H__
#define __DriverBmi160SPI_H__

#include "cwmDriverHal.h"

#ifdef DRIVER_USE_BMI160_SPI

extern int DriverBMI160SPIAccInit(pDriverSensorMem_t pSensorMem);
extern int DriverBMI160SPIMotionInit(pDriverSensorMem_t pSensorMem);
extern int DriverBMI160SPIGyroInit(pDriverSensorMem_t pSensorMem);

extern const DriverServiceCallback_t conBMI160SPIAccCallBack;
extern const DriverServiceCallback_t conBMI160SPIMotionCallBack;
extern const DriverServiceCallback_t conBMI160SPIGyroCallBack;

//===============================================================================
//      #### Register Map ####
//===============================================================================
#define REG_CHIP_ID         0x00
#define REG_ERR_REG         0x02
#define REG_PMU_STATUS      0x03
#define REG_RHALL_L         0x0A
#define REG_RHALL_H         0x0B
#define REG_DATA_GYR_XL     0x0C
#define REG_DATA_GYR_XH     0x0D
#define REG_DATA_GYR_YL     0x0E
#define REG_DATA_GYR_YH     0x0F
#define REG_DATA_GYR_ZL     0x10
#define REG_DATA_GYR_ZH     0x11
#define REG_DATA_ACC_XL     0x12
#define REG_DATA_ACC_XH     0x13
#define REG_DATA_ACC_YL     0x14
#define REG_DATA_ACC_YH     0x15
#define REG_DATA_ACC_ZL     0x16
#define REG_DATA_ACC_ZH     0x17
#define REG_SENSORTIME_0    0x18
#define REG_SENSORTIME_1    0x19
#define REG_SENSORTIME_2    0x1A
#define REG_STATUS          0x1B
#define REG_INT_STATUS_0    0x1C
#define REG_INT_STATUS_1    0x1D
#define REG_INT_STATUS_2    0x1E
#define REG_INT_STATUS_3    0x1F
#define REG_TEMPERATURE_0   0x20
#define REG_TEMPERATURE_1   0x21
#define REG_FIFO_LENGTH_0   0x22
#define REG_FIFO_LENGTH_1   0x23
#define REG_FIFO_DATA       0x24
#define REG_ACC_CONF        0x40
#define REG_ACC_RANGE       0x41
#define REG_GYR_CONF        0x42
#define REG_GYR_RANGE       0x43
#define REG_MAG_CONF        0x44
#define REG_FIFO_DOWNS      0x45
#define REG_FIFO_CONFIG_0   0x46
#define REG_FIFO_CONFIG_1   0x47
#define REG_MAG_IF_0        0x4B
#define REG_MAG_IF_1        0x4C
#define REG_MAG_IF_2        0x4D
#define REG_MAG_IF_3        0x4E
#define REG_MAG_IF_4        0x4F
#define REG_INT_EN_0        0x50
#define REG_INT_EN_1        0x51
#define REG_INT_EN_2        0x52
#define REG_INT_OUT_CTRL    0x53
#define REG_INT_LATCH       0x54
#define REG_INT_MAP_0       0x55
#define REG_INT_MAP_1       0x56
#define REG_INT_MAP_2       0x57
#define REG_INT_DATA_0      0x58
#define REG_INT_DATA_1      0x59
#define REG_INT_LOWHIGH_0   0x5A
#define REG_INT_LOWHIGH_1   0x5B
#define REG_INT_LOWHIGH_2   0x5C
#define REG_INT_LOWHIGH_3   0x5D
#define REG_INT_LOWHIGH_4   0x5E
#define REG_INT_MOTION_0    0x5F
#define REG_INT_MOTION_1    0x60
#define REG_INT_MOTION_2    0x61
#define REG_INT_MOTION_3    0x62
#define REG_INT_TAP_0       0x63
#define REG_INT_TAP_1       0x64
#define REG_ORIENT_0        0x65
#define REG_ORIENT_1        0x66
#define REG_INT_FLAT_0      0x67
#define REG_INT_FLAT_1      0x68
#define REG_FOC_CONF        0x69
#define REG_CONF            0x6A
#define REG_IF_CONF         0x6B
#define REG_PMU_TRIGGER     0x6C
#define REG_SELF_TEST       0x6D
#define REG_NV_CONF         0x70
#define REG_OFFSET_0        0x71
#define REG_OFFSET_1        0x72
#define REG_OFFSET_2        0x73
#define REG_OFFSET_3        0x74
#define REG_OFFSET_4        0x75
#define REG_OFFSET_5        0x76
#define REG_OFFSET_6        0x77
#define REG_STEP_CNT_1      0x78
#define REG_STEP_CNT_0      0x79
#define REG_STEP_CONF_0     0x7A
#define REG_STEP_CONF_1     0x7B
#define REG_CMD             0x7E

//===============================================================================
//      #### Common Parameters Definition ####
//===============================================================================
#define ACC_CHIP_ID         0xD1
#define GYR_CHIP_ID         0xD1

#define ACC_SLAVE_ADDR_0    0xD0
#define ACC_SLAVE_ADDR_1    0xD2

#define GYR_SLAVE_ADDR_0    0xD0
#define GYR_SLAVE_ADDR_1    0xD2

/*
** Digital filter setting 
    0x20: Normal mode
    0x10: OSR2 mode
    0x00: OSR4 mode
*/
#define ODR_MASK            0x20

#ifndef GRAVITY_EARTH
#define GRAVITY_EARTH       9.8066f
#endif

#ifndef M_PI
#define M_PI                3.1415926f
#endif

#define FIFO_WATER_MARK_TH0     (4*0)
#define FIFO_WATER_MARK_TH4     (4*1)
#define FIFO_WATER_MARK_TH8     (4*2)
#define FIFO_WATER_MARK_TH12    (4*3)
#define FIFO_WATER_MARK_TH16    (4*4)
#define FIFO_WATER_MARK_TH20    (4*5)
#define FIFO_WATER_MARK_TH24    (4*6)
#define FIFO_WATER_MARK_TH28    (4*7)
#define FIFO_WATER_MARK_TH32    (4*8)
#define FIFO_WATER_MARK_TH36    (4*9)
#define FIFO_WATER_MARK_TH40    (4*10)

//===============================================================================
//      #### Parameters Definition for Accelerometer ####
//===============================================================================
#define ACC_MODE_RD_MASK    0x30
#define ACC_MODE_WR_MASK    0x10
#define ACC_MODE_SUSPEND    0x00
#define ACC_MODE_NORMAL     0x01
#define ACC_MODE_LOWPOWER   0x02

#define ACC_RATE_6Hz    0x04
#define ACC_RATE_12Hz   0x05
#define ACC_RATE_25Hz   0x06
#define ACC_RATE_50Hz   0x07
#define ACC_RATE_100Hz  0x08
#define ACC_RATE_200Hz  0x09
#define ACC_RATE_400Hz  0x0A
#define ACC_RATE_800Hz  0x0B
#define ACC_RATE_1600Hz 0x0C

#define ACC_RANGE_2G    0x03
#define ACC_RANGE_4G    0x05
#define ACC_RANGE_8G    0x08
#define ACC_RANGE_16G   0x0C

// at 2G condition: 1 LSB = 1 / 16384 g = (1/16384)*1000 mg
#define ACC_RANGE_2G_CONVERT    (GRAVITY_EARTH*(1.0f/16384.0f)) //unit: m/s^2
#define ACC_RANGE_4G_CONVERT    (GRAVITY_EARTH*(1.0f/8192.0f))
#define ACC_RANGE_8G_CONVERT    (GRAVITY_EARTH*(1.0f/4096.0f))
#define ACC_RANGE_16G_CONVERT   (GRAVITY_EARTH*(1.0f/2048.0f))

#define BMI160SPI_ACC_SELF_TEST_AXIS_X_RANGE   3000
#define BMI160SPI_ACC_SELF_TEST_AXIS_Y_RANGE   3000
#define BMI160SPI_ACC_SELF_TEST_AXIS_Z_RANGE   1500

//===============================================================================
//      #### Parameters Definition for Accelerometer Motion Interrupt ####
//===============================================================================
#define MOT_MODE_RD_MASK    0x30
#define MOT_MODE_WR_MASK    0x10
#define MOT_MODE_SUSPEND    0x00
#define MOT_MODE_NORMAL     0x01
#define MOT_MODE_LOWPOWER   0x02

#define MOT_RATE_6Hz    0x04
#define MOT_RATE_12Hz   0x05
#define MOT_RATE_25Hz   0x06
#define MOT_RATE_50Hz   0x07
#define MOT_RATE_100Hz  0x08
#define MOT_RATE_200Hz  0x09
#define MOT_RATE_400Hz  0x0A
#define MOT_RATE_800Hz  0x0B

#define MOT_RANGE_2G    0x03
#define MOT_RANGE_4G    0x05
#define MOT_RANGE_8G    0x08
#define MOT_RANGE_16G   0x0C

//===============================================================================
//      #### Parameters Definition for Gyroscope ####
//===============================================================================
#define GYR_MODE_RD_MASK        0x0c
#define GYR_MODE_WR_MASK        0x14
#define GYR_MODE_SUSPEND        0x00
#define GYR_MODE_NORMAL         0x01
#define GYR_MODE_RESERVED       0x02
#define GYR_MODE_FAST_STARTUP   0x03

#define GYR_RATE_6Hz    0x04
#define GYR_RATE_12Hz   0x05
#define GYR_RATE_25Hz   0x06
#define GYR_RATE_50Hz   0x07
#define GYR_RATE_100Hz  0x08
#define GYR_RATE_200Hz  0x09
#define GYR_RATE_400Hz  0x0A
#define GYR_RATE_800Hz  0x0B

#define GYR_RANGE_2000DPS   0x00
#define GYR_RANGE_1000DPS   0x01
#define GYR_RANGE_500DPS    0x02
#define GYR_RANGE_250DPS    0x03
#define GYR_RANGE_125DPS    0x04

// at 2000 dps condition: 1 LSB = 1 / 16.4 dps = (1/16.4)*1000 mdps
//                to rps: dps * (PI/180.0f)
#define GYR_RANGE_2000DPS_CONVERT   ((1.0f/16.4f) *(M_PI/180.0f)) //unit: rps
#define GYR_RANGE_1000DPS_CONVERT   ((1.0f/32.8f) *(M_PI/180.0f))
#define GYR_RANGE_500DPS_CONVERT    ((1.0f/65.6f) *(M_PI/180.0f))
#define GYR_RANGE_250DPS_CONVERT    ((1.0f/131.2f)*(M_PI/180.0f))
#define GYR_RANGE_125DPS_CONVERT    ((1.0f/262.4f)*(M_PI/180.0f))

//===============================================================================
//      #### Driver Control Definition ####
//===============================================================================
// @arg: FULL_SCALE_USE_2G
//       FULL_SCALE_USE_4G
//       FULL_SCALE_USE_8G
//       FULL_SCALE_USE_16G
#define FULL_SCALE_USE_8G

// @arg: FULL_SCALE_USE_2000DPS
//       FULL_SCALE_USE_1000DPS
//       FULL_SCALE_USE_500DPS
//       FULL_SCALE_USE_250DPS
//       FULL_SCALE_USE_125DPS
#define FULL_SCALE_USE_2000DPS

// Interrupt GPIO structure & Active type
// @arg: INT_ACT_HIGH_PP
//       INT_ACT_LOW_PP
//       INT_ACT_LOW_OD
#define INT_ACT_HIGH_PP

// Interrupt PINs selection
// @arg: MOTION_INT1
//       MOTION_INT2
#define MOTION_INT1

#endif /* DRIVER_USE_BMI160_SPI */

#endif /* __DriverBmi160_H__ */
