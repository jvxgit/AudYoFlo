#ifndef __CJVXSPNMULTIPLEXER_CONNECTOR_H__
#define __CJVXSPNMULTIPLEXER_CONNECTOR_H__

#include "common/CjvxSingleOutputConnector.h"

class CjvxSpNMultiplexer;

/*
 * One of the output connectors out#0 .. out#N-1 of the multiplexer. In contrast to the
 * base class, this connector is not the start of a secondary chain but is driven by the
 * multiplexer in the SAME chain as the input connector ("default").
 */
class CjvxSpNMultiplexerOutputConnector : public CjvxSingleOutputConnector
{
public:
	CjvxSpNMultiplexer* parent = nullptr;

	//! Position of connector, yields name out#<idx>
	jvxSize idx = 0;

	//! Channel bundle within the input channels which is forwarded on this connector
	jvxSize chanOffset = 0;
	jvxSize chanNum = 0;

	//! Connector was connected when the chain was connected. If not, it remains unconnected until the chain is disconnected
	jvxBool inChain = false;

	jvxBool isPrepared = false;
	jvxBool isStarted = false;

	//! Buffers of the successor were linked into the input buffer field on prepare
	jvxBool isLinked = false;

	//! process_start was successful and requires a process_stop
	jvxBool inCycle = false;

public:
	CjvxSpNMultiplexerOutputConnector(CjvxSpNMultiplexer* parentArg, jvxSize idxArg);

	jvxErrorType available_to_connect_ocon() override;

	jvxErrorType select_connect_ocon(IjvxConnectorBridge* obj, IjvxConnectionMaster* master,
		IjvxDataConnectionCommon* ass_connection_common, IjvxOutputConnector** replace_connector) override;
};

#endif
