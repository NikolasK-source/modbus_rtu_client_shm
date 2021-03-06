/*
 * Copyright (C) 2021-2022 Nikolas Koesling <nikolas@koesling.info>.
 * This program is free software. You can redistribute it and/or modify it under the terms of the MIT License.
 */

#include "Modbus_RTU_Slave.hpp"

#include <stdexcept>

namespace Modbus {
namespace RTU {

static constexpr int MAX_REGS = 0x10000;

Slave::Slave(const std::string &device,
             int                id,
             char               parity,
             int                data_bits,
             int                stop_bits,
             int                baud,
             bool               rs232,
             bool               rs485,
             modbus_mapping_t  *mapping) {
    // create modbus object
    modbus = modbus_new_rtu(device.c_str(), baud, parity, data_bits, stop_bits);
    if (modbus == nullptr) {
        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("failed to create modbus instance: " + error_msg);
    }

    if (mapping == nullptr) {
        // create new mapping with the maximum number of registers
        this->mapping = modbus_mapping_new(MAX_REGS, MAX_REGS, MAX_REGS, MAX_REGS);
        if (this->mapping == nullptr) {
            const std::string error_msg = modbus_strerror(errno);
            modbus_free(modbus);
            throw std::runtime_error("failed to allocate memory: " + error_msg);
        }
        delete_mapping = true;
    } else {
        // use the provided mapping object
        this->mapping  = mapping;
        delete_mapping = false;
    }

    if (modbus_set_slave(modbus, id)) { throw std::runtime_error("invalid modbus id"); }

    // connect
    int tmp = modbus_connect(modbus);
    if (tmp < 0) {
        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("modbus_connect failed: " + error_msg);
    }

    // set mode
    if (rs485 && modbus_rtu_set_serial_mode(modbus, MODBUS_RTU_RS485)) {
        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("Failed to set modbus rtu mode to RS485: " + error_msg);
    }

    if (rs232 && modbus_rtu_set_serial_mode(modbus, MODBUS_RTU_RS232)) {
        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("Failed to set modbus rtu mode to RS232: " + error_msg);
    }
}

Slave::~Slave() {
    if (modbus != nullptr) {
        modbus_close(modbus);
        modbus_free(modbus);
    }
    if (mapping != nullptr && delete_mapping) modbus_mapping_free(mapping);
}

void Slave::set_debug(bool debug) {
    if (modbus_set_debug(modbus, debug)) {
        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("failed to enable modbus debugging mode: " + error_msg);
    }
}

bool Slave::handle_request() {
    // receive modbus request
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int     rc = modbus_receive(modbus, query);

    if (rc > 0) {
        // handle request
        modbus_reply(modbus, query, rc, mapping);
    } else if (rc == -1) {
        if (errno == ECONNRESET) return true;

        const std::string error_msg = modbus_strerror(errno);
        throw std::runtime_error("modbus_receive failed: " + error_msg + ' ' + std::to_string(errno));
    }

    return false;
}

}  // namespace RTU
}  // namespace Modbus
