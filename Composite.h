#pragma once

#include <string>
#include <unordered_map>

namespace Composite {
	class FileSystem {
		// component
		class Entry {
		protected:
			std::string name;
		public:
			Entry(std::string aName) {
				name = aName;
			}
			virtual ~Entry() {}
			const std::string &getName() {
				return name;
			}
			virtual bool hasEntry(const std::string &name) const = 0;
			virtual Entry *getEntry(const std::string &name) const = 0;
			virtual std::string desc() const = 0;
			virtual void llist(const std::string &opt) const = 0;
			virtual bool add(Entry *entry) = 0;
		};
		
		// leaf
		class File : public Entry {
		public:
			File(std::string aName) : Entry(aName) {}
			virtual bool hasEntry(const std::string &name) const {
				return false;
			}
			virtual ~File() {}
			virtual Entry *getEntry(const std::string &name) const {
				return nullptr;
			}
			virtual std::string desc() const {
				return "File : " + name;
			}
			virtual void llist(const std::string &opt) const {
				std::cout << "---" << std::endl;
				std::cout << name << " is not a directory." << std::endl;
			}
			virtual bool add(Entry *entry) {
				std::cout << "---" << std::endl;
				std::cout << name << " is not a directory." << std::endl;
				return false;
			}
		};
		
		// composite
		class Directory : public Entry {
			std::unordered_map<std::string, Entry *> children;
		public:
			Directory(std::string aName) : Entry(aName) {}
			virtual ~Directory() {
				for (auto &entry : children) {
					delete entry.second;
					entry.second = nullptr;
				}
			}
			virtual bool hasEntry(const std::string &name) const {
				return bool(children.count(name));
			}
			virtual Entry *getEntry(const std::string &name) const {
				return children.at(name);
			}
			virtual std::string desc() const {
				return "Directory : " + name;
			}
			virtual void llist(const std::string &opt) const {
				std::cout << "---" << std::endl;
				if (children.empty())
					std::cout << name << " is empty." << std::endl;
				for (auto &entry : children) {
					std::cout << entry.second -> desc() << std::endl;
					if (opt == "r")
						entry.second -> llist(opt);
				}
			}
			virtual bool add(Entry *entry) {
				if (hasEntry(entry -> getName())) {
					std::cout << "---" << std::endl;
					std::cout << entry -> getName() << " is already there." << std::endl;
					return false;
				}
				children[entry -> getName()] = entry;
				return true;
			}
		};
		
		// iterator
		class Iterator {
			Entry *current;
		public:
			Iterator() {
				current = nullptr;
			}
			Iterator(Entry *aCurrent) {
				current = aCurrent;
			}
			bool hasNext(const std::string &entry) {
				return current -> hasEntry(entry);
			}
			void next(const std::string &entry) {
				current = current -> getEntry(entry);
			}
			Entry*operator->() const {
				return current;
			}
		};
		
		Directory *root;
		Iterator itr;
	public:
		FileSystem() {
			root = new Directory("/");
			itr = Iterator(root);
		}
		virtual ~FileSystem() {
			delete root;
			root = nullptr;
		}
		void reset() {
			itr = Iterator(root);
		}
		void ls(const std::string &opt = "") {
			itr -> llist(opt);
		}
		void cd(const std::string &dirName) {
			if (itr.hasNext(dirName))
				itr.next(dirName);
		}
		void touch(const std::string &fileName) {
			File *file = new File(fileName);
			if (!(itr -> add(file))) {
				delete file;
				file = nullptr;
			}
		}
		void mkdir(const std::string &dirName) {
			Directory *dir = new Directory(dirName);
			if (!(itr -> add(dir))) {
				delete dir;
				dir = nullptr;
			}
		}
	};
	
	void TestSuite() {
		FileSystem *fs = new FileSystem();
		
		fs -> ls();
		fs -> mkdir("foo");
		fs -> mkdir("bar");
		fs -> touch("baz");
		fs -> ls();
		
		fs -> cd("foo");
		fs -> ls();
		fs -> mkdir("dir1");
		fs -> touch("file1");
		fs -> ls();
		
		fs -> reset();
		fs -> cd("bar");
		fs -> ls();
		fs -> mkdir("dir2");
		fs -> touch("file2");
		fs -> ls();
		
		std::cout << "***" << std::endl;
		fs -> reset();
		fs -> ls("r");
		
		delete fs;
	}
}