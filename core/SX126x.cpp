/*
    This file is part of SX126x Portable driver.
    Copyright (C) 2020 ReimuNotMoe

    This program is based on sx126x-driver from Semtech S.A.,
    see LICENSE-SEMTECH.txt for details.

    Original maintainer of sx126x-driver: Miguel Luis, Gregory Cristian
    and Matthieu Verdy

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include "SX126x.h"

void SX126x::Init( void ){
	Reset();
	Wakeup();
	SetStandby(STDBY_RC);

	OperatingMode = MODE_STDBY_RC;

	SetPacketType( PACKET_TYPE_LORA );
}

SX126x::RadioOperatingModes_t SX126x::GetOperatingMode( void )
{
	return OperatingMode;
}

void SX126x::CheckDeviceReady( void )
{
	if( ( GetOperatingMode( ) == MODE_SLEEP ) || ( GetOperatingMode( ) == MODE_RX_DC ) )
	{
		Wakeup( );
	}
}

void SX126x::SetPayload( uint8_t *payload, uint8_t size )
{
	WriteBuffer( 0x00, payload, size );
}

uint8_t SX126x::GetPayload( uint8_t *buffer, uint8_t *size,  uint8_t maxSize )
{
	uint8_t offset = 0;

	GetRxBufferStatus( size, &offset );
	if( *size > maxSize )
	{
		return 1;
	}
	ReadBuffer( offset, buffer, *size );
	return 0;
}

void SX126x::SendPayload( uint8_t *payload, uint8_t size, uint32_t timeout )
{
	SetPayload( payload, size );
	SetTx( timeout );
}

uint8_t SX126x::SetSyncWord( uint8_t *syncWord )
{
	WriteRegister( REG_LR_SYNCWORDBASEADDRESS, syncWord, 8 );
	return 0;
}

void SX126x::SetCrcSeed( uint16_t seed )
{
	uint8_t buf[2];

	buf[0] = ( uint8_t )( ( seed >> 8 ) & 0xFF );
	buf[1] = ( uint8_t )( seed & 0xFF );

	switch( GetPacketType( ) )
	{
		case PACKET_TYPE_GFSK:
			WriteRegister( REG_LR_CRCSEEDBASEADDR, buf, 2 );
			break;

		default:
			break;
	}
}

void SX126x::SetCrcPolynomial( uint16_t polynomial )
{
	uint8_t buf[2];

	buf[0] = ( uint8_t )( ( polynomial >> 8 ) & 0xFF );
	buf[1] = ( uint8_t )( polynomial & 0xFF );

	switch( GetPacketType( ) )
	{
		case PACKET_TYPE_GFSK:
			WriteRegister( REG_LR_CRCPOLYBASEADDR, buf, 2 );
			break;

		default:
			break;
	}
}

void SX126x::SetWhiteningSeed( uint16_t seed )
{
	uint8_t regValue = 0;

	switch( GetPacketType( ) )
	{
		case PACKET_TYPE_GFSK:
			regValue = ReadReg( REG_LR_WHITSEEDBASEADDR_MSB ) & 0xFE;
			regValue = ( ( seed >> 8 ) & 0x01 ) | regValue;
			WriteReg( REG_LR_WHITSEEDBASEADDR_MSB, regValue ); // only 1 bit.
			WriteReg( REG_LR_WHITSEEDBASEADDR_LSB, ( uint8_t )seed );
			break;

		default:
			break;
	}
}

uint32_t SX126x::GetRandom( void )
{
	uint8_t buf[] = { 0, 0, 0, 0 };

	// Set radio in continuous reception
	SetRx(0);

	sleep_ms(1);

	ReadRegister( RANDOM_NUMBER_GENERATORBASEADDR, buf, 4 );

	SetStandby( STDBY_RC );

	return ( buf[0] << 24 ) | ( buf[1] << 16 ) | ( buf[2] << 8 ) | buf[3];
}

void SX126x::SetSleep( SleepParams_t sleepConfig )
{
#ifdef ADV_DEBUG
	printf("SetSleep ");
#endif

	WriteCommand( RADIO_SET_SLEEP, &sleepConfig.Value, 1 );
	OperatingMode = MODE_SLEEP;
}

void SX126x::SetStandby( RadioStandbyModes_t standbyConfig )
{
#ifdef ADV_DEBUG
	printf("SetStandby ");
#endif
	WriteCommand( RADIO_SET_STANDBY, ( uint8_t* )&standbyConfig, 1 );
	if( standbyConfig == STDBY_RC )
	{
		OperatingMode = MODE_STDBY_RC;
	}
	else
	{
		OperatingMode = MODE_STDBY_XOSC;
	}
}

void SX126x::SetFs( void )
{
#ifdef ADV_DEBUG
	printf("SetFs ");
#endif
	WriteCommand( RADIO_SET_FS, 0, 0 );
	OperatingMode = MODE_FS;
}

void SX126x::SetTx( uint32_t timeout )
{
	uint8_t buf[3];

	OperatingMode = MODE_TX;

#ifdef ADV_DEBUG
	printf("SetTx ");
#endif

	buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
	buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
	buf[2] = ( uint8_t )( timeout & 0xFF );

	HalPostRx();
	HalPreTx();
	WriteCommand( RADIO_SET_TX, buf, 3 );
}

void SX126x::SetRxBoosted( uint32_t timeout )
{
	uint8_t buf[3];

	OperatingMode = MODE_RX;

#ifdef ADV_DEBUG
	printf("SetRxBoosted ");
#endif

	WriteReg( REG_RX_GAIN, 0x96 ); // max LNA gain, increase current by ~2mA for around ~3dB in sensivity

	buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
	buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
	buf[2] = ( uint8_t )( timeout & 0xFF );

	HalPostTx();
	HalPreRx();
	WriteCommand( RADIO_SET_RX, buf, 3 );
}

void SX126x::SetRx( uint32_t timeout )
{
	uint8_t buf[3];

	OperatingMode = MODE_RX;

#ifdef ADV_DEBUG
	printf("SetRx ");
#endif

	buf[0] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
	buf[1] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
	buf[2] = ( uint8_t )( timeout & 0xFF );

	HalPostTx();
	HalPreRx();
	WriteCommand( RADIO_SET_RX, buf, 3 );
}

void SX126x::SetRxDutyCycle( uint32_t rxTime, uint32_t sleepTime )
{
	uint8_t buf[6];

	buf[0] = ( uint8_t )( ( rxTime >> 16 ) & 0xFF );
	buf[1] = ( uint8_t )( ( rxTime >> 8 ) & 0xFF );
	buf[2] = ( uint8_t )( rxTime & 0xFF );
	buf[3] = ( uint8_t )( ( sleepTime >> 16 ) & 0xFF );
	buf[4] = ( uint8_t )( ( sleepTime >> 8 ) & 0xFF );
	buf[5] = ( uint8_t )( sleepTime & 0xFF );

	HalPostTx();
	HalPreRx();
	WriteCommand( RADIO_SET_RXDUTYCYCLE, buf, 6 );
	OperatingMode = MODE_RX_DC;
}

void SX126x::SetCad( void )
{
	HalPostTx();
	HalPreRx();
	WriteCommand( RADIO_SET_CAD, 0, 0 );
	OperatingMode = MODE_CAD;
}

void SX126x::SetTxContinuousWave( void )
{
#ifdef ADV_DEBUG
	printf("SetTxContinuousWave ");
#endif

	HalPostRx();
	HalPreTx();
	WriteCommand( RADIO_SET_TXCONTINUOUSWAVE, 0, 0 );
}

void SX126x::SetTxInfinitePreamble( void )
{
#ifdef ADV_DEBUG
	printf("SetTxContinuousPreamble ");
#endif

	HalPostRx();
	HalPreTx();
	WriteCommand( RADIO_SET_TXCONTINUOUSPREAMBLE, 0, 0 );
}

void SX126x::SetStopRxTimerOnPreambleDetect( bool enable )
{
	WriteCommand( RADIO_SET_STOPRXTIMERONPREAMBLE, ( uint8_t* )&enable, 1 );
}

void SX126x::SetLoRaSymbNumTimeout( uint8_t SymbNum )
{
	WriteCommand( RADIO_SET_LORASYMBTIMEOUT, &SymbNum, 1 );
}

void SX126x::SetRegulatorMode( RadioRegulatorMode_t mode )
{
#ifdef ADV_DEBUG
	printf("SetRegulatorMode ");
#endif
	WriteCommand( RADIO_SET_REGULATORMODE, ( uint8_t* )&mode, 1 );
}

void SX126x::Calibrate( CalibrationParams_t calibParam )
{
	WriteCommand( RADIO_CALIBRATE, &calibParam.Value, 1 );
}

void SX126x::CalibrateImage( uint32_t freq )
{
	uint8_t calFreq[2];

	if( freq > 900000000 )
	{
		calFreq[0] = 0xE1;
		calFreq[1] = 0xE9;
	}
	else if( freq > 850000000 )
	{
		calFreq[0] = 0xD7;
		calFreq[1] = 0xD8;
	}
	else if( freq > 770000000 )
	{
		calFreq[0] = 0xC1;
		calFreq[1] = 0xC5;
	}
	else if( freq > 460000000 )
	{
		calFreq[0] = 0x75;
		calFreq[1] = 0x81;
	}
	else if( freq > 425000000 )
	{
		calFreq[0] = 0x6B;
		calFreq[1] = 0x6F;
	}
	WriteCommand( RADIO_CALIBRATEIMAGE, calFreq, 2 );
}

void SX126x::SetPaConfig( uint8_t paDutyCycle, uint8_t HpMax, uint8_t deviceSel, uint8_t paLUT )
{
	uint8_t buf[4];

#ifdef ADV_DEBUG
	printf("SetPaConfig ");
#endif

	buf[0] = paDutyCycle;
	buf[1] = HpMax;
	buf[2] = deviceSel;
	buf[3] = paLUT;
	WriteCommand( RADIO_SET_PACONFIG, buf, 4 );
}

void SX126x::SetRxTxFallbackMode( uint8_t fallbackMode )
{
	WriteCommand( RADIO_SET_TXFALLBACKMODE, &fallbackMode, 1 );
}

void SX126x::SetDioIrqParams( uint16_t irqMask, uint16_t dio1Mask, uint16_t dio2Mask, uint16_t dio3Mask )
{
	uint8_t buf[8];

#ifdef ADV_DEBUG
	printf("SetDioIrqParams ");
#endif

	buf[0] = ( uint8_t )( ( irqMask >> 8 ) & 0x00FF );
	buf[1] = ( uint8_t )( irqMask & 0x00FF );
	buf[2] = ( uint8_t )( ( dio1Mask >> 8 ) & 0x00FF );
	buf[3] = ( uint8_t )( dio1Mask & 0x00FF );
	buf[4] = ( uint8_t )( ( dio2Mask >> 8 ) & 0x00FF );
	buf[5] = ( uint8_t )( dio2Mask & 0x00FF );
	buf[6] = ( uint8_t )( ( dio3Mask >> 8 ) & 0x00FF );
	buf[7] = ( uint8_t )( dio3Mask & 0x00FF );
	WriteCommand( RADIO_CFG_DIOIRQ, buf, 8 );
}

uint16_t SX126x::GetIrqStatus( void )
{
	uint8_t irqStatus[2];

	ReadCommand( RADIO_GET_IRQSTATUS, irqStatus, 2 );
	return ( irqStatus[0] << 8 ) | irqStatus[1];
}

void SX126x::SetDio2AsRfSwitchCtrl( uint8_t enable )
{
#ifdef ADV_DEBUG
	printf("SetDio2AsRfSwitchCtrl ");
#endif
	WriteCommand( RADIO_SET_RFSWITCHMODE, &enable, 1 );
}

void SX126x::SetDio3AsTcxoCtrl( RadioTcxoCtrlVoltage_t tcxoVoltage, uint32_t timeout )
{
	uint8_t buf[4];

	buf[0] = tcxoVoltage & 0x07;
	buf[1] = ( uint8_t )( ( timeout >> 16 ) & 0xFF );
	buf[2] = ( uint8_t )( ( timeout >> 8 ) & 0xFF );
	buf[3] = ( uint8_t )( timeout & 0xFF );

	WriteCommand( RADIO_SET_TCXOMODE, buf, 4 );
}

void SX126x::SetRfFrequency( uint32_t frequency )
{
	uint8_t buf[4];
	uint32_t freq = 0;

#ifdef ADV_DEBUG
	printf("SetRfFrequency ");
#endif

	if( ImageCalibrated == false )
	{
		CalibrateImage( frequency );
		ImageCalibrated = true;
	}

	freq = ( uint32_t )( ( double )frequency / ( double )FREQ_STEP );
	buf[0] = ( uint8_t )( ( freq >> 24 ) & 0xFF );
	buf[1] = ( uint8_t )( ( freq >> 16 ) & 0xFF );
	buf[2] = ( uint8_t )( ( freq >> 8 ) & 0xFF );
	buf[3] = ( uint8_t )( freq & 0xFF );
	WriteCommand( RADIO_SET_RFFREQUENCY, buf, 4 );
}

void SX126x::SetPacketType( RadioPacketTypes_t packetType )
{
#ifdef ADV_DEBUG
	printf("SetPacketType ");
#endif

	// Save packet type internally to avoid questioning the radio
	this->PacketType = packetType;
	WriteCommand( RADIO_SET_PACKETTYPE, ( uint8_t* )&packetType, 1 );
}

SX126x::RadioPacketTypes_t SX126x::GetPacketType( void )
{
	return this->PacketType;
}

void SX126x::SetTxParams(int8_t power, RadioRampTimes_t rampTime)
{
	uint8_t buf[2];

#ifdef ADV_DEBUG
	printf("SetTxParams ");
#endif

	if (DeviceType == SX1261) {
		if( power == 15 )
		{
			SetPaConfig( 0x06, 0x00, 0x01, 0x01 );
		}
		else
		{
			SetPaConfig( 0x04, 0x00, 0x01, 0x01 );
		}
		if( power >= 14 )
		{
			power = 14;
		}
		else if( power < -3 )
		{
			power = -3;
		}
		WriteReg( REG_OCP, 0x18 ); // current max is 80 mA for the whole device
	}
	else // sx1262 or sx1268
	{
		SetPaConfig( 0x04, 0x07, 0x00, 0x01 );
		if( power > 22 )
		{
			power = 22;
		}
		else if( power < -3 )
		{
			power = -3;
		}
		WriteReg( REG_OCP, 0x38 ); // current max 160mA for the whole device
	}
	buf[0] = power;
	buf[1] = (uint8_t)rampTime;

//	if( OPT == 0 )
//	{
//		if( ( uint8_t )rampTime < RADIO_RAMP_200_US )
//		{
//			buf[1] = RADIO_RAMP_200_US;
//		}
//		else
//		{
//			buf[1] = ( uint8_t )rampTime;
//		}
//	}
//	else
//	{
//		buf[1] = ( uint8_t )rampTime;
//	}

	WriteCommand( RADIO_SET_TXPARAMS, buf, 2 );
}

void SX126x::SetModulationParams(const ModulationParams_t& modulationParams )
{
	uint8_t n;
	uint32_t tempVal = 0;
	uint8_t buf[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#ifdef ADV_DEBUG
	printf("SetModulationParams ");
#endif

	// Check if required configuration corresponds to the stored packet type
	// If not, silently update radio packet type
	if( this->PacketType != modulationParams.PacketType )
	{
		this->SetPacketType( modulationParams.PacketType );
	}

	switch( modulationParams.PacketType )
	{
		case PACKET_TYPE_GFSK:
			n = 8;
			tempVal = ( uint32_t )( 32 * ( ( double )XTAL_FREQ / ( double )modulationParams.Params.Gfsk.BitRate ) );
			buf[0] = ( tempVal >> 16 ) & 0xFF;
			buf[1] = ( tempVal >> 8 ) & 0xFF;
			buf[2] = tempVal & 0xFF;
			buf[3] = modulationParams.Params.Gfsk.ModulationShaping;
			buf[4] = modulationParams.Params.Gfsk.Bandwidth;
			tempVal = ( uint32_t )( ( double )modulationParams.Params.Gfsk.Fdev / ( double )FREQ_STEP );
			buf[5] = ( tempVal >> 16 ) & 0xFF;
			buf[6] = ( tempVal >> 8 ) & 0xFF;
			buf[7] = ( tempVal& 0xFF );
			break;
		case PACKET_TYPE_LORA:
			n = 4;

			buf[0] = modulationParams.Params.LoRa.SpreadingFactor;
			buf[1] = modulationParams.Params.LoRa.Bandwidth;
			buf[2] = modulationParams.Params.LoRa.CodingRate;
			buf[3] = modulationParams.Params.LoRa.LowDatarateOptimize;

			// WORKAROUND - Modulation Quality with 500 kHz LoRa Bandwidth, see DS_SX1261-2_V1.2 datasheet chapter 15.1
			if(modulationParams.PacketType == RadioPacketTypes_t::PACKET_TYPE_LORA && modulationParams.Params.LoRa.Bandwidth == RadioLoRaBandwidths_t::LORA_BW_500 )
			{
				WriteReg(REG_TX_MODULATION, ReadReg(REG_TX_MODULATION) & ~(1 << 2));
			}
			else
			{
				WriteReg( REG_TX_MODULATION, ReadReg( REG_TX_MODULATION ) | (1 << 2));
			}
			break;
		default:
		case PACKET_TYPE_NONE:
			return;
	}
	WriteCommand( RADIO_SET_MODULATIONPARAMS, buf, n );

	CurrentModParams = modulationParams;
}

void SX126x::SetPacketParams(const PacketParams_t& packetParams )
{
	uint8_t n;
	uint8_t crcVal = 0;
	uint8_t buf[9] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

#ifdef ADV_DEBUG
	printf("SetPacketParams ");
#endif

	// Check if required configuration corresponds to the stored packet type
	// If not, silently update radio packet type
	if( this->PacketType != packetParams.PacketType )
	{
		this->SetPacketType( packetParams.PacketType );
	}

	switch( packetParams.PacketType )
	{
		case PACKET_TYPE_GFSK:
			if( packetParams.Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_IBM )
			{
				SetCrcSeed( CRC_IBM_SEED );
				SetCrcPolynomial( CRC_POLYNOMIAL_IBM );
				crcVal = RADIO_CRC_2_BYTES;
			}
			else if(  packetParams.Params.Gfsk.CrcLength == RADIO_CRC_2_BYTES_CCIT )
			{
				SetCrcSeed( CRC_CCITT_SEED );
				SetCrcPolynomial( CRC_POLYNOMIAL_CCITT );
				crcVal = RADIO_CRC_2_BYTES_INV;
			}
			else
			{
				crcVal = packetParams.Params.Gfsk.CrcLength;
			}
			n = 9;
			buf[0] = ( packetParams.Params.Gfsk.PreambleLength >> 8 ) & 0xFF;
			buf[1] = packetParams.Params.Gfsk.PreambleLength;
			buf[2] = packetParams.Params.Gfsk.PreambleMinDetect;
			buf[3] = ( packetParams.Params.Gfsk.SyncWordLength << 3 ); // convert from byte to bit
			buf[4] = packetParams.Params.Gfsk.AddrComp;
			buf[5] = packetParams.Params.Gfsk.HeaderType;
			buf[6] = packetParams.Params.Gfsk.PayloadLength;
			buf[7] = crcVal;
			buf[8] = packetParams.Params.Gfsk.DcFree;
			break;
		case PACKET_TYPE_LORA:
			n = 6;
			buf[0] = ( packetParams.Params.LoRa.PreambleLength >> 8 ) & 0xFF;
			buf[1] = packetParams.Params.LoRa.PreambleLength;
			buf[2] = packetParams.Params.LoRa.HeaderType;
			buf[3] = packetParams.Params.LoRa.PayloadLength;
			buf[4] = packetParams.Params.LoRa.CrcMode;
			buf[5] = packetParams.Params.LoRa.InvertIQ;

			// WORKAROUND - Optimizing the Inverted IQ Operation, see DS_SX1261-2_V1.2 datasheet chapter 15.4
			if(packetParams.Params.LoRa.InvertIQ == RadioLoRaIQModes_t::LORA_IQ_INVERTED)
			{
				WriteReg(REG_IQ_POLARITY, ReadReg(REG_IQ_POLARITY) & ~(1 << 2));
			}
			else
			{
				WriteReg(REG_IQ_POLARITY, ReadReg(REG_IQ_POLARITY) | ( 1 << 2 ));
			}
			break;
		default:
		case PACKET_TYPE_NONE:
			return;
	}
	WriteCommand( RADIO_SET_PACKETPARAMS, buf, n );

	CurrentPacketParams = packetParams;
}

void SX126x::SetCadParams( RadioLoRaCadSymbols_t cadSymbolNum, uint8_t cadDetPeak, uint8_t cadDetMin, RadioCadExitModes_t cadExitMode, uint32_t cadTimeout )
{
	uint8_t buf[7];

	buf[0] = ( uint8_t )cadSymbolNum;
	buf[1] = cadDetPeak;
	buf[2] = cadDetMin;
	buf[3] = ( uint8_t )cadExitMode;
	buf[4] = ( uint8_t )( ( cadTimeout >> 16 ) & 0xFF );
	buf[5] = ( uint8_t )( ( cadTimeout >> 8 ) & 0xFF );
	buf[6] = ( uint8_t )( cadTimeout & 0xFF );
	WriteCommand( RADIO_SET_CADPARAMS, buf, 7 );
	OperatingMode = MODE_CAD;
}

void SX126x::SetBufferBaseAddresses( uint8_t txBaseAddress, uint8_t rxBaseAddress )
{
	uint8_t buf[2];

#ifdef ADV_DEBUG
	printf("SetBufferBaseAddresses ");
#endif

	buf[0] = txBaseAddress;
	buf[1] = rxBaseAddress;
	WriteCommand( RADIO_SET_BUFFERBASEADDRESS, buf, 2 );
}

SX126x::RadioStatus_t SX126x::GetStatus( void )
{
	uint8_t stat = 0;
	RadioStatus_t status;

	ReadCommand( RADIO_GET_STATUS, ( uint8_t * )&stat, 1 );
	status.Value = stat;
	return status;
}

int8_t SX126x::GetRssiInst( void )
{
	uint8_t rssi;

	ReadCommand( RADIO_GET_RSSIINST, ( uint8_t* )&rssi, 1 );
	return( -( rssi / 2 ) );
}

void SX126x::GetRxBufferStatus( uint8_t *payloadLength, uint8_t *rxStartBufferPointer )
{
	uint8_t status[2];

	ReadCommand( RADIO_GET_RXBUFFERSTATUS, status, 2 );

	// In case of LORA fixed header, the payloadLength is obtained by reading
	// the register REG_LR_PAYLOADLENGTH
	if( ( this->GetPacketType( ) == PACKET_TYPE_LORA ) && ( ReadReg( REG_LR_PACKETPARAMS ) >> 7 == 1 ) )
	{
		*payloadLength = ReadReg( REG_LR_PAYLOADLENGTH );
	}
	else
	{
		*payloadLength = status[0];
	}
	*rxStartBufferPointer = status[1];
}

void SX126x::GetPacketStatus( PacketStatus_t *pktStatus )
{
	uint8_t status[3];

	ReadCommand( RADIO_GET_PACKETSTATUS, status, 3 );

	pktStatus->packetType = this -> GetPacketType( );
	switch( pktStatus->packetType )
	{
		case PACKET_TYPE_GFSK:
			pktStatus->Params.Gfsk.RxStatus = status[0];
			pktStatus->Params.Gfsk.RssiSync = -status[1] / 2;
			pktStatus->Params.Gfsk.RssiAvg = -status[2] / 2;
			pktStatus->Params.Gfsk.FreqError = 0;
			break;

		case PACKET_TYPE_LORA:
			pktStatus->Params.LoRa.RssiPkt = -status[0] / 2;
			( status[1] < 128 ) ? ( pktStatus->Params.LoRa.SnrPkt = status[1] / 4 ) : ( pktStatus->Params.LoRa.SnrPkt = ( ( status[1] - 256 ) /4 ) );
			pktStatus->Params.LoRa.SignalRssiPkt = -status[2] / 2;
			pktStatus->Params.LoRa.FreqError = FrequencyError;
			break;

		default:
		case PACKET_TYPE_NONE:
			// In that specific case, we set everything in the pktStatus to zeros
			// and reset the packet type accordingly
			memset( pktStatus, 0, sizeof( PacketStatus_t ) );
			pktStatus->packetType = PACKET_TYPE_NONE;
			break;
	}
}

SX126x::RadioError_t SX126x::GetDeviceErrors( void ) {
	RadioError_t error;

	ReadCommand( RADIO_GET_ERROR, ( uint8_t * )&error, 2 );
	return error;
}

void SX126x::ClearIrqStatus( uint16_t irq )
{
	uint8_t buf[2];
#ifdef ADV_DEBUG
	printf("ClearIrqStatus ");
#endif
	buf[0] = ( uint8_t )( ( ( uint16_t )irq >> 8 ) & 0x00FF );
	buf[1] = ( uint8_t )( ( uint16_t )irq & 0x00FF );
	WriteCommand( RADIO_CLR_IRQSTATUS, buf, 2 );
}

void SX126x::ProcessIrqs() {
	uint16_t irqRegs = GetIrqStatus( );
	ClearIrqStatus( IRQ_RADIO_ALL );

#ifdef ADV_DEBUG
	printf("0x%04x\n\r", irqRegs );
#endif

	auto& txDone = callbacks.txDone;
	auto& rxDone = callbacks.rxDone;
	auto& rxSyncWordDone = callbacks.rxSyncWordDone;
	auto& rxHeaderDone = callbacks.rxHeaderDone;
	auto& txTimeout = callbacks.txTimeout;
	auto& rxTimeout = callbacks.rxTimeout;
	auto& rxError = callbacks.rxError;
	auto& cadDone = callbacks.cadDone;


	if( ( irqRegs & IRQ_HEADER_VALID ) == IRQ_HEADER_VALID )
	{
		// LoRa Only
//		FrequencyError = 0x000000 | ( ( 0x0F & ReadReg( REG_FREQUENCY_ERRORBASEADDR ) ) << 16 );
//		FrequencyError = FrequencyError | ( ReadReg( REG_FREQUENCY_ERRORBASEADDR + 1 ) << 8 );
//		FrequencyError = FrequencyError | ( ReadReg( REG_FREQUENCY_ERRORBASEADDR + 2 ) );

		if (rxHeaderDone)
			rxHeaderDone();
	}

	if( ( irqRegs & IRQ_TX_DONE ) == IRQ_TX_DONE )
	{
		HalPostTx();
		if (txDone)
			txDone();

	}

	if( ( irqRegs & IRQ_RX_DONE ) == IRQ_RX_DONE )
	{
		if( ( irqRegs & IRQ_CRC_ERROR ) == IRQ_CRC_ERROR )
		{
			if (rxError)
				rxError(IRQ_CRC_ERROR_CODE);
		}
		else
		{
			if (rxDone)
				rxDone();

		}
	}

	if (( irqRegs & IRQ_SYNCWORD_VALID ) == IRQ_SYNCWORD_VALID )
	{
		if (rxSyncWordDone)
			rxSyncWordDone();
	}


	if( ( irqRegs & IRQ_CAD_DONE ) == IRQ_CAD_DONE ) {
		if (cadDone) {
			cadDone( ( irqRegs & IRQ_CAD_ACTIVITY_DETECTED ) == IRQ_CAD_ACTIVITY_DETECTED );
		}
	}

	if( ( irqRegs & IRQ_RX_TX_TIMEOUT ) == IRQ_RX_TX_TIMEOUT ) {
		if (OperatingMode == MODE_TX) {
			HalPostTx();

			if (txTimeout)
				txTimeout();
		} else if (OperatingMode == MODE_TX) {
			HalPostRx();

			if (rxTimeout)
				rxTimeout();
		}
	}

/*
    //IRQ_PREAMBLE_DETECTED                   = 0x0004,
    if( irqRegs & IRQ_PREAMBLE_DETECTED )
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_PBL_DETECT_CODE);

        }
    }

    //IRQ_SYNCWORD_VALID                      = 0x0008,
    if( irqRegs & IRQ_SYNCWORD_VALID )
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_SYNCWORD_VALID_CODE  );
        }
    }

    //IRQ_HEADER_VALID                        = 0x0010,
    if ( irqRegs & IRQ_HEADER_VALID )
    {
        if( rxPblSyncWordHeader != NULL )
        {
            rxPblSyncWordHeader( IRQ_HEADER_VALID_CODE );
        }
    }

    //IRQ_HEADER_ERROR                        = 0x0020,
    if( irqRegs & IRQ_HEADER_ERROR )
    {
        if( rxError != NULL )
        {
            rxError( IRQ_HEADER_ERROR_CODE );
        }
    }
*/
}

void SX126x::HalSpiRead(uint8_t *buffer_in, uint16_t size) {
	uint8_t useless[size];
	memset(useless, 0, size);
	HalSpiTransfer(buffer_in, useless, size);
}

void SX126x::HalSpiWrite(const uint8_t *buffer_out, uint16_t size) {
	uint8_t useless[size];
	HalSpiTransfer(useless, buffer_out, size);
}

void SX126x::SetNetworkType(SX126x::NetworkType_t nt) {
	if (nt == PUBLIC) {
		// Change LoRa modem Sync Word for Public Networks
		WriteReg(REG_LR_SYNCWORD, (LORA_MAC_PUBLIC_SYNCWORD >> 8) & 0xFF);
		WriteReg(REG_LR_SYNCWORD + 1, LORA_MAC_PUBLIC_SYNCWORD & 0xFF);
	} else {
		WriteReg(REG_LR_SYNCWORD, (LORA_MAC_PRIVATE_SYNCWORD >> 8) & 0xFF);
		WriteReg(REG_LR_SYNCWORD + 1, LORA_MAC_PRIVATE_SYNCWORD & 0xFF);
	}
}

void SX126x::Reset() {
	HalGpioWrite(GPIO_PIN_RESET, 0);
	sleep_ms(10);
	HalGpioWrite(GPIO_PIN_RESET, 1);
	sleep_ms(10);
}

void SX126x::Wakeup() {
	if (SX126x_DEBUG) {
		printf("SX126x: Wakeup\n");
	}

	uint8_t buf[2] = {RADIO_GET_STATUS, 0};

	HalSpiWrite(buf, 2);

	// Wait for chip to be ready.
	WaitOnBusyLong();

	if (SX126x_DEBUG) {
		printf("SX126x: Wakeup done\n");
	}
}

void SX126x::WriteCommand(SX126x::RadioCommands_t opcode, uint8_t *buffer, uint16_t size) {
	auto *merged_buf = (uint8_t *)alloca(size+1);

	merged_buf[0] = opcode;
	memcpy(merged_buf+1, buffer, size);

	if (SX126x_DEBUG) {
		printf("SX126x: WriteCommand: 0x%02x %u\n", opcode, size);
	}

	WaitOnBusy();

	HalSpiWrite(merged_buf, size+1);

	WaitOnCounter();
}

void SX126x::ReadCommand(SX126x::RadioCommands_t opcode, uint8_t *buffer, uint16_t size) {
	WaitOnBusy();

	auto total_transfer_size = 2+size;

	auto *buf_out = (uint8_t *)alloca(total_transfer_size);
	auto *buf_in = (uint8_t *)alloca(total_transfer_size);

	memset(buf_out, 0, total_transfer_size);
	buf_out[0] = opcode;

	HalSpiTransfer(buf_in, buf_out, total_transfer_size);
	memcpy(buffer, buf_in+2, size);
}

void SX126x::WriteRegister(uint16_t address, uint8_t *buffer, uint16_t size) {
	if (SX126x_DEBUG) {
		printf("SX126x: WriteRegister: 0x%04x %u\n", address, size);
	}

	WaitOnBusy();

	auto total_transfer_size = 3+size;
	auto *buf_out = (uint8_t *)alloca(total_transfer_size);

	buf_out[0] = RADIO_WRITE_REGISTER;
	buf_out[1] = ((address & 0xFF00) >> 8);
	buf_out[2] = (address & 0x00FF);
	memcpy(buf_out+3, buffer, size);

	HalSpiWrite(buf_out, total_transfer_size);

	if (SX126x_DEBUG) {
		printf("SX126x: WriteRegister: send done\n");
	}
}

void SX126x::WriteReg(uint16_t address, uint8_t value) {
	WriteRegister(address, &value, 1);
}

void SX126x::ReadRegister(uint16_t address, uint8_t *buffer, uint16_t size) {
	WaitOnBusy();

	auto total_transfer_size = 4+size;
	auto *buf_out = (uint8_t *)alloca(total_transfer_size);
	auto *buf_in = (uint8_t *)alloca(total_transfer_size);

	memset(buf_out, 0, total_transfer_size);
	buf_out[0] = RADIO_READ_REGISTER;
	buf_out[1] = ((address & 0xFF00) >> 8);
	buf_out[2] = (address & 0x00FF);

	HalSpiTransfer(buf_in, buf_out, total_transfer_size);

	memcpy(buffer, buf_in+4, size);
}

uint8_t SX126x::ReadReg(uint16_t address) {
	uint8_t data;

	ReadRegister( address, &data, 1 );
	return data;
}

void SX126x::WriteBuffer(uint8_t offset, uint8_t *buffer, uint8_t size) {
	WaitOnBusy();

	auto total_transfer_size = 2+size;
	auto *buf_out = (uint8_t *)alloca(total_transfer_size);

	buf_out[0] = RADIO_WRITE_BUFFER;
	buf_out[1] = offset;

	memcpy(buf_out+2, buffer, size);

	HalSpiWrite(buf_out, total_transfer_size);
}

void SX126x::ReadBuffer(uint8_t offset, uint8_t *buffer, uint8_t size) {
	WaitOnBusy();

	auto total_transfer_size = 3+size;
	auto *buf_out = (uint8_t *)alloca(total_transfer_size);
	auto *buf_in = (uint8_t *)alloca(total_transfer_size);

	memset(buf_out, 0, total_transfer_size);

	buf_out[0] = RADIO_READ_BUFFER;
	buf_out[1] = offset;

	HalSpiTransfer(buf_in, buf_out, total_transfer_size);

	memcpy(buffer, buf_in+3, size);
}

void SX126x::SetDeviceType(SX126x::DeviceType_t devtype) {
	DeviceType = devtype;
}

void SX126x::WaitOnBusyLong() {
	while (HalGpioRead(GPIO_PIN_BUSY)) {
		sleep_us(1);
	}
}

void SX126x::WaitOnBusy() {
	while (HalGpioRead(GPIO_PIN_BUSY)) {
		sleep_us(10);
	}
}

void SX126x::WaitOnCounter() {
	sleep_us(126);
}

uint32_t SX126x::GetTimeOnAir(const SX126x::ModulationParams_t &modparams, const SX126x::PacketParams_t &pktparams) {
	double ToA = 0;

	if (modparams.PacketType == PACKET_TYPE_LORA) {
		// Bandwidth (in kHz)
		uint32_t BW;
		switch (modparams.Params.LoRa.Bandwidth) {
			case LORA_BW_500:
				BW = 500;
				break;
			case LORA_BW_250:
				BW = 250;
				break;
			case LORA_BW_125:
				BW = 125;
				break;
			case LORA_BW_062:
				BW = 62;
				break;
			case LORA_BW_041:
				BW = 41;
				break;
			case LORA_BW_031:
				BW = 31;
				break;
			case LORA_BW_020:
				BW = 20;
				break;
			case LORA_BW_015:
				BW = 15;
				break;
			case LORA_BW_010:
				BW = 10;
				break;
			case LORA_BW_007:
				BW = 7;
				break;
			default:
				BW = 500;
				break;
		}

		// Spreading Factor (5 to 12)
		uint8_t SF = modparams.Params.LoRa.SpreadingFactor;

		// CR is 1, 2, 3 or 4 for respective coding rates 4/5, 4/6, 4/7 or 4/8
		uint8_t CR = modparams.Params.LoRa.CodingRate;

		uint8_t PayloadLength = pktparams.Params.LoRa.PayloadLength;
		uint8_t CrcBits = pktparams.Params.LoRa.CrcMode == LORA_CRC_ON ? 16 : 0;
		uint8_t HeaderSymbols = pktparams.Params.LoRa.HeaderType == LORA_PACKET_VARIABLE_LENGTH ? 20 : 0;

		double Symbols = pktparams.Params.LoRa.PreambleLength;

		if (SF == 5 || SF == 6) {
			Symbols += 6.25 + 8 + ceil((fmax(8*PayloadLength+CrcBits-4*SF+HeaderSymbols, 0)/(4*SF))) * (CR + 4);
		} else {
			if (modparams.Params.LoRa.LowDatarateOptimize) {
				Symbols += 4.25 + 8 + ceil((fmax(8*PayloadLength+CrcBits-4*SF+8+HeaderSymbols, 0)/(4*(SF-2)))) * (CR + 4);
			} else {
				Symbols += 4.25 + 8 + ceil((fmax(8*PayloadLength+CrcBits-4*SF+8+HeaderSymbols, 0)/(4*SF))) * (CR + 4);
			}
		}

		ToA = (pow(2, SF) / BW) * Symbols;

	} else if (modparams.PacketType == PACKET_TYPE_GFSK) {
		uint16_t BitCount = 0;

		BitCount = 4 + ( pktparams.Params.Gfsk.PreambleLength >> 4 ) * 4;              // preamble
		BitCount = BitCount + 8 + ( pktparams.Params.Gfsk.SyncWordLength >> 1 ) * 8;   // sync word
		BitCount = BitCount + ( ( pktparams.Params.Gfsk.HeaderType == RADIO_PACKET_VARIABLE_LENGTH ) ? 8 : 0 );
		BitCount = BitCount + pktparams.Params.Gfsk.PayloadLength * 8;
		BitCount = BitCount + ( pktparams.Params.Gfsk.CrcLength >> 4 ) * 8;

		ToA = (((double)BitCount * modparams.Params.Gfsk.BitRate) / (32 * 32));
	}

	return (uint32_t)ceil(ToA);
}

uint32_t SX126x::GetTimeOnAir() {
	return GetTimeOnAir(CurrentModParams, CurrentPacketParams);
}



