#pragma mark

namespace Builder {
	class MainItem {};
	class Sandwich : public MainItem {};
	class Steak : public MainItem {};
	
	class SideItem {};
	class Oatmeal : public SideItem {};
	class Fries : public SideItem {};

	class Drink {};
	class Soda : public Drink {};
	class Beer : public Drink {};
	
	class Toy {};
	
	class Product {
		MainItem *mainItem;
		SideItem *sideItem;
		Drink *drink;
		Toy *toy;
		
		void setMainItem(MainItem *aMainImtem) {
			mainItem = aMainImtem;
		}
		void setSideItem(SideItem *aSideItem) {
			sideItem = aSideItem;
		}
		void setDrink(Drink *aDrink) {
			drink = aDrink;
		}
		void setToy(Toy *aToy) {
			toy = aToy;
		}
	public:
		Product() {
			mainItem = nullptr;
			sideItem = nullptr;
			drink = nullptr;
			toy = nullptr;
		}
		~Product() {
			if (mainItem) delete mainItem;
			if (sideItem) delete sideItem;
			if (drink) delete drink;
			if (toy) delete toy;
			mainItem = nullptr;
			sideItem = nullptr;
			drink = nullptr;
			toy = nullptr;
		}
		friend class BreakfastCrew;
		friend class DinnerCrew;
	};
	
	// builder
	class BaseCrew {
	protected:
		Product *product;
		
		virtual void createProduct() {
			if (product) delete product;
			product = new Product();
		}
		virtual void putMainItem() = 0;
		virtual void putSideItem() = 0;
		virtual void putDrink() = 0;
		virtual void putToy() = 0;
	public:
		BaseCrew() {
			product = nullptr;
		}
		virtual ~BaseCrew() {
			if (product) delete product;
			product = nullptr;
		}
		Product *getProduct() {
			Product *p = product;
			product = nullptr;
			return p;
		}
		friend class Cashier;
	};
	class BreakfastCrew : public BaseCrew {
		virtual void putMainItem() {
			product -> setMainItem(new Sandwich());
		}
		virtual void putSideItem() {
			product -> setSideItem(new Oatmeal());
		}
		virtual void putDrink() {
			product -> setDrink(new Soda());
		}
		virtual void putToy() {
			product -> setToy(new Toy());
		}
	public:
	};
	class DinnerCrew : public BaseCrew {
		virtual void putMainItem() {
			product -> setMainItem(new Steak());
		}
		virtual void putSideItem() {
			product -> setSideItem(new Fries());
		}
		virtual void putDrink() {
			product -> setDrink(new Beer());
		}
		virtual void putToy() {}
	public:
	};

	
	// director
	class Cashier {
		BaseCrew *crew;
	public:
		void setCrew(BaseCrew *aCrew) {
			crew = aCrew;
		}
		void orderProduct() {
			if (!crew) return;
			crew -> createProduct();
			crew -> putMainItem();
			crew -> putSideItem();
			crew -> putDrink();
			crew -> putToy();
		}
	};
	
	void TestSuite() {
		Cashier *cashier = new Cashier();
		BreakfastCrew *bCrew = new BreakfastCrew();
		DinnerCrew *dCrew = new DinnerCrew();
		
		cashier -> setCrew(bCrew);
		cashier -> orderProduct();
		Product *breakfast = bCrew -> getProduct();
		
		cashier -> setCrew(dCrew);
		cashier -> orderProduct();
		Product *dinner = bCrew -> getProduct();
		
		delete breakfast;
		delete dinner;
		
		delete cashier;
		delete bCrew;
		delete dCrew;
	}
}