#include <iostream>
#include <string>
#include <vector>
#include <iomanip>

/*
    Autor: Jakub Hlaváček
    Projekt: Práce s velkými čísly s vysokou přesností – case study

    Zadání (case study):

    Vytvořte třídy CFloat a CInt pro práci s čísly s vysokou přesností.
    Příklad použití:

    void printNum(const CNumber & num)
    {
        std::cout << num << std::endl;
    }

    int main()
    {
        const CFloat af("5.13545623465465465465465464121313132465415");
        const CFloat bf("8.21564564456123132123132146544646546546546528564");
        printNum(af + bf);
        printNum(af - bf);

        const CInt ai("132115465615664583461973866555483255524643727514673745194967728");
        const CInt bi("55376461616151515944594954467327731717171727158844");
        printNum(ai + bi);
        printNum(ai - bi);

        return 0;
    }

    Cílem je implementace vlastních číselných tříd schopných reprezentovat a počítat s velmi dlouhými čísly s přesností nad rámec běžných typů (`double`, `int`).
*/

/// Abstraktní základní třída pro čísla
class CNumber {
public:
    virtual ~CNumber() = default;

    // Výstup na std::ostream – musí být přetížen v potomcích
    virtual void print(std::ostream& os) const = 0;
};

/// Přetížení operátoru << pro CNumber – volá virtuální metodu print()
inline std::ostream& operator<<(std::ostream& os, const CNumber& num) {
    num.print(os);
    return os;
}

/// Třída pro práci s celými čísly s libovolnou přesností
class CInt : public CNumber {
public:
    // Konstruktor z řetězce
    explicit CInt(const std::string& value) {
        m_digits.clear();
        m_negative = false;

        size_t start = 0;
        if (!value.empty() && value[0] == '-') {
            m_negative = true;
            start = 1;
        }

        for (size_t i = value.length(); i-- > start;) {
            if (isdigit(value[i])) {
                m_digits.push_back(value[i] - '0');
            }
        }

        removeLeadingZeros();
    }

    // Přetížení operátoru +
    CInt operator+(const CInt& other) const {
        // Sčítání pouze pro stejná znaménka (kladná čísla)
        if (m_negative == other.m_negative) {
            CInt result("0");
            result.m_digits.clear();

            const std::vector<int>& a = this->m_digits;
            const std::vector<int>& b = other.m_digits;
            int carry = 0;
            size_t maxSize = std::max(a.size(), b.size());

            for (size_t i = 0; i < maxSize || carry; ++i) {
                int digitA = (i < a.size()) ? a[i] : 0;
                int digitB = (i < b.size()) ? b[i] : 0;
                int sum = digitA + digitB + carry;
                result.m_digits.push_back(sum % 10);
                carry = sum / 10;
            }

            result.m_negative = m_negative;
            result.removeLeadingZeros();
            return result;
        } else {
            // a + (-b) → a - b
            // (-a) + b → b - a
            if (m_negative) return other - CInt(*this).setNegative(false);
            else return *this - CInt(other).setNegative(false);
        }
    }

    // Přetížení operátoru -
    CInt operator-(const CInt& other) const {
        // Zatím řešíme pouze a - b pro a > b, obě kladná
        if (m_negative || other.m_negative) {
            // (-a) - b = -(a + b)
            // a - (-b) = a + b
            // (-a) - (-b) = b - a
            CInt tempOther = other;
            tempOther.m_negative = !tempOther.m_negative;
            return *this + tempOther;
        }

        if (*this < other) {
            CInt result = other - *this;
            result.m_negative = true;
            return result;
        }

        CInt result("0");
        result.m_digits.clear();

        int borrow = 0;
        for (size_t i = 0; i < m_digits.size(); ++i) {
            int digitA = m_digits[i];
            int digitB = (i < other.m_digits.size()) ? other.m_digits[i] : 0;
            int diff = digitA - digitB - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.m_digits.push_back(diff);
        }

        result.removeLeadingZeros();
        return result;
    }

    void print(std::ostream& os) const override {
        if (m_digits.empty()) {
            os << "0";
            return;
        }

        if (m_negative && !(m_digits.size() == 1 && m_digits[0] == 0))
            os << "-";

        for (auto it = m_digits.rbegin(); it != m_digits.rend(); ++it)
            os << *it;
    }
    
private:
    // Pomocná metoda pro odstranění nul z čela čísla
    void removeLeadingZeros() {
        while (m_digits.size() > 1 && m_digits.back() == 0)
            m_digits.pop_back();
    }

    // Pomocná metoda pro úpravu znaménka (pro + a -)
    CInt& setNegative(bool neg) {
        m_negative = neg;
        return *this;
    }

    // Porovnání < pro odčítání
    bool operator<(const CInt& other) const {
        if (m_digits.size() != other.m_digits.size())
            return m_digits.size() < other.m_digits.size();
        for (size_t i = m_digits.size(); i-- > 0;) {
            if (m_digits[i] != other.m_digits[i])
                return m_digits[i] < other.m_digits[i];
        }
        return false;
    }

    std::vector<int> m_digits;  // Číslice v opačném pořadí
    bool m_negative = false;    // Znaménko
};

class CFloat : public CNumber {
public:
    explicit CFloat(const std::string& value) {
        m_negative = false;
        size_t start = 0;
        std::string intStr, fracStr;

        if (!value.empty() && value[0] == '-') {
            m_negative = true;
            start = 1;
        }

        size_t dotPos = value.find('.');
        intStr = value.substr(start, dotPos - start);
        if (dotPos != std::string::npos)
            fracStr = value.substr(dotPos + 1);

        // Celá část (reverzně)
        for (size_t i = intStr.length(); i-- > 0;) {
            if (isdigit(intStr[i])) {
                m_intPart.push_back(intStr[i] - '0');
            }
        }

        // Desetinná část (normálně)
        for (char ch : fracStr) {
            if (isdigit(ch)) {
                m_fracPart.push_back(ch - '0');
            }
        }

        removeTrailingZeros();
        removeLeadingZeros();
    }

    CFloat operator+(const CFloat& other) const {
        CFloat result("0.0");
        result.m_intPart.clear();
        result.m_fracPart.clear();
        result.m_negative = false;

        // 1. Zarovnat desetinné části
        size_t maxFrac = std::max(m_fracPart.size(), other.m_fracPart.size());
        std::vector<int> a_frac = m_fracPart;
        std::vector<int> b_frac = other.m_fracPart;
        a_frac.resize(maxFrac, 0);
        b_frac.resize(maxFrac, 0);

        // 2. Sečti desetinnou část zprava
        int carry = 0;
        for (int i = static_cast<int>(maxFrac) - 1; i >= 0; --i) {
            int sum = a_frac[i] + b_frac[i] + carry;
            result.m_fracPart.insert(result.m_fracPart.begin(), sum % 10);
            carry = sum / 10;
        }

        // 3. Zarovnat celé části
        size_t maxInt = std::max(m_intPart.size(), other.m_intPart.size());
        std::vector<int> a_int = m_intPart;
        std::vector<int> b_int = other.m_intPart;
        a_int.resize(maxInt, 0);
        b_int.resize(maxInt, 0);

        // 4. Sečti celé číslice zprava
        for (size_t i = 0; i < maxInt; ++i) {
            int sum = a_int[i] + b_int[i] + carry;
            result.m_intPart.push_back(sum % 10);
            carry = sum / 10;
        }
        if (carry)
            result.m_intPart.push_back(carry);

        result.removeTrailingZeros();
        result.removeLeadingZeros();
        return result;
    }

    CFloat operator-(const CFloat& other) const {
        CFloat result("0.0");
        result.m_intPart.clear();
        result.m_fracPart.clear();
        result.m_negative = false;

        // 1. Zarovnat desetinné části
        size_t maxFrac = std::max(m_fracPart.size(), other.m_fracPart.size());
        std::vector<int> a_frac = m_fracPart;
        std::vector<int> b_frac = other.m_fracPart;
        a_frac.resize(maxFrac, 0);
        b_frac.resize(maxFrac, 0);

        // 2. Odečti desetinnou část zprava doleva
        int borrow = 0;
        for (int i = static_cast<int>(maxFrac) - 1; i >= 0; --i) {
            int diff = a_frac[i] - b_frac[i] - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.m_fracPart.insert(result.m_fracPart.begin(), diff);
        }

        // 3. Zarovnání celé části
        size_t maxInt = std::max(m_intPart.size(), other.m_intPart.size());
        std::vector<int> a_int = m_intPart;
        std::vector<int> b_int = other.m_intPart;
        a_int.resize(maxInt, 0);
        b_int.resize(maxInt, 0);

        // 4. Odečíst celé číslice zprava doleva
        for (size_t i = 0; i < maxInt; ++i) {
            int digitA = a_int[i];
            int digitB = b_int[i];
            int diff = digitA - digitB - borrow;
            if (diff < 0) {
                diff += 10;
                borrow = 1;
            } else {
                borrow = 0;
            }
            result.m_intPart.push_back(diff);
        }

        result.removeTrailingZeros();
        result.removeLeadingZeros();
        return result;
    }
    void print(std::ostream& os) const override {
        if (m_negative) os << "-";

        // Celá část
        if (m_intPart.empty()) os << "0";
        else {
            for (auto it = m_intPart.rbegin(); it != m_intPart.rend(); ++it)
                os << *it;
        }

        // Desetinná část
        if (!m_fracPart.empty()) {
            os << ".";
            for (int d : m_fracPart)
                os << d;
        }
    }

private:
    std::vector<int> m_intPart;   // reverzně
    std::vector<int> m_fracPart;  // normálně
    bool m_negative = false;

    void removeTrailingZeros() {
        while (!m_fracPart.empty() && m_fracPart.back() == 0)
            m_fracPart.pop_back();
    }

    void removeLeadingZeros() {
        while (m_intPart.size() > 1 && m_intPart.back() == 0)
            m_intPart.pop_back();
    }
};

/// Testovací výstupní funkce (jak ve vzorovém zadání)
void printNum(const CNumber& num) {
    std::cout << num << std::endl;
}

/// Vstupní bod programu
int main() {
    const CFloat af("5.13545623465465465465465464121313132465415");
    const CFloat bf("8.21564564456123132123132146544646546546546528564");
    printNum(af + bf);
    printNum(af - bf);

    const CInt ai("132115465615664583461973866555483255524643727514673745194967728");
    const CInt bi("55376461616151515944594954467327731717171727158844");
    printNum(ai + bi);
    printNum(ai - bi);

    std::cout << "Press return to close the window..." << std::endl;
    std::cin.get();  // čeká na stisk klávesy

    return 0;
}