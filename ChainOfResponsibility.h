#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include <list>

namespace ChainOfResponsibility {
	class Role {
		std::string spec;
	public:
		Role(const std::string &aSpec) : spec(aSpec) {
		}
		virtual ~Role() {
		}
		const std::string &getSpec() const {
			return spec;
		}
	};
	class Human {
		std::shared_ptr<Role> pRole;
	public:
		Human(std::unique_ptr<Role> &ppRole) : pRole(std::move(ppRole)) {
		}
		std::shared_ptr<const Role> getRole() {
			return pRole;
		}
	};

	class Patient : public Role {
	public:
		Patient(const std::string &aCompliant) : Role(aCompliant) {
		}
	};
	class Handler {
	protected:
		std::weak_ptr<const Handler> pNext;
	public:
		Handler(std::weak_ptr<const Handler> pHandler) : pNext(pHandler) {
		}
		virtual ~Handler() {
		}
		virtual bool handle(const std::unique_ptr<Human> &pPnt) const {
			if (std::shared_ptr<const Handler> ppNext = pNext.lock())
				return ppNext -> handle(pPnt);
			return false;
		}	
	};
	class Doctor : public Role, public Handler {
	public:
		Doctor(const std::string &anExpertise, std::weak_ptr<const Doctor> pNextDtr) : Role(anExpertise), Handler(pNextDtr) {
		}
		virtual bool handle(const std::unique_ptr<Human> &pPnt) const {
			bool isHandled = false;
			std::cout << "Doctor [" << getSpec() << "] is examining..." << std::endl;

			std::shared_ptr<const Role> ppRole = pPnt -> getRole();
			std::shared_ptr<const Patient> pRole = std::dynamic_pointer_cast<const Patient>(ppRole);
			if (getSpec().find(pRole -> getSpec()) != std::string::npos) {
				std::cout << "Diagnosed by [" << getSpec() << "]" << std::endl;
				isHandled = true;
			}

			if (!isHandled)
				isHandled = Handler::handle(pPnt);
			return isHandled;
		}	
	};

	class Hospital {
		std::list<std::unique_ptr<Human>> pDtrs;
	public:
		Hospital() {
			std::vector<std::string> expertises =  {
				"Dentists: tooth", 
				"Dermatologists‎: skin",
				"Cardiologists‎: heart",
				"Psychiatrists: mind"
			};
			for (auto &expertise : expertises) {
				std::shared_ptr<const Doctor> ppRole;
				if (!pDtrs.empty()) {
					std::shared_ptr<const Role> pppRole = pDtrs.front() -> getRole();
					ppRole = std::dynamic_pointer_cast<const Doctor>(pppRole);
				}

				std::unique_ptr<Role> pRole(new Doctor(expertise, ppRole));
				std::unique_ptr<Human> pDtr(new Human(pRole));

				pDtrs.push_front(std::move(pDtr));
			}
		}
		void examine(const std::unique_ptr<Human> &pPnt) {
			if (!pDtrs.empty()) {
				std::shared_ptr<const Patient> pRole = std::dynamic_pointer_cast<const Patient>(pPnt -> getRole());
				std::cout << "Examining patient with complait [" << pRole -> getSpec() << "]..." << std::endl;

				std::shared_ptr<const Role> ppRole = pDtrs.front() -> getRole();
				std::shared_ptr<const Doctor> pppRole = std::dynamic_pointer_cast<const Doctor>(ppRole);
				pppRole -> handle(pPnt);
			}
		}
	};

	void TestSuite() {
		std::unique_ptr<Hospital> pHospital(new Hospital());
		
		std::unique_ptr<Role> pRole1(new Patient("heart"));
		std::unique_ptr<Human> pPatient1(new Human(pRole1));
		pHospital -> examine(pPatient1);

		std::unique_ptr<Role> pRole2(new Patient("skin"));
		std::unique_ptr<Human> pPatient2(new Human(pRole2));
		pHospital -> examine(pPatient2);
	}
}
