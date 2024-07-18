///===-----------------------------------------------------------------------------------------===//
///
/// Copyright (c) PWr in Space. All rights reserved.
/// Created: 31.05.2024 by Michał Kos
///
///===-----------------------------------------------------------------------------------------===//
///
/// \file
/// This file contains declaration of the CAN BUS commands that are used in the communication over
/// the CAN BUS with TANWA submodules.
///===-----------------------------------------------------------------------------------------===//

#ifndef PWRINSPACE_TANWA_CAN_COMMANDS_H_
#define PWRINSPACE_TANWA_CAN_COMMANDS_H_

#include "mcu_twai_config.h"

///===-----------------------------------------------------------------------------------------===//
// FLC CAN commands IDs
///===-----------------------------------------------------------------------------------------===//

typedef enum {
    // Commands to the FLC submodule
    CAN_FLC_TX_GET_STATUS = 0x0D0,
    CAN_FLC_TX_GET_DATA = 0x0D1,
    // place for new commands
    CAN_FLC_TX_SOFT_RESET = 0x0D9,
    CAN_FLC_TX_NOTHING = 0x0DF,
    // Responses from the FLC submodule
    CAN_FLC_RX_STATUS = 0x0DA,
    CAN_FLC_RX_DATA = 0x0DB,
} can_flc_commands_t;



// FLC submodule

#define CAN_FLC_GET_STATUS() {           \
    .identifier = CAN_FLC_TX_GET_STATUS, \
    .data_length_code = 0,               \
    .data = {0, 0, 0, 0, 0, 0, 0, 0}     \
}

#define CAN_FLC_GET_DATA() {             \
    .identifier = CAN_FLC_TX_GET_DATA,   \
    .data_length_code = 0,               \
    .data = {0, 0, 0, 0, 0, 0, 0, 0}     \
}

#define CAN_FLC_SOFT_RESET() {           \
    .identifier = CAN_FLC_TX_SOFT_RESET, \
    .data_length_code = 0,               \
    .data = {0, 0, 0, 0, 0, 0, 0, 0}     \
}




#endif /* PWRINSPACE_TANWA_CAN_COMMANDS_H_ */
