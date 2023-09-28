/**********************************************************************************************************************
 * \file credentials_config.h
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

#ifndef CREDENTIALS_CONFIG_H_
#define CREDENTIALS_CONFIG_H_

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
/* AWS IoT thing name */
#define CLIENT_IDENTIFIER                       "Enter you AWS IoT thing name here"

/* AWS IoT device data endpoint */
#define AWS_IOT_ENDPOINT                        "Enter your AWS IoT device data endpoint here"

/* AWS IoT MQTT port number*/
#define AWS_MQTT_PORT                           (8883)

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
/* AWS Broker connection Info */
/* The aws_root_ca_certificate field requires Amazon Root CA 1 certificate for AWS OTA update feature.
 * AmazonRootCA1 is generated while 'Thing' creation on AWS portal.
 *
 * Must follow the below format and include the PEM header and footer:
        "-----BEGIN CERTIFICATE-----\n"
        ".........base64 data.......\n"
        "-----END CERTIFICATE-----";
 */
static const char aws_root_ca_certificate[] = "";

/* AWS - Device certificate generated while 'Thing' creation on AWS IoT portal */
/* Must follow the below format and include the PEM header and footer:
        "-----BEGIN CERTIFICATE-----\n"
        ".........base64 data.......\n"
        "-----END CERTIFICATE-------";
 */
static const char aws_client_cert[] = "";

/* AWS - Device Private key generated while 'Thing' creation on AWS IoT portal */
/* Must follow the below format and include the PEM header and footer:
        "-----BEGIN PRIVATE KEY-----\n"
        ".........base64 data.......\n"
        "-----END PRIVATE KEY-----";
 */
static const char aws_client_key[] = "";

#endif /* CREDENTIALS_CONFIG_H_ */

/* [] END OF FILE */