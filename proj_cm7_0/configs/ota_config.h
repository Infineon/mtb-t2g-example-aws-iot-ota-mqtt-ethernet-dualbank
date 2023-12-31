/**********************************************************************************************************************
 * \file ota_config.h
 * \copyright Copyright (C) Infineon Technologies AG 2019
 * 
 * Use of this file is subject to the terms of use agreed between (i) you or the company in which ordinary course of 
 * business you are acting and (ii) Infineon Technologies AG or its licensees. If and as long as no such terms of use
 * are agreed, use of this file is subject to following:
 * 
 * Boost Software License - Version 1.0 - August 17th, 2003
 * 
 * Permission is hereby granted, free of charge, to any person or organization obtaining a copy of the software and 
 * accompanying documentation covered by this license (the "Software") to use, reproduce, display, distribute, execute,
 * and transmit the Software, and to prepare derivative works of the Software, and to permit third-parties to whom the
 * Software is furnished to do so, all subject to the following:
 * 
 * The copyright notices in the Software and this entire statement, including the above license grant, this restriction
 * and the following disclaimer, must be included in all copies of the Software, in whole or in part, and all 
 * derivative works of the Software, unless such copies or derivative works are solely in the form of 
 * machine-executable object code generated by a source language processor.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL THE 
 * COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN 
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE.
 *********************************************************************************************************************/

#ifndef OTA_CONFIG_H_
#define OTA_CONFIG_H_

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/*
 * Log base 2 of the size of the file data block message (excluding the header).
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '12'
 */
#define otaconfigLOG2_FILE_BLOCK_SIZE           12UL


/*
 * Size of the file data block message (excluding the header).
 */
#define otaconfigFILE_BLOCK_SIZE                ( 1UL << otaconfigLOG2_FILE_BLOCK_SIZE )

/**
 * Milliseconds to wait for the self test phase to succeed before we force reset.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '16000'
 */
#define otaconfigSELF_TEST_RESPONSE_WAIT_MS     16000U

/**
 * Milliseconds to wait before requesting data blocks from the OTA service if nothing is happening.
 *
 * The wait timer is reset whenever a data block is received from the OTA service
 * so we will only send the request message after being idle for this amount of time.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '10000'
 */
#define otaconfigFILE_REQUEST_WAIT_MS           10000U

/**
 * The maximum allowed length of the thing name used by the OTA agent.
 *
 * AWS IoT requires Thing names to be unique for each device that connects to the broker.
 * Likewise, the OTA agent requires the developer to construct and pass in the Thing name when initializing the OTA agent.
 * The agent uses this size to allocate static storage for the Thing name used in all OTA base topics.
 * Namely $aws/things/thingName
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '64'
 */
#define otaconfigMAX_THINGNAME_LEN              64U

/**
 * The maximum number of data blocks requested from OTA streaming
 * service.
 *
 * This configuration parameter is sent with data requests and represents the maximum number of data blocks the service
 * will send in response.
 * The maximum limit for this must be calculated from the maximum data response limit (128 KB from service) divided
 * by the block size.
 * For example if block size is set as 1 KB then the maximum number of data blocks that we can request is 128/1 = 128 blocks.
 * Configure this parameter to this maximum limit or lower based on how many data blocks response is expected for each
 * data requests.
 *
 * Possible values: Any unsigned 32 integer value greater than 0.
 * Default value: '1'
 */
#define otaconfigMAX_NUM_BLOCKS_REQUEST         8U

/**
 * The maximum number of requests allowed to send without a response
 * before we abort.
 *
 * This configuration parameter sets the maximum number of times the requests are made over the selected communication channel
 * before aborting and returning error.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '32'
 */
#define otaconfigMAX_NUM_REQUEST_MOMENTUM       32U

/**
 * How frequently the device will report its OTA progress to the cloud.
 *
 * Device will update the job status with the number of blocks it has received every certain number of blocks it receives.
 * For example, 64 means device will update job status every 64 blocks it receives.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '64'
 */
#define otaconfigOTA_UPDATE_STATUS_FREQUENCY    64U

/**
 * The number of data buffers reserved by the OTA agent.
 *
 * This configurations parameter sets the maximum number of static data buffers used by the OTA agent for job and file data
 * blocks received.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '1'
 */
#define otaconfigMAX_NUM_OTA_DATA_BUFFERS       10U

/**
 * Flag to enable booting into updates that have an identical or lower version than the current version.
 *
 * Set this configuration parameter to '1' to disable version checks.
 * This allows updates to an identical or lower version.
 * This is provided for testing purpose and it's recommended to always update to higher version and keep this configuration
 * disabled.
 *
 * Possible values: Any unsigned 32 integer.
 * Default value: '0'
 */
#ifndef otaconfigAllowDowngrade
    #define otaconfigAllowDowngrade                 0U
#endif

/**
 * The file type id received in the job document.
 *
 * The file type id received in the job document that allows devices to identify the type of file received from the cloud.
 * This configuration defines the file type id used for firmware updates.
 * If this is changed then the updated value must be used while creating firmware update jobs.
 *
 */
#define configOTA_FIRMWARE_UPDATE_FILE_TYPE_ID      141U

/**
 * The protocol selected for OTA control operations.
 *
 * This configurations parameter sets the default protocol for all the OTA control operations like requesting OTA job,
 * updating the job status etc.
 *
 * Only MQTT is supported at this time for control operations.
 *
 * Possible values: OTA_CONTROL_OVER_MQTT
 * Default value: 'OTA_CONTROL_OVER_MQTT'
 */
#define configENABLED_CONTROL_PROTOCOL      ( OTA_CONTROL_OVER_MQTT )

/**
 * The protocol selected for OTA data operations.
 *
 * This configurations parameter sets the protocols selected for the data operations like requesting file blocks from
 * the service.
 *
 * Possible values:
 * Enable data over MQTT - ( OTA_DATA_OVER_MQTT )
 * Enable data over HTTP - ( OTA_DATA_OVER_HTTP )
 * Enable data over both MQTT & HTTP - ( OTA_DATA_OVER_MQTT | OTA_DATA_OVER_HTTP )
 * Default value: 'OTA_DATA_OVER_MQTT'
 */
#define configENABLED_DATA_PROTOCOLS        ( OTA_DATA_OVER_MQTT )

/**
 * The preferred protocol selected for OTA data operations.
 *
 * Primary data protocol will be the protocol used for downloading file if more than one protocol is selected while
 * creating OTA job.
 *
 * Possible values:
 * Data over MQTT - ( OTA_DATA_OVER_MQTT )
 * Data over HTTP - ( OTA_DATA_OVER_HTTP )
 * Default value:  'OTA_DATA_OVER_MQTT'
 */
#define configOTA_PRIMARY_DATA_PROTOCOL     ( OTA_DATA_OVER_MQTT )

/**
 * Code signing certificate is used for OTA image signing.
 * If a platform does not support a file system the signing certificate can be pasted here.
 */
#ifndef AWS_IOT_OTA_SIGNING_CERT
#define AWS_IOT_OTA_SIGNING_CERT            "Insert your AWS code signing certificate here"
#endif

#endif /* OTA_CONFIG_H_ */

/* [] END OF FILE */
