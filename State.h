#pragma once

#include <string>
#include <unordered_map>

namespace State {
	// context
	class Process {
		class ProcessState *state;
	public:
		Process();
		~Process();
		std::string stateDescription();
		void setState(ProcessState *aState);
		void addmit();
		void interrupt();
		void dispatch();
		void exit();
	};
	
	// state
	class ProcessState {
	public:
		virtual ~ProcessState() {
		};
		virtual std::string description() = 0;
		virtual void addmit(Process *process) {
			std::cout << "Cannot Addmit" << std::endl;
		}
		virtual void interrupt(Process *process) {
			std::cout << "Cannot Interrupt" << std::endl;
		}
		virtual void dispatch(Process *process) {
			std::cout << "Cannot Dispatch" << std::endl;
		}
		virtual void exit(Process *process) {
			std::cout << "Cannot Exit" << std::endl;
		}
	};
	class New : public ProcessState {
	public:
		virtual std::string description() {
			return "New";
		}
		virtual void exit(Process *process);
		virtual void addmit(Process *process);
	};
	class Ready : public ProcessState {
	public:
		virtual std::string description() {
			return "Ready";
		}
		virtual void exit(Process *process);
		virtual void addmit(Process *process);
		virtual void dispatch(Process *process);
	};
	class Running : public ProcessState {
	public:
		virtual std::string description() {
			return "Running";
		}
		virtual void exit(Process *process);
		virtual void interrupt(Process *process);
	};
	class Terminated : public ProcessState {
	public:
		virtual std::string description() {
			return "Terminated";
		}
		virtual void exit(Process *process);
	};
	
	// state definition
	void New::exit(Process *process) {
		std::cout << "New -> Terminated" << std::endl;
		process -> setState(new Terminated());
	}
	void New::addmit(Process *process) {
		std::cout << "New -> Ready" << std::endl;
		process -> setState(new Ready());
	}
	void Ready::exit(Process *process) {
		std::cout << "Ready -> Terminated" << std::endl;
		process -> setState(new Terminated());
	}
	void Ready::addmit(Process *process) {
		std::cout << "Already Ready" << std::endl;
	}
	void Ready::dispatch(Process *process) {
		std::cout << "Ready -> Running" << std::endl;
		process -> setState(new Running());
	}
	void Running::exit(Process *process) {
		std::cout << "Running -> Terminated" << std::endl;
		process -> setState(new Terminated());
	}
	void Running::interrupt(Process *process) {
		std::cout << "Running -> Ready" << std::endl;
		process -> setState(new Ready());
	}
	void Terminated::exit(Process *process) {
		std::cout << "Already Terminated" << std::endl;
	}

	// context definition
	Process::Process() {
		state = new New();
	}
	Process::~Process() {
		delete state;
		state = nullptr;
	}
	std::string Process::stateDescription() {
		return state -> description();
	}
	void Process::setState(ProcessState *aState) {
		if (!aState)
			return;
		delete state;
		state = aState;
	}
	void Process::addmit() {
		state -> addmit(this);
	}
	void Process::interrupt() {
		state -> interrupt(this);
	}
	void Process::dispatch() {
		state -> dispatch(this);
	}
	void Process::exit() {
		state -> exit(this);
	}
	
	void TestSuite() {
		Process *process = new Process();
		std::unordered_map<std::string, void(Process::* )()> events = {
			{"Addmit", &Process::addmit},
			{"Dispatch", &Process::dispatch},
			{"Interrupt", &Process::interrupt},
			{"Exit", &Process::exit}
		};
		
		std::string reply;
		while (reply != "Done") {
			reply.clear();
			std::cout << "Current State: " << process -> stateDescription() << std::endl;
			std::cout << "Trigger Event:" << std::endl;
			for (auto &e : events)
				std::cout << "   " << e.first << std::endl;
			std::cout << "Done (to stop)" << std::endl;
			std::cin >> reply;
			if (events.find(reply) != events.end()) {
				void(Process::* ef)() = events[reply];
				((*process).*ef)();
			}
		}
		
		delete process;
	}
}