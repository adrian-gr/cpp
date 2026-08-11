// Adapter pattern example
// Compile with: g++ -std=c++20 -O2 design_patterns/structural/adapter_examples.cpp -o bin/adapter_example && ./bin/adapter_example

#include <iostream>
#include <string>
#include <utility>

// Description:
// Adapter converts one interface into another interface expected by a client.
// It lets existing or third-party code collaborate without changing its source.
//
// Use Adapter when:
// - a legacy or third-party API has the wrong interface;
// - you need to isolate an external dependency behind your own interface;
// - several incompatible implementations should look the same to a client.
//
// Avoid Adapter when:
// - changing the original interface is easy and under your control;
// - the adapter would contain substantial business logic instead of translation;
// - many adapters would hide a confused or unstable design.
//
// Tips:
// - Keep the adapter focused on conversion and delegation.
// - Prefer composition so the adapted object can be replaced or tested easily.
// - Give the target interface domain-specific names instead of leaking legacy terminology.

class PaymentGateway {
public:
    virtual ~PaymentGateway() = default;
    virtual void charge(double amount) = 0;
};

// Existing API that cannot be changed.
class LegacyBillingSystem {
public:
    void makePaymentInCents(int cents, const std::string& account) {
        std::cout << "legacy charge: " << cents << " cents for " << account << "\n";
    }
};

class BillingAdapter final : public PaymentGateway {
public:
    BillingAdapter(LegacyBillingSystem& billing, std::string account)
        : billing_(billing), account_(std::move(account)) {}

    void charge(double amount) override {
        int cents = static_cast<int>(amount * 100.0);
        billing_.makePaymentInCents(cents, account_);
    }

private:
    LegacyBillingSystem& billing_;
    std::string account_;
};

void checkout(PaymentGateway& gateway) {
    gateway.charge(19.99);
}

int main() {
    std::cout << "Adapter pattern example:\n";

    LegacyBillingSystem legacyBilling;
    BillingAdapter adapter(legacyBilling, "account-42");
    checkout(adapter);

    return 0;
}
