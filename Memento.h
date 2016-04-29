#pragma once

#include <iostream>
#include <memory>
#include <vector>

namespace Memento_ {
	// memento API
	template <class Memento>
	class MementoAPI {
	public:
		virtual std::unique_ptr<Memento> createMemento() = 0;
		virtual void restoreMemento(const std::unique_ptr<Memento> &pMemento) = 0;
	};

	// tab memento
	class Tab;
	class TabMemento {
		const std::string pageName;
		TabMemento() {	
		}
	public:
		TabMemento(const std::string &aPageName) : pageName(aPageName) {
			// std::cout << "Creating memento...[" + pageName + "]" << std::endl;
		}
		friend class Tab;
		friend std::ostream &operator<<(std::ostream &os, const TabMemento &tMemento);	
	};
	std::ostream &operator<<(std::ostream &os, const TabMemento &tMemento){
		os << tMemento.pageName;
		return os;
	}

	// tab API
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
		virtual void restoreMemento(const std::unique_ptr<TabMemento> &pMemento) {
			goToPage(pMemento -> pageName);
		}
	};
	const std::string Tab::HOME_PAGA_NAME = "google.com";

	template <class Memento>
	class MementoManager {
		std::vector<std::unique_ptr<Memento>> undoRepo;
		std::vector<std::unique_ptr<Memento>> redoRepo;
	protected:
		std::unique_ptr<MementoAPI<Memento>> pTarget;
		MementoManager() : pTarget(nullptr) {
		}
	public:
		MementoManager(std::unique_ptr<MementoAPI<Memento>> ppTarget) : pTarget(std::move(ppTarget)) {
			save();
		}
		virtual ~MementoManager() {
		}
		void save() {
			redoRepo.clear();
			std::unique_ptr<Memento> pMemento = pTarget -> createMemento();
			undoRepo.push_back(std::move(pMemento));
		}	
		void undo() {
			if (undoRepo.size() <= 1)
				return;
			redoRepo.push_back(std::move(undoRepo.back()));
			undoRepo.pop_back();
			pTarget -> restoreMemento(undoRepo.back());
		}
		void redo() {
			if (redoRepo.empty())
				return;
			pTarget -> restoreMemento(redoRepo.back());
			undoRepo.push_back(std::move(redoRepo.back()));
			redoRepo.pop_back();
		}
		void show() {
			std::cout << std::endl;
			std::cout << "*** Begin ***" << std::endl; 
			std::cout << "Undo : " << std::endl;
			for (int i = 0; i < undoRepo.size(); ++i) {
				const auto &pMemento = undoRepo[i];
				std::cout << "[" << *pMemento << "]";
				if (i == undoRepo.size() - 1)
					std::cout << "- current";	
				std::cout << std::endl;
			}
			std::cout << "Redo : " << std::endl;
			for (int i = redoRepo.size() - 1; i >= 0; --i) {
				const auto &pMemento = redoRepo[i];
				std::cout << "[" << *pMemento << "]";
				std::cout << std::endl;
			}
			std::cout << "*** End ***" << std::endl;
			std::cout << std::endl;
		}
	};

	class TabManager : public TabAPI, public MementoManager<TabMemento> {
		TabManager() : MementoManager<TabMemento>() {
		}
	public:
		TabManager(std::unique_ptr<Tab> ppTab) : MementoManager<TabMemento>(std::move(ppTab)) {
		}
		virtual ~TabManager() {
			std::unique_ptr<Tab> pTab(static_cast<Tab *>(pTarget.release()));
		}
		virtual void goToPage(const std::string &pageName) {
			std::unique_ptr<Tab> pTab(static_cast<Tab *>(pTarget.release()));
			pTab -> goToPage(pageName);
			pTarget = std::move(pTab);
			save();
		}
		void goBackward() {
			undo();
		}
		void goForward() {
			redo();
		}
		void showHistory() {
			show();
		}
	};

	void TestSuite() {
		std::unique_ptr<Tab> pTab(new Tab());
		TabManager tabManager(std::move(pTab));

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