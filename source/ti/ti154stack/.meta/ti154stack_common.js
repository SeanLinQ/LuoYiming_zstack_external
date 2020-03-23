/*
 * Copyright (c) 2017-2019 Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 *  ======== ti154stack_common.js ========
 */

"use strict";

// Max values for C types
const cTypeMax = {
    u_int8: 255,
    u_int16: 65535,
    u_int32: 4294967295,
    int8_t: 127
};

// Settings for ti/devices/CCFG module
const ti154stackCCFGSettings = {
    CC1312R1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352R1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P1_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P_2_LAUNCHXL_CCFG_SETTINGS: {},
    CC1352P_4_LAUNCHXL_CCFG_SETTINGS: {},
    CC26X2R1_LAUNCHXL_CCFG_SETTINGS: {},
    CC2652RB_LAUNCHXL_CCFG_SETTINGS: {}
};

// Dictionary mapping a device name to default LaunchPad
const deviceToBoard = {
    CC1352R: "CC1352R1_LAUNCHXL",
    CC1352P: "CC1352P1_LAUNCHXL",
    CC1312R: "CC1312R1_LAUNCHXL",
    CC2652R1: "CC26X2R1_LAUNCHXL",
    CC2652RB: "CC2652RB_LAUNCHXL"
};

// Name of board
let BOARD = null;
BOARD = getDeviceOrLaunchPadName(true);
const ccfgSettings = ti154stackCCFGSettings[BOARD + "_CCFG_SETTINGS"];

/*!
 *  ======== IS_SUB1GHZ_DEVICE ========
 *  Returns whether device supports Sub-1 GHz frequencies
 *
 *  @returns - Boolean
 */
function IS_SUB1GHZ_DEVICE()
{
    return(getDeviceOrLaunchPadName(true).includes("CC13"));
}

/*!
 *  ======== IS_24GHZ_DEVICE ========
 *  Returns whether device supports 2.4 GHz frequency band
 *
 *  @returns - Boolean
 */
function IS_24GHZ_DEVICE()
{
    const board = getDeviceOrLaunchPadName(true);
    return(board.includes("CC26") || board.includes("CC1352R1")
        || board.includes("CC1352P_2"));
}

/*!
 *  ======== IS_433MHZ_DEVICE ========
 *  Returns whether device supports 433 MHz frequency band
 *
 *  @returns - Boolean
 */
function IS_433MHZ_DEVICE()
{
    return(getDeviceOrLaunchPadName(true).includes("P_4"));
}

/*!
 *  ======== IS_HIGHPA_DEVICE ========
 *  Returns whether device supports high PA
 *
 *  @returns - Boolean
 */
function IS_HIGHPA_DEVICE()
{
    return(getDeviceOrLaunchPadName(true).includes("CC1352P"));
}

/*!
 *  ======== getDeviceOrLaunchPadName ========
 *  Get the name of the board (or device)
 *
 *  @param convertToBoard - Boolean. When true, return the associated LaunchPad
 *                          name if a device is being used without a LaunchPad
 *
 *  @returns String - Name of the board with prefix /ti/boards
 *                    stripped off. If no board was specified,
 *                    the device name is returned.
 */
function getDeviceOrLaunchPadName(convertToBoard)
{
    let name = system.deviceData.deviceId;

    if(system.deviceData.board != null)
    {
        // Get <platform>_LAUNCHXL name
        name = system.deviceData.board.source;

        /* Strip off everything up to and including the last '/' */
        name = name.replace(/.*\//, "");

        /* Strip off everything after and including the first '.' */
        name = name.replace(/\..*/, "");
    }

    // Check if this is a standalone device without a LaunchPad
    if(convertToBoard && !name.includes("LAUNCHXL"))
    {
        // if you are using a custom board, use the previously defined
        // launchpad as default
        if(BOARD !== null)
        {
            if(BOARD.includes("LAUNCHXL") && (BOARD) !== null)
            {
                // Set the previous launchpad name if custom board is selected
                name = BOARD;
                return(name);
            }
        }

        // Find the LaunchPad name in deviceToBoard dictionary
        let key = null;
        for(key in deviceToBoard)
        {
            if(name.includes(key))
            {
                name = deviceToBoard[key];
                break;
            }
        }
    }
    return(name);
}

/*!
 *  ======== toHexString ========
 *  Convert decimal number to hexadecimal string
 *
 *  @param number - decimal number to be converted
 *  @param padding - length of padding to be added
 *  @returns String - hexadecimal string representation of decimal value
 */
function toHexString(number, padding)
{
    let str = _.toUpper(number.toString(16));
    if(padding)
    {
        str = _.padStart(str, padding, "0");
    }
    return("0x" + str);
}

/*!
 *  ======== isPositiveInteger ========
 *  Returns whether number passed is a positive whole number
 *
 *  @param value - number to be verified
 *  @returns Boolean - true if positive whole number, false otherwise
 */
function isPositiveInteger(value)
{
    return(Number.isInteger(value) && value >= 0);
}

/*!
 *  ======== validateRange ========
 *  Validates value of config is within range passed
 *
 *  @param inst       - module instance containing the config to be validated
 *  @param validation - object to hold detected validation issues
 *  @param cfgName    - name of config to be validated
 *  @param min        - minimum value allowed
 *  @param max        - maximum value allowed
 *  @param transform  - function to transform value to hex/decimal
 *  @returns Boolean  - true if value does not fall within range
 */
function validateRange(inst, validation, cfgName, min, max, transForm)
{
    const value = inst[cfgName];

    if(!isPositiveInteger(value))
    {
        validation.logError("Must be a whole number", inst, cfgName);
    }

    if(value < min || value > max)
    { // not in range
        validation.logError(`Must be between ${transForm(min)} and `
            + `${transForm(max)}`, inst, cfgName);
    }

    return((value < min) || (value > max));
}

/*!
 *  ======== validateRangeHex ========
 *  Validates the value of config is within range passed and prints error
 *  message, if any, with hexadecimal values
 *
 *  @param inst       - module instance containing the config to be validated
 *  @param validation - object to hold detected validation issues
 *  @param cfgName    - name of config to be validated
 *  @param min        - minimum value allowed
 *  @param max        - maximum value allowed
 */
function validateRangeHex(inst, validation, cfgName, min, max)
{
    validateRange(inst, validation, cfgName, min, max, toHexString);
}

/*!
 *  ======== validateRangeInt ========
 *  Validates the value of config is within range passed and prints error
 *  message, if any, with integer values
 *
 *  @param inst       - module instance containing the config to be validated
 *  @param validation - object to hold detected validation issues
 *  @param cfgName    - name of config to be validated
 *  @param min        - minimum value allowed
 *  @param max        - maximum value allowed
 */
function validateRangeInt(inst, validation, cfgName, min, max)
{
    validateRange(inst, validation, cfgName, min, max, Number);
}

/*!
 *  ======== convertToCByteArray ========
 *  Turns an array specifying bits (starting from bit 0) to a byte array with
 *  the specified bits set. LSB is the 0th byte of the array.
 *  Example:
 *  bitsArray = [1,8]
 *  corresponding c byte array
 *  cByteArray = [2,1,0,0,0,0,0,0,0,0,0,.. up to num bytes]
 *
 *  @param bitsArray - array of bits to be set, e.g. [1,8]
 *  @param numBytes  - total number of bytes
 *  @returns Array   - byte array with specified bits set
 */
function convertToCByteArray(bitsArray, numBytes)
{
    const arr = _.fill(Array(numBytes), 0); // array filled with number of 0's
    _.each(bitsArray, (bit) =>
    {
        const arrIndex = Math.floor(bit / 8);
        const arrBit = bit - (arrIndex * 8);
        arr[arrIndex] |= (1 << arrBit);
    });
    return(arr);
}

/*!
 *  ======== channelMaskCHexStrArr ========
 *  Returns array of bytes where each byte is presented as a hexadecimal string
 *  Example: ["0x02","0x00",...]
 *
 *  @param channelMask - channels selected in config
 *  @returns String    - array of hexadecimal strings representing channel mask
 */
function channelMaskCHexStrArr(channelMask)
{
    const channelMaskCByteArr = convertToCByteArray(channelMask, 17);

    return(_.map(channelMaskCByteArr, (byte) => toHexString(byte, 2)));
}

/*
 * ======== findConfig ========
 * Finds and returns the configurable with the matching provided name
 *
 * @param config  - A module's configurable array
 * @param configName - The name of the configurable to search for
 *
 * @returns - undefined if the configurable is not found, otherwise the entire
 *            configurable object
 */
function findConfig(config, configName)
{
    let enumDef;

    let element = null;
    for(element of config)
    {
        // If the element contains a group, need to search it's configurables
        if("config" in element)
        {
            // Recursively search the sub-groups config array
            enumDef = findConfig(element.config, configName);

            // Stop searching if the configurable was found in the sub-group
            if(enumDef !== undefined)
            {
                break;
            }
        }
        else if(element.name === configName)
        {
            // Stop searching if the current element is the correct configurable
            enumDef = element;
            break;
        }
    }

    return(enumDef);
}


/*
 * ======== restoreDefaultValue ========
 * Restore the configurable's value to the default.
 *
 * @param inst  - The module instance
 * @param _cfg - the config that will be restored to default
 * @param cfgName - The name of the configurable to change back to default
 *
 * @returns - void
 */
function restoreDefaultValue(inst, _cfg, cfgName)
{
    /* if you find a match, set the default and return */
    if(_cfg.name === cfgName)
    {
        /* Make sure a default exists */
        if(_cfg.default !== null && _cfg.default !== undefined)
        {
            /* Set the instance based on the default. */
            inst[cfgName] = _cfg.default;
        }
    }
}

exports = {
    ccfgSettings: ccfgSettings,
    BOARD: BOARD,
    IS_433MHZ_DEVICE: IS_433MHZ_DEVICE,
    IS_SUB1GHZ_DEVICE: IS_SUB1GHZ_DEVICE,
    IS_24GHZ_DEVICE: IS_24GHZ_DEVICE,
    IS_HIGHPA_DEVICE: IS_HIGHPA_DEVICE,
    getDeviceOrLaunchPadName: getDeviceOrLaunchPadName,
    cTypeMax: cTypeMax,
    toHexString: toHexString,
    validateRangeHex: validateRangeHex,
    validateRangeInt: validateRangeInt,
    channelMaskCHexStrArr: channelMaskCHexStrArr,
    findConfig: findConfig,
    restoreDefaultValue: restoreDefaultValue
};
