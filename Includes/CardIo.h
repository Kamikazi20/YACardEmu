#ifndef CARDIO_H
#define CARDIO_H

#include <vector>
#include <iostream>
#include <fstream>
#include <filesystem>

class CardIo
{
public:
	enum StatusCode {
		Okay,
		SyncError,
		ChecksumError,
		EmptyResponseError,
		ServerWaitingReply,
	};

	CardIo();
	CardIo::StatusCode BuildPacket(std::vector<uint8_t> *buffer);
	CardIo::StatusCode ReceivePacket(std::vector<uint8_t> *buffer);
private:
	const uint8_t START_OF_TEXT = 0x02;
	const uint8_t END_OF_TEXT = 0x03;
	const uint8_t ENQUIRY = 0x05;
	const uint8_t ACK = 0x06;

	const uint8_t CARD_SIZE = 0x45;

	enum class ReaderStatus {
		NoCard = 30,
		HasCard = 31,
		UNK_34 = 34,
	};

	enum class PrinterStatus {
		Idle = 30,
		Print = 31,
	};

	enum class UNK_Status {
		Idle = 30,
		UNK_32 = 32, // Used during LoadCard, assumption is - reading.
		UKN_34 = 34, // Only seen in ReadCard if we don't have a card?
	};

	typedef struct {
		ReaderStatus Reader = ReaderStatus::NoCard;
		PrinterStatus Printer = PrinterStatus::Idle;
		UNK_Status Status = UNK_Status::Idle;

		void Reset() {
			Reader = ReaderStatus::NoCard;
			Printer = PrinterStatus::Idle;
			Status = UNK_Status::Idle;
		}
	} machine_status;

	machine_status RPS;

	uint8_t GetByte(std::vector<uint8_t> *buffer);
	void HandlePacket(std::vector<uint8_t> *packet);

	const std::string card_name = "test.bin";
	std::vector<uint8_t> *card_data;

	void LoadCardFromFS(std::string card_name);
	void SaveCardToFS(std::string card_name);

	void PutStatusInBuffer();

	// Commands
	int WMMT_Command_10_Init();
	int WMMT_Command_20_Get_Card_State();
	int WMMT_Command_33_Read_Card();
	int WMMT_Command_40_Is_Card_Present();
	int WMMT_Command_53_Write_Card(std::vector<uint8_t> *packet);

	int WMMT_Command_73_UNK();
	
	int WMMT_Command_78_UNK(); // Important;
	int WMMT_Command_7A_UNK();
	int WMMT_Command_7B_UNK(); // card inserted?
	int WMMT_Command_7C_Write_Card_Text();
	int WMMT_Command_7D_UNK();
	int WMMT_Command_80_Eject_Card();

	int WMMT_Command_A0_Clean_Card();
	int WMMT_Command_B0_Load_Card();
	int WMMT_Command_D0_UNK();

	std::vector<uint8_t> ResponseBuffer;	// Command Response
};

#endif
