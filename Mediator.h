#pragma once

#include <iostream>
#include <memory>

#include <unordered_map>
#include <vector>
#include <list>

#define MAX_MSGS_COUNT 10

namespace Mediator {
	// local time
	class LocalTime {
		static LocalTime *sharedIntance;
	protected:
		LocalTime() {
		}
	public:
		static LocalTime *getSI() {
			if (!sharedIntance)
				sharedIntance = new LocalTime();
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
	LocalTime *LocalTime::sharedIntance = nullptr;

	// message
	class Message {
		const std::string text;
		std::string lTime;
		int sUID;
		std::vector<int> rUIDs;
	public:
		Message(const std::string &aText, std::string aLTime = LocalTime::getSI() -> now()) : text(aText), lTime(aLTime), sUID(-1) {
		}
		void setSenderUID(int UID) {
			if (sUID < 0)
				sUID = UID;
		}
		int getSenderUID() {
			return sUID;
		}
		bool isEmpty() {
			return text.empty();
		}
		std::unique_ptr<Message> copy() {
			std::unique_ptr<Message> cpMsg(new Message(text, lTime));
			cpMsg -> setSenderUID(sUID);
			return cpMsg;
		}
		friend std::ostream &operator<<(std::ostream &os, const Message &msg);
	};
	std::ostream &operator<<(std::ostream &os, const Message &msg) {
		os << "[" + msg.lTime + "] : " << msg.text;
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
		if (chatWindow.pMsgs.empty()) {
			os << "Empty" << std::endl;
			return os;
		}
		for (auto &pMsg : chatWindow.pMsgs)
			os << *pMsg << std::endl;
		return os;
	}	

	// client
	class ChatSession;
	class Client {
		static int UIDs;
		const int UID;
		const std::string name;
		std::shared_ptr<ChatWindow> pChatWindow;
	public:
		Client(const std::string &aName) : UID(UIDs++), name(aName), pChatWindow(std::make_shared<ChatWindow>()) {
		}
		void postMsg(std::unique_ptr<Message> &pMsg) {
			pMsg -> setSenderUID(UID);
			pChatWindow -> postMsg(pMsg);
		}
		friend std::ostream &operator<<(std::ostream &os, const Client &client);
		friend class ChatSession;
	};
	int Client::UIDs = 0;
	std::ostream &operator<<(std::ostream &os, const Client &client) {
		os << "UID : " << client.UID << "  Name : " << client.name << std::endl;
		os << *(client.pChatWindow);
		return os;
	}

	// chat session
	class ChatSession : public std::enable_shared_from_this<ChatSession> {
		std::unordered_map<int, std::weak_ptr<ChatWindow>> pChatWindows;
	public:
		void registerClient(std::unique_ptr<Client> &pClient) {
			pChatWindows[pClient -> UID] = pClient -> pChatWindow;

			std::shared_ptr<ChatSession> sh = shared_from_this();
			pClient -> pChatWindow -> setChatSession(sh);
		}
		void postMsg(std::unique_ptr<Message> &pMsg) {
			if (!pMsg || pMsg -> isEmpty())
				return;
			int sUID = pMsg -> getSenderUID();
			for (auto &t : pChatWindows)
				if (auto pChatWindow = t.second.lock()) {
					int UID = t.first;
					if (sUID != UID) {
						std::cout << sUID << "  " << UID << std::endl;
						std::unique_ptr<Message> cpMsg = pMsg -> copy();
						pChatWindow -> pushMsg(cpMsg);
					}
				}
		}
		friend std::ostream &operator<<(std::ostream &os, const ChatSession &chatSession);
	};
	std::ostream &operator<<(std::ostream &os, const ChatSession &chatSession) {
		for (auto &t : chatSession.pChatWindows)
			if (auto pChatWindow = t.second.lock()) {
				os << "UID : " << t.first << std::endl;
				os << *pChatWindow;
			}
		return os;
	}

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
		pMsgs.push_front(std::move(pMsg));
		if (pMsgs.size() > MAX_MSGS_COUNT)
			pMsgs.pop_back();
	}
	void ChatWindow::setChatSession(std::weak_ptr<ChatSession> ppChatSession) {
		pChatSession = ppChatSession;
	}

	void TestSuite() {
		std::unique_ptr<Client> pClient1(new Client("Andrii"));
		std::unique_ptr<Client> pClient2(new Client("Olga"));		

		std::shared_ptr<ChatSession> pChatSession(new ChatSession());
		pChatSession -> registerClient(pClient1);
		pChatSession -> registerClient(pClient2);

		std::cout << *pClient1 << std::endl;
		std::cout << *pClient2 << std::endl;

		std::string text = "Hello, Olga!";
		std::unique_ptr<Message> pMsg(new Message(text));
		pClient1 -> postMsg(pMsg);

		std::cout << *pClient1 << std::endl;
		std::cout << *pClient2 << std::endl;
	}
}
























