#include <iostream>
#include <string>

// Base Class
class Vehicle
{
protected:
    std::string brand;
    int speed;

public:
    Vehicle(const std::string &brand, int speed) : brand(brand), speed(speed) {}

    void start()
    {
        std::cout << brand << " vehicle started." << std::endl;
    }

    void stop()
    {
        std::cout << brand << " vehicle stopped." << std::endl;
    }

    virtual void display() const = 0; // Pure virtual function
};

// Custom class

class MotorCycle : public Vehicle
{
    private:
    String brand;

    public:
        // Set constructor
};

// Derived Class: Car
class Car : public Vehicle
{
private:
    int doors;

public:
    /*
        Notes about constructor
            - Initialization list
                - The part after the colon :
                - Used to initialize base class members and data members before the body of the constructor is executed 
            - Vehicle(brand, speed)
                - Calls the constructor of the base class 'Vehicle' 
                - Passes 'brand'/'speed' parameters to it
                - This is done to ensure the base class part of `Car` object is properly initialized
            - doors(doors)
                - Initializes 'doors' member of the 'Car' class with the value passed to the constructor
            - Base class initialization 
                - When having a derived class like `car` 
                    - The base class `Vehicle` constructor is called BEFORe the derived class's constructor body is executed 
                    - The Vehicle constructor is called with brand and speed first.
                    - After the Vehicle constructor completes, 
                        - The Car constructor continues with the initialization of doors.
                - Ensures that all parts of the object are fully initialized before any other operations are performed.
    */
    Car(const std::string &brand, int speed, int doors)
        : Vehicle(brand, speed), doors(doors) {}

    void honk() const
    {
        std::cout << brand << " car honks: Beep Beep!" << std::endl;
    }
    // const = so the state doesn't change
    // override = ensures display() method is correctly overriding the base class's virtual method
    // It's used to indicate that a member function in a derived class is to override a virtual function in the base class
    // This helps the compiler check if you're correctly overriding a base class method, catching errors if the base method's signature doesn't match 
    void display() const override
    {
        std::cout << "Car Brand: " << brand << std::endl;
        std::cout << "Speed: " << speed << " km/h" << std::endl;
        std::cout << "Doors: " << doors << std::endl;
    }
};

// Derived Class: Bike
class Bike : public Vehicle
{
private:
    bool hasBasket;

public:
    Bike(const std::string &brand, int speed, bool hasBasket)
        : Vehicle(brand, speed), hasBasket(hasBasket) {}

    void ringBell() const
    {
        std::cout << brand << " bike rings bell: Ring Ring!" << std::endl;
    }

    void display() const override
    {
        std::cout << "Bike Brand: " << brand << std::endl;
        std::cout << "Speed: " << speed << " km/h" << std::endl;
        std::cout << "Has Basket: " << (hasBasket ? "Yes" : "No") << std::endl;
    }
};

// Main Function
int main()
{
    Car myCar("Toyota", 180, 4);
    Bike myBike("Giant", 25, true);

    myCar.start();
    myCar.honk();
    myCar.display();
    myCar.stop();

    myBike.start();
    myBike.ringBell();
    myBike.display();
    myBike.stop();

    return 0;
}
