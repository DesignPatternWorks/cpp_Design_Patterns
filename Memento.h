#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace Memento {
	template <class TMemento>
	class MementoAPI {
	public:
		virtual std::unique_ptr<TMemento> createMemento() = 0;
		virtual void reinstate(const std::unique_ptr<TMemento> &pMemento) = 0;
	};

	class Tab;
	class TabMemento {
		friend class Tab;
		const std::string pageName;
		TabMemento() {	
		}
	public:
		TabMemento(const std::string &aPageName) : pageName(aPageName) {
			// std::cout << "Creating memento...[" + pageName + "]" << std::endl;
		}

		friend std::ostream &operator<<(std::ostream &os, const TabMemento &tMemento);	
	};
	std::ostream &operator<<(std::ostream &os, const TabMemento &tMemento){
		os << tMemento.pageName;
		return os;
	}

	class TabAPI {
	public:
		virtual void goToPage(const std::string &pageName) = 0;
	};

	class Tab : public TabAPI, public MementoAPI<TabMemento> {
		std::string curPageName;
		static const std::string HOME_PAGA_NAME;
	public:
		Tab() {
			goToPage(HOME_PAGA_NAME);
		}

		virtual void goToPage(const std::string &pageName) {
			std::cout << "Going to page...[" + pageName + "]" << std::endl;
			curPageName = pageName;
		}

		virtual std::unique_ptr<TabMemento> createMemento() {
			std::unique_ptr<TabMemento> pMemento(new TabMemento(curPageName));
			return std::move(pMemento);
		}

		virtual void reinstate(const std::unique_ptr<TabMemento> &pMemento) {
			goToPage(pMemento -> pageName);
		}
	};
	const std::string Tab::HOME_PAGA_NAME = "google.com";

	class TabManager : public TabAPI {
		std::unique_ptr<Tab> tab;
		std::vector<std::unique_ptr<TabMemento>> undoHistory;
		std::vector<std::unique_ptr<TabMemento>> redoHistory;

		TabManager() {
		}
	public:
		TabManager(std::unique_ptr<Tab> &pTab) : tab(std::move(pTab)) {
			std::unique_ptr<TabMemento> pMemento = tab -> createMemento();
			undoHistory.push_back(std::move(pMemento));
		}

		virtual void goToPage(const std::string &pageName) {
			redoHistory.clear();
			tab -> goToPage(pageName);

			std::unique_ptr<TabMemento> pMemento = tab -> createMemento();
			undoHistory.push_back(std::move(pMemento));
		}
		
		void goBackward() {
			if (undoHistory.size() <= 1)
				return;

			redoHistory.push_back(std::move(undoHistory.back()));
			undoHistory.pop_back();
			tab -> reinstate(undoHistory.back());
		}

		void goForward() {
			if (redoHistory.empty())
				return;

			tab -> reinstate(redoHistory.back());
			undoHistory.push_back(std::move(redoHistory.back()));
			redoHistory.pop_back();
		}

		void showHistory() {
			std::cout << std::endl;
			std::cout << "*** Begin ***" << std::endl; 
			std::cout << "Undo : " << std::endl;
			for (int i = 0; i < undoHistory.size(); ++i) {
				const auto &pMemento = undoHistory[i];
				std::cout << "[" << *pMemento << "]";
				if (i == undoHistory.size() - 1)
					std::cout << "- current";	
				std::cout << std::endl;
			}
			std::cout << "Redo : " << std::endl;
			for (int i = redoHistory.size() - 1; i >= 0; --i) {
				const auto &pMemento = redoHistory[i];
				std::cout << "[" << *pMemento << "]";
				std::cout << std::endl;
			}
			std::cout << "*** End ***" << std::endl;
			std::cout << std::endl;
		}
	};

	void TestSuite() {
		std::unique_ptr<Tab> pTab(new Tab());
		TabManager tabManager(pTab);
		tabManager.goToPage("mail.google.com");
		tabManager.goToPage("maps.google.com");
		tabManager.goToPage("earth.google.com");
		tabManager.showHistory();

		tabManager.goBackward();
		tabManager.goBackward();
		tabManager.showHistory();

		tabManager.goForward();
		tabManager.showHistory();

		tabManager.goToPage("test.google.com");
		tabManager.showHistory();
	}
}