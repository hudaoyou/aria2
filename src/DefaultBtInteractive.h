/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#ifndef _D_DEFAULT_BT_INTERACTIVE_H_
#define _D_DEFAULT_BT_INTERACTIVE_H_

#include "BtInteractive.h"
#include "Peer.h"
#include "BtContext.h"
#include "PieceStorage.h"
#include "PeerStorage.h"
#include "BtMessageReceiver.h"
#include "BtMessageDispatcher.h"
#include "BtRequestFactory.h"
#include "PeerConnection.h"
#include "BtRegistry.h"
#include "Logger.h"
#include "LogFactory.h"
#include "TimeA2.h"

class FloodingStat {
private:
  uint32_t chokeUnchokeCount;
  uint32_t keepAliveCount;
public:
  FloodingStat():chokeUnchokeCount(0), keepAliveCount(0) {}
  
  void incChokeUnchokeCount() {
    if(chokeUnchokeCount < UINT32_MAX) {
      chokeUnchokeCount++;
    }
  }

  void incKeepAliveCount() {
    if(keepAliveCount < UINT32_MAX) {
      keepAliveCount++;
    }
  }

  uint32_t getChokeUnchokeCount() const {
    return chokeUnchokeCount;
  }

  uint32_t getKeepAliveCount() const {
    return keepAliveCount;
  }

  void reset() {
    chokeUnchokeCount = 0;
    keepAliveCount = 0;
  }
};

class DefaultBtInteractive : public BtInteractive {
private:
  int32_t cuid;
  PeerHandle peer;
  BtContextHandle btContext;
  PeerStorageHandle peerStorage;
  PieceStorageHandle pieceStorage;
  BtMessageReceiverHandle btMessageReceiver;
  BtMessageDispatcherHandle dispatcher;
  BtRequestFactoryHandle btRequestFactory;
  PeerConnectionHandle peerConnection;
  const Logger* logger;
  uint32_t allowedFastSetSize;
  Time haveCheckPoint;
  Time keepAliveCheckPoint;
  Time floodingCheckPoint;
  FloodingStat floodingStat;
  const Option* option;

  static const uint32_t FLOODING_CHECK_INTERVAL = 5;

  void addBitfieldMessageToQueue();
  void addAllowedFastMessageToQueue();
  void decideChoking();
  void checkHave();
  void sendKeepAlive();
  void decideInterest();
  void fillPiece(int maxPieceNum);
  void addRequests();
  void detectMessageFlooding();
public:
  DefaultBtInteractive():peer(0),
			 btContext(0),
			 peerStorage(0),
			 pieceStorage(0),
			 btMessageReceiver(0),
			 dispatcher(0),
			 btRequestFactory(0),
			 peerConnection(0),
			 logger(LogFactory::getInstance()),
			 allowedFastSetSize(10)
  {
    logger->debug("DefaultBtInteractive instantiated.");
  }

  virtual ~DefaultBtInteractive() {
    logger->debug("DefaultBtInteractive deleted.");
  }

  virtual void initiateHandshake();

  virtual BtMessageHandle receiveHandshake(bool quickReply = false);

  virtual BtMessageHandle receiveAndSendHandshake();

  virtual void doPostHandshakeProcessing();

  virtual void doInteractionProcessing();

  virtual void cancelAllPiece();

  virtual void sendPendingMessage();

  void receiveMessages();

  virtual uint32_t countPendingMessage() {
    return dispatcher->countMessageInQueue();
  }
  
  virtual bool isSendingMessageInProgress() {
    return dispatcher->isSendingInProgress();
  }

  void setCuid(int32_t cuid) {
    this->cuid = cuid;
  }

  void setPeer(const PeerHandle& peer) {
    this->peer = peer;
  }

  void setBtContext(const BtContextHandle& btContext) {
    this->btContext = btContext;
    this->peerStorage = PEER_STORAGE(btContext);
    this->pieceStorage = PIECE_STORAGE(btContext);
  }

  void setBtMessageReceiver(const BtMessageReceiverHandle& receiver) {
    this->btMessageReceiver = receiver;
  }

  void setDispatcher(const BtMessageDispatcherHandle& dispatcher) {
    this->dispatcher = dispatcher;
  }

  void setBtRequestFactory(const BtRequestFactoryHandle& factory) {
    this->btRequestFactory = factory;
  }

  void setPeerConnection(const PeerConnectionHandle& peerConnection) {
    this->peerConnection  = peerConnection;
  }

  void setOption(const Option* option) {
    this->option = option;
  }
};

typedef SharedHandle<DefaultBtInteractive> DefaultBtInteractiveHandle;

#endif // _D_DEFAULT_BT_INTERACTIVE_H_
