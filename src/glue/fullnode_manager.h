/*
bumo is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

bumo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with bumo.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef FULLNODE_MANAGER_H_
#define FULLNODE_MANAGER_H_

#include <utils/headers.h>
#include <ledger/environment.h>
#include <common/private_key.h>
#include <common/network.h>
#include "fullnode.h"


namespace bumo {

	typedef std::shared_ptr<FullNode> FullNodePointer;

	class FullNodeManager : 
		public utils::Singleton<bumo::FullNodeManager>,
		public StatusModule,
		public TimerNotify,
		public utils::Runnable{
	private:
		std::map<std::string, FullNodePointer> full_node_info_;
		std::vector<std::string> sorted_full_nodes_;
		int64_t last_inspect_seq_;
		int64_t random_seq_;
		int64_t last_on_inspect_seq_;
		int64_t fullnode_check_timer_;
		std::string local_address_;
		PrivateKey priv_key_;
		utils::Thread *thread_ptr_;    /* The pointer of the thread */
		
	public:
		FullNodeManager();
		~FullNodeManager();

		virtual void GetModuleStatus(Json::Value &data);
		virtual void OnTimer(int64_t current_time);
		virtual void OnSlowTimer(int64_t current_time);
		virtual void Run(utils::Thread *this_thread);

		bool Initialize();
		bool Exit();

		FullNodePointer get(const std::string& key);
		bool add(FullNode& fn);
		bool add(FullNodePointer fp);
		void remove(const std::string& key);
		bool update(FullNodePointer fp);
		
		// called in LedgerManager::CloseLedger after AtomMap commit
		void getFullNode(const std::string& addr, Json::Value& node);
		bool setFullNode(Json::Value& node, const std::string& operation, std::shared_ptr<WRITE_BATCH> batch);
		bool updateDb(std::shared_ptr<WRITE_BATCH> batch);

		// reload or update full nodes list from db
		bool loadAllFullNode();

		// head of 1/1000 check tail of 1/1000
		bool isInspector(const std::string& addr);
		bool isUnderInspection(const std::string& addr);
		bool getPeerAddr(const std::string& addr, std::string& peer);

		// sorted by latest block hash
		bool sortFullNode(const std::string& blockHash);

		// receiver check authority of sender  
		bool verifyCheckAuthority(const std::string& checkerAddr, const std::string& beCheckedAddr);
		
		// do full node check 
		void inspect();
		bool OnInspectResponse(protocol::WsMessage &msg, int64_t conn_id);
		bool OnInspect(protocol::WsMessage &msg, int64_t conn_id);

		// impeach inactive or out-sync full node
		bool impeach(const std::string& impeach_addr, const std::string& reason);
		
		// unImpeach active full node
		bool unimpeach(const std::string& address, const std::string& unimpeach_addr);

		// reward the top one of sorted full nodes list with 10% of block reward and tx fee
		bool reward(std::shared_ptr<Environment> env, int64_t fullnode_reward);
	};
}
#endif
