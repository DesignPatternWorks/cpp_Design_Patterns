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
		const std::string getSpec() {
			return spec;
		}
	};
	class Human {
	public:
		// TODO:
		std::shared_ptr<Role> pRole;
		Human(std::shared_ptr<Role> ppRole) : pRole(ppRole) {
		}
	};

	class Patient : public Role {
	public:
		Patient(const std::string &aCompliant) : Role(aCompliant) {
		}
	};
	class Handler {
	protected:
		std::weak_ptr<Handler> pNext;
	public:
		Handler(std::weak_ptr<Handler> pHandler) : pNext(pHandler) {
		}
		virtual bool handle(const std::unique_ptr<Human> &pPnt) {
			if (std::shared_ptr<Handler> ppNext = pNext.lock())
				return ppNext -> handle(pPnt);
			return false;
		}	
	};
	class Doctor : public Role, public Handler {
	public:
		Doctor(const std::string &anExpertise, std::weak_ptr<Doctor> pNextDtr) : Role(anExpertise), Handler(pNextDtr) {
		}
		// virtual ~Doctor() {
		// 	std::unique_ptr<Doctor> pNextDtr(static_cast<Doctor *>(pNext.release()));
		// }
		virtual bool handle(const std::unique_ptr<Human> &pPnt) {
			bool isHandled = false;
			std::cout << "Doctor [" << getSpec() << "] is examining..." << std::endl;

			std::shared_ptr<Patient> pRole = std::dynamic_pointer_cast<Patient>(pPnt -> pRole);
			if (getSpec().find(pRole -> getSpec()) != std::string::npos) {
				std::cout << "Diagnosed." << std::endl;
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
			std::list<std::string> expertises =  {
				"Dentists: tooth", 
				"Dermatologists‎: skin",
				"Cardiologists‎: heart",
				"Psychiatrists: mind"
			};
			for (auto &expertise : expertises) {
				std::shared_ptr<Doctor> ppRole;
				if (!pDtrs.empty())
					ppRole = std::dynamic_pointer_cast<Doctor>(pDtrs.front() -> pRole);

				// TODO:
				std::shared_ptr<Doctor> pRole(new Doctor(expertise, ppRole));
				std::unique_ptr<Human> pDtr(new Human(pRole));

				pDtrs.push_front(std::move(pDtr));
			}
		}
		void examine(const std::unique_ptr<Human> &pPnt) {
			std::shared_ptr<Doctor> ppRole;
			if (!pDtrs.empty())
				ppRole = std::dynamic_pointer_cast<Doctor>(pDtrs.front() -> pRole);
			if (ppRole)
				ppRole -> handle(pPnt);
		}
	};

	void TestSuite() {
		std::unique_ptr<Hospital> pHospital(new Hospital());

		// TODO:
		std::shared_ptr<Patient> pRole1(new Patient("heart"));
		std::unique_ptr<Human> pPatient1(new Human(pRole1));
		pHospital -> examine(pPatient1);

		// TODO:
		std::shared_ptr<Patient> pRole2(new Patient("skin"));
		std::unique_ptr<Human> pPatient2(new Human(pRole2));
		pHospital -> examine(pPatient2);
	}
}
