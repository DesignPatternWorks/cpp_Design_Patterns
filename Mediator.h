#pragma once

#include <iostream>
#include <memory>

#include <unordered_map>
#include <vector>
#include <list>

#define MAX_MSGS_COUNT 10

namespace Mediator {
	// clients DB
	class ClientsDB {
		std::unordered_map<int, const std::string> records =  {
			{0, "Andrii"}, 
			{1, "Olga"}, 
			{2, "Rafael"}
		};
		static ClientsDB *sharedInstance;
	public:
		static ClientsDB *getSI() {
			if (!sharedInstance)
				sharedInstance = new ClientsDB();
			return sharedInstance;
		}
		const std::unordered_map<int, const std::string> &clients() {
			return records;
		}
	};
	ClientsDB *ClientsDB::sharedInstance = nullptr;

	// local time
	class Time {
		static Time *sharedIntance;
	protected:
		Time() {
		}
	public:
		static Time *getSI() {
			if (!sharedIntance)
				sharedIntance = new Time();
			return sharedIntance;
		}
		const std::string now() {
			time_t rawTime;
			struct tm *timeInfo;
			time(&rawTime);
			timeInfo = localtime(&rawTime);
			std::string localTime = (std::string)asctime(timeInfo);
			if (!localTime.empty() && localTime.back() == '\n')
	    		localTime.pop_back();
	 		return localTime;
		}
	};
	Time *Time::sharedIntance = nullptr;

	// message
	class Message {
		std::string time;
		const std::string text;
		int fromUID;
		std::vector<int> toUIDs;
	public:
		Message(const std::string &aText, int aFromUID = -1, const std::vector<int> &aToUIDs = {}) : text(aText), fromUID(aFromUID), toUIDs(aToUIDs){
		}
		void setTime(const std::string &aTime) {
			if (time.empty())
				time = aTime;
		}
		const std::string &getTime() {
			return time;
		}
		void setFromUID(int UID) {
			if (fromUID < 0 && UID >= 0)
				fromUID = UID;
		}
		int getFromUID() {
			return fromUID;
		}
		void setToUIDs(const std::vector<int> &aToUIDs) {
			if (toUIDs.empty())
				toUIDs = aToUIDs;
		}
		const std::vector<int> & getToUIDs() {
			return toUIDs;
		}
		bool isEmpty() {
			return text.empty();
		}
		std::unique_ptr<Message> copy() {
			std::unique_ptr<Message> cpMsg(new Message(text, fromUID));
			cpMsg -> setTime(time);
			return cpMsg;
		}
		friend std::ostream &operator<<(std::ostream &os, const Message &msg);
	};
	std::ostream &operator<<(std::ostream &os, const Message &msg) {
		auto &clients = ClientsDB::getSI() -> clients();
		const std::string &name = clients.at(msg.fromUID);
		os << name <<  " wrote [" + msg.time + "] : " << msg.text;
		return os;
	}

	// chat window (interface)
	class ChatSession;
	class ChatWindow {
		std::list<std::unique_ptr<Message>> pMsgs;
		std::weak_ptr<ChatSession> pChatSession;
	public:
		void postMsg(std::unique_ptr<Message> &pMsg);
		void pushMsg(std::unique_ptr<Message> &pMsg);
		void setChatSession(std::weak_ptr<ChatSession> ppChatSession);
		friend std::ostream &operator<<(std::ostream &os, const ChatWindow &chatWindow);
	};
	std::ostream &operator<<(std::ostream &os, const ChatWindow &chatWindow) {
		os << "Chat window contents : " << std::endl;
		if (!chatWindow.pMsgs.empty())
			for (auto &pMsg : chatWindow.pMsgs)
				os << *pMsg << std::endl;
		else
			os << "Empty" << std::endl;
		return os;
	}	

	// client
	class ChatSession;
	class Client {
		const int UID;
		std::shared_ptr<ChatWindow> pChatWindow;
	public:
		Client(int aUID, const std::string &aName) : UID(aUID), pChatWindow(std::make_shared<ChatWindow>()) {
		}
		void postMsg(std::unique_ptr<Message> &pMsg) {
			pChatWindow -> postMsg(pMsg);
		}
		friend std::ostream &operator<<(std::ostream &os, const Client &client);
		friend class ChatSession;
	};
	std::ostream &operator<<(std::ostream &os, const Client &client) {
		auto &clients = ClientsDB::getSI() -> clients();
		const std::string &name = clients.at(client.UID);
		os << "UID : " << client.UID << "  Name : " << name << std::endl;
		os << *(client.pChatWindow);
		return os;
	}

	// chat session
	class ChatSession : public std::enable_shared_from_this<ChatSession> {
		std::unordered_map<int, std::weak_ptr<ChatWindow>> pChatWindows;
	public:
		void registerClient(const std::unique_ptr<Client> &pClient) {
			pChatWindows[pClient -> UID] = pClient -> pChatWindow;

			std::shared_ptr<ChatSession> sh = shared_from_this();
			pClient -> pChatWindow -> setChatSession(sh);
		}
		void postMsg(std::unique_ptr<Message> &pMsg) {
			if (!pMsg || pMsg -> isEmpty())
				return;
			int fromUID = pMsg -> getFromUID();
			for (auto &t : pChatWindows)
				if (auto pChatWindow = t.second.lock()) {
					int UID = t.first;
					if (fromUID != UID) {
						std::unique_ptr<Message> cpMsg = pMsg -> copy();
						pChatWindow -> pushMsg(cpMsg);
					}
				}
		}
	};

	// chat window (implementation)
	void ChatWindow::postMsg(std::unique_ptr<Message> &pMsg) {
		if (!pMsg || pMsg -> isEmpty())
			return;
		std::unique_ptr<Message> cpMsg = pMsg -> copy();
		pushMsg(pMsg);
		if (auto ppChatSession = pChatSession.lock())
			ppChatSession -> postMsg(cpMsg);
	}
	void ChatWindow::pushMsg(std::unique_ptr<Message> &pMsg) {
		if (!pMsg || pMsg -> isEmpty())
			return;
		pMsg -> setTime(Time::getSI() -> now());
		pMsgs.push_back(std::move(pMsg));
		if (!pMsgs.empty() && pMsgs.size() > MAX_MSGS_COUNT)
			pMsgs.pop_front();
	}
	void ChatWindow::setChatSession(std::weak_ptr<ChatSession> ppChatSession) {
		pChatSession = ppChatSession;
	}

	// chat server
	class ChatServer {
		std::shared_ptr<ChatSession> pChatSession;
		std::unordered_map<int, std::unique_ptr<Client>> clientsMap;
	public:
		ChatServer() : pChatSession(std::make_shared<ChatSession>()) {
			auto &clients = ClientsDB::getSI() -> clients();
			for (const auto &t : clients) {
				std::unique_ptr<Client> pClient(new Client(t.first, t.second));
				clientsMap[t.first] = std::move(pClient);
			}
			for (const auto &t : clientsMap)
				pChatSession -> registerClient(t.second);
		}
		void postMsg(std::unique_ptr<Message> &pMsg, bool shouldPrint = false) {
			int fromUID = pMsg -> getFromUID();
			clientsMap.at(fromUID) -> postMsg(pMsg);
			// push notifications to recipients 
			// to inform them to retrieve the new messages 
			// from their chat windows on the server

			if (shouldPrint)
				for (const auto &t : clientsMap)
					std::cout << *(t.second) << std::endl;
		}
	};

	void TestSuite() {
		std::unique_ptr<ChatServer> pChatServer(new ChatServer());

		// the message comes in form the client
		std::unique_ptr<Message> pMsg0(new Message("Hello, guys!", 0));
		pChatServer -> postMsg(pMsg0);

		// the message comes in form the client
		std::unique_ptr<Message> pMsg1(new Message("Hello, amigo!", 2));
		pChatServer -> postMsg(pMsg1);

		// the message comes in form the client
		std::unique_ptr<Message> pMsg2(new Message("Hello, my dear husband!", 1));
		pChatServer -> postMsg(pMsg2);

		// the message comes in form the client
		std::unique_ptr<Message> pMsg3(new Message("Let's go for some coffee!", 2));
		pChatServer -> postMsg(pMsg3, true);
	}
}
























