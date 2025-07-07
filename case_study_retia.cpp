#include <iostream>


int add(int a, int b) {
    return a + b;
}

// int main() {
//     int a = 2;
//     int b = 3;
//     int result = add(a, b);
//     std::cout << "Hello, world!" << result << std::endl;
//     std::cin.get();  // čeká na stisk klávesy
//     return 0;
// }

/*
//FOR CYCLE:
for (int i = 0; i < 5; ++i) {
    std::cout << i << "\n";
}
*/

/*
//FUNKCE:
int add(int a, int b) {
    return a + b;
}
*/

/*
//práce s vektory:

#include <vector>   // potřebujeme knihovnu vector
#include <iostream>

int main() {
    std::vector<int> numbers = {1, 2, 3};
    numbers.push_back(4);  // přidání prvku

    std::cout << numbers[0] << "\n";  // výstup prvního prvku

    // moderní for-cyklus v C++11 (range-based for)
    for (int x : numbers) {
        std::cout << x << " ";
    }

    return 0;
}
*/

/*
//classes:

#include <iostream>
#include <string>

class Person {
private:
    std::string name;

public:
    // Konstruktor
    Person(std::string initName) {
        name = initName;
    }

    // Metoda
    void sayHello() {
        std::cout << "Hello, my name is " << name << "\n";
    }
};

int main() {
    Person p("Kuba");
    p.sayHello();

    return 0;
}
*/


//Ukazatele:

// #include <iostream>

// int main() {
//     int a = 10;
//     int* p = &a;  // p ukazuje na adresu proměnné a

//     std::cout << "Hodnota a: " << a << "\n";
//     std::cout << "Adresa a: " << &a << "\n";
//     std::cout << "Ukazatel p: " << p << "\n";
//     std::cout << "Hodnota na kterou p ukazuje: " << *p << "\n";

//     *p = 20;  // změníme hodnotu a přes ukazatel
//     std::cout << "Nová hodnota a: " << a << "\n";
//     std::cout << "Hodnota na kterou p ukazuje: " << *p << "\n";

//     return 0;
// }


// #include <iostream>
// #include <vector>

// class Student {
// public:
//     std::string name;
//     int age;

//     Student(std::string n, int a) : name(n), age(a) {}

//     void greet() {
//         std::cout << "Hi, I'm " << name << " and I'm " << age << " years old.\n";
//     }
// };

// int main() {
//     std::vector<Student> students;
//     students.push_back(Student("Kuba", 22));
//     students.push_back(Student("Eva", 24));

//     for (Student& s : students) {
//         s.greet();
//     }

//     return 0;
// }

#include <iostream>
#include <string>

// Nadřazená (základní) třída
class Person {
protected:
    std::string name;

public:
    Person(std::string name) : name(name) {}

    void sayHello() {
        std::cout << "Hi, I'm " << name << "\n";
    }
};

// Odvozená třída (dědí z Person)
class Student : public Person {
private:
    int studentId;

public:
    Student(std::string name, int id) : Person(name), studentId(id) {}

    void showId() {
        std::cout << "My student ID is " << studentId << "\n";
    }
};

int main() {
    Student s("Kuba", 12345);
    s.sayHello();   // zděděná metoda z Person
    s.showId();     // vlastní metoda

    return 0;
}