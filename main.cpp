#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <cstdio>
#include <cctype>
using namespace std;

// data structures
// store main details of one order
struct Order {
    string orderID;
    string studentID;
    string cuisineType;
    string orderStatus;
    string assignedStallID;
    double total;
    string orderTime;
};
// store each item added into cart
struct CartItem {
    string itemID;
    string itemName;
    int quantity;
    double unitPrice;
    double lineTotal;
};

/* Task 3: Session History (SITI NUR ZAFIRAH BINTI ZUNAIDI TP077333)*/

//constants
const int maximum_steps_in_session = 100;
const int maximum_items_in_cart = 20;

//Struct
struct ItemInsideCart //store 1 item in cart
{
    string itemID;
    string itemName;
    int quantity;
    double unitPrice;
    double totalItemPrice;
};

struct KioskFullCondition //store complete kiosk condition at the time
{
    string screenName;
    string selectedCuisineType;
    ItemInsideCart cartItem[maximum_items_in_cart];
    int cartCount;
    double total;
};

struct UserStepInKiosk //store action by user + resulting state of kiosk
{
    int stepNumber;
    string actionType; //action (ex: Update Quantity)
    string description; //important information (ex: Changed Nasi Lemak quantity from 1 to 2)
    KioskFullCondition state; //full state (ex: Complete cart after the update)
};

struct SessionNode //link Stack node
{
    UserStepInKiosk step;
    SessionNode* next;
};

class SessionHistory
{
private:
    SessionNode* top;
    int stackSize;
    int nextStepNumber;

    UserStepInKiosk completeHistory[maximum_steps_in_session];
    int completeHistoryCount;

    UserStepInKiosk navigationTrace[maximum_steps_in_session];
    int navigationTraceCount;

public:
    SessionHistory() //constructor - initialise all values + prepare empty session
    {
        top = nullptr; //stack has x node yet
        stackSize = 0; // x active step
        nextStepNumber = 1; //first record action = step 1
        completeHistoryCount = 0; //history empty
        navigationTraceCount = 0; //back empty
    }


    ~SessionHistory() //destructor - calls clearSessionHistory() automatically
    {
        clearSessionHistory();
    }

    bool isSessionStackEmpty() //check session empty or not
    {
        if (top == nullptr) {
            return true;
        }
        else {
            return false;
        }
    }

    bool isSessionStackAtBeginning() //check whether first stack
    {
        if (top != nullptr && top -> next == nullptr) {
            return true;
        }
        else {
            return false;
        }
    }

    bool isSessionStackFull() //check if stack already full (compare w maximum size)
    {
        if (completeHistoryCount >= maximum_steps_in_session) {
            return true;
        }
        else {
            return false;
        }
    }

    //when user click new step = Create step, save in history, push into session stack
    bool recordUserNewStep(string actionType,string description, const KioskFullCondition& currentState)
    {
        //check limit
        if (isSessionStackFull()) {
            cout << "\nSession Stack already full. New step cannot be recorded.";
            return false; //stop if limit
        }

        //create UserStepInKiosk (stepNumber, actionType, desc, state)
        UserStepInKiosk newStep;
        newStep.stepNumber = nextStepNumber;
        newStep.actionType = actionType;
        newStep.description = description;
        newStep.state = currentState;

        //save in completeHistory bfr push
        completeHistory[completeHistoryCount] = newStep;
        completeHistoryCount++;

        //create dynamic node for stack and push into stack
        SessionNode* newNode = new SessionNode;
        newNode -> step = newStep;
        newNode -> next = top;
        top = newNode;

        //update counters
        stackSize++;
        nextStepNumber++;

        cout << "\nStep recorded: " << newStep.description << endl;
        return true;
    }

    bool navigateUserBackStep(KioskFullCondition& restoredState)
    {
        //Check whethher empty or at beginning
        if (isSessionStackEmpty() || isSessionStackAtBeginning()) {
            cout << "\nAlready at beginning. No earlier step is available." << endl;
            return false;
        }

        //save current top in navigationTrace
        navigationTrace[navigationTraceCount] = top -> step;
        navigationTraceCount++;

        //pop, then delete current top
        SessionNode* currentTopNode = top;
        top = top -> next;
        delete currentTopNode;
        stackSize--;

        //new top become current state
        //then return new top's KioskState
        restoredState = top -> step.state;

        cout << "\nSuccessful Back navigation." << endl;
        cout << "Left step: " << navigationTrace[navigationTraceCount - 1].description << endl;
        cout << "Returned to: " << top -> step.description << endl;

        return true;
    }

    //show all action include done & undone
    void displayFullSessionHistory()
    {
        if (completeHistoryCount == 0) {
            cout << "\nNo history." << endl;
            return;
        }

        //Session history: step | action type | description (for all)
        cout << "\n--- Session History ---" <<endl;
        for (int i = 0; i < completeHistoryCount; i++) {
            UserStepInKiosk currentStep = completeHistory[i];

            cout << "Step " << currentStep.stepNumber
            << " | " << currentStep.actionType
            << " | " << currentStep.description << endl;
        }
    }

    //show current state. the most top of stack
    void displaySessionCurrentState()
    {
        if (isSessionStackEmpty()) {
            cout << "\nNo current state in this session history." << endl;
            return;
        }

        KioskFullCondition currentState = top -> step.state; //load top as current state

        cout << "\n--- Current Kiosk State ---" << endl;
        cout << "Current Screen: " << currentState.screenName << endl;
        cout << "Cuisine Type: " << currentState.selectedCuisineType << endl;
        cout << "Items in the Cart: " << currentState.cartCount << endl;

        //item in cart:  - name x RMx
        for (int i = 0; i < currentState.cartCount; i++)
        {
            ItemInsideCart currentItem = currentState.cartItem[i];
            cout << "  - " << currentItem.itemName << " x" << currentItem.quantity << " (RM" << fixed << setprecision(2) << currentItem.totalItemPrice << ")" << endl;
        }

        //then total price all items in cart
        cout << "Total: RM" << fixed << setprecision(2) << currentState.total << endl;
    }

    //Back. show actions before.
    void displayBackNavigationTrace()
    {
        if (navigationTraceCount == 0) {
            cout << "\nNo navigation yet. Perform action to back." << endl;
            return;
        }

        //Navigation trace: step | action type | description (all past only)
        cout << "\n--- Navigation Trace ---" << endl;
        for (int i = 0; i < navigationTraceCount; i++) {

            UserStepInKiosk currentStep = navigationTrace[i];
            cout << "Step " << currentStep.stepNumber << " | " << currentStep.actionType << " | " << currentStep.description << endl;
        }
    }

    //Show remaining navigable steps
    void displayCurrentActiveStack()
    {
        if (isSessionStackEmpty()) {
            cout << "\nNo active stack." << endl;
            return;
        }

        //active stack: step | action type | description (only active)
        cout << "\n--- Active Stack ---" << endl;
        SessionNode* current = top; //start with top
        while (current != nullptr) { //repeat until bottom
            cout << "Step " << current -> step.stepNumber << " | " << current -> step.actionType << " | " << current -> step.description << endl;
            current = current -> next;
        }
    }

    void clearSessionHistory()
    {
        while (top != nullptr){
            SessionNode* nodeToDelete = top;
            top = top -> next;
            delete nodeToDelete;
        }

        stackSize = 0; // reset counter to match
        nextStepNumber = 1;
        completeHistoryCount = 0;
        navigationTraceCount = 0;
    }
};

// display each Session History output separately for the student
void sessionHistoryAndNavigationMenu(SessionHistory& sessionHistory)
{
    int historyChoice;

    do {
        cout << "\n===== SESSION HISTORY AND NAVIGATION =====" << endl;
        cout << "1. View Complete Session History" << endl;
        cout << "2. View Current Kiosk State" << endl;
        cout << "3. View Back Navigation Trace" << endl;
        cout << "4. View Remaining Active Stack" << endl;
        cout << "0. Return to Food Menu" << endl;
        cout << "Enter choice: ";

        if (!(cin >> historyChoice)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            historyChoice = -1;
            continue;
        }

        switch (historyChoice) {
            case 1:
                sessionHistory.displayFullSessionHistory();
                break;

            case 2:
                sessionHistory.displaySessionCurrentState();
                break;

            case 3:
                sessionHistory.displayBackNavigationTrace();
                break;

            case 4:
                sessionHistory.displayCurrentActiveStack();
                break;

            case 0:
                cout << "Returning to food menu..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (historyChoice != 0);
}

// Task 1: Order Queue Management Module (BALQIS SYAZWINA BINTI HISYAM TP077278)
// store pending orders using linked list queue

const int MAX_QUEUE_SIZE = 50;

// data structures

// node for one pending order in the queue
struct QueueNode {
    Order data;
    CartItem items[20];
    int itemCount;
    QueueNode *next;
};

struct CompletedNode {
    Order data;
    CartItem items[20];
    int itemCount;
    CompletedNode *next;
};

// linked list queue used to manage pending orders
class OrderQueue {
    private:
        QueueNode *front;
        QueueNode *rear;
        int queueCount;
        CompletedNode *completedHead;
        int completedCount;

    public:
        OrderQueue();

        bool isEmpty();
        bool isFull();
        int getQueueCount();

        bool enqueue(Order newOrder, CartItem cartItems[], int cartCount);
        Order dequeue(CartItem removedItems[], int &removedItemCount);
        void addCompletedOrder(Order completedOrder, CartItem completedItems[], int completedItemCount);

        void displayPendingOrders();
        void displayCompletedOrders();
        void displayCompletedOrderItems(CompletedNode *node);
        void displayOrderItems(QueueNode *node);
        void viewFrontOrderDetails();

        Order getFrontOrder();
};

OrderQueue::OrderQueue() {
    front = NULL;
    rear = NULL;
    queueCount = 0;
    completedHead = NULL;
    completedCount = 0;
}

// check if there is no pending orders
bool OrderQueue::isEmpty() {
    if (front == NULL)
        return true;
    else
        return false;
}

// limit queue to not store more than 50 orders
// O(1) - compares queueCount wth max queue size only
bool OrderQueue::isFull() {
    if (queueCount >= MAX_QUEUE_SIZE)
        return true;
    else
        return false;
}

int OrderQueue::getQueueCount() {
    return queueCount;
}

// if order is the first order, front and rear point to same node
// O(n) - insert directly using rear pointer without searching through list
bool OrderQueue::enqueue(Order newOrder, CartItem cartItems[], int cartCount) {
    if (isFull()) {
        cout << "\nOrder queue is full. Cannot accept more orders right now." << endl;
        return false;
    }

    QueueNode *newNode = new QueueNode;

    newNode->data = newOrder;
    newNode->itemCount = cartCount;

    for (int i = 0; i < cartCount; i++) {
        newNode->items[i] = cartItems[i];
    }

    newNode->next = NULL;

    if (front == NULL) {
        front = newNode;
        rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }

    queueCount++;
    return true;
}

// remove n return next pending order from the front
Order OrderQueue::dequeue(CartItem removedItems[], int &removedItemCount) {
    Order removedOrder;

    if (isEmpty()) {
        removedOrder.orderID = "";
        removedItemCount = 0;
        return removedOrder;
    }

    // O(1) - direct access to the first node using front pointer
    QueueNode *temp = front;
    removedOrder = front->data;
    removedItemCount = front->itemCount;

    for (int i = 0; i < front->itemCount; i++) {
        removedItems[i] = front->items[i];
    }

    front = front->next;

    if (front == NULL) {
        rear = NULL;
    }

    // free removed node after moving the front pointer
    delete temp;
    queueCount--;

    return removedOrder;
}

// add an order into completed order list
void OrderQueue::addCompletedOrder(Order completedOrder, CartItem completedItems[], int completedItemCount) {
    CompletedNode *newNode = new CompletedNode;

    newNode->data = completedOrder;
    newNode->itemCount = completedItemCount;

    for (int i = 0; i < completedItemCount; i++) {
        newNode->items[i] = completedItems[i];
    }

    newNode->next = NULL;

    if (completedHead == NULL) {
        completedHead = newNode;
    } else {
        CompletedNode *current = completedHead;

        while (current->next != NULL) {
            current = current->next;
        }

        current->next = newNode;
    }

    completedCount++;
}

void OrderQueue::displayOrderItems(QueueNode *node) {
    cout << "\nItems:" << endl;

    cout << left
         << setw(6) << "No."
         << setw(30) << "Item Name"
         << setw(8) << "Qty"
         << "Total Price" << endl;

    cout << string(60, '-') << endl;

    for (int i = 0; i < node->itemCount; i++) {
        cout << left
             << setw(6) << i + 1
             << setw(30) << node->items[i].itemName
             << setw(8) << node->items[i].quantity
             << "RM" << fixed << setprecision(2) << node->items[i].lineTotal
             << endl;
    }

    cout << string(60, '-') << endl;
}

// display items inside a completed order
void OrderQueue::displayCompletedOrderItems(CompletedNode *node) {
    cout << "\nItems:" << endl;

    cout << left
         << setw(6) << "No."
         << setw(30) << "Item Name"
         << setw(8) << "Qty"
         << "Total Price" << endl;

    cout << string(60, '-') << endl;

    for (int i = 0; i < node->itemCount; i++) {
        cout << left
             << setw(6) << i + 1
             << setw(30) << node->items[i].itemName
             << setw(8) << node->items[i].quantity
             << "RM" << fixed << setprecision(2) << node->items[i].lineTotal
             << endl;
    }

    cout << string(60, '-') << endl;
}

// show all pending orders and allow user to view items in the order
void OrderQueue::displayPendingOrders() {
    if (isEmpty()) {
        cout << "\nNo pending orders. Queue is empty." << endl;
    } else {
        QueueNode *current = front;
        QueueNode *pendingList[100];
        int displayNumber = 0;

        cout << "\n===== Pending Orders =====" << endl;
        cout << left
             << setw(6) << "No."
             << setw(12) << "Order ID"
             << setw(14) << "Student ID"
             << setw(12) << "Cuisine"
             << setw(12) << "Status"
             << setw(12) << "Stall"
             << setw(10) << "Total"
             << "Order Time" << endl;

        cout << string(97, '-') << endl;

        // O(n) - visit each node from front to rear
        while (current != NULL) {
            pendingList[displayNumber] = current;
            displayNumber++;

            cout << left
                 << setw(6) << displayNumber
                 << setw(12) << current->data.orderID
                 << setw(14) << current->data.studentID
                 << setw(12) << current->data.cuisineType
                 << setw(12) << current->data.orderStatus
                 << setw(12) << current->data.assignedStallID
                 << "RM"
                 << setw(8) << fixed << setprecision(2) << current->data.total
                 << current->data.orderTime
                 << endl;

            current = current->next;
        }

        cout << string(97, '-') << endl;
        cout << "\nTotal Pending Orders: " << queueCount << endl;

        int selectedNo;
        cout << "\nEnter pending order number to view details, or 0 to return: ";

        if (!(cin >> selectedNo)) {
            cout << "\nInvalid input. Returning to menu." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        if (selectedNo >= 1 && selectedNo <= displayNumber) {
            QueueNode *selectedOrder = pendingList[selectedNo - 1];

            cout << "\n===== Pending Order Details =====" << endl;
            cout << "Order ID: " << selectedOrder->data.orderID << endl;
            cout << "Student ID: " << selectedOrder->data.studentID << endl;
            cout << "Cuisine Type: " << selectedOrder->data.cuisineType << endl;
            cout << "Status: " << selectedOrder->data.orderStatus << endl;
            cout << "Total: RM" << fixed << setprecision(2) << selectedOrder->data.total << endl;
            cout << "Order Time: " << selectedOrder->data.orderTime << endl;

            displayOrderItems(selectedOrder);
        } else if (selectedNo == 0) {
            cout << "\nReturning to Order Queue Management menu." << endl;
        } else {
            cout << "\nInvalid order number." << endl;
        }
    }
}

// show all completed orders n allow user to view items in the order
void OrderQueue::displayCompletedOrders() {
    if (completedHead == NULL) {
        cout << "\nNo completed orders yet." << endl;
    } else {
        CompletedNode *current = completedHead;
        CompletedNode *completedList[100];
        int displayNumber = 0;

        cout << "\n===== Completed Orders =====" << endl;
        cout << left
             << setw(6) << "No."
             << setw(12) << "Order ID"
             << setw(14) << "Student ID"
             << setw(12) << "Cuisine"
             << setw(12) << "Status"
             << setw(12) << "Stall"
             << setw(10) << "Total"
             << "Order Time" << endl;

        cout << string(97, '-') << endl;

        while (current != NULL) {
            completedList[displayNumber] = current;
            displayNumber++;

            cout << left
                 << setw(6) << displayNumber
                 << setw(12) << current->data.orderID
                 << setw(14) << current->data.studentID
                 << setw(12) << current->data.cuisineType
                 << setw(12) << current->data.orderStatus
                 << setw(12) << current->data.assignedStallID
                 << "RM"
                 << setw(8) << fixed << setprecision(2) << current->data.total
                 << current->data.orderTime
                 << endl;

            current = current->next;
        }

        cout << string(97, '-') << endl;
        cout << "\nTotal Completed Orders: " << completedCount << endl;

        int selectedNo;
        cout << "\nEnter completed order number to view details, or 0 to return: ";

        if (!(cin >> selectedNo)) {
            cout << "\nInvalid input. Returning to menu." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            return;
        }

        if (selectedNo >= 1 && selectedNo <= displayNumber) {
            CompletedNode *selectedOrder = completedList[selectedNo - 1];

            cout << "\n===== Completed Order Details =====" << endl;
            cout << "Order ID: " << selectedOrder->data.orderID << endl;
            cout << "Student ID: " << selectedOrder->data.studentID << endl;
            cout << "Cuisine Type: " << selectedOrder->data.cuisineType << endl;
            cout << "Status: " << selectedOrder->data.orderStatus << endl;
            cout << "Total: RM" << fixed << setprecision(2) << selectedOrder->data.total << endl;
            cout << "Order Time: " << selectedOrder->data.orderTime << endl;

            displayCompletedOrderItems(selectedOrder);
        } else if (selectedNo == 0) {
            cout << "\nReturning to Order Queue Management menu." << endl;
        }
        else
        {
            cout << "\nInvalid order number." << endl;
        }
    }
}

// return next order wout removing it from the queue
// O(1) - only read front node
Order OrderQueue::getFrontOrder()
{
    Order frontOrder;

    if (isEmpty())
    {
        frontOrder.orderID = "";
        return frontOrder;
    }

    return front->data;
}

// display the next pending order + item details
void OrderQueue::viewFrontOrderDetails() {
    if (isEmpty()) {
        cout << "No pending orders." << endl;
    } else {
        cout << "\n===== Next Order =====" << endl;
        cout << "Order ID: " << front->data.orderID << endl;
        cout << "Student ID: " << front->data.studentID << endl;
        cout << "Cuisine Type: " << front->data.cuisineType << endl;
        cout << "Status: " << front->data.orderStatus << endl;
        cout << "Total: RM" << fixed << setprecision(2) << front->data.total << endl;
        cout << "Order Time: " << front->data.orderTime << endl;

        displayOrderItems(front);
    }
}

// Task 2: Stall Assignment module (NURUN NAJIHAH BINTI A. AZMI TP077342)
const int stall_MAX = 50;
const int assigned_MAX = 50;
const int ordered_MAX = 20;

struct Stall{
    string stallID;
    string stallName;
    string cuisineType;
    string stallStatus;
    int capacity;
    int currentOrderCount;
    int rotationPos;
};

// store each assign order
struct assignedOrder{
    Order order;
    CartItem items[ordered_MAX];
    int itemCount;
};

// class circular queue begins
class StallCircularQueue{

// store each stalls
private:
    Stall stalls[stall_MAX];
    int countSTALL;

    // each of the cruisinetype has each pointer
    int malay_Position;
    int chinese_Position;
    int western_Position;
    int indian_Position;
    int beveragesPosition;
    int healthyFoodPosition;

public:

    StallCircularQueue(){
	// start with no stalls
        countSTALL = 0;

	// pointer does not exist yet
        malay_Position = -1;
        chinese_Position = -1;
        western_Position = -1;
        indian_Position = -1;
        beveragesPosition = -1;
        healthyFoodPosition = -1;
    }

    bool isEmpty(){
        return countSTALL==0;
    }

    bool isFull(){
        return countSTALL>=stall_MAX;
    }

    int getcountSTALL(){
        return countSTALL;
    }

    Stall &getStall(int index){
        return stalls[index];
    }

bool addStall(Stall loadStall){

    if (isFull()){
        cout << "\nStall circular queue is full." << endl;
        return false;
    }

    stalls[countSTALL]=loadStall;

    // Save the first stall index for each cuisine.
    if (loadStall.cuisineType=="Malay" && malay_Position==-1){
        malay_Position=countSTALL;
    }

    else if (loadStall.cuisineType=="Chinese" && chinese_Position==-1){
        chinese_Position = countSTALL;
    }

    else if (loadStall.cuisineType=="Western" && western_Position==-1){
        western_Position = countSTALL;
    }

    else if (loadStall.cuisineType=="Indian" && indian_Position==-1){
        indian_Position = countSTALL;
    }

    else if (loadStall.cuisineType=="Beverages" && beveragesPosition==-1){
        beveragesPosition = countSTALL;
    }

    else if (loadStall.cuisineType=="Healthy Food" && healthyFoodPosition==-1){
        healthyFoodPosition = countSTALL;
    }

    countSTALL++;
    return true;
}

// return the current pointer index to check where the last circular stops
int getPointer(string type){
    if (type=="Malay") {
        return malay_Position;
    }

    if (type=="Chinese") {
        return chinese_Position;
    }

    if (type=="Western") {
        return western_Position;
    }

    if (type=="Indian") {
        return indian_Position;
    }

    if (type=="Beverages") {
        return beveragesPosition;
    }

    if (type=="Healthy Food") {
        return healthyFoodPosition;
    }

    return -1;
}

// change the pointer to the next index
void setPointer(string type, int newP){
    if (type=="Malay") {
        malay_Position=newP;
    }

    else if (type=="Chinese") {
        chinese_Position=newP;
    }

    else if (type=="Western") {
        western_Position=newP;
    }

    else if (type=="Indian") {
        indian_Position=newP;
    }

    else if (type=="Beverages") {
        beveragesPosition=newP;
    }

    else if (type=="Healthy Food") {
        healthyFoodPosition=newP;
    }
}

// count how many stalls for each type
int countSTALLSeach(string type){
    int count = 0;

    for (int i=0;i<countSTALL;i++){
        if (stalls[i].cuisineType==type){
            count++;
        }
    }
        return count;
}

int getnextSTALLindex(string type,int current){

    if (countSTALL==0){
            return -1;
    }

    // use modulo -- if at index 10 then back to index 0 create circular queue
    int index=(current+1) % countSTALL;

    while (index!=current){
	// if the landing index not the same as the cruisinetype, skip the stalls
        if (stalls[index].cuisineType==type){
            return index;
        }
            index = (index+1) % countSTALL;
    }

        if (stalls[current].cuisineType==type){
            return current;
    }

        return -1;
}

// check the possible stall to assign the order
int eligibleSTALL(string cuisine){

    // take the current pointer
    int startPosition=getPointer(cuisine);

    if (startPosition==-1){
            return -1;
        }

    int allSTALL=countSTALLSeach(cuisine);

    if (allSTALL==0){
        return -1;
    }

    // currentIndex is the current stall
    int currentIndex = startPosition;
    int checkedCount=0;

    // max looping below than the total of the stall of each cruisine stall
    while (checkedCount<allSTALL){
        Stall &currentStall = stalls[currentIndex];

	// check the stall status
        bool isOpen = currentStall.stallStatus!="Closed";
        // check the stall capacity
        bool hasCapacity = currentStall.currentOrderCount <currentStall.capacity;

        // stall must open and has capacity to assign order
        if (isOpen&&hasCapacity){
            return currentIndex;
            }

        currentIndex = getnextSTALLindex(cuisine,currentIndex);
        checkedCount++;
    }

    return -1;
}

// move to next pointer after successful order assignment
void moveNext(string type,int stallIndex){

    int nextPosition=getnextSTALLindex(type,stallIndex);

        if (nextPosition!=-1){
            setPointer(type,nextPosition);
    }
}

// easily trace why the stall is selected
void rotationTrace(string type,int stallIndex){
// get the current pointer
    int startPosition = getPointer(type);
    int allSTALL = countSTALLSeach(type);

// if no stall on that type existed
    if (startPosition == -1||allSTALL == 0){
        cout << "\nNo stall rotation exists for "<< type<< "."<< endl;

        return;
    }

    cout << "\n              "<< type<< " Rotation Check        "<< endl;
    cout << "--------------------------------------------------------------" << endl;

    cout << "Current Pointer :"<< stalls[startPosition].stallID<< " - "<< stalls[startPosition].stallName<< endl;

    int currentIndex = startPosition;
    int checked = 0;

// max based on how many stalls foe each cuisine
    while(checked<allSTALL){

        Stall &currentS = stalls[currentIndex];

        cout << "\n["<< currentS.stallID<< " - "<< currentS.stallName<< "]" <<endl;
        cout << "Status   : "<< currentS.stallStatus<< endl;
        cout << "Capacity : " << currentS.currentOrderCount<< "/"<< currentS.capacity<< endl;

        if (currentS.stallStatus=="Closed"){
            cout << "Result   : Skipped - Stall is closed"<< endl;
        }

        else if (currentS.currentOrderCount>=currentS.capacity){
            cout << "Result   : Skipped - Stall is at capacity"<< endl;
        }

        else if (currentIndex == stallIndex){
            cout << "Result   : Stall selected for next pending order"<< endl;

            break;
        }

        else{
            cout << "Result   : Not selected"<< endl;
        }

        currentIndex =getnextSTALLindex(type,currentIndex);
        checked++;

        if (checked<allSTALL){
            cout << "\nRotating to next "<< type << " stall..."<< endl;
        }
    }
}

void displaySTALLS(){

    if (isEmpty()){
        cout << "\nNo stall records are available."<< endl;

        return;
    }

    cout << "\n------------------------- **** Stall Status Overview **** -------------------------"<< endl;
    cout << "Note: Each cuisine type has its own circular rotation sequence.\n"<< endl;

    cout << left
         << setw(6)<<"No."
         << setw(10)<<"Stall ID"
         << setw(25)<<"Stall Name"
         << setw(17)<<"Cuisine"
         << setw(13)<<"Status"
         << setw(13)<<"Capacity"
         << "Rotation"
         << endl;

    cout << string(90,'-') << endl;

    for(int i=0;i<countSTALL;i++){

        // merge the current order with capacity
        string mergecapacity=to_string(stalls[i].currentOrderCount) +"/" +to_string(stalls[i].capacity);

        cout << left
             << setw(6)  << i + 1
             << setw(10) << stalls[i].stallID
             << setw(25) << stalls[i].stallName
             << setw(17) << stalls[i].cuisineType
             << setw(13) << stalls[i].stallStatus
             << setw(13) << mergecapacity << stalls[i].rotationPos
             << endl;
        }

    cout << string(90, '-') << endl;

    cout << "Total Stalls: "<< countSTALL<< endl;
}

// update stall.txt
bool updateSTALLS(){

    ofstream file("TempStall.txt");

    if (!file){
        cout << "\nUnable to create TempStall.txt." << endl;
        return false;
    }

    file << "stallID|stallName|cuisineType|stallStatus|"
         << "maxCapacity|currentOrderCount|rotationPos"
         << endl;

    for (int i = 0; i < countSTALL; i++){
        file << stalls[i].stallID << "|"
             << stalls[i].stallName << "|"
             << stalls[i].cuisineType << "|"
             << stalls[i].stallStatus << "|"
             << stalls[i].capacity << "|"
             << stalls[i].currentOrderCount << "|"
             << stalls[i].rotationPos
             << endl;
    }

    file.close();

    if (remove("Stall.txt") != 0){
        cout << "\nUnable to remove the old Stall.txt." << endl;
        remove("TempStall.txt");
        return false;
    }

    if (rename("TempStall.txt", "Stall.txt") != 0){
        cout << "\nUnable to replace Stall.txt." << endl;
        return false;
    }

    return true;
}

// display according to each cuisineType
void cuisineRotation(string Crtype){

    // get the current position
    int current = getPointer(Crtype);
    // count how many stall for each type
    int allSTALL = countSTALLSeach(Crtype);

    cout << "\n[" <<Crtype<< " Rotation]"          << endl;

    if (current==-1||allSTALL == 0){
        cout << "No stalls available." << endl;
        return;
    }

    cout << "Rotation cycle   : ";

    bool firstStall = true;
    string firstStallID = "";

    for (int i = 0; i < countSTALL; i++){

        if (stalls[i].cuisineType == Crtype){

            if (firstStallID == ""){
                firstStallID = stalls[i].stallID;
            }

            if (!firstStall){
                cout << " -> ";
            }

            cout << stalls[i].stallID;
            firstStall = false;
        }
    }

    cout << " -> " << firstStallID << endl;
    cout << "Current Rotation : "<< stalls[current].stallID<< " - "<< stalls[current].stallName;

    if (stalls[current].stallStatus == "Closed"){
        cout << " (Closed)";
    }

    else if (stalls[current].currentOrderCount>=stalls[current].capacity){
        cout << " (At Capacity)";
    }

    cout << endl;
}

// show all rotation and the current rotation
void displayALLRotations(){

    if (isEmpty()){

        cout << "\nNo stall records are available."<< endl;
        return;
        }

        cout << "\n--------- *** Current Cuisine Rotations *** ---------"<< endl;
        cuisineRotation("Malay");
        cuisineRotation("Chinese");
        cuisineRotation("Western");
        cuisineRotation("Indian");
        cuisineRotation("Beverages");
        cuisineRotation("Healthy Food");
    }

void manageSTALLS(){

    displaySTALLS();

    int stallNo;

    cout << "\nEnter stall number (0 to cancel): ";

    if (!(cin >> stallNo)){
        cout << "\nInvalid input! Please enter a number." << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    if (stallNo==0){
        return;
    }

    if (stallNo<1||stallNo>getcountSTALL()){
        cout << "\nInvalid stall number! Please try again." << endl;
        return;
    }

    Stall &selectedStall=getStall(stallNo-1);

    int choice;

    cout << "\n1. Open Stall" << endl;
    cout << "2. Close Stall" << endl;
    cout << "\n0. Cancel" << endl;
    cout << "Enter choice: ";

    if (!(cin >> choice)){
        cout << "\nInvalid input. Please enter a number." << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    switch (choice)
    {
        case 1:

            if (selectedStall.stallStatus!="Closed"){
                cout << "\nThis stall is already open." << endl;
                break;
            }


            selectedStall.stallStatus = "Available";
            // update stall status
            updateSTALLS();
            cout << "\nStall has been opened successfully!" << endl;

            break;

        case 2:
            // if status=closed
            if (selectedStall.stallStatus == "Closed"){
                cout << "\nThis stall is already closed." << endl;
                break;
            }

            // restrict the stall from closing when theres order
            if (selectedStall.currentOrderCount > 0){
                cout << "\nUnable to close stall as it currently handling " << selectedStall.currentOrderCount << " active orders."<< endl;
                break;
            }

            selectedStall.stallStatus = "Closed";
            updateSTALLS();
            cout << "\nStall has been closed successfully!" << endl;

            break;

        case 0:
            cout << "\nOperation cancelled." << endl;
            break;

        default:
            cout << "\nInvalid choice." << endl;
   	 }
}
};    // circular queue ends here

// declare global variable
StallCircularQueue circularQueue;
assignedOrder orderA[assigned_MAX];

int countOrder=0;
bool isLoaded = false;
bool A_orderLoaded = false;

void updateSTALLavailable(int stallIndex){

    Stall &stall = circularQueue.getStall(stallIndex);

    if (stall.stallStatus == "Closed")
        return;

    if (stall.currentOrderCount > 0)
        stall.stallStatus = "Busy";

    else
        stall.stallStatus = "Available";
}

// retrieve the cruisine type
string getSTALLtype(string line, int fieldnum){

    string field = "";
    int current = 0;

    for (int i=0;i<static_cast<int>(line.length());i++){

        if (line[i] == '|'){
            current++;
        }
        else if (current == fieldnum){
            field += line[i];
        }
    }

    return field;
}

// update order.txt
bool updateORDER(const Order &updated){
    ifstream inputFile("Order.txt");
    ofstream tempFile("TempOrder.txt");

    if (!inputFile) {
        cout << "\nUnable to open Order.txt." << endl;
        return false;
    }

    if (!tempFile){
        cout << "\nUnable to create TempOrder.txt." << endl;
        inputFile.close();
        return false;
    }

    string line;
    bool found = false;

    // copy the header
    if (getline(inputFile, line)){
        tempFile << line << endl;
    }

    while (getline(inputFile, line)){
        if (line == ""){
            continue;
        }

        string ID = getSTALLtype(line, 0);

        if (ID == updated.orderID){
            tempFile << updated.orderID << "|"
                     << updated.studentID << "|"
                     << updated.cuisineType << "|"
                     << updated.orderStatus << "|"
                     << updated.assignedStallID << "|"
                     << updated.total << "|"
                     << updated.orderTime
                     << endl;

            found = true;
        }

        else{
            tempFile << line << endl;
        }
    }

    inputFile.close();
    tempFile.close();

    if (!found){
        remove("TempOrder.txt");
        cout << "\nOrder ID not found in Order.txt." << endl;
        return false;
    }

    remove("Order.txt");

    if (rename("TempOrder.txt", "Order.txt") != 0){
        cout << "\nUnable to replace Order.txt." << endl;
        return false;
    }

    return true;
}

// retrieve the index for the selected stall
// help for reducing capacity
int retrieveSTALLindex(string id){

    for (int i=0;i<circularQueue.getcountSTALL();i++){

        if (circularQueue.getStall(i).stallID == id){
            return i;
        }
    }

    return -1;
}

// decrement the assigned order array after the status is completed
void removeOrder(int index){

    for(int i=index;i<countOrder-1;i++){
        // change the postion
        orderA[i]=orderA[i+1];
    }

    countOrder--;
}

// load all the stall in Stall.txt
void loadSTALLS(){

    if (isLoaded){
        return;
    }

    ifstream file("Stall.txt");

    if (!file){
        cout << "\nStall.txt could not be opened."<< endl;
        cout << "Ensure Stall.txt is inside the project folder."<< endl;
        return;
    }

    string line;
    getline(file, line);    // skip header
    while (getline(file, line)){

        if (line == ""){
            continue;
        }

        Stall S_add;

        S_add.stallID =getSTALLtype(line, 0);
        S_add.stallName =getSTALLtype(line, 1);
        S_add.cuisineType =getSTALLtype(line, 2);
        S_add.stallStatus =getSTALLtype(line, 3);
        S_add.capacity =stoi(getSTALLtype(line, 4));
        S_add.currentOrderCount =stoi(getSTALLtype(line, 5));
        S_add.rotationPos =stoi(getSTALLtype(line, 6));
        circularQueue.addStall(S_add);
    }

    file.close();

    isLoaded = true;
    cout << "\n"<< circularQueue.getcountSTALL()<< " stall records loaded successfully."<< endl;
}

void loadORDERS(){
    ifstream file("Order.txt");

    if (!file){
        cout << "\nUnable to open Order.txt." << endl;
        return;
    }

    countOrder = 0;

    string line;
    getline(file, line); // skip header

    while (getline(file, line)){
        if (line == "") {
            continue;
        }

        string status = getSTALLtype(line, 3);

        if (status=="Assigned" || status=="Preparing"){
            if (countOrder>=assigned_MAX){
                break;
            }

            Order loadedOrder;

            loadedOrder.orderID = getSTALLtype(line, 0);
            loadedOrder.studentID = getSTALLtype(line, 1);
            loadedOrder.cuisineType = getSTALLtype(line, 2);
            loadedOrder.orderStatus = getSTALLtype(line, 3);
            loadedOrder.assignedStallID = getSTALLtype(line, 4);
            loadedOrder.total = stod(getSTALLtype(line, 5));
            loadedOrder.orderTime = getSTALLtype(line, 6);

            orderA[countOrder].order = loadedOrder;
            orderA[countOrder].itemCount = 0;

            countOrder++;
        }
    }

    file.close();
    A_orderLoaded = true;
}

// displayed all assigned order
// third menu option from Main Menu
void displayorderA(){

    if (countOrder == 0){
        cout << "\nNo active assigned orders."<< endl;
        return;
    }

    cout << "\n                              [Active Assigned Order]              "<< endl;
    cout << "---------------------------------------------------------------------------------------"<< endl;
    cout << left<< setw(6) << "No."
                << setw(12) << "Order ID"
                << setw(14) << "Student ID"
                << setw(17) << "Cuisine"
                << setw(14) << "Status"
                << setw(12) << "Stall ID"
                << "Total"
                << endl;

    cout << string(87, '-') << endl;

    for (int i=0;i<countOrder;i++){

        cout << left<< setw(6) << i + 1
                    << setw(12)<< orderA[i].order.orderID
                    << setw(14)<< orderA[i].order.studentID
                    << setw(17)<< orderA[i].order.cuisineType
                    << setw(14) << orderA[i].order.orderStatus
                    << setw(12)<< orderA[i].order.assignedStallID << "RM"<< fixed
                    << setprecision(2) << orderA[i].order.total
                    << endl;
    }

    cout << string(87, '-') << endl;
    cout << "Total Active Assigned Orders: "<< countOrder<< endl;
}

// displayed next pending order
void assignNEXT(OrderQueue &orderQueue){

    if (!isLoaded){
        loadSTALLS();
    }

    if (circularQueue.isEmpty()){
        cout << "\nNo stalls are available in the system."<< endl;
        return;
    }

    if (orderQueue.isEmpty()){
        cout << "\nNo pending orders are available."<< endl;
        return;
    }

    if (countOrder >= assigned_MAX){
        cout << "\nAssigned order storage is full."<< endl;
        return;
    }

    // Preview the front order without removing it.
    Order nextOrder = orderQueue.getFrontOrder();

    if (nextOrder.orderID == ""){
        cout << "\nUnable to retrieve the next order."<< endl;
        return;
    }

    cout << "\n----*** Next Pending Order ***----"<< endl;
    cout << "Order ID     : "<< nextOrder.orderID<< endl;
    cout << "Student ID   : "<< nextOrder.studentID<< endl;
    cout << "Cuisine Type : "<< nextOrder.cuisineType<< endl;
    cout << "Total        : RM"<< fixed<< setprecision(2)<< nextOrder.total<< endl;
    cout << "Order Time   : "<< nextOrder.orderTime<< endl;

    int chosenS_Index =circularQueue.eligibleSTALL(nextOrder.cuisineType);

    if (chosenS_Index == -1){
        /*
        Display every stall in the requested cuisine so
        the user can see whether each one is closed or full.
        */
        cout << "\n===== "<< nextOrder.cuisineType << " Rotation Check ====="<< endl;

        // current index
        int currentIx =circularQueue.getPointer(nextOrder.cuisineType);
        // count the cuisine
        int cuisineC =circularQueue.countSTALLSeach(nextOrder.cuisineType);

        if (currentIx == -1 || cuisineC == 0){
            cout << "No stalls exist for this cuisine."<< endl;
        }

        else {
            cout << "Current Pointer: "<< circularQueue.getStall(currentIx).stallID
                 << " - "<< circularQueue.getStall(currentIx).stallName<< endl;

            int checked = 0;

            while (checked < cuisineC){

                Stall &checkingS =circularQueue.getStall(currentIx);

                cout << "\nChecking "<< checkingS.stallID << " - "<< checkingS.stallName<< endl;
                cout << "Status   : "<< checkingS.stallStatus<< endl;
                cout << "Capacity : "<< checkingS.currentOrderCount << "/"<< checkingS.capacity<< endl;

                if (checkingS.stallStatus == "Closed"){
                    cout << "Result   : Skipped - Stall is closed"<< endl;
                }

                else if (checkingS.currentOrderCount >=checkingS.capacity) {
                    cout << "Result   : Skipped - Stall is at capacity"<< endl;
                }

                checked++;

                if (checked < cuisineC){
                    cout << "\nRotating to next "<< nextOrder.cuisineType<< " stall..."<< endl;
                    currentIx =circularQueue.getnextSTALLindex(nextOrder.cuisineType,currentIx);
                }
            }
        }

        cout << "\nNo eligible stall is currently available for this order."<< endl;
        cout << "Order "<< nextOrder.orderID << " remains in the pending queue."<< endl;

        return;
    }

    circularQueue.rotationTrace(nextOrder.cuisineType,chosenS_Index);
    Stall &assignStall =circularQueue.getStall(chosenS_Index);

    cout << "\nAssigning order to the selected stall..." << endl;

    // remove the assignment order
    CartItem removedItems[ordered_MAX];
    int removedItemCount = 0;

    // selected order
    Order orderS =orderQueue.dequeue(removedItems,removedItemCount);

    orderS.assignedStallID =assignStall.stallID;

    orderS.orderStatus ="Assigned";

    // update txt file
    updateORDER(orderS);

    // update the stall count order
    assignStall.currentOrderCount++;

    updateSTALLavailable(chosenS_Index);
    circularQueue.updateSTALLS() ;
    orderA[countOrder].order=orderS;
    orderA[countOrder].itemCount=removedItemCount;

    for(int i=0;i<removedItemCount;i++){
        orderA[countOrder].items[i]=removedItems[i];
    }

    countOrder++;

    // Move only the matching cuisine pointer.
    circularQueue.moveNext(orderS.cuisineType,chosenS_Index);

    // next pointer index
    int nextP_Index =circularQueue.getPointer(orderS.cuisineType);

    cout << "\n----*** Assignment Successful ***----" << endl;
    cout << "Stall ID   : "<< assignStall.stallID<< endl;
    cout << "Stall Name : "<< assignStall.stallName<< endl;
    cout << "Status     : "<< assignStall.stallStatus<< endl;
    cout << "Capacity   : "<< assignStall.currentOrderCount << "/"<< assignStall.capacity<< endl;

    if (nextP_Index != -1){
        cout << "\nNext "<< orderS.cuisineType
             << " Pointer: "<< circularQueue.getStall(nextP_Index).stallID
             << " - " << circularQueue.getStall(nextP_Index).stallName<< endl;
    }
}

// stall updating the assigned order status
void updateOrderS(OrderQueue &Queue)
{
    int orderNO;

    if (countOrder == 0){
        cout << "\nNo active assigned orders yet! " << "Please assign the next pending order first." << endl;
        return;
    }

    displayorderA();

    cout << "\nEnter order number to update status (0 to cancel): ";

    if (!(cin >> orderNO)){
        cout << "\nInvalid input! Please enter a number." << endl;
        cin.clear();
        cin.ignore(50, '\n');
        return;
    }

    if (orderNO == 0){
        cout << "\nUpdate cancelled." << endl;
        return;
    }

    if (orderNO < 1 || orderNO > countOrder){
        cout << "\nInvalid order number." << endl;
        return;
    }

    // selected order index
    int S_OrderIndex = orderNO - 1;

    assignedOrder & S_Order = orderA[S_OrderIndex];

    int statuschoice;

    do{
        cout << "\n1. Mark as Preparing" << endl;
        cout << "2. Mark as Completed" << endl;
        cout << "0. Cancel" << endl;
        cout << "\nEnter choice: ";

        if (!(cin >> statuschoice)){
            cout << "\nInvalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(50, '\n');
            continue;
        }

        string status = S_Order.order.orderStatus;

        switch (statuschoice){
            case 1: {

                if (status!="Assigned") {
                    cout << "\nOnly an Assigned order can be changed to Preparing! Please try again." << endl;
                    break;
                }

                S_Order.order.orderStatus = "Preparing";
                updateORDER(S_Order.order);

                cout << "\nOrder status updated:" << endl;
                cout << "Assigned -> Preparing" << endl;

                return;
            }

            case 2:{

                if (status!="Preparing") {
                    cout << "\nOnly a Preparing order can be changed to Completed! Please try again." << endl;
                    break;
                }

                S_Order.order.orderStatus = "Completed";
                updateORDER(S_Order.order);

                int stallIndex = retrieveSTALLindex(S_Order.order.assignedStallID);

                if (stallIndex != -1) {
                    Stall &assignedStall = circularQueue.getStall(stallIndex);

                    if (assignedStall.currentOrderCount > 0) {
                        assignedStall.currentOrderCount--;
                    }

                    updateSTALLavailable(stallIndex);
                    circularQueue.updateSTALLS();
                }

                Queue.addCompletedOrder(S_Order.order, S_Order.items, S_Order.itemCount);

                cout << "\nOrder status updated:" << endl;
                cout << "Preparing -> Completed" << endl;
                cout << "Order "<< S_Order.order.orderID << " has been completed." << endl;
                cout << "The stall capacity has been released." << endl;

                removeOrder(S_OrderIndex);

                return;
            }

            case 0:
                cout << "\nStatus update cancelled." << endl;
                break;

            default:
                cout << "\nInvalid status choice." << endl;
        }

    } while (statuschoice != 0);
}

// MAIN MENU
void stallAssignment_MainMenu(OrderQueue &orderQueue){

    // starting by loaded all the stall in the txt file
    if (!isLoaded){

        loadSTALLS();
    }

    // loaded order that are assigned and preparing
    if (!A_orderLoaded){
        loadORDERS();
    }

    int choice;

    do {

        cout << "\n      ** Stall Assignment Management  **          " << endl;
        cout << "-----------------------------------------------" << endl;
        cout << "[Queue Summary]                                " << endl;
        cout << "Total Pending Orders  : "<< orderQueue.getQueueCount() << endl;
        cout << "Active Assigned Orders: "<< countOrder;

        cout << "\n\n[Main Menu]                                   ";
        cout << "\n1. Assign Next Pending Order                 " << endl;
        cout << "2. Update Order Status"<< endl;
        cout << "3. Display Assigned Orders" << endl;
        cout << "4. Manage Stall Status" << endl;
        cout << "5. View Current Rotation"<< endl;
        cout << "\n0. Back to Main Menu"<< endl;
        cout << "\nEnter choice: ";

        if (!(cin>>choice)){
            cout << "\nInvalid input! Please enter a number."<< endl;
            cin.clear();
            cin.ignore(50,'\n');
	    choice=-1;
        }

        switch (choice){

            case 1:
                assignNEXT(orderQueue);
                break;

            case 2:
                updateOrderS(orderQueue);
                break;

            case 3:
                displayorderA();
                break;

            case 4:
                circularQueue.manageSTALLS();
                break;

            case 5:
                circularQueue.displayALLRotations();
                break;

            case 0:
                cout << "\nReturning to main menu..."<< endl;
                break;

            default:
                cout << "\nInvalid choice! Please try again."<< endl;
        }

    } while (choice != 0);
}

struct MenuItem {
    string itemID;
    string itemName;
    string itemCategory;
    string cuisineType;
    double price;
    string availability;
    int prepTime;
};

struct OrderItem {
    string orderItemID;
    string orderID;
    string itemID;
    int quantity;
    double unitPrice;
    double lineTotal;
    string custNotes;
};

// Global Data

int nextOrderNumber = 1001;
MenuItem menuItems[100];
int menuCount = 0;
int displayedItemIndex[100];
int displayedItemCount = 0;

OrderItem orderItems[200];
int orderItemCount = 0;

// Task 4: Menu Item Search and Management Module (NUR AINA DALILI BINTI MOHD RAFIX TP077312)

struct MenuNode
{
    MenuItem data;  //stores menu info
    MenuNode *left;  //after that links to small item ID
    MenuNode *right;
};
MenuNode *menuRoot = NULL;  //root Node will empty until menu are loaded)
MenuNode* createNode(MenuItem item)//create new bst node and store each menu item inside//
{
    MenuNode *newNode = new MenuNode;

    newNode->data = item; //copy the menu info
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}

MenuNode* insertNode(MenuNode *menuRoot, MenuItem item)// 	Store item details such as Item ID, name, stall, and price //
{
    if (menuRoot == NULL)
    {
        return createNode(item);
    }

    if (item.itemID < menuRoot->data.itemID)
    {
        menuRoot->left = insertNode(menuRoot->left, item);
    }
    else if (item.itemID > menuRoot->data.itemID)
    {
        menuRoot->right = insertNode(menuRoot->right, item);

    }

    return menuRoot;
}
void displayAllMenuItems(MenuNode *menuRoot, int &numCount)
{
    if (menuRoot == NULL)
    {
        return;
    }

    displayAllMenuItems(menuRoot->left, numCount); // Visit the left subtree
    cout << left<< setw(5) << ++numCount << setw(10) << menuRoot->data.itemID<< setw(30) << menuRoot->data.itemName
    << setw(15) << menuRoot->data.itemCategory << setw(15) << menuRoot->data.cuisineType
    << "RM "<< setw(8) << fixed << setprecision(2) << menuRoot->data.price
    << setw(15) << menuRoot->data.availability<< menuRoot->data.prepTime << " mins"<< endl;
    displayAllMenuItems(menuRoot->right, numCount);
}

void displayMenuTable()
{
    int numCount=0;
    cout << "\n*********** ALL MENU ITEM LIST ***********\n\n";
    cout << string(113,'-') << endl;
    cout << left<< setw(5)  << "No." << setw(10) << "Item ID"<< setw(30) << "Item Name"<< setw(15) << "Category"
    << setw(15) << "Cuisine"<< setw(11) << "Price"<< setw(15) << "Status"<< "Prep Time" << endl;
    cout << string(113,'-') << endl;
    displayAllMenuItems(menuRoot,numCount);
    cout << string(113, '-') << endl;

}
void showAllMenu() //function 1: view all items
{
    displayMenuTable();
    int displayChoice;
    if (menuRoot == NULL)
    {
        cout << "\nNo menu items available in the data.\n";
        return;
    }
    do
    {
        cout << "\nEnter 0 to return back: ";

        if (!(cin >> displayChoice))
        {
            cout << "Invalid input! Please enter 0 only.\n";
            cin.clear();cin.ignore(1000,'\n');displayChoice = -1;
        }
        else if (displayChoice != 0)
        {
            cout << "Invalid input! Please enter 0 only!.\n";
        }

    } while (displayChoice != 0);

}

void initializeMenuTree() // create the bst using all menu items from MenuItem.txt
{
    menuRoot = NULL;
    for(int i = 0; i < menuCount; i++)  // insert every menu item into the BST
    {
        menuRoot = insertNode(menuRoot,menuItems[i]);
    }
}

/* search for a menu item based on Item ID
/ returns the address of  menu node if found, if not return null*/
MenuNode* searchMenuItem(MenuNode *current, string searchID)
{
    if (current == NULL)
    {
        return NULL;
    }
    if (searchID == current->data.itemID) //menu item found
    {
        return current;
    }
    if (searchID < current->data.itemID)// if the id is smaller search left subtree
    {
        return searchMenuItem(current->left, searchID);
    }
    return searchMenuItem(current->right, searchID); //for the bigger id
}

/*function 2: search item the menu for search item (for users)*/
void searchMenu()
{
    string searchID;
    do
    {
        cout << "\n*********** SEARCH MENU ITEM ***********" << endl;
        cout << "\nEnter Item ID (Enter 0 to return): ";
        cin >> searchID;

        if (searchID == "0")
        {
            return;
        }

        MenuNode *foundMenu = searchMenuItem(menuRoot, searchID); // search for bst
        if (foundMenu != NULL) //menu found, then display
        {
            cout << endl << string(20,'-');
            cout << "\n| MENU ITEM FOUND! |\n";
            cout << string(20,'-') << endl;
            cout << "\nItem ID: " << foundMenu->data.itemID << endl;
            cout << "Item Name: " << foundMenu->data.itemName << endl;
            cout << "Category: " << foundMenu->data.itemCategory << endl;
            cout << "Cuisine: " << foundMenu->data.cuisineType << endl;
            cout << "Price: RM " << fixed << setprecision(2)<< foundMenu->data.price << endl;  //fixed setprecision for float RM0.50 like that
            cout << "Availability: " << foundMenu->data.availability << endl;
            cout << "Prep Time: " << foundMenu->data.prepTime << " mins" << endl;
        }
        else
        {
            cout << "\nMenu Item not found. Please search again!\n";
        }

    } while(true);
}


bool isValidItemID(string itemID) // chech the item id follows the txt formal
{
    if (itemID.length() != 5) // itemid need have 5 char only for the verification
    {return false;}
    if (itemID[0] != 'M' || itemID[1] != 'I')// first needs to char M and I
    {return false;}
    if (!isdigit(itemID[2]) ||!isdigit(itemID[3]) ||!isdigit(itemID[4])) //last 3 is a digit
    {return false;}
    return true;
}

void insertMenu() // 	Insert new menu items into the system // funtion 3: Add new item
{
    MenuItem newItem;
    cout << "\n*********** INSERT NEW MENU ITEM ***********\n";

    while (true) // keep asking until the Item ID is valid
    {
        cout << "\nEnter Item ID (E.g: MI025): ";
        cin >> newItem.itemID;

        if (!isValidItemID(newItem.itemID)) // check id format needs to be MI ___ 3 number
        {
            cout << "Invalid Item ID format! Please use the format MI001, MI002, MI025." << endl;
            continue;
        }

        if (searchMenuItem(menuRoot, newItem.itemID) != NULL) //check for duplicate id
        {
            cout << "This Item ID already exists! Please enter another Item ID" << endl;
            continue;
        }
        break;  // correct format and does not exist
    }cin.ignore();
    do
    {
        cout << "Enter Item Name: "; //menu name and cannot be empty
        getline(cin, newItem.itemName);

        if(newItem.itemName.empty())
        {
            cout << "Item Name cannot be empty!\n";
        }

    }while(newItem.itemName.empty());

    int categoryChoice;
    do
    {
        cout << "\n------------------------" << endl;
        cout << "Item Category" << endl;
        cout << "------------------------" << endl;
        cout << "1. Main Meal" << endl;
        cout << "2. Snack" << endl;
        cout << "3. Beverage" << endl;
        cout << "4. Dessert" << endl;
        cout << "5. Side Dish" << endl;
        cout << "\nEnter your choice (1-5): ";

        if (!(cin >> categoryChoice))
        {
            cout << "\nInvalid input! Please enter 1-5 only.\n";
            cin.clear();cin.ignore(1000, '\n');categoryChoice = 0;
            continue;
        }

        switch (categoryChoice)
        {
            case 1:
                newItem.itemCategory = "Main Meal";
                break;

            case 2:
                newItem.itemCategory = "Snack";
                break;

            case 3:
                newItem.itemCategory = "Beverage";
                break;

            case 4:
                newItem.itemCategory = "Dessert";
                break;

            case 5:
                newItem.itemCategory = "Side Dish";
                break;

            default:
                cout << "\nInvalid choice! Please enter 1-5 only.\n";
        }

    } while (categoryChoice < 1 || categoryChoice > 5);

    int cuisineChoice;
    do
    {
        cout << "\n------------------------" << endl;
        cout << "Cuisine Type" << endl;
        cout << "------------------------" << endl;
        cout << "1. Malay" << endl;
        cout << "2. Chinese" << endl;
        cout << "3. Indian" << endl;
        cout << "4. Western" << endl;
        cout << "5. Beverages" << endl;
        cout << "6. Healthy Food" << endl;
        cout << "\nEnter your choice (1-6): ";

        if (!(cin >> cuisineChoice))
        {
            cout << "\nInvalid input! Please enter 1-6 only.\n";
            cin.clear();cin.ignore(1000,'\n');cuisineChoice = 0;
            continue;
        }

        switch (cuisineChoice)
        {
            case 1:
                newItem.cuisineType = "Malay";
                break;

            case 2:
                newItem.cuisineType = "Chinese";
                break;

            case 3:
                newItem.cuisineType = "Indian";
                break;

            case 4:
                newItem.cuisineType = "Western";
                break;

            case 5:
                newItem.cuisineType = "Beverages";
                break;

            case 6:
                newItem.cuisineType = "Healthy Food";
                break;

            default:
                cout << "\nInvalid choice! Please enter 1-6 only.\n";
        }

    } while (cuisineChoice < 1 || cuisineChoice > 6);

    do
    {
        cout << "\nEnter Price (RM): ";

        if (!(cin >> newItem.price))
        {
            cout << "Invalid input! Please enter numbers only.\n";
            cin.clear();cin.ignore(1000,'\n');newItem.price = -1;continue;
        }

        if (newItem.price <= 0)
        {
            cout << "Price must be greater than RM0.\n";
        }

    } while (newItem.price <= 0);


    int availabilityChoice;
    do
    {
        cout << "\n------------------------" << endl;
        cout << "Availability Status" << endl;
        cout << "------------------------" << endl;
        cout << "1. Available" << endl;
        cout << "2. Unavailable" << endl;
        cout << "3. Sold Out" << endl;
        cout << "\nEnter your choice (1-3): ";

        if (!(cin >> availabilityChoice))
        {cout << "\nInvalid input! Please enter 1-3 only.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            availabilityChoice = 0;
            continue;}

        switch (availabilityChoice)
        {
            case 1:
                newItem.availability = "Available";
                break;

            case 2:
                newItem.availability = "Unavailable";
                break;

            case 3:
                newItem.availability = "Sold Out";
                break;

            default:
                cout << "\nInvalid choice! Please enter 1-3 only.\n";
        }

    } while (availabilityChoice < 1 || availabilityChoice > 3);

    do
    {
        cout << "\nEnter Preparation Time (minutes): ";

        if (!(cin >> newItem.prepTime))
        {
            cout << "Invalid input! Please enter numbers only.\n";
            cin.clear();
            cin.ignore(1000, '\n');
            newItem.prepTime = -1;
            continue;
        }

        if (newItem.prepTime <= 0)
        {
            cout << "Preparation time must be greater than 0 mins.\n";
        }

    } while (newItem.prepTime <= 0);

    menuRoot = insertNode(menuRoot, newItem); // insert the new menu item into bst
    menuItems[menuCount] = newItem; // stores the new menu item into the array
    menuCount++;

    ofstream outFile("MenuItem.txt", ios::app);// append new record to MenuItem.txt
    if (outFile)
    {
        outFile<< newItem.itemID << "|"<< newItem.itemName << "|"<< newItem.itemCategory << "|"<< newItem.cuisineType << "|"<< newItem.price << "|"
        << newItem.availability << "|"<< newItem.prepTime << endl;
        outFile.close();
    }

    cout << "\nMenu Item inserted successfully into the data!" << endl;
    cout << "\n***** NEW MENU ITEM ADDED *****\n";
    cout << "Item ID: " << newItem.itemID << endl;
    cout << "Item Name: " << newItem.itemName << endl;
    cout << "Category: " << newItem.itemCategory << endl;
    cout << "Cuisine: " << newItem.cuisineType << endl;
    cout << "Price: RM " << fixed << setprecision(2) << newItem.price << endl;
    cout << "Availability: " << newItem.availability << endl;
    cout << "Prep Time: " << newItem.prepTime << " mins" << endl;
    }


void updateMenu() // Function 4: Update an existing menu item
{
    string updateID;
    displayMenuTable();
    cout << "\n*********** UPDATE MENU ITEM ***********" << endl;
    cout << "\nEnter Item ID to update (Enter 0 to return): ";
    cin >> updateID;
    if (updateID == "0")
    {
        return;
    }

    // search menu infor in bst
    MenuNode *foundMenu = searchMenuItem(menuRoot, updateID);

    if (foundMenu == NULL)
    {
        cout << "\nMenu Item not found!" << endl;
        return;
    }

    string oldName = foundMenu->data.itemName;     // save the original information before updating to show the changes later
    string oldCategory = foundMenu->data.itemCategory;
    string oldCuisine = foundMenu->data.cuisineType;
    double oldPrice = foundMenu->data.price;
    string oldAvailability = foundMenu->data.availability;
    int oldPrepTime = foundMenu->data.prepTime;

    cout << "\n########## CURRENT MENU ITEM ##########" << endl;
    cout << "Item ID: " << foundMenu->data.itemID << endl;
    cout << "Item Name: " << foundMenu->data.itemName << endl;
    cout << "Category: " << foundMenu->data.itemCategory << endl;
    cout << "Cuisine: " << foundMenu->data.cuisineType << endl;
    cout << "Price: RM " << fixed << setprecision(2)<< foundMenu->data.price << endl;
    cout << "Availability: " << foundMenu->data.availability << endl;
    cout << "Prep Time: " << foundMenu->data.prepTime << " mins" << endl;
    int updateChoice;
    do
    {
        cout<<"\n----------------------------------------"<< endl;
        cout<<"        UPDATE MENU ITEM" << endl;
        cout<<"----------------------------------------"<< endl;
        cout<<"1. Update Item Name" << endl;
        cout<<"2. Update Category" << endl;
        cout<<"3. Update Cuisine Type" << endl;
        cout<<"4. Update Price" << endl;
        cout<<"5. Update Availability" << endl;
        cout<<"6. Update Preparation Time" << endl;
        cout<<"0. Cancel" << endl;
        cout<<"========================================"<< endl;
        cout<<"\nEnter your choice (0-6): ";

        if (!(cin >> updateChoice))
        {
            cout << "\nInvalid input! Please enter 0-6 only.\n";
            cin.clear();cin.ignore(1000, '\n');
            updateChoice = -1;continue;
        }
        cin.ignore();
        //update according to user choicee
        switch(updateChoice)
        {
        case 0:
            return;

        case 1:
            do
            {
                cout << "\nEnter New Item Name: ";
                getline(cin, foundMenu->data.itemName);
                if(foundMenu->data.itemName.empty())
                {
                    cout << "Item Name cannot be empty!\n";
                }

            }while(foundMenu->data.itemName.empty());
            break;

        case 2:
            {
                int categoryChoice;
                do
                {
                    cout << "\n------------------------" << endl;
                    cout << "Item Category" << endl;
                    cout << "------------------------" << endl;
                    cout << "1. Main Meal" << endl;
                    cout << "2. Snack" << endl;
                    cout << "3. Beverage" << endl;
                    cout << "4. Dessert" << endl;
                    cout << "5. Side Dish" << endl;
                    cout << "Enter your choice (1-5): ";
                    if (!(cin >> categoryChoice))
                    {
                        cout << "\nInvalid input! Please enter 1-5 only.\n";
                        cin.clear();
                        cin.ignore(1000,'\n');
                        categoryChoice = 0;
                        continue;
                    }
                    switch(categoryChoice)
                    {
                        case 1:
                            foundMenu->data.itemCategory = "Main Meal";
                            break;
                        case 2:
                            foundMenu->data.itemCategory = "Snack";
                            break;
                        case 3:
                            foundMenu->data.itemCategory = "Beverage";
                            break;
                        case 4:
                            foundMenu->data.itemCategory = "Dessert";
                            break;
                        case 5:
                            foundMenu->data.itemCategory = "Side Dish";
                            break;
                        default:
                            cout << "Invalid choice!\n";
                    }
                }while(categoryChoice < 1 || categoryChoice > 5);
                break;
            }
        case 3:
            {
                int cuisineChoice;
                do
                {
                    cout << "\n------------------------" << endl;
                    cout << "Cuisine Type" << endl;
                    cout << "------------------------" << endl;
                    cout << "1. Malay" << endl;cout << "2. Chinese" << endl;
                    cout << "3. Indian" << endl;cout << "4. Western" << endl;
                    cout << "5. Beverages" << endl;cout << "6. Healthy Food" << endl;
                    cout << "Enter your choice (1-6): ";

                    if (!(cin >> cuisineChoice))
                    {
                        cout << "\nInvalid input! Please enter 1-6 only.\n";
                        cin.clear();
                        cin.ignore(1000, '\n');
                        cuisineChoice = 0;
                        continue;
                    }
                    switch(cuisineChoice)
                    {
                        case 1:
                            foundMenu->data.cuisineType = "Malay";
                            break;
                        case 2:
                            foundMenu->data.cuisineType = "Chinese";
                            break;
                        case 3:
                            foundMenu->data.cuisineType = "Indian";
                            break;
                        case 4:
                            foundMenu->data.cuisineType = "Western";
                            break;
                        case 5:
                            foundMenu->data.cuisineType = "Beverages";
                            break;
                        case 6:
                            foundMenu->data.cuisineType = "Healthy Food";
                            break;
                        default:
                            cout << "Invalid choice!\n";
                    }

                } while(cuisineChoice < 1 || cuisineChoice > 6);

                break;

                }

            case 4:
                do
                {
                    cout << "Enter New Price (RM): ";
                    if (!(cin >> foundMenu->data.price))
                    {
                        cout << "Invalid input! Please enter numbers only.\n";
                        cin.clear();cin.ignore(1000, '\n');foundMenu->data.price = -1;continue;
                    }
                    if (foundMenu->data.price <= 0)
                    {
                        cout << "Price must be greater than RM0.\n";
                    }
                }while(foundMenu->data.price <= 0);
                break;

            case 5:
                {
                    int availabilityChoice;
                    do
                     {
                         cout << "\n------------------------" << endl;
                         cout << "\nAvailability Status" << endl;
                         cout << "------------------------" << endl;
                         cout << "1. Available" << endl;
                         cout << "2. Unavailable" << endl;
                         cout << "3. Sold Out" << endl;
                         cout << "Enter your choice (1-3): ";
                         if (!(cin >> availabilityChoice))
                        {
                            cout << "\nInvalid input! Please enter 1-3 only.\n";
                            cin.clear();
                            cin.ignore(1000, '\n');
                            availabilityChoice = 0;
                            continue;
                        }
                         switch(availabilityChoice)
                         {
                             case 1:
                                 foundMenu->data.availability = "Available";
                                 break;
                             case 2:
                                 foundMenu->data.availability = "Unavailable";
                                 break;
                             case 3:
                                 foundMenu->data.availability = "Sold Out";
                                 break;
                             default:
                                 cout << "Invalid choice!\n";
                        }
                    } while(availabilityChoice < 1 || availabilityChoice > 3);

                    break;
                }
            case 6:
                do
                {
                    cout << "Enter New Preparation Time (minutes): ";
                    if (!(cin >> foundMenu->data.prepTime))
                    {
                        cout << "Invalid input!\n";
                        cin.clear();
                        cin.ignore(1000, '\n');
                        foundMenu->data.prepTime = -1;
                        continue;
                    }
                    if(foundMenu->data.prepTime <= 0)
                    {
                        cout << "Preparation time must be greater than 0.\n";
                    }
                }while(foundMenu->data.prepTime <= 0);
                break;

            default:
                cout << "\nInvalid choice! Please enter 0-6 only.\n";
        }
        cout << "\n^^^^^ CHANGES MADE ^^^^^" << endl;

        if (oldName != foundMenu->data.itemName)
        {
            cout << "\nItem Name: "<< oldName << " ---> "<< foundMenu->data.itemName << endl;
        }

        if (oldCategory != foundMenu->data.itemCategory)
        {
            cout << "\nCategory: "<< oldCategory << " ---> "<< foundMenu->data.itemCategory << endl;
        }

        if (oldCuisine != foundMenu->data.cuisineType)
        {
            cout << "\nCuisine: "<< oldCuisine << " ---> "<< foundMenu->data.cuisineType << endl;
        }

        if (oldPrice != foundMenu->data.price)
        {
            cout << fixed << setprecision(2);
            cout << "\nPrice: RM "<< oldPrice << " ---> RM "<< foundMenu->data.price << endl;
        }

        if (oldAvailability != foundMenu->data.availability)
        {
            cout << "\nAvailability: "<< oldAvailability << " ---> "<< foundMenu->data.availability << endl;
        }

        if (oldPrepTime != foundMenu->data.prepTime)
        {
            cout << "\nPrep Time: "<< oldPrepTime << " mins ---> "<< foundMenu->data.prepTime << " mins" << endl;
        }

        for(int i = 0; i < menuCount; i++)
        {
            if(menuItems[i].itemID == foundMenu->data.itemID)
                {
                    menuItems[i] = foundMenu->data;
                    break;
                }
        }
        ofstream outFile("MenuItem.txt");
        outFile << "itemID|itemName|itemCategory|cuisineType|price|availability|prepTime" << endl;
        for(int i = 0; i < menuCount; i++)
        {
            outFile << menuItems[i].itemID << "|"<< menuItems[i].itemName << "|"<< menuItems[i].itemCategory << "|"
            << menuItems[i].cuisineType << "|"<< menuItems[i].price << "|"<< menuItems[i].availability << "|"<< menuItems[i].prepTime << endl;
        }
        outFile.close();
        cout << "\nSuccessfully updated the data!" <<endl;

    }while(updateChoice < 0 || updateChoice > 6);
}

MenuNode* findMin(MenuNode *current)//find the smallest node in the right subtree
{
    while(current->left != NULL)
    {
        current = current->left;
    }
    return current;
}
MenuNode* deletenode(MenuNode *menuRoot, string deleteID) //THEN delete based on the inserted menuid (delete bst have 3 cases)
{
    if(menuRoot == NULL)
    {
        return NULL;
    }

    if(deleteID < menuRoot->data.itemID)
    {
        menuRoot->left = deletenode(menuRoot->left, deleteID);
    }

    else if(deleteID > menuRoot->data.itemID)
    {
        menuRoot->right = deletenode(menuRoot->right, deleteID);
    }

    else
    {
        if(menuRoot->left == NULL && menuRoot->right == NULL) // no child case
        {
            delete menuRoot;
            return NULL;
        }
        else if(menuRoot->left == NULL) //one child case right
        {
            MenuNode *temp = menuRoot->right;
            delete menuRoot;
            return temp;
        }
        else if(menuRoot->right == NULL) // one child case left
        {
            MenuNode *temp = menuRoot->left;
            delete menuRoot;
            return temp;
        }
        MenuNode *temp = findMin(menuRoot->right); // two children
        menuRoot->data = temp->data;

        menuRoot->right = deletenode(menuRoot->right,temp->data.itemID);
    }

    return menuRoot;
}
void deleteMenu()
{
    displayMenuTable();
    string deleteID;
cout << "\n*********** DELETE MENU ITEM ***********\n";
cout << "\nEnter Item ID to delete (Enter 0 to return): ";
cin >> deleteID;

if(deleteID == "0")
{
    return;
}
MenuNode *foundMenu = searchMenuItem(menuRoot, deleteID); //search the bst
if(foundMenu == NULL)
{
    cout << "\nMenu Item not found!\n";
    return;
}
cout << "\n########## CURRENT MENU ITEM ##########" << endl;
cout << "Item ID: " <<foundMenu->data.itemID << endl;
cout << "Item Name: " <<foundMenu->data.itemName << endl;
cout << "Category: " <<foundMenu->data.itemCategory << endl;
cout << "Cuisine: " <<foundMenu->data.cuisineType << endl;
cout << "Price: RM " <<fixed << setprecision(2)<< foundMenu->data.price << endl;
cout << "Availability: " <<foundMenu->data.availability << endl;
cout << "Prep Time: " <<foundMenu->data.prepTime << " mins" << endl;
char confirmdel;

cout << "\nAre you sure you want to delete this menu item? (Y/N): ";
cin >> confirmdel;

if(confirmdel != 'Y' && confirmdel != 'y')
{
    cout << "\nDeletion cancelled. Returning...\n";
    return;
}
menuRoot = deletenode(menuRoot, deleteID);
int index = -1;

for(int i = 0; i < menuCount; i++)
{
    if(menuItems[i].itemID == deleteID)
    {
        index = i;
        break;
    }
}

if(index != -1)
{
    for(int i = index; i < menuCount - 1; i++)
    {
        menuItems[i] = menuItems[i + 1];
    }

    menuCount--;
}
ofstream outFile("MenuItem.txt"); //update the deleted from txt file'
outFile << "itemID|itemName|itemCategory|cuisineType|price|availability|prepTime" << endl;
if (!outFile)
{
    cout << "Unable to update MenuItem.txt!" << endl;
    return;
}

for(int i = 0; i < menuCount; i++)
{
    outFile << menuItems[i].itemID <<"|"<< menuItems[i].itemName << "|"<< menuItems[i].itemCategory << "|"<< menuItems[i].cuisineType << "|"<< menuItems[i].price << "|"<< menuItems[i].availability << "|"<< menuItems[i].prepTime << endl;
}
outFile.close();
cout << "\nMenu Item deleted successfully! Please review from Option View All Menu\n";
}

void menuManagement() //menu management Menu Page (serach, insert, update, delete menu item)
{
    int menuChoice;
    do
    {
        cout<<"\n--------------------------------------------"<<endl;
        cout<<"  *** MENU ITEM SEARCH AND MANAGEMENT *** "<<endl;
        cout<<"--------------------------------------------" <<endl;
        cout<<"1. Display All Menu Items\n2. Search For Menu Item\n3. Insert New Menu Item\n4. Update Menu Item\n5. Delete Menu Item\n6. Return to Main Menu" << endl;
        cout<<"............................................"<<endl;
        cout<<"\nEnter your choice (1-6): ";

        //user string or character
        if (!(cin>>menuChoice))
            {
                cout<<"\nInvalid input! Please enter a correct choice (1-6 only!)."<<endl;
                cin.clear();cin.ignore(1000,'\n');menuChoice = 0;continue;
            }

        //user choice inputsz
        if (menuChoice == 1)
        {
            showAllMenu();
        }
        else if (menuChoice == 2)
        {searchMenu();
        }
        else if (menuChoice == 3)
        {insertMenu();
        }
        else if (menuChoice == 4)
        {updateMenu();
        }
        else if (menuChoice == 5)
        {deleteMenu();
        }
        else if (menuChoice == 6)
        {
            cout<<"Returning to the Main Menu...."<<endl;
        }
        else
        {
            cout<<"\nInvalid choice! Please insert the correct input! 1-6 only."<<endl;
        }

    } while(menuChoice != 6);
}

// Helper Functions
// gnerate orderID using last id
string generateOrderID() {
    stringstream ss;
    ss << "ORD" << nextOrderNumber;
    nextOrderNumber++;
    return ss.str();
}
// get one field from a line, separate using |
string getField(string line, int fieldNumber) {
    string field = "";
    int currentField = 0;

    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] == '|') {
            currentField++;
        } else if (currentField == fieldNumber) {
            field = field + line[i];
        }
    }

    return field;
}

void updateNextOrderNumberFromFile() {
    ifstream file("Order.txt");
    string line;
    string lastOrderID = "";

    if (!file) {
        cout << "Order.txt could not be opened. Starting order ID from ORD1001." << endl;
        return;
    }

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line != "") {
            lastOrderID = getField(line, 0);
        }
    }

    file.close();

    if (lastOrderID != "") {
        string numberPart = "";

        for (int i = 3; i < (int)lastOrderID.length(); i++) {
            numberPart = numberPart + lastOrderID[i];
        }

        nextOrderNumber = stoi(numberPart) + 1;
    }
}
// load from MenuItem.txt to menu array
void loadMenuItemsFromFile() {
    ifstream file("MenuItem.txt");
    string line;

    menuCount = 0;

    if (!file) {
        cout << "MenuItem.txt could not be opened." << endl;
        return;
    }

    getline(file, line); // skip header row

    while (getline(file, line)) {
        if (line != "") {
            menuItems[menuCount].itemID = getField(line, 0);
            menuItems[menuCount].itemName = getField(line, 1);
            menuItems[menuCount].itemCategory = getField(line, 2);
            menuItems[menuCount].cuisineType = getField(line, 3);
            menuItems[menuCount].price = stod(getField(line, 4));
            menuItems[menuCount].availability = getField(line, 5);
            menuItems[menuCount].prepTime = stoi(getField(line, 6));

            menuCount++;
        }
    }

    file.close();
}
// load saved order item records from OrderItem.txt
void loadOrderItemsFromFile() {
    ifstream file("OrderItem.txt");
    string line;

    orderItemCount = 0;

    if (!file) {
        cout << "OrderItem.txt could not be opened." << endl;
        return;
    }

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line != "") {
            orderItems[orderItemCount].orderItemID = getField(line, 0);
            orderItems[orderItemCount].orderID = getField(line, 1);
            orderItems[orderItemCount].itemID = getField(line, 2);
            orderItems[orderItemCount].quantity = stoi(getField(line, 3));
            orderItems[orderItemCount].unitPrice = stod(getField(line, 4));
            orderItems[orderItemCount].lineTotal = stod(getField(line, 5));
            orderItems[orderItemCount].custNotes = getField(line, 6);

            orderItemCount++;
        }
    }

    file.close();
}

// find menu item using MenuItemID
string findMenuItemNameByID(string itemID) {
    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].itemID == itemID) {
            return menuItems[i].itemName;
        }
    }

    return "Unknown Item";
}

// load only pending orders into queue when program starts
void loadPendingOrdersFromFile(OrderQueue &orderQueue) {
    ifstream file("Order.txt");
    string line;
    int loadedCount = 0;

    if (!file) {
        cout << "Order.txt could not be opened. No existing pending orders loaded." << endl;
        return;
    }

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line != "") {
            Order fileOrder;

            fileOrder.orderID = getField(line, 0);
            fileOrder.studentID = getField(line, 1);
            fileOrder.cuisineType = getField(line, 2);
            fileOrder.orderStatus = getField(line, 3);
            fileOrder.assignedStallID = getField(line, 4);
            fileOrder.total = stod(getField(line, 5));
            fileOrder.orderTime = getField(line, 6);

            if (fileOrder.orderStatus == "Pending") {
                CartItem fileCartItems[20];
                int fileCartCount = 0;

                for (int i = 0; i < orderItemCount; i++) {
                    if (orderItems[i].orderID == fileOrder.orderID) {
                        fileCartItems[fileCartCount].itemID = orderItems[i].itemID;
                        fileCartItems[fileCartCount].itemName = findMenuItemNameByID(orderItems[i].itemID);
                        fileCartItems[fileCartCount].quantity = orderItems[i].quantity;
                        fileCartItems[fileCartCount].unitPrice = orderItems[i].unitPrice;
                        fileCartItems[fileCartCount].lineTotal = orderItems[i].lineTotal;
                        fileCartCount++;
                    }
                }

                orderQueue.enqueue(fileOrder, fileCartItems, fileCartCount);
                loadedCount++;
            }
        }
    }

    file.close();
}

// get current date n time
string getCurrentTime() {
    time_t now = time(0);
    tm *localTime = localtime(&now);

    char timeText[80];
    strftime(timeText, 80, "%Y-%m-%d %H:%M:%S", localTime);

    return string(timeText);
}

// convert the actual kiosk cart into one complete condition for Session History
KioskFullCondition createKioskFullCondition(
    string screenName,
    string selectedCuisineType,
    CartItem cartItems[],
    int cartCount,
    double total)
{
    KioskFullCondition currentState;
    currentState.screenName = screenName;
    currentState.selectedCuisineType = selectedCuisineType;
    currentState.cartCount = cartCount;
    currentState.total = total;

    for (int i = 0; i < cartCount; i++) {
        currentState.cartItem[i].itemID = cartItems[i].itemID;
        currentState.cartItem[i].itemName = cartItems[i].itemName;
        currentState.cartItem[i].quantity = cartItems[i].quantity;
        currentState.cartItem[i].unitPrice = cartItems[i].unitPrice;
        currentState.cartItem[i].totalItemPrice = cartItems[i].lineTotal;
    }

    return currentState;
}

// copy a saved condition back into the actual kiosk variables
void restoreKioskFullCondition(
    const KioskFullCondition& restoredState,
    string& selectedCuisineType,
    CartItem cartItems[],
    int& cartCount,
    double& total)
{
    selectedCuisineType = restoredState.selectedCuisineType;
    cartCount = restoredState.cartCount;
    total = restoredState.total;

    for (int i = 0; i < cartCount; i++) {
        cartItems[i].itemID = restoredState.cartItem[i].itemID;
        cartItems[i].itemName = restoredState.cartItem[i].itemName;
        cartItems[i].quantity = restoredState.cartItem[i].quantity;
        cartItems[i].unitPrice = restoredState.cartItem[i].unitPrice;
        cartItems[i].lineTotal = restoredState.cartItem[i].totalItemPrice;
    }
}

void displayStudentCart(CartItem cartItems[], int cartCount, double total)
{
    cout << "\n===== Shopping Cart =====" << endl;

    if (cartCount == 0) {
        cout << "Cart is empty." << endl;
        return;
    }

    for (int i = 0; i < cartCount; i++) {
        cout << i + 1 << ". "
             << cartItems[i].itemName
             << " x " << cartItems[i].quantity
             << " = RM" << fixed << setprecision(2)
             << cartItems[i].lineTotal << endl;
    }

    cout << "Total: RM" << fixed << setprecision(2) << total << endl;
}

// allow student to update or remove an item and record the new cart condition
void manageStudentCart(
    CartItem cartItems[],
    int& cartCount,
    double& total,
    string selectedCuisineType,
    SessionHistory& sessionHistory)
{
    displayStudentCart(cartItems, cartCount, total);

    if (cartCount == 0) {
        return;
    }

    int cartChoice;
    cout << "\n1. Update Item Quantity" << endl;
    cout << "2. Remove Item" << endl;
    cout << "0. Return to Food Menu" << endl;
    cout << "Enter choice: ";

    if (!(cin >> cartChoice)) {
        cout << "Invalid input. Please enter a number." << endl;
        cin.clear();
        cin.ignore(1000, '\n');
        return;
    }

    if (cartChoice == 1) {
        int itemNumber;
        int newQuantity;

        cout << "Enter cart item number: ";
        cin >> itemNumber;

        if (itemNumber < 1 || itemNumber > cartCount) {
            cout << "Invalid cart item number." << endl;
            return;
        }

        cout << "Enter new quantity: ";
        cin >> newQuantity;

        if (newQuantity <= 0) {
            cout << "Quantity must be at least 1." << endl;
            return;
        }

        int cartIndex = itemNumber - 1;
        int oldQuantity = cartItems[cartIndex].quantity;

        total = total - cartItems[cartIndex].lineTotal;
        cartItems[cartIndex].quantity = newQuantity;
        cartItems[cartIndex].lineTotal = cartItems[cartIndex].unitPrice * newQuantity;
        total = total + cartItems[cartIndex].lineTotal;

        KioskFullCondition updatedState = createKioskFullCondition(
            "Shopping Cart", selectedCuisineType, cartItems, cartCount, total
        );

        sessionHistory.recordUserNewStep(
            "Update Quantity",
            "Changed " + cartItems[cartIndex].itemName + " quantity from "
                + to_string(oldQuantity) + " to " + to_string(newQuantity),
            updatedState
        );

        cout << "Item quantity updated successfully." << endl;
    }
    else if (cartChoice == 2) {
        int itemNumber;

        cout << "Enter cart item number to remove: ";
        cin >> itemNumber;

        if (itemNumber < 1 || itemNumber > cartCount) {
            cout << "Invalid cart item number." << endl;
            return;
        }

        int cartIndex = itemNumber - 1;
        string removedItemName = cartItems[cartIndex].itemName;
        total = total - cartItems[cartIndex].lineTotal;

        for (int i = cartIndex; i < cartCount - 1; i++) {
            cartItems[i] = cartItems[i + 1];
        }

        cartCount--;

        KioskFullCondition removedState = createKioskFullCondition(
            "Shopping Cart", selectedCuisineType, cartItems, cartCount, total
        );

        sessionHistory.recordUserNewStep(
            "Remove Item",
            "Removed " + removedItemName + " from the cart",
            removedState
        );

        cout << "Item removed successfully." << endl;
    }
    else if (cartChoice != 0) {
        cout << "Invalid choice." << endl;
    }
}

string chooseCuisineType() {
    int choice;

    do {
        cout << "\n===== Choose Cuisine Type =====" << endl;
        cout << "1. Malay" << endl;
        cout << "2. Chinese" << endl;
        cout << "3. Western" << endl;
        cout << "4. Indian" << endl;
        cout << "5. Beverages" << endl;
        cout << "6. Healthy Food" << endl;
        cout << "0. Cancel" << endl;
        cout << "Enter cuisine choice: ";

        if (!(cin >> choice)) {
            cout << "Invalid cuisine choice. Please try again." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            choice = -1;
        }

        switch (choice) {
            case 1:
                return "Malay";
            case 2:
                return "Chinese";
            case 3:
                return "Western";
            case 4:
                return "Indian";
            case 5:
                return "Beverages";
            case 6:
                return "Healthy Food";
            case 0:
                return "";
            default:
                cout << "\nInvalid cuisine choice. Please try again." << endl;
        }

    } while (choice != 0);

    return "";
}

// display only menu items that match selected cuisine
void displayMenuItemsByCuisine(string selectedCuisineType) {
    displayedItemCount = 0;

    cout << "\n===== " << selectedCuisineType << " Menu Items =====" << endl;

    cout << left
         << setw(6) << "No."
         << setw(10) << "Item ID"
         << setw(30) << "Item Name"
         << setw(16) << "Cuisine"
         << setw(10) << "Price"
         << "Availability" << endl;

    cout << string(91, '-') << endl;

    for (int i = 0; i < menuCount; i++) {
        if (menuItems[i].cuisineType == selectedCuisineType) {
            displayedItemIndex[displayedItemCount] = i;
            displayedItemCount++;

            cout << left
                 << setw(6) << displayedItemCount
                 << setw(10) << menuItems[i].itemID
                 << setw(30) << menuItems[i].itemName
                 << setw(16) << menuItems[i].cuisineType
                 << "RM"
                 << setw(8) << fixed << setprecision(2) << menuItems[i].price
                 << menuItems[i].availability
                 << endl;
        }
    }

    cout << string(91, '-') << endl;
    cout << "\n0. Submit Order" << endl;
    cout << "-1. Navigate Back" << endl;
    cout << "-2. Session History and Navigation" << endl;
    cout << "-3. Manage Cart" << endl;
    cout << "-4. Change Cuisine\n" << endl;
}

// check studentID follow format TPXXXXXX
bool isValidStudentID(string studentID) {
    if (studentID.length() != 8)
        return false;

    if (studentID[0] != 'T' || studentID[1] != 'P')
        return false;

    return true;
}

// Student Kiosk Interface
// handles student ordering from choosing cuisine until checkout
void studentKiosk(OrderQueue &orderQueue) {
    SessionHistory sessionHistory;
    string studentID;
    int itemChoice;
    int quantity;
    double total = 0;
    string selectedCuisineType;

    // temporary cart before student confirm order
    CartItem cartItems[20];
    int cartCount = 0;

    cout << "\n===== Welcome to Campus Food Court Kiosk =====\n" << endl;
    cout << "Enter Student ID (e.g. TP012345): ";
    cin >> studentID;

    if (!isValidStudentID(studentID)) {
        cout << "\nInvalid Student ID format. Student ID must start with TP and contain 8 characters." << endl;
        cout << "Returning to main menu." << endl;
        return;
    }

    cout << "\nWelcome, " << studentID << "!" << endl;

    KioskFullCondition loginState = createKioskFullCondition(
        "Student Login", "None", cartItems, cartCount, total
    );

    sessionHistory.recordUserNewStep(
        "Login",
        "Logged in as " + studentID,
        loginState
    );

    selectedCuisineType = chooseCuisineType();

    if (selectedCuisineType == "") {
        cout << "Cuisine selection cancelled. Returning to main menu." << endl;
        return;
    }

    cout << "Selected Cuisine: " << selectedCuisineType << endl;

    KioskFullCondition cuisineState = createKioskFullCondition(
        selectedCuisineType + " Menu", selectedCuisineType,
        cartItems, cartCount, total
    );

    sessionHistory.recordUserNewStep(
        "Browse Menu",
        "Browsed " + selectedCuisineType + " cuisine menu",
        cuisineState
    );

    do {
        if (selectedCuisineType == "None") {
            cout << "\nYou are at the beginning of the kiosk session." << endl;
            selectedCuisineType = chooseCuisineType();

            if (selectedCuisineType == "") {
                cout << "Returning to main menu." << endl;
                return;
            }

            KioskFullCondition newCuisineState = createKioskFullCondition(
                selectedCuisineType + " Menu", selectedCuisineType,
                cartItems, cartCount, total
            );

            sessionHistory.recordUserNewStep(
                "Browse Menu",
                "Browsed " + selectedCuisineType + " cuisine menu",
                newCuisineState
            );
        }

        displayMenuItemsByCuisine(selectedCuisineType);

        cout << "Choose an option: ";

        if (!(cin >> itemChoice)) {
            cout << "Invalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            continue;
        }

        if (itemChoice >= 1 && itemChoice <= displayedItemCount) {

            int actualIndex = displayedItemIndex[itemChoice - 1];
            MenuItem selectedItem = menuItems[actualIndex];

            if (selectedItem.availability != "Available") {
                cout << "\n" << selectedItem.itemName << " is currently " << selectedItem.availability << "." << endl;
                cout << "Please choose another item." << endl;
            } else {
                cout << "Enter quantity: ";

                if (!(cin >> quantity)) {
                    cout << "Invalid quantity. Please enter a number." << endl;
                    cin.clear();
                    cin.ignore(1000, '\n');
                    continue;
                }

                if (quantity <= 0) {
                    cout << "Quantity must be at least 1." << endl;
                }
                else {
                    if (cartCount >= 20) {
                        cout << "Cart is full. Please submit the current order first." << endl;
                        continue;
                    }

                    double lineTotal = selectedItem.price * quantity;
                    total = total + lineTotal;
                    cartItems[cartCount].itemID = selectedItem.itemID;
                    cartItems[cartCount].itemName = selectedItem.itemName;
                    cartItems[cartCount].quantity = quantity;
                    cartItems[cartCount].unitPrice = selectedItem.price;
                    cartItems[cartCount].lineTotal = lineTotal;
                    cartCount++;

                    KioskFullCondition addedItemState = createKioskFullCondition(
                        "Shopping Cart", selectedCuisineType,
                        cartItems, cartCount, total
                    );

                    sessionHistory.recordUserNewStep(
                        "Add Item",
                        "Added " + selectedItem.itemName + " x " + to_string(quantity),
                        addedItemState
                    );

                    cout << "Added to cart: "
                         << selectedItem.itemName
                         << " x "
                         << quantity
                         << " = RM"
                         << lineTotal
                         << endl;

                    cout << "Current Total: RM" << total << endl;
                }
            }
        } else if (itemChoice == -1) {
            KioskFullCondition restoredState;

            if (sessionHistory.navigateUserBackStep(restoredState)) {
                restoreKioskFullCondition(
                    restoredState,
                    selectedCuisineType,
                    cartItems,
                    cartCount,
                    total
                );

                cout << "Previous kiosk condition restored." << endl;
                cout << "Current Screen: " << restoredState.screenName << endl;
                displayStudentCart(cartItems, cartCount, total);
            }
        } else if (itemChoice == -2) {
            sessionHistoryAndNavigationMenu(sessionHistory);
        } else if (itemChoice == -3) {
            manageStudentCart(
                cartItems,
                cartCount,
                total,
                selectedCuisineType,
                sessionHistory
            );
        } else if (itemChoice == -4) {
            if (cartCount > 0) {
                cout << "Please remove all cart items before changing cuisine." << endl;
            } else {
                string newCuisineType = chooseCuisineType();

                if (newCuisineType != "") {
                    selectedCuisineType = newCuisineType;

                    KioskFullCondition changedCuisineState = createKioskFullCondition(
                        selectedCuisineType + " Menu", selectedCuisineType,
                        cartItems, cartCount, total
                    );

                    sessionHistory.recordUserNewStep(
                        "Change Cuisine",
                        "Changed cuisine to " + selectedCuisineType,
                        changedCuisineState
                    );
                }
            }
        } else if (itemChoice == 0) {
            if (total > 0) {
                cout << "\n===== Cart Summary =====\n" << endl;

                for (int i = 0; i < cartCount; i++) {
                    cout << i + 1 << ". "
                         << cartItems[i].itemName
                         << " x "
                         << cartItems[i].quantity
                         << " = RM"
                         << fixed << setprecision(2)
                         << cartItems[i].lineTotal
                         << endl;
                }

                cout << "\nTotal: RM" << fixed << setprecision(2) << total << endl;

                char confirm;

                do {
                    cout << "\nConfirm order? (Y = Submit, N = Continue Ordering, C = Cancel Order): ";
                    cin >> confirm;

                    if (confirm == 'N' || confirm == 'n') {
                        cout << "Returning to menu..." << endl;
                        break;
                    } else if (confirm == 'C' || confirm == 'c') {
                        cout << "Order cancelled. Returning to main menu." << endl;
                        return;
                    } else if (confirm != 'Y' && confirm != 'y') {
                        cout << "Invalid confirmation choice. Please enter Y, N, or C." << endl;
                    }

                } while (confirm != 'Y' && confirm != 'y' && confirm != 'N' && confirm != 'n');

                if (confirm == 'N' || confirm == 'n') {
                    itemChoice = -1;
                    continue;
                }
                // create order record after student confirms checkout
                Order newOrder;

                newOrder.orderID = generateOrderID();
                newOrder.studentID = studentID;
                newOrder.cuisineType = selectedCuisineType;
                newOrder.orderStatus = "Pending";
                newOrder.assignedStallID = "NONE";
                newOrder.total = total;
                newOrder.orderTime = getCurrentTime();

                bool orderAdded = orderQueue.enqueue(newOrder, cartItems, cartCount);

                if (!orderAdded) {
                    cout << "Order could not be submitted. Please try again later." << endl;
                    return;
                }

                KioskFullCondition checkoutState = createKioskFullCondition(
                    "Order Confirmation", selectedCuisineType,
                    cartItems, cartCount, total
                );

                sessionHistory.recordUserNewStep(
                    "Checkout",
                    "Submitted order " + newOrder.orderID,
                    checkoutState
                );

                cout << "\nOrder submitted successfully.\n" << endl;
                cout << "Order ID: " << newOrder.orderID << endl;
                cout << "Student ID: " << newOrder.studentID << endl;
                cout << "Total: RM" << newOrder.total << endl;
                cout << "Order Time: " << newOrder.orderTime << "\n" << endl;
                cout << "Your order is now waiting in the queue." << endl;

                cout << "\n===== Final Session Information =====" << endl;
                sessionHistory.displayFullSessionHistory();
                sessionHistory.displaySessionCurrentState();
                sessionHistory.displayBackNavigationTrace();
                sessionHistory.displayCurrentActiveStack();
            } else {
                cout << "No items selected. Order cancelled." << endl;
            }
        } else {
            cout << "Invalid item number." << endl;
        }

    } while (itemChoice != 0);
}

// Order Queue Management Interface
// menu for viewing pending and completed orders
void orderQueueManagement(OrderQueue &orderQueue) {
    int choice;

    do {
        cout << "\n===== Order Queue Management =====" << endl;
        cout << "\nPending Orders: " << orderQueue.getQueueCount() << endl;
        cout << "\n1. View Next Order" << endl;
        cout << "2. Display Pending Orders" << endl;
        cout << "3. Display Completed Orders" << endl;
        cout << "4. Back to Main Menu" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

        switch (choice) {
            case 0:
                break;
            case 1:
                orderQueue.viewFrontOrderDetails();
                break;

            case 2:
                orderQueue.displayPendingOrders();
                break;

            case 3:
                orderQueue.displayCompletedOrders();
                break;

            case 4:
                cout << "Returning to main menu..." << endl;
                break;

            default:
                cout << "Invalid choice. Please try again." << endl;
        }

    } while (choice != 4);
}

// main menu
int main() {
    // load existing records before showing main menu
    loadMenuItemsFromFile();
    initializeMenuTree();
    updateNextOrderNumberFromFile();
    loadOrderItemsFromFile();

    OrderQueue orderQueue;
    loadPendingOrdersFromFile(orderQueue);

    int choice;

    do {
        cout << "\n===== Campus Food Court Kiosk System =====" << endl;
        cout << "1. Student Kiosk" << endl;
        cout << "2. Order Queue Management" << endl;
        cout << "3. Stall Assignment Management" << endl;
        cout << "4. Menu Management" << endl;
        cout << "5. Exit" << endl;
        cout << "Enter choice: ";

        if (!(cin >> choice)) {
            cout << "\nInvalid input. Please enter a number." << endl;
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

        switch (choice) {
            case 0:
                break;
            case 1:
                studentKiosk(orderQueue);
                break;

            case 2:
                orderQueueManagement(orderQueue);
                break;

            case 3:
                stallAssignment_MainMenu(orderQueue);
                break;

            case 4:
                menuManagement();
                break;

            case 5:
                cout << "Exiting program..." << endl;
                break;

            default:
                cout << "\nInvalid choice. Please try again." << endl;
        }

    } while (choice != 5);

    return 0;
}
