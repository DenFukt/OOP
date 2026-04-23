#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

template <typename T>
class DoublyLinkedList{
private:
    struct Node{
        T data;
        Node* next;
        Node* prev;
        Node(T val) : data(val), next(nullptr), prev(nullptr) {}
    };

    Node* head;
    Node* tail;
    int count;

public:
    DoublyLinkedList() : head(nullptr), tail(nullptr), count(0) {}

    ~DoublyLinkedList(){
        clear();
    }

    void push_back(T val){
        Node* newNode = new Node(val);
        if(!head){
            head = tail = newNode;
        }
        else{
            tail->next = newNode;
            newNode->prev = tail;
            tail = newNode;
        }
        count++;
    }

    void clear(){
        Node* current = head;
        while(current){
            Node* next = current->next;
            delete current;
            current = next;
        }
        head = tail = nullptr;
        count = 0;
    }

    int size() const { return count; }
    bool empty() const { return count == 0; }

    class Iterator{
    private:
        Node* current;
    public:
        Iterator(Node* node) : current(node) {}

        T& operator*(){ 
            return current->data; 
        }

        Iterator& operator++(){
            if(current){
                current = current->next;
            }
            return *this;
        }

        Iterator& operator--() {
            if(current){
                current = current->prev;
            }
            return *this;
        }

        bool operator!=(const Iterator& other) const { return current != other.current; }
        bool operator==(const Iterator& other) const { return current == other.current; }
        
        Node* getNode() const { return current; }
    };

    Iterator begin() { return Iterator(head); }
    Iterator end() { return Iterator(nullptr); }

    Iterator erase(Iterator it) {
        Node* nodeToDelete = it.getNode();
        if(!nodeToDelete){
            return end();
        }

        Node* nextNode = nodeToDelete->next;

        if(nodeToDelete->prev){
            nodeToDelete->prev->next = nodeToDelete->next;
        }
        else{
            head = nodeToDelete->next;
        }

        if(nodeToDelete->next){
            nodeToDelete->next->prev = nodeToDelete->prev;
        }
        else{
            tail = nodeToDelete->prev;
        }

        delete nodeToDelete;
        count--;

        return Iterator(nextNode);
    }
};

class Property{
    public:
        class PropertyPassport{
            public:
                string registrationDate;
                void printPassport(){cout << "Дата реєстрації: " << registrationDate << endl;}
        };

        static int totalObjects;
    private:
        string name;
        double price;
        int id;
        bool occupied;
        bool bought;
    protected:
        string address;
        double area;
        double rentPrice;
        PropertyPassport passport;
    public:
        Property(string n, string addr, double a, double pr, int i, double rp, string regDate):
            name(n), id(i), address(addr), area(a), price(pr), rentPrice(rp), occupied(false), bought(false){
                totalObjects++;
                passport.registrationDate = regDate;
            }
        
        virtual ~Property() {
            totalObjects--;
        }

        virtual void show() = 0;

        string getName() {return name;}
        int getid() {return id;}
        bool get_status() {return occupied;}
        double get_rent() {return rentPrice;}
        double get_price() {return price;}
        bool get_boughtness() {return bought;}

        void setStatus(bool status) {occupied = status;}
        void setboughtness(bool status) {bought = status;}
        void set_new_price(double newPrice) {if(newPrice>0) price = newPrice;}
};

int Property::totalObjects = 0;

class Apartment : public Property{
    private:
        int floor;
        int rooms;
    public:
        Apartment(string n, string addr, double a, double pr, int i, double rp, string regDate, int fl, int rm):
            Property(n, addr, a, pr, i, rp, regDate)
            {
                floor = fl;
                rooms = rm;
            }
    
        void show() override{
            cout << (get_boughtness() ? "[ВЛАСНІСТЬ] " : "[РИНОК] ") << (get_status() ? "[ЗАЙНЯТО]" : "[ВІЛЬНО]") << "АПАРТАМЕНТИ Адреса: " << address << ". Квадратура: " << area << ". Поверх: " << floor << ". К-сть кімнат: " << rooms << endl;
            passport.printPassport();
        }
};

class IRepairable{
    public:
        virtual void repair() = 0;

        virtual ~IRepairable() {};
};

class Commercial : public Property, public IRepairable{
    private:
        string company_name;
        int parkingplaces;
        int last_repair;
    public:
        Commercial(string n, string addr, double a, double pr, int i, double rp, int yr, string regDate, string cm, int pp):
            Property(n, addr, a, pr, i, rp, regDate)
            {
                company_name = cm;
                parkingplaces = pp;
                last_repair = yr;
            }
        void repair() override{
            last_repair = 2026;
            cout << "Офіс компанії " << company_name << " відремонтовано!" << endl;
        }

        void show() override{
            cout << (get_boughtness() ? "[ВЛАСНІСТЬ] " : "[РИНОК] ") << (get_status() ? "[ЗАЙНЯТО]" : "[ВІЛЬНО]") <<"КОММЕРЦІЯ Адреса: " << address << ". Площа: " << area << ". Назва компанії: " << company_name << ". К-сть паркомісць: " << parkingplaces << ". Ремонт: " << last_repair << endl;
            passport.printPassport();
        }
};

class ownHouse : public Property, public IRepairable{
    private:
        double outdoorsarea;
        int floors;
        int last_repair;
    public:
        ownHouse(string n, string addr, double a, double pr, int i, double rp, int yr, string regDate, double outa, int fls):
            Property(n, addr, a, pr, i, rp, regDate)
            {
                outdoorsarea = outa;
                floors = fls;
                last_repair = yr;
            }
        void repair() override{
            last_repair = 2026;
            cout << "Будинок відремонтовано!" << endl;
        }

        void show() override{
            cout << (get_boughtness() ? "[ВЛАСНІСТЬ] " : "[РИНОК] ") << (get_status() ? "[ЗАЙНЯТО]" : "[ВІЛЬНО]") << "ПРИВАТНІ Адреса: " <<address << ". Площа: " << area << ". Площа земельної ділянки: " <<outdoorsarea << ". Кількість поверхів: " <<floors << ". Ремонт: " << last_repair << endl;
            passport.printPassport();
        }
};

int trueint(){
    int a;
    while(!(cin>>a)){
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Неправильно введено! Спробуйте ще раз:" << endl; 
    }
    return a;
}

void addObject(DoublyLinkedList<Property*>& objects) {
    cout << "Який тип об'єкта додати?" << endl;
    cout << "1. Апартаменти" << endl;
    cout << "2. Комерційна нерухомість" << endl;
    cout << "3. Приватний будинок" << endl;
    cout << "Ваш вибір: ";
    
    int type = trueint();
    while(type<1 || type>3){
        cout << "Неправильний ввід! Спробуйте ще раз: ";
        type = trueint();
    }

    string n, addr, regDate;
    double a, pr, rp;
    int i;

    cout << "Введіть дату реєстрації паспорта: ";
    getline(cin >> ws, regDate);
    cout << "Введіть назву: ";
    getline(cin >> ws, n);
    cout << "Введіть адресу: ";
    getline(cin >> ws, addr);
    cout << "Введіть площу: ";
    cin >> a;
    cout << "Введіть ціну: ";
    cin >> pr;
    cout << "Введіть ID: ";
    i = trueint();
    cout << "Введіть ціну оренди: ";
    cin >> rp;

    if(type == 1){
        int fl, rm;
        cout << "Введіть поверх: ";
        fl = trueint();
        cout << "Введіть к-сть кімнат: ";
        rm = trueint();
        objects.push_back(new Apartment(n, addr, a, pr, i, rp, regDate, fl, rm));
    } 
    else if(type == 2){
        int yr, pp;
        string cm;
        cout << "Введіть рік останнього ремонту: ";
        yr = trueint();
        cout << "Введіть назву компанії: ";
        getline(cin >> ws, cm);
        cout << "Введіть к-сть паркомісць: ";
        pp = trueint();
        objects.push_back(new Commercial(n, addr, a, pr, i, rp, yr, regDate, cm, pp));
    } 
    else if(type == 3){
        int fls, yr;
        double outa;
        cout << "Введіть рік останнього ремонту: ";
        yr = trueint();
        cout << "Введіть площу ділянки: ";
        cin >> outa;
        cout << "Введіть к-сть поверхів: ";
        fls = trueint();
        objects.push_back(new ownHouse(n, addr, a, pr, i, rp, yr, regDate, outa, fls));
    }
    cout << "Об'єкт успішно додано!" << endl;
}

class {
    public:
        void print_message(){
            cout << "Вітаю в системі Estate! Обирайте що бажаєте зробити:" << endl;
        }
} systemprinter;

void displayAllProperties(DoublyLinkedList<Property*>& objects){
    if(objects.empty()){
        cout << "Поки що немає об'єктів!" << endl;
        return;
    }
    cout << "Список об'єктів нерухомості" << endl;
    for(Property* p : objects){
        p->show();
    }
}

void filterByMaxPrice(DoublyLinkedList<Property*>& objects){
    cout << "Введіть максимальний бюджет: ";
    double budget;
    cin >> budget;
    cout << "Доступні варіанти до " << budget << " грн:" << endl;
    bool found = false;
    for(Property* p : objects){
        if(p->get_price() <= budget){
            p->show();
            found = true;
        }
    }
    if(!found){
        cout << "Нічого не знайдено за таку ціну." << endl;
    }
}

void updatePropertyPrice(DoublyLinkedList<Property*>& objects){
    cout << "Введіть ID об'єкта для зміни ціни: ";
    int searchid = trueint();
    for(Property* p : objects){
        if(p->getid() == searchid){
            cout << "Об'єкт: " << p->getName() << ". Введіть нову ціну: ";
            double newPrice;
            cin >> newPrice;
            p->set_new_price(newPrice);
            cout << "Ціну успішно оновлено!" << endl;
            return;
        }
    }
    cout << "Об'єкт з ID " << searchid << " не знайдено." << endl;
}

void deletePropertyByName(DoublyLinkedList<Property*>& objects) {
    cout << "Введіть назву для видалення: ";
    string name;
    getline(cin >> ws, name);

    bool found = false;
    auto it = objects.begin();
    while(it != objects.end()){
        if((*it)->getName() == name){
            found = true;
            delete *it;
            it = objects.erase(it);
            cout << "Видалено!" << endl;
        }
        else{
            ++it;
        }
    }
    if(!found){
        cout << "Об'єкт не знайдено." << endl;
    }
}

void repairProperty(DoublyLinkedList<Property*>& objects){
    cout << "Введіть назву об'єкта для ремонту: ";
    string name;
    getline(cin >> ws, name);
    for(Property* p : objects){
        if(p->getName() == name){
            IRepairable* r = dynamic_cast<IRepairable*>(p);
            if(r){
                r->repair();
            }
            else{
                cout << "Об'єкт '" << name << "' не підлягає ремонту!" << endl;
            }
            return;
        }
    }
    cout << "Об'єкт не знайдено!" << endl;
}

void toggleRentStatus(DoublyLinkedList<Property*>& objects){
    cout << "Введіть назву об'єкта: ";
    string name;
    getline(cin >> ws, name);
    for(Property* p : objects){
        if(p->getName() == name){
            p->setStatus(!p->get_status());
            cout << "Статус об'єкта '" << name << "' змінено!" << endl;
            return;
        }
    }
    cout << "Об'єкт не знайдено!" << endl;
}

void purchaseProperty(DoublyLinkedList<Property*>& objects){
    cout << "Введіть назву об'єкта для придбання: ";
    string name;
    getline(cin >> ws, name);
    for(Property* p : objects){
        if(p->getName() == name){
            cout << "Вартість: " << p->get_price() << " грн. Підтвердити? (y/n): ";
            char answer;
            cin >> answer;
            if(answer == 'y' || answer == 'Y'){
                p->setboughtness(true);
                cout << "Вітаю з придбанням!" << endl;
            }
            return;
        }
    }
    cout << "Нерухомість не знайдено!" << endl;
}

void sellProperty(DoublyLinkedList<Property*>& objects){
    cout << "Введіть назву об'єкта для продажу: ";
    string name;
    getline(cin >> ws, name);
    for(Property* p : objects){
        if(p->getName() == name){
            if(p->get_boughtness()){
                p->setboughtness(false);
                p->setStatus(false);
                cout << "Об'єкт продано та виставлено на ринок!" << endl;
            }
            else{
                cout << "Ви не можете продати те, що вам не належить!" << endl;
            }
            return;
        }
    }
    cout << "Об'єкт не знайдено!" << endl;
}

void calculateTotalIncome(DoublyLinkedList<Property*>& objects){
    double totalRent = 0;
    cout << "Розрахунок пасивного доходу" << endl;
    for(Property* p : objects){
        if(p->get_boughtness() && p->get_status()){
            cout << "+ " << p->getName() << ": " << p->get_rent() << " грн." << endl;
            totalRent += p->get_rent();
        }
    }
    cout << "Загальний прибуток за місяць: " << totalRent << " грн." << endl;
}

void initializeSystem(DoublyLinkedList<Property*>& objects){
    objects.push_back(new Apartment("Екстравагант", "вул. Князя Романа, 5", 68.4, 25000, 1, 12000, "26.02.2026", 4, 2));
    objects.push_back(new Commercial("Незнайки", "вул. Наукова, 46", 674, 200000, 2, 67000, 2024, "26.02.2026", "SoftServe", 10));
    objects.push_back(new ownHouse("Прескураж", "вул. Стрийська, 144", 134, 80000, 3, 0, 2022, "27.02.2026", 344.5, 2));
}

void cleanupSystem(DoublyLinkedList<Property*>& objects){
    for(Property* p : objects){
        delete p;
    }
    objects.clear();
}

int main(){
    systemprinter.print_message();

    DoublyLinkedList<Property*> objects;

    initializeSystem(objects);

    int choice = -1;
    while(choice != 0){
        cout << "Керування нерухомістю" << endl;
        cout << "1. Показати всі об'єкти" << endl;
        cout << "2. Змінити ціну(за ID)" << endl;
        cout << "3. Видалити об'єкт" << endl;
        cout << "4. Здійснити ремонт" << endl;
        cout << "5. Додати новий об'єкт" << endl;
        cout << "6. Здати в оренду/Повернути" << endl;
        cout << "7. Придбати об'єкт" << endl;
        cout << "8. Розрахувати прибуток" << endl;
        cout << "9. Продати об'єкт" << endl;
        cout << "10. Загальна кількість об'єктів" << endl;
        cout << "11. Фільтр за ціною" << endl;
        cout << "0. Вихід" << endl;
        cout << "Ваш вибір: ";
        
        choice = trueint();
        switch(choice){
            case 1:
                displayAllProperties(objects);
                break;
            case 2:
                updatePropertyPrice(objects);
                break;
            case 3:
                deletePropertyByName(objects);
                break;
            case 4:
                repairProperty(objects);
                break;
            case 5:
                addObject(objects);
                break;
            case 6:
                toggleRentStatus(objects);
                break;
            case 7:
                purchaseProperty(objects);
                break;
            case 8:
                calculateTotalIncome(objects);
                break;
            case 9:
                sellProperty(objects);
                break;
            case 10:
                cout << "Загальна кількість об'єктів у системі: " << Property::totalObjects << endl;
                break;
            case 11:
                filterByMaxPrice(objects);
                break;
            case 0:
                cleanupSystem(objects);
                cout << "До зустрічі!";
                return 0;
            default:
                cout << "Неправильний вибір!" << endl;
        }
    }
}