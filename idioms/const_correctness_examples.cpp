// Const-correctness example
// Compile with: g++ -std=c++20 -O2 idioms/const_correctness_examples.cpp -o bin/const_correctness_example && ./bin/const_correctness_example

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

// Description:
// const-correctness means using `const` everywhere an object, parameter, or method
// is not meant to modify what it touches. It's not just documentation — the
// compiler enforces it, turning "this shouldn't change" from a comment into a
// checked guarantee. A violation becomes a compile error, not a runtime surprise.
//
// const applies in three main places, each expressing a different promise:
// - const variables/parameters: "this value won't be modified once initialized."
// - const reference/pointer parameters (const T&, const T*): "this function
//   only reads through this parameter, and won't mutate the caller's object."
// - const member functions (void foo() const): "calling this method doesn't
//   modify the object's observable state" — enforced by the compiler, which
//   rejects any attempt inside the method to mutate non-mutable members.
//
// Use const when:
// - a parameter, variable, or method is genuinely read-only — mark it const so
//   the compiler catches accidental mutation instead of a bug surfacing later;
// - you're writing a class's accessor/query methods — mark every one const so
//   they can be called on const instances and const references to the class;
// - a function takes a non-trivial type (string, vector, user-defined class) and
//   only needs to read it — pass by const reference to avoid a copy while still
//   preventing mutation (see the value-vs-reference-semantics example).
//
// Avoid / adapt const when:
// - the method genuinely needs to mutate the object's logical state — don't mark
//   it const just to satisfy a caller; that's a correctness bug waiting to happen;
// - a member needs to change even from a const method for reasons that don't
//   affect the object's externally-observable state (e.g. a cache, a lazily
//   computed value, a mutex used only for internal synchronization) — that's
//   exactly what `mutable` is for.
//
// Tips:
// - Mark member functions const by default; only drop const when the method
//   truly needs to mutate the object.
// - Prefer const T& parameters over T for non-trivial types you only read.
// - const correctness composes: a const object can only call const methods on it,
//   so getting this right at the class level protects every caller automatically.

class BankAccount {
public:
    explicit BankAccount(double initialBalance) : balance_(initialBalance) {}

    // const method: only reads balance_, never modifies it. Callable on both
    // BankAccount and const BankAccount&.
    double balance() const {
        return balance_;
    }

    // const method calling another const method — the compiler verifies this
    // chain never mutates the object.
    bool canWithdraw(double amount) const {
        return amount <= balance();
    }

    // Non-const: this genuinely changes the object's state, so it must NOT be const.
    void deposit(double amount) {
        if (amount < 0) {
            throw std::invalid_argument("deposit amount cannot be negative");
        }
        balance_ += amount;
    }

    void withdraw(double amount) {
        if (!canWithdraw(amount)) {
            throw std::runtime_error("insufficient funds");
        }
        balance_ -= amount;
    }

    // mutable lets accessCount_ change even inside a const method, because it's
    // bookkeeping that doesn't affect the account's externally-observable balance.
    double balanceWithLogging() const {
        ++accessCount_;   // allowed: accessCount_ is mutable
        std::cout << "balance checked " << accessCount_ << " time(s)\n";
        return balance_;
    }

private:
    double balance_;
    mutable int accessCount_ = 0;
};

// const T& parameter: describeAccount only needs to read the account, so it takes
// a const reference — no copy, and the compiler enforces read-only access.
void describeAccount(const BankAccount& account) {
    std::cout << "balance: " << account.balance() << "\n";
    // account.deposit(10.0);   // would NOT compile: deposit() is non-const,
    //                          // and account is a const reference here.
}

// Non-const T& parameter: applyMonthlyFee genuinely needs to mutate the account
// the caller passed in, so it takes a non-const reference.
void applyMonthlyFee(BankAccount& account, double fee) {
    account.withdraw(fee);
}

int main() {
    std::cout << "Const-correctness example:\n";

    const double interestRate = 0.02;   // const variable: never reassigned after init
    std::cout << "interest rate: " << interestRate << "\n";
    // interestRate = 0.03;   // would NOT compile: interestRate is const.

    BankAccount account(100.0);

    describeAccount(account);       // read-only use: works fine, no copy made
    applyMonthlyFee(account, 5.0);  // mutating use: caller's account really changes
    describeAccount(account);       // reflects the fee that was just applied

    const BankAccount frozenAccount(500.0);
    std::cout << "frozen balance: " << frozenAccount.balance() << "\n";
    // frozenAccount.deposit(50.0);   // would NOT compile: deposit() is non-const,
    //                                // and frozenAccount is a const object.

    frozenAccount.balanceWithLogging();   // OK: balanceWithLogging() is const,
                                           // and mutable accessCount_ can still change.

    return 0;
}