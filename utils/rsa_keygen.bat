:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
::
:: File Name: rsa_keygen.bat
::
:: Version: 1.20
::
:: Description:
::  Script to generate RSA-2048 public and private keys using OpenSSL.
::  Once created, the script calls a python script to print out the public key
::  modulus that is ready to be inserted into cy_publicKey struct in
::  cy_si_keyStorage.c.
::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Copyright 2017-2018, Cypress Semiconductor Corporation. All rights reserved.
:: This software is owned by Cypress Semiconductor Corporation and is protected
:: by and subject to worldwide patent and copyright laws and treaties.
:: Therefore, you may use this software only as provided in the license agreement
:: accompanying the software package from which you obtained this software.
:: CYPRESS AND ITS SUPPLIERS MAKE NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
:: WITH REGARD TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT,
:: IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

set OUT_DIR="%~dp0\keys_generated"
set PRIV_NAME=rsa_private_generated.txt
set PUB_NAME=rsa_public_generated.txt
set PRIV_RNAME=rsa_private.txt
set PUB_RNAME=rsa_public.txt
set MOD_NAME=rsa_to_c_generated.txt

if not exist %OUT_DIR% mkdir %OUT_DIR%

:: Generate the RSA-2048 public and private keys
openssl genrsa -out %OUT_DIR%\%PRIV_NAME% 2048
openssl rsa -in %OUT_DIR%\%PRIV_NAME% -outform PEM -pubout -out %OUT_DIR%\%PUB_NAME%

copy %OUT_DIR%\%PRIV_NAME% %OUT_DIR%\%PRIV_RNAME%
copy %OUT_DIR%\%PUB_NAME% %OUT_DIR%\%PUB_RNAME%

:: Create C-code ready public key
%~dp0\rsa_to_c.py %OUT_DIR%\%PUB_NAME% > %OUT_DIR%\%MOD_NAME%
