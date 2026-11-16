#include <iostream>
#include <string>
#include <iomanip>
#include <limits>
using namespace std;
class Stock {
private:
    string name;
    double currentPrice;
    double averageBuyPrice;
    int quantity;
    static int liveObjects;
    static int totalStocksCreated;
public:
    Stock(string n = "", double price = 0.0, int qty = 0, double buyPrice = -1.0)
        : name(n), currentPrice(price), averageBuyPrice(buyPrice < 0 ? price : buyPrice), quantity(qty) {
        ++liveObjects;
    }
    Stock(const Stock &s)
        : name(s.name), currentPrice(s.currentPrice), averageBuyPrice(s.averageBuyPrice), quantity(s.quantity) {
        ++liveObjects;
    }
    Stock& operator=(const Stock &s) {
        if (this != &s) { name=s.name; currentPrice=s.currentPrice; averageBuyPrice=s.averageBuyPrice; quantity=s.quantity; }
        return *this;
    }
    ~Stock() { --liveObjects; }

    string getName() const { return name; }
    double getPrice() const { return currentPrice; }
    double getBuyPrice() const { return averageBuyPrice; }
    int getQuantity() const { return quantity; }
    double marketValue() const { return currentPrice * quantity; }
    double investedValue() const { return averageBuyPrice * quantity; }
    double profitLoss() const { return (currentPrice - averageBuyPrice) * quantity; }

    void setCurrentPrice(double p) { currentPrice = p; }
    void setQuantity(int q) { quantity = q; }
    void setAverageBuyPrice(double p) { averageBuyPrice = p; }
    void addQuantity(int q, double buyPrice) {
        if (q <= 0) return;
        double totalCost = averageBuyPrice * quantity + buyPrice * q;
        quantity += q;
        averageBuyPrice = totalCost / quantity;
    }
    bool reduceQuantity(int q) {
        if (q <= 0 || q > quantity) return false;
        quantity -= q;
        if (quantity == 0) averageBuyPrice = 0.0;
        return true;
    }
    void display() const {
        cout << left << setw(14) << name << right << setw(12) << fixed << setprecision(2) << currentPrice
             << setw(10) << quantity << setw(14) << averageBuyPrice << setw(14) << marketValue()
             << setw(14) << profitLoss() << '\n';
    }
    static void recordCreation() { ++totalStocksCreated; }
    static void recordDeletion() { if (totalStocksCreated > 0) --totalStocksCreated; }
    static int getLiveObjects() { return liveObjects; }
    static void showTotalStocks() { cout << "Current distinct stock records: " << totalStocksCreated << '\n'; }
    bool operator<(const Stock &other) const { return currentPrice < other.currentPrice; }
    bool operator>(const Stock &other) const { return currentPrice > other.currentPrice; }
    friend void showPrivateStockDetails(const Stock &s);
};
int Stock::liveObjects = 0;
int Stock::totalStocksCreated = 0;

void showPrivateStockDetails(const Stock &s) {
    cout << "[Friend function] Stock: " << s.name << ", current price: Rs." << s.currentPrice
         << ", quantity: " << s.quantity << '\n';
}

class Node {
public:
    Stock data;
    Node *next;
    explicit Node(const Stock &s) : data(s), next(NULL) {}
};

class Portfolio {
private:
    Node *head;
    Node* findNode(const string &name) const {
        Node *cur = head;
        while (cur) { if (cur->data.getName() == name) return cur; cur = cur->next; }
        return NULL;
    }
    void clear() {
        while (head) { Node *t=head; head=head->next; delete t; Stock::recordDeletion(); }
    }
public:
    Portfolio() : head(NULL) {}
    Portfolio(const Portfolio &other) : head(NULL) {
        Node *cur=other.head;
        while (cur) { addNewStock(cur->data); cur=cur->next; }
    }
    Portfolio& operator=(const Portfolio &other) {
        if (this != &other) { clear(); Node *cur=other.head; while(cur) { addNewStock(cur->data); cur=cur->next; } }
        return *this;
    }
    ~Portfolio() { clear(); }

    bool addNewStock(const Stock &s) {
        if (s.getName().empty() || s.getPrice() < 0 || s.getQuantity() <= 0) return false;
        if (findNode(s.getName())) return false;
        Node *n = new Node(s);
        if (!head) head=n;
        else { Node *cur=head; while(cur->next) cur=cur->next; cur->next=n; }
        Stock::recordCreation();
        return true;
    }
    bool removeStock(const string &name) {
        if (!head) return false;
        if (head->data.getName()==name) { Node *t=head; head=head->next; delete t; Stock::recordDeletion(); return true; }
        Node *cur=head;
        while(cur->next && cur->next->data.getName()!=name) cur=cur->next;
        if (!cur->next) return false;
        Node *t=cur->next; cur->next=t->next; delete t; Stock::recordDeletion(); return true;
    }
    Stock* searchStock(const string &name) {
        Node *n=findNode(name); return n ? &n->data : NULL;
    }
    const Stock* searchStock(const string &name) const {
        Node *n=findNode(name); return n ? &n->data : NULL;
    }
    bool updateCurrentPrice(const string &name, double price) {
        Stock *s=searchStock(name); if(!s || price<0) return false; s->setCurrentPrice(price); return true;
    }
    void display() const {
        if (!head) { cout << "Portfolio is empty.\n"; return; }
        cout << left << setw(14) << "STOCK" << right << setw(12) << "PRICE" << setw(10) << "QTY"
             << setw(14) << "AVG BUY" << setw(14) << "VALUE" << setw(14) << "P/L" << '\n';
        cout << string(78,'-') << '\n';
        Node *cur=head; while(cur) { cur->data.display(); cur=cur->next; }
    }
    int count() const { int n=0; Node *cur=head; while(cur){++n;cur=cur->next;} return n; }
    double totalInvestment() const { double x=0; Node *cur=head; while(cur){x+=cur->data.investedValue();cur=cur->next;} return x; }
    double currentValue() const { double x=0; Node *cur=head; while(cur){x+=cur->data.marketValue();cur=cur->next;} return x; }
    double totalPL() const { return currentValue()-totalInvestment(); }
    void sortByPrice() {
        if (!head || !head->next) return;
        bool swapped;
        do { swapped=false; Node *cur=head; while(cur->next) { if(cur->data > cur->next->data) { Stock t=cur->data; cur->data=cur->next->data; cur->next->data=t; swapped=true; } cur=cur->next; } } while(swapped);
    }
    void sortByQuantity() {
        if (!head || !head->next) return;
        bool swapped;
        do { swapped=false; Node *cur=head; while(cur->next) { if(cur->data.getQuantity() > cur->next->data.getQuantity()) { Stock t=cur->data; cur->data=cur->next->data; cur->next->data=t; swapped=true; } cur=cur->next; } } while(swapped);
    }
    void dashboard() const {
        cout << "\n========== PORTFOLIO DASHBOARD ==========\n";
        cout << fixed << setprecision(2);
        cout << "Distinct holdings       : " << count() << '\n';
        cout << "Total invested          : Rs. " << totalInvestment() << '\n';
        cout << "Current market value    : Rs. " << currentValue() << '\n';
        cout << "Unrealized profit/loss  : Rs. " << totalPL() << '\n';
        if (totalInvestment() > 0)
            cout << "Return                  : " << (totalPL()/totalInvestment())*100.0 << "%\n";
        else cout << "Return                  : N/A\n";
        const Stock *best=NULL, *worst=NULL;
        Node *cur=head;
        while(cur) {
            if(cur->data.getQuantity()>0) {
                if(!best || cur->data.profitLoss() > best->profitLoss()) best=&cur->data;
                if(!worst || cur->data.profitLoss() < worst->profitLoss()) worst=&cur->data;
            }
            cur=cur->next;
        }
        cout << "Best holding (absolute P/L): " << (best ? best->getName() : "N/A") << '\n';
        cout << "Worst holding (absolute P/L): " << (worst ? worst->getName() : "N/A") << '\n';
        cout << "=========================================\n";
    }
    void allocationReport() const {
        double total=currentValue();
        if(total<=0) { cout << "No market value to calculate allocation.\n"; return; }
        cout << "\nPortfolio allocation (% of market value):\n";
        Node *cur=head;
        while(cur) { cout << cur->data.getName() << " : " << fixed << setprecision(2)
                          << (cur->data.marketValue()/total)*100.0 << "%\n"; cur=cur->next; }
    }
    void performanceRanking() {
        int n=count(); if(n==0){cout << "Portfolio is empty.\n";return;}
        Stock *arr = new Stock[n]; Node *cur=head; int i=0;
        while(cur){arr[i++]=cur->data;cur=cur->next;}
        for(int a=0;a<n-1;++a) for(int b=0;b<n-a-1;++b)
            if(arr[b].profitLoss()<arr[b+1].profitLoss()){Stock t=arr[b];arr[b]=arr[b+1];arr[b+1]=t;}
        cout << "\nHoldings ranked by unrealized P/L (highest first):\n";
        for(i=0;i<n;++i) cout << i+1 << ". " << arr[i].getName() << " : Rs. " << fixed << setprecision(2) << arr[i].profitLoss() << '\n';
        delete[] arr;
    }
};

class Transaction {
public:
    string type, stockName;
    int quantity;
    double price, realizedPL;
    Transaction(string t="", string n="", int q=0, double p=0, double pl=0)
        : type(t), stockName(n), quantity(q), price(p), realizedPL(pl) {}
    void display() const {
        cout << type << " | " << stockName << " | Qty: " << quantity << " | Price: Rs. "
             << fixed << setprecision(2) << price << " | Amount: Rs. " << quantity*price;
        if(type=="SELL") cout << " | Realized P/L: Rs. " << realizedPL;
        cout << '\n';
    }
};
class TransactionManager {
private:
    static const int CAPACITY=200;
    Transaction records[CAPACITY];
    int used;
public:
    TransactionManager() : used(0) {}
    bool buy(Portfolio &p, const string &name, int qty, double price) {
        if(name.empty() || qty<=0 || price<=0) return false;
        Stock *s=p.searchStock(name);
        if(s) s->addQuantity(qty,price);
        else if(!p.addNewStock(Stock(name,price,qty,price))) return false;
        if(used==CAPACITY) { cout << "Transaction log full; portfolio updated but transaction was not logged.\n"; return true; }
        records[used++]=Transaction("BUY",name,qty,price,0);
        return true;
    }
    bool sell(Portfolio &p, const string &name, int qty, double price) {
        if(qty<=0 || price<=0) return false;
        Stock *s=p.searchStock(name);
        if(!s || qty>s->getQuantity()) return false;
        double realized=(price-s->getBuyPrice())*qty;
        s->reduceQuantity(qty);
        if(s->getQuantity()==0) p.removeStock(name);
        if(used==CAPACITY) { cout << "Transaction log full; sale completed but transaction was not logged.\n"; return true; }
        records[used++]=Transaction("SELL",name,qty,price,realized);
        return true;
    }
    void display() const {
        if(!used){cout << "No transactions yet.\n";return;}
        cout << "\nTransaction history (newest first):\n";
        for(int i=used-1;i>=0;--i) records[i].display();
    }
};

class OrderQueue {
    static const int CAPACITY=50;
    int ids[CAPACITY], front, rear, count;
public:
    OrderQueue() : front(0), rear(-1), count(0) {}
    bool enqueue(int id) {
        if(count==CAPACITY) return false;
        rear=(rear+1)%CAPACITY; ids[rear]=id; ++count; return true;
    }
    bool dequeue(int &id) {
        if(count==0) return false;
        id=ids[front]; front=(front+1)%CAPACITY; --count; return true;
    }
    void display() const {
        if(!count){cout << "Normal order queue is empty.\n";return;}
        cout << "Normal queue (front to rear): ";
        for(int i=0;i<count;++i) cout << ids[(front+i)%CAPACITY] << ' ';
        cout << '\n';
    }
};

class PriorityNode {
public:
    int id, priority; PriorityNode *next;
    PriorityNode(int i,int p):id(i),priority(p),next(NULL){}
};
class PriorityOrderQueue {
    PriorityNode *head;
public:
    PriorityOrderQueue():head(NULL){}
    bool insert(int id,int priority) {
        PriorityNode *n=new PriorityNode(id,priority);
        if(!head || priority>head->priority){n->next=head;head=n;return true;}
        PriorityNode *cur=head;
        while(cur->next && cur->next->priority>=priority) cur=cur->next;
        n->next=cur->next;cur->next=n;return true;
    }
    bool process(int &id,int &priority) {
        if(!head)return false;
        PriorityNode *t=head;id=t->id;priority=t->priority;head=head->next;delete t;return true;
    }
    void display() const {
        if(!head){cout << "Priority queue is empty.\n";return;}
        cout << "Priority queue (highest first):\n";
        PriorityNode *cur=head;while(cur){cout << "Order "<<cur->id<<" | Priority "<<cur->priority<<'\n';cur=cur->next;}
    }
    ~PriorityOrderQueue(){while(head){PriorityNode*t=head;head=head->next;delete t;}}
};

class ActionStack {
    static const int CAPACITY=50;
    string actions[CAPACITY]; int top;
public:
    ActionStack():top(-1){}
    bool push(const string &s){if(top==CAPACITY-1)return false;actions[++top]=s;return true;}
    bool pop(string &s){if(top<0)return false;s=actions[top--];return true;}
    void display() const {if(top<0){cout<<"Action history is empty.\n";return;}for(int i=top;i>=0;--i)cout<<actions[i]<<'\n';}
};
class User {
    string username;
public:
    explicit User(const string &name):username(name){}
    void display() const {cout << "Active demo user: " << username << '\n';}
};

void clearBadInput(){cin.clear();cin.ignore(numeric_limits<streamsize>::max(),'\n');}
int readInt(const string &prompt){int x;while(true){cout<<prompt;if(cin>>x)return x;cout<<"Enter a valid integer.\n";clearBadInput();}}
double readDouble(const string &prompt){double x;while(true){cout<<prompt;if(cin>>x && x>=0)return x;cout<<"Enter a valid non-negative number.\n";clearBadInput();}}
string readName(const string &prompt){string s;cout<<prompt;cin>>s;return s;}

int main(){
    Portfolio portfolio;
    TransactionManager transactions;
    OrderQueue normalQueue;
    PriorityOrderQueue priorityQueue;
    ActionStack actionHistory;
    User user("StockWise Demo");
    int choice;
    do {
        cout << "\n============== STOCKWISE ==============\n"
             << "1. Add holding manually\n2. Display portfolio\n3. Search holding\n4. Delete holding\n"
             << "5. Sort holdings by current price\n6. Portfolio dashboard\n7. Copy constructor demo\n"
             << "8. Friend function demo\n9. Operator overloading demo\n10. Show stock record count\n"
             << "11. Add normal order\n12. Process normal order\n13. Add priority order\n14. Process priority order\n"
             << "15. Update current market price\n16. Sort holdings by quantity\n17. Allocation report\n"
             << "18. Show action history\n19. Remove latest action-history entry\n20. Show live Stock objects\n"
             << "21. Display user\n22. Display normal queue\n23. Buy stock\n24. Sell stock\n"
             << "25. Display transaction history\n26. Rank holdings by unrealized P/L\n0. Exit\n";
        choice=readInt("Enter choice: ");
        string name; double price; int qty;
        switch(choice){
        case 1: {
            name=readName("Stock symbol/name (no spaces): "); price=readDouble("Current price: "); qty=readInt("Quantity: ");
            if(qty<=0){cout<<"Quantity must be positive.\n";break;}
            if(portfolio.addNewStock(Stock(name,price,qty,price))){cout<<"Holding added.\n";actionHistory.push("Added holding: "+name);}else cout<<"Could not add: duplicate name or invalid details.\n";
            break;}
        case 2: portfolio.display(); break;
        case 3: {name=readName("Enter stock name: ");const Stock*s=portfolio.searchStock(name);if(s){cout<<"Found: ";s->display();}else cout<<"Stock not found.\n";break;}
        case 4: {name=readName("Enter stock name to delete: ");if(portfolio.removeStock(name)){cout<<"Holding deleted.\n";actionHistory.push("Deleted holding: "+name);}else cout<<"Stock not found.\n";break;}
        case 5: portfolio.sortByPrice();cout<<"Sorted by current price (ascending).\n";portfolio.display();break;
        case 6: portfolio.dashboard();break;
        case 7: {Portfolio copied(portfolio);cout<<"Independent portfolio copy created:\n";copied.display();break;}
        case 8: {name=readName("Enter stock name: ");const Stock*s=portfolio.searchStock(name);if(s)showPrivateStockDetails(*s);else cout<<"Stock not found.\n";break;}
        case 9: {name=readName("First stock name: ");const Stock*a=portfolio.searchStock(name);string second=readName("Second stock name: ");const Stock*b=portfolio.searchStock(second);if(!a||!b)cout<<"Both stocks must exist.\n";else if(*a>*b)cout<<a->getName()<<" has the higher current price.\n";else if(*a<*b)cout<<b->getName()<<" has the higher current price.\n";else cout<<"Both have equal current prices.\n";break;}
        case 10: Stock::showTotalStocks();break;
        case 11: {int id=readInt("Order ID: ");if(normalQueue.enqueue(id))cout<<"Order queued.\n";else cout<<"Normal queue is full.\n";break;}
        case 12: {int id;if(normalQueue.dequeue(id))cout<<"Processed normal order: "<<id<<'\n';else cout<<"Normal queue is empty.\n";break;}
        case 13: {int id=readInt("Order ID: ");int pr=readInt("Priority (higher number = higher priority): ");priorityQueue.insert(id,pr);cout<<"Priority order added.\n";break;}
        case 14: {int id,pr;if(priorityQueue.process(id,pr))cout<<"Processed priority order "<<id<<" (priority "<<pr<<").\n";else cout<<"Priority queue is empty.\n";break;}
        case 15: {name=readName("Stock name: ");price=readDouble("New current market price: ");if(portfolio.updateCurrentPrice(name,price)){cout<<"Price updated.\n";actionHistory.push("Updated market price: "+name);}else cout<<"Stock not found.\n";break;}
        case 16: portfolio.sortByQuantity();cout<<"Sorted by quantity (ascending).\n";portfolio.display();break;
        case 17: portfolio.allocationReport();break;
        case 18: actionHistory.display();break;
        case 19: {string action;if(actionHistory.pop(action))cout<<"Removed history entry: "<<action<<"\n(Note: this is an activity log stack, not a data-undo feature.)\n";else cout<<"No action-history entry.\n";break;}
        case 20: cout<<"Live Stock objects (includes temporary/copy objects): "<<Stock::getLiveObjects()<<'\n';break;
        case 21: user.display();break;
        case 22: normalQueue.display();break;
        case 23: {name=readName("Stock name: ");qty=readInt("Buy quantity: ");price=readDouble("Buy price per share: ");if(transactions.buy(portfolio,name,qty,price)){cout<<"BUY completed and portfolio updated.\n";actionHistory.push("BUY "+name+" x "+to_string(qty));}else cout<<"BUY failed. Check quantity/price/name.\n";break;}
        case 24: {name=readName("Stock name: ");qty=readInt("Sell quantity: ");price=readDouble("Sell price per share: ");if(transactions.sell(portfolio,name,qty,price)){cout<<"SELL completed and portfolio updated.\n";actionHistory.push("SELL "+name+" x "+to_string(qty));}else cout<<"SELL failed. Holding may not exist or quantity may be insufficient.\n";break;}
        case 25: transactions.display();break;
        case 26: portfolio.performanceRanking();break;
        case 0: cout<<"Thank you for using StockWise.\n";break;
        default: cout<<"Invalid menu choice.\n";
        }
    } while(choice!=0);
    return 0;
}
