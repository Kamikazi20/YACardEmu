/*
    YACardEmu
    ----------------
    Copyright (C) 2020-2023 wutno (https://github.com/GXTX)


    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "SerIo.h"

SerIo::SerIo(SerIo::Settings *settings)
{
	m_portSettings = settings;
	m_readBuffer.reserve(512); // max length * 2
	m_writeBuffer.reserve(512);
}

SerIo::~SerIo()
{
	if (m_isPipe) {
#ifdef _WIN32	
		CloseHandle(m_pipeHandle);
#endif
	} else {
		sp_close(m_portHandle);
	}	
}

bool SerIo::Open()
{
#ifdef _WIN32
	if (m_portSettings->devicePath.find("pipe") != std::string::npos) {
		m_pipeHandle = CreateNamedPipeA(m_portSettings->devicePath.c_str(), PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, 1, 0, 0, NMPWAIT_USE_DEFAULT_WAIT, NULL);

		if (m_pipeHandle == INVALID_HANDLE_VALUE) {
			spdlog::critical("SerIo::Init: Failed to create pipe");
			return false;
		}

		m_isPipe = true;
		return true;
	}
#endif

	sp_new_config(&m_portConfig);
	sp_set_config_baudrate(m_portConfig, m_portSettings->baudRate);
	sp_set_config_bits(m_portConfig, 8);
	sp_set_config_parity(m_portConfig, m_portSettings->parity);
	sp_set_config_stopbits(m_portConfig, 1);
	sp_set_config_flowcontrol(m_portConfig, SP_FLOWCONTROL_NONE);

	sp_get_port_by_name(m_portSettings->devicePath.c_str(), &m_portHandle);

	if (sp_open(m_portHandle, SP_MODE_READ_WRITE) != SP_OK) {
		spdlog::critical("SerIo::Init: Failed to open {}", m_portSettings->devicePath);
		return false;
	}

	sp_set_config(m_portHandle, m_portConfig);

	return true;
}

void SerIo::SendAck()
{
	constexpr static const uint8_t ack = 0x06;

	spdlog::debug("SerIo::SendAck: 06");

#ifdef _WIN32
	if (m_isPipe) {
		DWORD dwRet = 0;
		WriteFile(m_pipeHandle, &ack, 1, &dwRet, NULL);
		m_portSettings->status = Status::Okay;
		return;
	}
#endif

	sp_blocking_write(m_portHandle, &ack, 1, 0);
	sp_drain(m_portHandle);
	m_portSettings->status = Status::Okay;
}

void SerIo::Write()
{
	if (m_writeBuffer.empty()) {
		m_portSettings->status = Status::ZeroSizeError;
		return;
	}

	spdlog::debug("SerIo::Write: ");
	spdlog::debug("{:Xn}", spdlog::to_hex(m_writeBuffer));

	int ret = 0;

	if (m_isPipe) {
#ifdef _WIN32
		DWORD dwRet = 0;
		WriteFile(m_pipeHandle, &m_writeBuffer[0], static_cast<DWORD>(m_writeBuffer.size()), &dwRet, NULL);
		ret = dwRet;
#endif
	} else {
		ret = sp_blocking_write(m_portHandle, &m_writeBuffer[0], m_writeBuffer.size(), 0);
		sp_drain(m_portHandle);
	}

	// TODO: Should we care about write errors?
	if (ret <= 0) {
		m_portSettings->status = Status::WriteError;
	} else if (ret != static_cast<int>(m_writeBuffer.size())) {
		spdlog::error("Only wrote {0:X} of {1:X} to the port!", ret, m_writeBuffer.size());
		m_portSettings->status = Status::WriteError;
	} else {
		m_portSettings->status = Status::Okay;
	}
}

void SerIo::Read()
{
	int bytes = 0;

	if (m_isPipe) {
#ifdef _WIN32
		DWORD dwBytes = 0;
		if (PeekNamedPipe(m_pipeHandle, 0, 0, 0, &dwBytes, 0) == 0) {
			DWORD error = ::GetLastError();
			if (error == ERROR_BROKEN_PIPE) {
				spdlog::warn("Pipe broken! Resetting...");
				DisconnectNamedPipe(m_pipeHandle);
				ConnectNamedPipe(m_pipeHandle, NULL);
			}
			m_portSettings->status = Status::ReadError;
			return;
		}
		bytes = dwBytes;
#endif
	} else {
		bytes = sp_input_waiting(m_portHandle);
	}

	if (bytes < 1) {
		m_portSettings->status = Status::ZeroSizeError;
		return;
	}

	m_readBuffer.clear();
	m_readBuffer.resize(static_cast<size_t>(bytes));

	int ret = 0;

	if (m_isPipe) {
#ifdef _WIN32
		DWORD dwRet = 0;
		BOOL bRet = ReadFile(m_pipeHandle, &m_readBuffer[0], static_cast<DWORD>(m_readBuffer.size()), &dwRet, NULL);
		ret = bRet;
#endif
	} else {
		ret = sp_nonblocking_read(m_portHandle, &m_readBuffer[0], m_readBuffer.size());
	}

	if (ret <= 0) {
		m_portSettings->status = Status::ReadError;
		return;
	}

	spdlog::debug("SerIo::Read: ");
	spdlog::debug("{:Xn}", spdlog::to_hex(m_readBuffer));

	m_portSettings->status = Status::Okay;
}
