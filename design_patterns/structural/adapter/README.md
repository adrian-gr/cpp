# Adapter

## Description

Adapter is a structural design pattern that converts one interface into another
interface a client expects. It lets existing or third-party code — code you can't
or don't want to modify — collaborate with the rest of your system, by wrapping it
behind an interface your client code already understands.

The adapter's only job is translation and delegation: it implements the target
interface the client expects, and internally forwards each call to the wrapped
(adaptee) object, converting arguments and return values as needed along the way.

Typical participants:

- **Target** — the interface the client code expects to work with (e.g.
  `PaymentGateway`).
- **Adaptee** — the existing class with an incompatible interface that needs to be
  reused as-is (e.g. `LegacyBillingSystem`).
- **Adapter** — implements the Target interface, holds a reference to the Adaptee,
  and translates calls from one interface to the other (e.g. `BillingAdapter`).
- **Client** — depends only on the Target interface, unaware that an adapter and a
  legacy system exist behind it (e.g. `checkout()`).

## When to Use

- A legacy or third-party API has the wrong interface for your codebase, and you
  want your client code to work against a clean interface instead of the
  original's shape.
- You need to isolate an external dependency behind your own interface — so if the
  dependency ever changes or gets replaced, only the adapter needs to change, not
  every caller.
- Several incompatible implementations need to look the same to a client — one
  target interface, multiple adapters, each wrapping a different underlying
  library or system.

## When to Avoid

- Changing the original interface is easy and fully under your control — just
  change it directly rather than introducing a wrapper around it.
- The "adapter" would end up containing substantial business logic rather than
  pure translation — that's a sign the class has grown beyond what Adapter is for,
  and the logic belongs somewhere else.
- Many adapters are piling up to paper over a confused or unstable underlying
  design — at that point, the adapters are masking a deeper problem rather than
  solving a genuine interface mismatch.

## How to Implement

1. **Define the Target interface.** What your client code actually wants to call.

   ```cpp
   class PaymentGateway {
   public:
       virtual ~PaymentGateway() = default;
       virtual void charge(double amount) = 0;
   };
   ```

2. **Identify the Adaptee.** The existing class you can't change, with an
   incompatible interface.

   ```cpp
   class LegacyBillingSystem {
   public:
       void makePaymentInCents(int cents, const std::string& account) {
           std::cout << "legacy charge: " << cents << " cents for " << account << "\n";
       }
   };
   ```

3. **Implement the Adapter.** Realizes the Target interface, holds a reference to
   the Adaptee, and translates each call.

   ```cpp
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
   ```

4. **Write client code against the Target interface only.**

   ```cpp
   void checkout(PaymentGateway& gateway) {
       gateway.charge(19.99);
   }

   int main() {
       LegacyBillingSystem legacyBilling;
       BillingAdapter adapter(legacyBilling, "account-42");
       checkout(adapter);   // checkout() never references LegacyBillingSystem
   }
   ```

## Implementation Tips

- Keep the adapter focused purely on conversion and delegation — argument
  translation, return-value mapping, and forwarding the call. Anything more is a
  sign the logic belongs in a different class.
- Prefer composition (holding a reference/pointer to the adaptee, as above) over
  inheriting from the adaptee — composition makes the adapted object easier to
  replace, mock, or test independently of the adapter.
- Give the Target interface domain-specific names (`charge`, not
  `makePaymentInCents`) rather than leaking the adaptee's legacy terminology — the
  whole point is that client code shouldn't need to know the legacy system's
  vocabulary exists.
- If you need to adapt an entire family of related legacy calls, consider whether
  a single adapter class covering all of them, or several smaller adapters each
  covering one responsibility, better matches how your client code is organized.
- An adapter for a class you can't subclass (e.g. it's `final`, or you need to
  adapt multiple unrelated adaptees behind one target) is a good candidate for the
  object-adapter (composition) style shown here, rather than a class-adapter
  (multiple inheritance) style.