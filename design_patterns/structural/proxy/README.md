# Proxy

## Description

Proxy is a structural design pattern that provides a surrogate or placeholder for
another object to control access to it. The proxy implements the same interface as
the real object (the "real subject"), so clients interact with the proxy exactly as
they would with the real thing — while the proxy adds its own logic around
delegating to (or creating) that real object: lazy initialization, access control,
logging, caching, or remote communication.

Because the proxy and the real subject share an interface, client code never needs
to know — or be able to tell — whether it's holding a proxy or the real object.

Typical participants:

- **Subject** — the shared interface implemented by both the real object and its
  proxy (e.g. `Image`).
- **RealSubject** — the actual object that does the real work, potentially
  expensive to create or access (e.g. `HighResImage`).
- **Proxy** — implements the same interface, and controls access to a RealSubject
  it creates, holds, or communicates with (e.g. `ImageProxy`).

## When to Use

- Object creation or initialization is expensive, and you want to defer it until
  the object is actually needed — a **virtual proxy** (lazy-loading proxy), as in
  the example, where the real object isn't constructed until first use.
- You need to control access to an object — e.g. permission checks before allowing
  an operation to proceed — a **protection proxy**.
- The real object lives elsewhere (a different process or machine), and the proxy
  stands in locally, handling the communication details transparently — a **remote
  proxy**.
- You want to add behavior like caching or logging around calls to an object,
  without modifying the object itself — a **smart reference** style proxy.

## When to Avoid

- There's no real need to control, defer, or intercept access to the object —
  direct use of the real object is simpler, and the extra indirection buys nothing.
- The intent is really to extend behavior with new responsibilities (adding
  functionality) rather than controlling access to existing functionality — that's
  a better fit for Decorator, even though the structures look nearly identical.
- The overhead of an extra indirection layer isn't justified by whatever access
  control, laziness, or remoteness concern it would address — don't add a proxy
  "just in case."

## How to Implement

1. **Define the Subject interface.** Shared by the real object and its proxy.

   ```cpp
   class Image {
   public:
       virtual ~Image() = default;
       virtual void display() const = 0;
   };
   ```

2. **Implement the Real Subject.** The actual, potentially expensive object.

   ```cpp
   class HighResImage final : public Image {
   public:
       explicit HighResImage(std::string filename) : filename_(std::move(filename)) {
           std::cout << "loading high-res image from disk: " << filename_ << "\n";
       }

       void display() const override { std::cout << "displaying " << filename_ << "\n"; }

   private:
       std::string filename_;
   };
   ```

3. **Implement the Proxy.** Same interface as the real subject; controls access to
   it — here, by deferring creation until first use.

   ```cpp
   class ImageProxy final : public Image {
   public:
       explicit ImageProxy(std::string filename) : filename_(std::move(filename)) {}

       void display() const override {
           if (!realImage_) {
               realImage_ = std::make_unique<HighResImage>(filename_);
           }
           realImage_->display();
       }

   private:
       std::string filename_;
       mutable std::unique_ptr<HighResImage> realImage_;
   };
   ```

4. **Give client code the proxy; it can't tell the difference.**

   ```cpp
   std::vector<std::unique_ptr<Image>> images;
   images.push_back(std::make_unique<ImageProxy>("vacation.jpg"));  // no loading yet

   for (const auto& image : images) {
       image->display();   // HighResImage is created here, on first display()
   }
   ```

## Implementation Tips

- Keep the proxy implementing the exact same interface as the real subject, so
  clients genuinely can't tell — and don't need to — whether they're talking to
  the real object or the proxy.
- Keep proxy-specific logic (lazy initialization, permission checks, logging,
  caching) separate from the real subject's own logic — the proxy should not
  duplicate or reimplement the real subject's business logic, only control access
  to it.
- Distinguish Proxy's intent (controlling access to an existing object) from
  Decorator's (adding new responsibilities to an object) even though both wrap an
  object behind a shared interface — this distinction should guide which pattern
  name and structure you reach for, since the code can look very similar.
- For a protection proxy, keep the access-control decision itself
  (permission-checking logic) isolated and testable, rather than scattered
  through every method the proxy forwards.
- For a remote proxy, keep the actual communication/serialization logic separate
  from the interface-forwarding logic, so the networking details don't leak into
  what should otherwise look like a plain local method call to the client.