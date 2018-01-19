 //===============================================================================
//      #### System Include ####
//===============================================================================
#include "cwmProjectConfig.h"
#include "cwmDriverHal.h"
#include "cwmGpioConfig.h"

#ifdef DRIVER_USE_BMI160_SPI

//===============================================================================
//      #### Register file Include ####
//===============================================================================
#include "DriverBMI160_SPI.h"

static uint8_t Acc_en_status = 0;
static uint8_t Mot_en_status = 0;


extern void GPIO_ACC_Chip_Select(uint8_t uSelect);
extern void GPIO_Motion_Chip_Select(uint8_t uSelect);
extern void GPIO_Gyro_Chip_Select(uint8_t uSelect);
//===============================================================================
//      #### Variables Deceleration for Accelerometer ####
//===============================================================================
pDriverSensorMem_t m_pBMI160SPIAccMem;

#define m_pfun_AccRead(...) \
    DriverHALInterfaceRead(m_pBMI160SPIAccMem, ##__VA_ARGS__)
#define m_pfun_AccWrite(...) \
    DriverHALInterfaceWrite(m_pBMI160SPIAccMem, ##__VA_ARGS__)
    
#define m_AccRate           m_pBMI160SPIAccMem->rate
#define m_AccMode           m_pBMI160SPIAccMem->mode
#define m_AccStatus         m_pBMI160SPIAccMem->ustatus
#define m_AccSlaveAddr      m_pBMI160SPIAccMem->uHWSensorSlaveAddr
#define m_AccIntrThreshold  m_pBMI160SPIAccMem->uSensorThreshold
#define m_AccIntrMode       m_pBMI160SPIAccMem->uSensorIntrMode
#define m_AccPrivate_data   m_pBMI160SPIAccMem->private_data
#define m_AccODRHz          m_pBMI160SPIAccMem->odr_Hz
#define m_pfdrvAccDelay     osTaskDelay
/* SUPPORT_SPI */
#define m_AccInterfaceType  m_pBMI160SPIAccMem->pdrvInterafceSetting->eInterfaceType


//===============================================================================
//      #### Function Prototype for Accelerometer ####
//===============================================================================
int DriverBMI160SPIAccInit( pDriverSensorMem_t pSensorMem );
int DriverBMI160SPIAccHwInit( void );
int DriverBMI160SPIAccEnable( void );
int DriverBMI160SPIAccDisable( void );
int DriverBMI160SPIAccGetBypassData( float *rawOut );
int DriverBMI160SPIAccGetFIFOData( float *rawOut );
int DriverBMI160SPIAccGetStatus( uint8_t *pdata );
int DriverBMI160SPIAccSetStatus( uint8_t ustatus );
int DriverBMI160SPIAccReset( void );
int DriverBMI160SPIAccSelfTest( void );
int DriverBMI160SPIAccSetOffset( void );
int DriverBMI160SPIAccSetRate( int rate );
int DriverBMI160SPIAccSetMode( int mode );
int DriverBMI160SPIAccGetInfo( uint8_t *data );
int DriverBMI160SPIAccCheckChipId( uint8_t uSensorSlaveAddr );
int DriverBMI160SPIAccScanSlaveAddr( void );
int DriverBMI160SPIAccChipSelect(uint8_t uSelect);

//===============================================================================
//      #### Pointer Redirection for Accelerometer ####
//===============================================================================
const DriverServiceCallback_t conBMI160SPIAccCallBack =
{
	.eInterfaceType   = emSPI2_WL_RH,
    .HwInit           = DriverBMI160SPIAccHwInit,
    .Enable           = DriverBMI160SPIAccEnable,
    .Disable          = DriverBMI160SPIAccDisable,
    .GetBypassData    = DriverBMI160SPIAccGetBypassData,
    .GetFIFOData      = DriverBMI160SPIAccGetFIFOData,
    .GetStatus        = DriverBMI160SPIAccGetStatus,
    .SetStatus        = DriverBMI160SPIAccSetStatus,
    .SelftTest        = DriverBMI160SPIAccSelfTest,
    .SetOffset        = DriverBMI160SPIAccSetOffset,
    .SetRate          = DriverBMI160SPIAccSetRate,
    .SetMode          = DriverBMI160SPIAccSetMode,
    .CheckChipId      = DriverBMI160SPIAccCheckChipId,
    .ScanSlaveAddr    = DriverBMI160SPIAccScanSlaveAddr,
	/* SUPPORT_SPI */
    .SensorChipSelect = DriverBMI160SPIAccChipSelect,
    .uHWSensorType    = ACCELERATION,
    .uHWSensorID      = BMI160
};

//===============================================================================
//      #### Function Definition for Accelerometer ####
//===============================================================================

const DriverInterfaceSetting_t accDrvInterafceSetting[1] = 
{
   ACCELERATION,
   emSPI2_WL_RH,
};

const DriverInterfaceSetting_t * accDrvInterfaceSettingGet(
    void
    )
{
	return &accDrvInterafceSetting[0];
}

int DriverBMI160SPIAccInit( pDriverSensorMem_t pSensorMem )
{
    m_pBMI160SPIAccMem = pSensorMem;

    // Register this sensor default sensor info
    m_pBMI160SPIAccMem->pdrvCallBack = &conBMI160SPIAccCallBack;

    /* SUPPORT_SPI */
	#if 0
    m_pBMI160SPIAccMem->pdrvInterafceSetting =
        GetDrvInterfaceSetting((SENSORS_ID)conBMI160SPIAccCallBack.uHWSensorType);
	#else
	m_pBMI160SPIAccMem->pdrvInterafceSetting = accDrvInterfaceSettingGet();
	#endif

    return NO_ERROR;
}

int DriverBMI160SPIAccHwInit( void ) 
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // step 1: change mode to SUSPEND
    tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_SUSPEND;
    if( m_pfun_AccWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
    
    // step 2: verify mode for triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {
        m_pfdrvAccDelay(2);
        
        tmp_rdata = 0x00;
        if( m_pfun_AccRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return I2C_FAIL;
            
        tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 4;
        
        if( tmp_rdata == ACC_MODE_SUSPEND )
            check_flag = 1;
    }
    
    if( check_flag == 1 )
        return NO_ERROR;
    else
        return cwFAIL;
}


int DriverBMI160SPIAccEnable( void )
{   
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;

    // Power Mode Selection
    if( (Acc_en_status == 0) && (Mot_en_status == 0) )
    {
        // step 1: change mode to NORMAL
        tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_NORMAL;
        if( m_pfun_AccWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    
        // step 2: verify mode for triple times
        for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
        {
            m_pfdrvAccDelay(20);

            tmp_rdata = 0x00;
            if( m_pfun_AccRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
                return I2C_FAIL;

            tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
            tmp_rdata = tmp_rdata >> 4;

            if( tmp_rdata == ACC_MODE_NORMAL )
                check_flag = 1;
        }
        
        // Check Power Mode Pass or Fail
        if( check_flag == 0 )
            return DRIVER_ENABLE_FAIL;
    }
        
    // Full scale selection
#ifdef FULL_SCALE_USE_2G
    tmp_wdata = ACC_RANGE_2G;
#endif

#ifdef FULL_SCALE_USE_4G
    tmp_wdata = ACC_RANGE_4G;
#endif

#ifdef FULL_SCALE_USE_8G
    tmp_wdata = ACC_RANGE_8G;
#endif

#ifdef FULL_SCALE_USE_16G
    tmp_wdata = ACC_RANGE_16G;
#endif

    if( m_pfun_AccWrite(REG_ACC_RANGE, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
    Acc_en_status = 1;
    
    return NO_ERROR;
}

int DriverBMI160SPIAccDisable( void )
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // Power Mode Selection
    if( (Acc_en_status == 1) && (Mot_en_status == 0) )
    {
        // step 1: change mode to SUSPEND
        tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_SUSPEND;
        if( m_pfun_AccWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    
        // step 2: verify mode for triple times
        for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
        {       
            m_pfdrvAccDelay(20);
        
            tmp_rdata = 0x00;
            if( m_pfun_AccRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
                return I2C_FAIL;
            
            tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
            tmp_rdata = tmp_rdata >> 4;
        
            if( tmp_rdata == ACC_MODE_SUSPEND )
                check_flag = 1;
        }
        
        if( check_flag == 0 )
            return DRIVER_DISABLE_FAIL;
    }

    Acc_en_status = 0;
    
    return NO_ERROR;
}

int DriverBMI160SPIAccGetBypassData( float *rawOut )
{
    uint8_t tmp_raw8[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    int16_t tmp_raw16[3] = {0, 0, 0};
    
    //int decimation = 0;
    

    if( m_pfun_AccRead(REG_DATA_ACC_XL, tmp_raw8, 6) == I2C_FAIL )
        return DRIVER_GETDATA_FAIL;
   
    tmp_raw16[0] = (tmp_raw8[1]<<8) | tmp_raw8[0];
    tmp_raw16[1] = (tmp_raw8[3]<<8) | tmp_raw8[2];
    tmp_raw16[2] = (tmp_raw8[5]<<8) | tmp_raw8[4];

#ifdef FULL_SCALE_USE_2G
        rawOut[0] = (float)tmp_raw16[0]*ACC_RANGE_2G_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*ACC_RANGE_2G_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*ACC_RANGE_2G_CONVERT;
#endif

#ifdef FULL_SCALE_USE_4G
        rawOut[0] = (float)tmp_raw16[0]*ACC_RANGE_4G_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*ACC_RANGE_4G_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*ACC_RANGE_4G_CONVERT;
#endif

#ifdef FULL_SCALE_USE_8G
        rawOut[0] = (float)tmp_raw16[0]*ACC_RANGE_8G_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*ACC_RANGE_8G_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*ACC_RANGE_8G_CONVERT;
#endif

#ifdef FULL_SCALE_USE_16G
        rawOut[0] = (float)tmp_raw16[0]*ACC_RANGE_16G_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*ACC_RANGE_16G_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*ACC_RANGE_16G_CONVERT;
#endif
        
    return NO_ERROR;
}

int DriverBMI160SPIAccGetFIFOData(float *rawOut )
{   
    return NO_ERROR;
}

int DriverBMI160SPIAccGetStatus( uint8_t *pdata )
{
    uint8_t tmp_rdata;
    
    // Operation Mode Selection, Always be BYPASS
    if( m_pfun_AccRead(REG_STATUS, &tmp_rdata, 1) == I2C_FAIL )
        return I2C_FAIL;
            
    tmp_rdata = tmp_rdata & 0x80;
    tmp_rdata = tmp_rdata >> 7;
        
    if( tmp_rdata == 0x01 )
    {
        *pdata = 1;
        return SENSOR_DATA_READY;
    }
    else
    {
        *pdata = 0;
        return SENSOR_DATA_NOT_READY;
    }
}

int DriverBMI160SPIAccSetStatus( uint8_t ustatus )
{
    return NO_ERROR;
}

int DriverBMI160SPIAccReset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIAccSelfTest( void )
{
	#if 0
    uint8_t tmp_wdata;
    
    uint8_t loop;
    
    int8_t self_test_state = 0;
    
    uint8_t tmp_raw8[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    int16_t temp_16 = 0;
    
    int16_t acc_selft_data_positive[3] = {0, 0, 0};
    int16_t acc_selft_data_negative[3] = {0, 0, 0};
    
    // Step 1 ---> Enable Acc
    if( DriverBMI160SPIAccEnable() != NO_ERROR )
        return CALIB_STATUS_FAIL;
    
    // Step 2 ---> Change to 8G
    tmp_wdata = ACC_RANGE_8G;
    if( m_pfun_AccWrite(REG_ACC_RANGE, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
    
    // Step 3 ---> Change ODR to 1600Hz
    tmp_wdata = ODR_MASK | ACC_RATE_100Hz; 
    if( m_pfun_AccWrite(REG_ACC_CONF, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
    
    // Step 4 ---> wait 100ms
    m_pfdrvAccDelay( 100 );
    
    // Step 5 ---> Start Selftest, maximum retry 3 times
    for( loop = 0 ; (loop < 3) ; loop++ )
    {
        self_test_state = 0;
        
        // Positive setting
        tmp_wdata = 0x08 | 0x04 | 0x01;
        if( m_pfun_AccWrite(REG_SELF_TEST, &tmp_wdata, 1) == I2C_FAIL )
            return CALIB_STATUS_FAIL;
    
        m_pfdrvAccDelay( 100 );
    
        if( m_pfun_AccRead(REG_DATA_ACC_XL, tmp_raw8, 6) == cwFAIL )
            return CALIB_STATUS_FAIL;
            
        acc_selft_data_positive[0] = (tmp_raw8[1]<<8) | tmp_raw8[0];
        acc_selft_data_positive[1] = (tmp_raw8[3]<<8) | tmp_raw8[2];
        acc_selft_data_positive[2] = (tmp_raw8[5]<<8) | tmp_raw8[4];
        
        // Negative setting
        tmp_wdata = 0x08 | 0x01;
        if( m_pfun_AccWrite(REG_SELF_TEST, &tmp_wdata, 1) == I2C_FAIL )
            return CALIB_STATUS_FAIL;
        
        m_pfdrvAccDelay( 100 );
        
        if( m_pfun_AccRead(REG_DATA_ACC_XL, tmp_raw8, 6) == cwFAIL )
            return CALIB_STATUS_FAIL;
            
        acc_selft_data_negative[0] = (tmp_raw8[1]<<8) | tmp_raw8[0];
        acc_selft_data_negative[1] = (tmp_raw8[3]<<8) | tmp_raw8[2];
        acc_selft_data_negative[2] = (tmp_raw8[5]<<8) | tmp_raw8[4];
        
        // Check Criterion: X, Y, Z in order
        // X axis
        temp_16 = acc_selft_data_positive[0] - acc_selft_data_negative[0];
        
        if( temp_16 < 0 ) 
            temp_16 = temp_16*-1;
        
        if( temp_16 < BMI160SPI_ACC_SELF_TEST_AXIS_X_RANGE )
            self_test_state = -1;
        
        // Y axis
        temp_16 = acc_selft_data_positive[1] - acc_selft_data_negative[1];
        
        if( temp_16 < 0 )
            temp_16 = temp_16*-1;
    
        if( temp_16 < BMI160SPI_ACC_SELF_TEST_AXIS_Y_RANGE )
            self_test_state = -1;
    
        // Z axis
        temp_16 = acc_selft_data_positive[2] - acc_selft_data_negative[2];

        if( temp_16 < 0 )
            temp_16 = temp_16*-1;

        if( temp_16 < BMI160SPI_ACC_SELF_TEST_AXIS_Z_RANGE )
            self_test_state = -1;
        
        if( self_test_state == 0 )
            break;
    }

    // Step 6 ---> End Selftest
    tmp_wdata = 0x00;
    if( m_pfun_AccWrite(REG_SELF_TEST, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
        
    // Step 7 ---> Change to 4G
    tmp_wdata = ACC_RANGE_4G;
    if( m_pfun_AccWrite(REG_ACC_RANGE, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
    
    // Step 8 ---> Disable Acc
    if( DriverBMI160SPIAccDisable() != NO_ERROR )
        return CALIB_STATUS_FAIL;
    
    if( self_test_state == 0 )
        return CALIB_STATUS_PASS;
    else
        return CALIB_STATUS_FAIL;
	#else
	int Ret = SELFTEST_STATUS_FAIL;
	uint8_t tmp_rdata = 0x00;
	if( m_pfun_AccRead(REG_CHIP_ID, &tmp_rdata, 1) == I2C_FAIL ){
	    Ret = SELFTEST_STATUS_FAIL;
	}
	    
	if( tmp_rdata == ACC_CHIP_ID ){
	    Ret =  SELFTEST_STATUS_PASS;
	}

	return Ret;
	#endif
}

int DriverBMI160SPIAccSetOffset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIAccSetRate( int rate )
{
    uint8_t tmp_wdata = 0x00;
    
    m_AccRate = rate;
    
    // ODR selection
    switch( m_AccRate )
    {
        case FASTEST:
            tmp_wdata = ODR_MASK | ACC_RATE_100Hz; 
            break;
                
        case GAME:
            tmp_wdata = ODR_MASK | ACC_RATE_50Hz;
            break;
                
        case RATE_33:
            tmp_wdata = ODR_MASK | ACC_RATE_50Hz; 
            break;
                
        case RATE_25:
            tmp_wdata = ODR_MASK | ACC_RATE_25Hz;
            break;
                
        case UI:
            tmp_wdata = ODR_MASK | ACC_RATE_25Hz;
            break;
                
        case RATE_10:
            tmp_wdata = ODR_MASK | ACC_RATE_12Hz;
            break;
                
        case NORMAL:
            tmp_wdata = ODR_MASK | MOT_RATE_12Hz;
            break;
                
        default:
            tmp_wdata = ODR_MASK | ACC_RATE_100Hz; 
            break;
    }
    if( m_pfun_AccWrite(REG_ACC_CONF, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    return NO_ERROR;
}

int DriverBMI160SPIAccSetMode( int mode )
{
    return NO_ERROR;
}

int DriverBMI160SPIAccGetInfo( uint8_t *data )
{
    return NO_ERROR;
}

int DriverBMI160SPIAccCheckChipId( uint8_t uSensorSlaveAddr )
{
    uint8_t tmp_rdata = 0x00;

    switch(m_AccInterfaceType)
    {
		case emSPI1_WH_RL:
		case emSPI1_WL_RH:
		case emSPI2_WH_RL:
		case emSPI2_WL_RH:
            m_AccSlaveAddr = uSensorSlaveAddr;
            
            if( m_pfun_AccRead(REG_CHIP_ID, &tmp_rdata, 1) == I2C_FAIL )
                return DRIVER_CHECK_CHIP_ID_FAIL;
                
            if( tmp_rdata == ACC_CHIP_ID ){
                m_pBMI160SPIAccMem->uHWChipId[0] = tmp_rdata;
                m_pBMI160SPIAccMem->uHWChipId[1] = uSensorSlaveAddr;
                return NO_ERROR;
            }
                
            return DRIVER_CHECK_CHIP_ID_FAIL;
            break;

			
		
    }

    return NO_ERROR;
}

int DriverBMI160SPIAccScanSlaveAddr( void )
{
    int ret = DRIVER_CHECK_CHIP_ID_FAIL;

    if( DriverBMI160SPIAccCheckChipId(ACC_SLAVE_ADDR_0) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
    else if( DriverBMI160SPIAccCheckChipId(ACC_SLAVE_ADDR_1) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
        
    if( ret == DRIVER_CHECK_CHIP_ID_FAIL )
        m_pBMI160SPIAccMem->uHWSensorSlaveAddr = 0;
        
    return ret;
}

int DriverBMI160SPIAccChipSelect(uint8_t uSelect)
{
	/* SUPPORT_SPI */
    GPIO_ACC_Chip_Select(uSelect);

    return NO_ERROR;
}


//===============================================================================
//      #### Variables Deceleration for Accelerometer Motion Interrupt ####
//===============================================================================
#ifdef MOTION
pDriverSensorMem_t m_pBMI160SPIMotionMem;

#define m_pfun_MotionRead(...) \
    DriverHALInterfaceRead(m_pBMI160SPIMotionMem, ##__VA_ARGS__)
#define m_pfun_MotionWrite(...) \
    DriverHALInterfaceWrite(m_pBMI160SPIMotionMem, ##__VA_ARGS__)
    
#define m_MotionRate            m_pBMI160SPIMotionMem->rate
#define m_MotionMode            m_pBMI160SPIMotionMem->mode
#define m_MotionStatus          m_pBMI160SPIMotionMem->ustatus
#define m_MotionSlaveAddr       m_pBMI160SPIMotionMem->uHWSensorSlaveAddr
#define m_MotionIntrThreshold   m_pBMI160SPIMotionMem->uSensorThreshold
#define m_MotionIntrMode        m_pBMI160SPIMotionMem->uSensorIntrMode
#define m_MotionPrivate_data    m_pBMI160SPIMotionMem->private_data
#define m_MotionODRHz           m_pBMI160SPIMotionMem->odr_Hz
#define m_pfdrvMotionDelay      osTaskDelay
/* SUPPORT_SPI */
#define m_MotionInterfaceType   m_pBMI160SPIMotionMem->pdrvInterafceSetting->eInterfaceType


//===============================================================================
//      #### Function Prototype for Accelerometer Motion Interrupt ####
//===============================================================================
int DriverBMI160SPIMotionInit( pDriverSensorMem_t pSensorMem );
int DriverBMI160SPIMotionHwInit( void );
int DriverBMI160SPIMotionEnable( void );
int DriverBMI160SPIMotionDisable( void );
int DriverBMI160SPIMotionGetBypassData( float *rawOut );
int DriverBMI160SPIMotionGetFIFOData( float *rawOut );
int DriverBMI160SPIMotionGetStatus( uint8_t *pdata );
int DriverBMI160SPIMotionSetStatus( uint8_t ustatus );
int DriverBMI160SPIMotionReset( void );
int DriverBMI160SPIMotionSelfTest( void );
int DriverBMI160SPIMotionSetOffset( void );
int DriverBMI160SPIMotionSetRate( int rate );
int DriverBMI160SPIMotionSetMode( int mode );
int DriverBMI160SPIMotionGetInfo( uint8_t *data );
int DriverBMI160SPIMotionCheckChipId( uint8_t uSensorSlaveAddr );
int DriverBMI160SPIMotionScanSlaveAddr( void );
int DriverBMI160SPIMotionChipSelect(uint8_t uSelect);

//===============================================================================
//      #### Pointer Redirection for Accelerometer Motion Interrupt ####
//===============================================================================
const DriverServiceCallback_t conBMI160SPIMotionCallBack =
{
	.eInterfaceType   = emSPI2_WL_RH,
    .HwInit           = DriverBMI160SPIMotionHwInit,
    .Enable           = DriverBMI160SPIMotionEnable,
    .Disable          = DriverBMI160SPIMotionDisable,
    .GetBypassData    = DriverBMI160SPIMotionGetBypassData,
    .GetFIFOData      = DriverBMI160SPIMotionGetFIFOData,
    .GetStatus        = DriverBMI160SPIMotionGetStatus,
    .SetStatus        = DriverBMI160SPIMotionSetStatus,
    .SelftTest        = DriverBMI160SPIMotionSelfTest,
    .SetOffset        = DriverBMI160SPIMotionSetOffset,
    .SetRate          = DriverBMI160SPIMotionSetRate,
    .SetMode          = DriverBMI160SPIMotionSetMode,
    .CheckChipId      = DriverBMI160SPIMotionCheckChipId,
    .ScanSlaveAddr    = DriverBMI160SPIMotionScanSlaveAddr,
	/* SUPPORT_SPI */
    .SensorChipSelect = DriverBMI160SPIMotionChipSelect,

    .uHWSensorType    = MOTION,
    .uHWSensorID      = BMI160
};

//===============================================================================
//      #### Function Definition for Accelerometer Motion Interrupt ####
//===============================================================================
const DriverInterfaceSetting_t motionDrvInterafceSetting[1] = 
{
   MOTION,
   emSPI2_WL_RH,
};

const DriverInterfaceSetting_t * motionDrvInterfaceSettingGet(
    void
    )
{
	return &motionDrvInterafceSetting[0];
}

int DriverBMI160SPIMotionInit( pDriverSensorMem_t pSensorMem )
{
    m_pBMI160SPIMotionMem = pSensorMem;

    // Register this sensor default sensor info
    m_pBMI160SPIMotionMem->pdrvCallBack = &conBMI160SPIMotionCallBack;

	/* SUPPORT_SPI */
	#if 0
    m_pBMI160SPIMotionMem->pdrvInterafceSetting =
        GetDrvInterfaceSetting((SENSORS_ID)conBMI160SPIMotionCallBack.uHWSensorType);
	#else
	m_pBMI160SPIMotionMem->pdrvInterafceSetting = motionDrvInterfaceSettingGet();
	#endif


    return NO_ERROR;
}

int DriverBMI160SPIMotionHwInit( void ) 
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // Enable Acc for setting register
    // step 1: change mode to NORMAL
    tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_NORMAL;
    if( m_pfun_MotionWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
    
    // step 2: verify mode for triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {       
        m_pfdrvMotionDelay(20);
        
        tmp_rdata = 0x00;
        if( m_pfun_MotionRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return I2C_FAIL;
            
        tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 4;
        
        if( tmp_rdata == ACC_MODE_NORMAL )
        check_flag = 1;
    }
    
    // Check Power Mode Pass or Fail
    if( check_flag == 0 )
        return cwFAIL;
        
    // Interrupt as push-pull & active high, now use INT1
#ifdef INT_ACT_HIGH_PP
    tmp_wdata = 0x02;
#endif

#ifdef INT_ACT_LOW_PP
    tmp_wdata = 0x02;
#endif

#ifdef INT_ACT_LOW_OD
    tmp_wdata = 0x04;
#endif

    if( m_pfun_MotionWrite(REG_INT_OUT_CTRL, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
            
    // AnyMotion Interrupt mapped to INT1
    tmp_wdata = 0x04;
    if( m_pfun_MotionWrite(REG_INT_MAP_0, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // Disable Acc
    // step 1: change mode to SUSPEND
    tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_SUSPEND;
    if( m_pfun_MotionWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
    
    // step 2: verify mode for triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {       
        m_pfdrvMotionDelay(20);
        
        tmp_rdata = 0x00;
        if( m_pfun_MotionRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return I2C_FAIL;
            
        tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 4;
        
        if( tmp_rdata == ACC_MODE_SUSPEND )
        check_flag = 1;
    }
    
    // Check Power Mode Pass or Fail
    if( check_flag == 0 )
        return cwFAIL;
        
    return NO_ERROR;
}

int DriverBMI160SPIMotionEnable( void )
{   
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;

    // Power Mode Selection
    if( (Acc_en_status == 0) && (Mot_en_status == 0) )
    {
        // step 1: change mode to NORMAL
        tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_NORMAL;
        if( m_pfun_MotionWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    
        // step 2: verify mode for triple times
        for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
        {       
            m_pfdrvMotionDelay(20);
        
            tmp_rdata = 0x00;
            if( m_pfun_MotionRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
                return I2C_FAIL;
            
            tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
            tmp_rdata = tmp_rdata >> 4;
        
            if( tmp_rdata == ACC_MODE_NORMAL )
                check_flag = 1;
        }
        
        // Check Power Mode Pass or Fail
        if( check_flag == 0 )
            return DRIVER_ENABLE_FAIL;
    }
    
    // Operation Mode Selection, Always be MODE_BYPASS
    // FIFO_DOWNS --> 0
    //if( m_pfun_MotionRead(REG_FIFO_DOWNS, &tmp_rdata, 1) == I2C_FAIL )
    //    return I2C_FAIL;
                
    //tmp_wdata = tmp_rdata & 0x8F;
    //if( m_pfun_MotionWrite(REG_FIFO_DOWNS, &tmp_wdata, 1) == I2C_FAIL )
    //    return I2C_FAIL;
                
    // Disable fifo_acc_en
    //if( m_pfun_MotionRead(REG_FIFO_CONFIG_1, &tmp_rdata, 1) == I2C_FAIL )
    //    return I2C_FAIL;
            
    //tmp_wdata = tmp_rdata & 0xBF;
    //if( m_pfun_MotionWrite(REG_FIFO_CONFIG_1, &tmp_wdata, 1) == I2C_FAIL )
    //    return I2C_FAIL;
        
    // Full scale selection
#ifdef FULL_SCALE_USE_2G
    tmp_wdata = ACC_RANGE_2G;
#endif

#ifdef FULL_SCALE_USE_4G
    tmp_wdata = ACC_RANGE_4G;
#endif

#ifdef FULL_SCALE_USE_8G
    tmp_wdata = ACC_RANGE_8G;
#endif

#ifdef FULL_SCALE_USE_16G
    tmp_wdata = ACC_RANGE_16G;
#endif

    if( m_pfun_MotionWrite(REG_ACC_RANGE, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
            
    // ODR selection                    
    switch( m_AccRate )
    {
        case FASTEST:
            tmp_wdata = ODR_MASK | MOT_RATE_100Hz; 
            break;
                
        case GAME:
            tmp_wdata = ODR_MASK | MOT_RATE_50Hz;
            break;
                
        case RATE_33:
            tmp_wdata = ODR_MASK | MOT_RATE_50Hz; 
            break;
                
        case RATE_25:
            tmp_wdata = ODR_MASK | MOT_RATE_25Hz;
            break;
                
        case UI:
            tmp_wdata = ODR_MASK | MOT_RATE_25Hz;
            break;
                
        case RATE_10:
            tmp_wdata = ODR_MASK | MOT_RATE_12Hz;
            break;
                
        case NORMAL:
            tmp_wdata = ODR_MASK | MOT_RATE_12Hz;
            break;
                
        default:
            tmp_wdata = ODR_MASK | MOT_RATE_100Hz; 
            break;
    }
    if( m_pfun_MotionWrite(REG_ACC_CONF, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // any motion duration setting
    if( m_pBMI160SPIMotionMem->uSensorPeriod != 0 )
        tmp_wdata = m_pBMI160SPIMotionMem->uSensorPeriod;
    else
        tmp_wdata = 0x02;

    if( m_pfun_MotionWrite(REG_INT_MOTION_0, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // any motion threshold setting
    if( m_pBMI160SPIMotionMem->uSensorThreshold != 0 )
    	tmp_wdata = m_pBMI160SPIMotionMem->uSensorThreshold;
    else
    	tmp_wdata = 0x0F;
    
    if( m_pfun_MotionWrite(REG_INT_MOTION_1, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;

    if( m_pBMI160SPIMotionMem->uSensorIntrMode == INTERRUPT_USE_INT1 )
    {
        tmp_wdata = 0x0A;
        if( m_pfun_MotionWrite(REG_INT_OUT_CTRL, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;

        // AnyMotion Interrupt mapped to INT1
        tmp_wdata = 0x04;
        if( m_pfun_MotionWrite(REG_INT_MAP_0, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    }
    else
    {
        tmp_wdata = 0xA0;
        if( m_pfun_MotionWrite(REG_INT_OUT_CTRL, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;

        // AnyMotion Interrupt mapped to INT2
        tmp_wdata = 0x04;
        if( m_pfun_MotionWrite(REG_INT_MAP_2, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    }
        
    // Enable any motion X, Y, Z axis
    tmp_wdata = 0x07;
    if( m_pfun_MotionWrite(REG_INT_EN_0, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;

    Mot_en_status = 1;
    
    return NO_ERROR;
}

int DriverBMI160SPIMotionDisable( void )
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // Power Mode Selection
    if( (Acc_en_status == 0) && (Mot_en_status == 1) )
    {
        // step 1: change mode to SUSPEND
        tmp_wdata = ACC_MODE_WR_MASK | ACC_MODE_SUSPEND;
        if( m_pfun_MotionWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
            return I2C_FAIL;
    
        // step 2: verify mode for triple times
        for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
        {       
            m_pfdrvMotionDelay(20);
        
            tmp_rdata = 0x00;
            if( m_pfun_MotionRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
                return I2C_FAIL;
            
            tmp_rdata = tmp_rdata & ACC_MODE_RD_MASK;
            tmp_rdata = tmp_rdata >> 4;
        
            if( tmp_rdata == ACC_MODE_SUSPEND )
                check_flag = 1;
        }
        
        if( check_flag == 0 )
            return DRIVER_DISABLE_FAIL;
    }
    
    // Disable INT1_EN & Interrupt as push-pull & active high, now use INT1
#ifdef INT_ACT_HIGH_PP
    tmp_wdata = 0x02;
#endif

#ifdef INT_ACT_LOW_PP
    tmp_wdata = 0x02;
#endif

#ifdef INT_ACT_LOW_OD
    tmp_wdata = 0x04;
#endif

    if( m_pfun_MotionWrite(REG_INT_OUT_CTRL, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // Disable any motion X, Y, Z axis
    tmp_wdata = 0x00;
    if( m_pfun_MotionWrite(REG_INT_EN_0, &tmp_wdata, 1) == I2C_FAIL )
            return cwFAIL;
    
    Mot_en_status = 0;
    
    return NO_ERROR;
}

int DriverBMI160SPIMotionGetBypassData( float *rawOut )
{
    return DriverBMI160SPIAccGetBypassData(rawOut);
}

int DriverBMI160SPIMotionGetFIFOData( float *rawOut )
{   
    return NO_ERROR;
}

int DriverBMI160SPIMotionGetStatus( uint8_t *pdata )
{
	int  status = SENSOR_DATA_NOT_READY;
    if(BitCheck(m_MotionMode,MODE_INT))
    {
        if(m_MotionStatus == SENSOR_DATA_INTERRUPT)
        {
            status = SENSOR_DATA_READY;
            *pdata = 1;
            m_MotionStatus = SENSOR_DATA_NOT_READY;
        }
    }
    return status;
#if 0
    uint8_t tmp_rdata = 0;

    // We read data one time at least
    *pdata = 1;

    if(m_MotionMode == MODE_INT)
    {
        /* Status means we found interrupt from interrupt handler
         * And we read the register to make sure this real happen the motion */
        if(m_MotionStatus == SENSOR_DATA_INTERRUPT)
        {
            if( readRegister(ACC, REG_INT_STATUS_0, &tmp_rdata, 1) == cwFAIL )
            {
                m_MotionStatus = SENSOR_DATA_NOT_READY;
            }
            else
            {
                tmp_rdata = tmp_rdata & 0x04;
                tmp_rdata = tmp_rdata >> 2;
                
                if( tmp_rdata == 0x01 )
                     m_MotionStatus = SENSOR_DATA_READY;
                else
                    m_MotionStatus = SENSOR_DATA_NOT_READY;
            }
                return SENSOR_DATA_NOT_READY;
        }
    }
    return (int)SENSOR_DATA_NOT_READY;
#endif
}

int DriverBMI160SPIMotionSetStatus( uint8_t ustatus )
{
    /* If the Motion is in interrupt mode, we need to update the status from
     * interrupt handler */
	if(BitCheck(m_MotionMode,MODE_INT))
        m_MotionStatus = ustatus;

    return NO_ERROR;
}

int DriverBMI160SPIMotionReset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIMotionSelfTest( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIMotionSetOffset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIMotionSetRate( int rate )
{
    uint8_t tmp_wdata = 0x00;
    
    m_MotionRate = rate;
    
    // ODR selection                    
    switch( m_AccRate )
    {
        case FASTEST:
            tmp_wdata = ODR_MASK | MOT_RATE_100Hz; 
            break;
                
        case GAME:
            tmp_wdata = ODR_MASK | MOT_RATE_50Hz;
            break;
                
        case RATE_33:
            tmp_wdata = ODR_MASK | MOT_RATE_50Hz; 
            break;
                
        case RATE_25:
            tmp_wdata = ODR_MASK | MOT_RATE_25Hz;
            break;
                
        case UI:
            tmp_wdata = ODR_MASK | MOT_RATE_25Hz;
            break;
                
        case RATE_10:
            tmp_wdata = ODR_MASK | MOT_RATE_12Hz;
            break;
                
        case NORMAL:
            tmp_wdata = ODR_MASK | MOT_RATE_12Hz;
            break;
                
        default:
            tmp_wdata = ODR_MASK | MOT_RATE_100Hz; 
            break;
    }
    if( m_pfun_AccWrite(REG_ACC_CONF, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    return NO_ERROR;
}

int DriverBMI160SPIMotionSetMode( int mode )
{
    return NO_ERROR;
}

int DriverBMI160SPIMotionGetInfo( uint8_t *data )
{
    return NO_ERROR;
}

int DriverBMI160SPIMotionCheckChipId( uint8_t uSensorSlaveAddr )
{
    uint8_t tmp_rdata = 0x00;

    switch(m_MotionInterfaceType)
    {
		case emSPI1_WH_RL:
		case emSPI1_WL_RH:
		case emSPI2_WH_RL:
		case emSPI2_WL_RH:
            m_MotionSlaveAddr = uSensorSlaveAddr;
            
            if( m_pfun_MotionRead(REG_CHIP_ID, &tmp_rdata, 1) == I2C_FAIL )
                return DRIVER_CHECK_CHIP_ID_FAIL;
                
            if( tmp_rdata == ACC_CHIP_ID ){
                m_pBMI160SPIMotionMem->uHWChipId[0] = tmp_rdata;
                m_pBMI160SPIMotionMem->uHWChipId[1] = uSensorSlaveAddr;
                return NO_ERROR;
            }
                
            return DRIVER_CHECK_CHIP_ID_FAIL;
            break;
    }

    return NO_ERROR;
}

int DriverBMI160SPIMotionScanSlaveAddr( void )
{
    int ret = DRIVER_CHECK_CHIP_ID_FAIL;
    
    if( DriverBMI160SPIMotionCheckChipId(ACC_SLAVE_ADDR_0) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
    else if( DriverBMI160SPIMotionCheckChipId(ACC_SLAVE_ADDR_1) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
        
    if( ret == DRIVER_CHECK_CHIP_ID_FAIL )
        m_pBMI160SPIMotionMem->uHWSensorSlaveAddr = 0;
        
    return ret;
}

int DriverBMI160SPIMotionChipSelect ( uint8_t uSelect )
{
	/* SUPPORT_SPI */
    GPIO_Motion_Chip_Select(uSelect);

    return NO_ERROR;
}
#endif

static int16_t samplesToDiscard = 0;
//===============================================================================
//      #### Function Prototype for Gyroscope ####
//===============================================================================
int DriverBMI160SPIGyroInit( pDriverSensorMem_t pSensorMem );
int DriverBMI160SPIGyroHwInit( void );
int DriverBMI160SPIGyroEnable( void );
int DriverBMI160SPIGyroDisable( void );
int DriverBMI160SPIGyroGetBypassData( float *rawOut );
int DriverBMI160SPIGyroGetFIFOData( float *rawOut );
int DriverBMI160SPIGyroGetStatus( uint8_t *pdata );
int DriverBMI160SPIGyroSetStatus( uint8_t ustatus );
int DriverBMI160SPIGyroReset( void );
int DriverBMI160SPIGyroSelfTest( void );
int DriverBMI160SPIGyroSetOffset( void );
int DriverBMI160SPIGyroSetRate( int rate );
int DriverBMI160SPIGyroSetMode( int mode );
int DriverBMI160SPIGyroGetInfo( uint8_t *data );
int DriverBMI160SPIGyroCheckChipId( uint8_t uSensorSlaveAddr );
int DriverBMI160SPIGyroScanSlaveAddr( void );
int DriverBMI160SPIGyroChipSelect(uint8_t uSelect);

//===============================================================================
//      #### Pointer Redirection for Gyroscope ####
//===============================================================================
const DriverServiceCallback_t conBMI160SPIGyroCallBack =
{
	.eInterfaceType   = emSPI2_WL_RH,
    .HwInit           = DriverBMI160SPIGyroHwInit,
    .Enable           = DriverBMI160SPIGyroEnable,
    .Disable          = DriverBMI160SPIGyroDisable,
    .GetBypassData    = DriverBMI160SPIGyroGetBypassData,
    .GetFIFOData      = DriverBMI160SPIGyroGetFIFOData,
    .GetStatus        = DriverBMI160SPIGyroGetStatus,
    .SetStatus        = DriverBMI160SPIGyroSetStatus,
    .SelftTest        = DriverBMI160SPIGyroSelfTest,
    .SetOffset        = DriverBMI160SPIGyroSetOffset,
    .SetRate          = DriverBMI160SPIGyroSetRate,
    .SetMode          = DriverBMI160SPIGyroSetMode,
    .CheckChipId      = DriverBMI160SPIGyroCheckChipId,
    .ScanSlaveAddr    = DriverBMI160SPIGyroScanSlaveAddr,
	/* SUPPORT_SPI */
    .SensorChipSelect = DriverBMI160SPIGyroChipSelect,
    .uHWSensorType    = GYRO,
    .uHWSensorID      = BMI160
};

//===============================================================================
//      #### Variables Deceleration for Gyroscope ####
//===============================================================================
pDriverSensorMem_t m_pBMI160SPIGyroMem;

#define m_pfun_GyroRead(...) \
    DriverHALInterfaceRead(m_pBMI160SPIGyroMem, ##__VA_ARGS__)
#define m_pfun_GyroWrite(...) \
    DriverHALInterfaceWrite(m_pBMI160SPIGyroMem, ##__VA_ARGS__)
    
#define m_GyroRate              m_pBMI160SPIGyroMem->rate
#define m_GyroMode              m_pBMI160SPIGyroMem->mode
#define m_GyroStatus            m_pBMI160SPIGyroMem->ustatus
#define m_GyroSlaveAddr         m_pBMI160SPIGyroMem->uHWSensorSlaveAddr
#define m_GyroIntrThreshold     m_pBMI160SPIGyroMem->uSensorThreshold
#define m_GyroIntrMode          m_pBMI160SPIGyroMem->uSensorIntrMode
#define m_GyroPrivate_data      m_pBMI160SPIGyroMem->private_data
#define m_GyroODRHz             m_pBMI160SPIGyroMem->odr_Hz
#define m_pfdrvGyroDelay        osTaskDelay
/* SUPPORT_SPI */
#define m_GyroInterfaceType     m_pBMI160SPIGyroMem->pdrvInterafceSetting->eInterfaceType


//===============================================================================
//      #### Function Definition for Gyroscope ####
//===============================================================================
const DriverInterfaceSetting_t gyrDrvInterafceSetting[1] = 
{
   GYRO,
   emSPI2_WL_RH,
};

const DriverInterfaceSetting_t * gyrDrvInterfaceSettingGet(
    void
    )
{
	return &gyrDrvInterafceSetting[0];
}

int DriverBMI160SPIGyroInit( pDriverSensorMem_t pSensorMem )
{
    m_pBMI160SPIGyroMem = pSensorMem;

    // Register this sensor default sensor info
    m_pBMI160SPIGyroMem->pdrvCallBack = &conBMI160SPIGyroCallBack;

	/* SUPPORT_SPI */
	#if 0
    m_pBMI160SPIGyroMem->pdrvInterafceSetting =
        GetDrvInterfaceSetting((SENSORS_ID)conBMI160SPIGyroCallBack.uHWSensorType);
	#else
	m_pBMI160SPIGyroMem->pdrvInterafceSetting = gyrDrvInterfaceSettingGet();
	#endif

    return NO_ERROR;
}

int DriverBMI160SPIGyroHwInit( void ) 
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // step 1: change mode to SUSPEND
    tmp_wdata = GYR_MODE_WR_MASK | GYR_MODE_SUSPEND;
    if( m_pfun_GyroWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return cwFAIL;
            
    // step 2: verify mode triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {
        tmp_rdata = 0x00;
        
        m_pfdrvGyroDelay(60);
         
        if( m_pfun_GyroRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return cwFAIL;
            
            tmp_rdata = tmp_rdata & GYR_MODE_RD_MASK;
            tmp_rdata = tmp_rdata >> 2;

            if( tmp_rdata == GYR_MODE_SUSPEND )
                check_flag = 1;
        }
        
    if( check_flag == 1 )
        return NO_ERROR;
    else
        return cwFAIL;
}

/*
 * Important Note: FIFO_DOWNS and WATER_MARK_TH will 
 *                 affect behavior of REG_FIFO_LENGTH_0.
 *                 
 *                 Leave default vale as default value
 */
int DriverBMI160SPIGyroEnable( void )
{   
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;

    // Power Mode Selection
    // step 1: change mode to NORMAL
    tmp_wdata = GYR_MODE_WR_MASK | GYR_MODE_NORMAL;
    if( m_pfun_GyroWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // step 2: verify mode triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {
        tmp_rdata = 0x00;
        
        m_pfdrvGyroDelay(60);
         
        if( m_pfun_GyroRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return I2C_FAIL;
            
        tmp_rdata = tmp_rdata & GYR_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 2;

        if( tmp_rdata == GYR_MODE_NORMAL )
            check_flag = 1;
    }
        
    // Check Power Mode Pass or Fail
    if( check_flag == 0 )
        return DRIVER_ENABLE_FAIL;

    tmp_wdata = 0x80; // (tmp_rdata | 0x80) & 0xEF;
    if( m_pfun_GyroWrite(REG_FIFO_CONFIG_1, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
    samplesToDiscard = 15;
    return NO_ERROR;
}

int DriverBMI160SPIGyroDisable( void )
{
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;
    
    // Power Mode Selection
    // step 1: change mode to SUSPEND
    tmp_wdata = GYR_MODE_WR_MASK | GYR_MODE_SUSPEND;
    if( m_pfun_GyroWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    // step 2: verify mode triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {
        tmp_rdata = 0x00;
        
        m_pfdrvGyroDelay(60);
         
        if( m_pfun_GyroRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return I2C_FAIL;
            
        tmp_rdata = tmp_rdata & GYR_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 2;

        if( tmp_rdata == GYR_MODE_SUSPEND )
            check_flag = 1;
    }
    
    if( check_flag == 1 )
        return NO_ERROR;
    else
        return DRIVER_DISABLE_FAIL;
}

int DriverBMI160SPIGyroGetBypassData( float *rawOut )
{
    uint8_t tmp_raw8[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    int16_t tmp_raw16[3] = {0, 0, 0};
    
    if( m_pfun_GyroRead(REG_DATA_GYR_XL, tmp_raw8, 6) == I2C_FAIL )
        return DRIVER_GETDATA_FAIL;
            
    tmp_raw16[0] = (tmp_raw8[1]<<8) | tmp_raw8[0];
    tmp_raw16[1] = (tmp_raw8[3]<<8) | tmp_raw8[2];
    tmp_raw16[2] = (tmp_raw8[5]<<8) | tmp_raw8[4];
        
#ifdef FULL_SCALE_USE_2000DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_2000DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_2000DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_2000DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_1000DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_1000DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_1000DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_1000DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_500DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_500DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_500DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_500DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_250DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_250DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_250DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_250DPS_CONVERT;
#endif
    
#ifdef FULL_SCALE_USE_125DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_125DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_125DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_125DPS_CONVERT;
#endif

    return NO_ERROR;
}

int DriverBMI160SPIGyroGetFIFOData( float *rawOut )
{   
    uint8_t tmp_raw8[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    
    int16_t tmp_raw16[3] = {0, 0, 0};
    
    if( m_pfun_GyroRead(REG_FIFO_DATA, tmp_raw8, 6) == I2C_FAIL )
        return DRIVER_GETDATA_FAIL;
            
    tmp_raw16[0] = (tmp_raw8[1]<<8) | tmp_raw8[0];
    tmp_raw16[1] = (tmp_raw8[3]<<8) | tmp_raw8[2];
    tmp_raw16[2] = (tmp_raw8[5]<<8) | tmp_raw8[4];
        
#ifdef FULL_SCALE_USE_2000DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_2000DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_2000DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_2000DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_1000DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_1000DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_1000DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_1000DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_500DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_500DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_500DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_500DPS_CONVERT;
#endif

#ifdef FULL_SCALE_USE_250DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_250DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_250DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_250DPS_CONVERT;
#endif
    
#ifdef FULL_SCALE_USE_125DPS
        rawOut[0] = (float)tmp_raw16[0]*GYR_RANGE_125DPS_CONVERT;
        rawOut[1] = (float)tmp_raw16[1]*GYR_RANGE_125DPS_CONVERT;
        rawOut[2] = (float)tmp_raw16[2]*GYR_RANGE_125DPS_CONVERT;
#endif

    // Get ODR from sensor setting
    //pSensorEvent->odr_Hz = m_GyroODRHz;

    if(samplesToDiscard >0){
        samplesToDiscard--;
        return NO_DATA;
    }
    
    return NO_ERROR;
}

/*
 * Important Note: REG_FIFO_LENGTH_0 is LSB
 *                 REG_FIFO_LENGTH_1 is HSB
 *                 official datasheet is wrong, don't reference it
 */
int DriverBMI160SPIGyroGetStatus( uint8_t *pdata )
{
    uint16_t fifo_count;
    
    uint8_t tmp_rdata[2] = {0x00, 0x00};
    
    // Operation Mode Selection, Always be MODE_FIFO_STREAM
    if( m_pfun_GyroRead(REG_FIFO_LENGTH_0, tmp_rdata, 2) == I2C_FAIL )
        return I2C_FAIL;
    
    fifo_count = (tmp_rdata[1]<<8) | tmp_rdata[0];
    fifo_count = fifo_count & 0x07FF;
    fifo_count = fifo_count / 6;

    if( fifo_count > 0 )
    {
        if( fifo_count > 3 )
            fifo_count = 3;
            
        *pdata = fifo_count;
        
        return SENSOR_DATA_READY;
    }
    else
    {
        *pdata = 0;
        
        return SENSOR_DATA_NOT_READY;
    }
}

int DriverBMI160SPIGyroSetStatus( uint8_t ustatus )
{
    return NO_ERROR;
}

int DriverBMI160SPIGyroReset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIGyroSelfTest( void )
{
	#if 0
    uint8_t tmp_wdata = 0x00;
    uint8_t tmp_rdata = 0x00;
    
    uint8_t check_cnt  = 0x00;
    uint8_t check_flag = 0x00;

    int self_test_state = -1;
    
    // Enable Gyro
    // Step 1: change mode to NORMAL
    tmp_wdata = GYR_MODE_WR_MASK | GYR_MODE_NORMAL;
    if( m_pfun_GyroWrite(REG_CMD, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
        
    // Step 2: verify mode triple times
    for( check_cnt=0 ; (check_cnt<3) && (check_flag==0) ; check_cnt++ )
    {
        m_pfdrvGyroDelay( 60 );
        
        tmp_rdata = 0x00;         
        if( m_pfun_GyroRead(REG_PMU_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return CALIB_STATUS_FAIL;
            
        tmp_rdata = tmp_rdata & GYR_MODE_RD_MASK;
        tmp_rdata = tmp_rdata >> 2;

        if( tmp_rdata == GYR_MODE_NORMAL )
            check_flag = 1;
    }
        
    // Check Power Mode Pass or Fail
    if( check_flag == 0 )
        return cwFAIL;
    
    // Step 3: delay 50ms
    m_pfdrvGyroDelay( 50 );
    
    // Step 4: Start Selftest
    tmp_wdata =0x10;
    if( m_pfun_GyroWrite(REG_SELF_TEST, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL;
      
    
    // Step 5: check Criterion 3 times
    for( check_cnt=0 ; check_cnt<3 ; check_cnt++ )
    {
        m_pfdrvGyroDelay( 10 );
        
        tmp_rdata = 0x00;         
        if( m_pfun_GyroRead(REG_STATUS, &tmp_rdata, 1) == I2C_FAIL )
            return CALIB_STATUS_FAIL;
        
        if( (tmp_rdata & 0x02) == 0x02 )
        {
            self_test_state = 0;
            break;
        }
    }
    
    // Step 6: End Selftest
    tmp_wdata =0x00;
    if( m_pfun_GyroWrite(REG_SELF_TEST, &tmp_wdata, 1) == I2C_FAIL )
        return CALIB_STATUS_FAIL; 
    
    // Step 7: Disable Gyro
    DriverBMI160SPIGyroDisable();
    
    if( self_test_state == 0 )
        return CALIB_STATUS_PASS;
    else
        return CALIB_STATUS_FAIL;
	#else
	int Ret = SELFTEST_STATUS_FAIL;
	uint8_t tmp_rdata = 0x00;
	if( m_pfun_GyroRead(REG_CHIP_ID, &tmp_rdata, 1) == I2C_FAIL ){
		Ret = SELFTEST_STATUS_FAIL;
	}

	if( tmp_rdata == GYR_CHIP_ID ){
		Ret =  SELFTEST_STATUS_PASS;
	}

	return Ret;
	#endif
}

int DriverBMI160SPIGyroSetOffset( void )
{
    return NO_ERROR;
}

int DriverBMI160SPIGyroSetRate( int rate )
{
    uint8_t tmp_wdata = 0x00;
    
    // ODR selection
    m_GyroRate = rate;
    
    switch( m_GyroRate )
    {
        case FASTEST:
            tmp_wdata = ODR_MASK | GYR_RATE_100Hz;
            m_GyroODRHz = 100;
            break;
                
        case GAME:
            tmp_wdata = ODR_MASK | GYR_RATE_50Hz;
            m_GyroODRHz = 50;
            break;
                
        case RATE_33:
            tmp_wdata = ODR_MASK | GYR_RATE_50Hz;
            m_GyroODRHz = 50;
            break;
                
        case RATE_25:
            tmp_wdata = ODR_MASK | GYR_RATE_25Hz;
            m_GyroODRHz = 25;
            break;
                
        case UI:
            tmp_wdata = ODR_MASK | GYR_RATE_25Hz;
            m_GyroODRHz = 25;
            break;
                
        case RATE_10:
            tmp_wdata = ODR_MASK | GYR_RATE_25Hz;
            m_GyroODRHz = 25;
            break;
                
        case NORMAL:
            tmp_wdata = ODR_MASK | GYR_RATE_25Hz;
            m_GyroODRHz = 25;
            break;
                
        default:
            tmp_wdata = ODR_MASK | GYR_RATE_100Hz;
            m_GyroODRHz = 100;
            break;
    }
    if( m_pfun_GyroWrite(REG_GYR_CONF, &tmp_wdata, 1) == I2C_FAIL )
        return I2C_FAIL;
        
    return NO_ERROR;
}

int DriverBMI160SPIGyroSetMode( int mode )
{
    return NO_ERROR;
}

int DriverBMI160SPIGyroGetInfo( uint8_t *data )
{
    return NO_ERROR;
}

int DriverBMI160SPIGyroCheckChipId( uint8_t uSensorSlaveAddr )
{
    uint8_t tmp_rdata = 0x00;

    switch(m_GyroInterfaceType)
    {
		case emSPI1_WH_RL:
		case emSPI1_WL_RH:
		case emSPI2_WH_RL:
		case emSPI2_WL_RH:
            m_GyroSlaveAddr = uSensorSlaveAddr;
            
            if( m_pfun_GyroRead(REG_CHIP_ID, &tmp_rdata, 1) == I2C_FAIL )
                return DRIVER_CHECK_CHIP_ID_FAIL;
                
            if( tmp_rdata == GYR_CHIP_ID ){
                m_pBMI160SPIGyroMem->uHWChipId[0] = tmp_rdata;
                m_pBMI160SPIGyroMem->uHWChipId[1] = uSensorSlaveAddr;
                return NO_ERROR;
            }
                
            return DRIVER_CHECK_CHIP_ID_FAIL;
            break;
    }

    return NO_ERROR;
}

int DriverBMI160SPIGyroScanSlaveAddr( void )
{
    int ret = DRIVER_CHECK_CHIP_ID_FAIL;
    
    if( DriverBMI160SPIGyroCheckChipId(GYR_SLAVE_ADDR_0) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
    else if( DriverBMI160SPIGyroCheckChipId(GYR_SLAVE_ADDR_1) !=  DRIVER_CHECK_CHIP_ID_FAIL )
        ret = NO_ERROR;
        
    if( ret == DRIVER_CHECK_CHIP_ID_FAIL )
        m_pBMI160SPIGyroMem->uHWSensorSlaveAddr = 0;
        
    return ret;
}

int DriverBMI160SPIGyroChipSelect(uint8_t uSelect)
{
    GPIO_Gyro_Chip_Select(uSelect);
    
    return NO_ERROR;
}

#endif /* End of DRIVER_USE_BMI160_SPI */
