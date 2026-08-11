# Chain of Responsibility

## Description

Chain of Responsibility is a behavioral design pattern that passes a request along a
chain of handler objects. Each handler decides, independently, whether to process
the request itself or to pass it along to the next handler in the chain. The object
that issues the request doesn't need to know which handler will ultimately process
it, how many handlers exist, or how they're arranged.

This decouples senders from receivers: instead of a sender picking a specific
handler (or running a big `if`/`switch` to decide), it hands the request to the
front of a chain and lets the chain sort out who's responsible.

Typical participants:

- **Handler** — declares the interface for handling requests and for holding a
  reference to the next handler in the chain (e.g. `LogHandler`).
- **ConcreteHandler** — implements the check for whether it can handle a given
  request, and the logic to process it if so (e.g. `ConsoleHandler`, `EmailHandler`,
  `PagerHandler`).
- **Client** — builds the chain by linking handlers together, then submits requests
  to the head of the chain.

## When to Use

- More than one object may be able to handle a request, and which one actually does
  isn't known in advance — it should be determined at runtime by trying handlers in
  sequence.
- You want to issue a request without hardcoding or specifying its receiver — the
  sender just hands the request to the chain.
- The set of handlers, and the order they're tried in, should be configurable
  independently from the code that issues requests — e.g. building different chains
  for different environments or configurations.
- You're modeling a naturally staged escalation or filtering process — logging
  levels, middleware pipelines, event bubbling, approval workflows with multiple
  possible approvers.

## When to Avoid

- Exactly one handler will always process a given kind of request — a direct call to
  that handler is clearer than routing it through a chain that only ever stops at
  one fixed link.
- It's important that every request is guaranteed to be handled. A chain can let a
  request fall through with no handler claiming it, silently, unless you deliberately
  add a fallback/default handler at the end.
- A long chain becomes hard to debug because there's no single place that shows the
  full decision logic for how a request gets routed — tracing behavior means reading
  every handler in sequence.
- The relationship between handlers isn't really sequential — if multiple handlers
  need to *all* run, or if ordering doesn't reflect real precedence, this isn't the
  right pattern (consider Observer, or an explicit pipeline).

## How to Implement

1. **Define the Handler base class.** It holds a link to the next handler and
   exposes a template method (`handle()`) that checks whether it can handle the
   request, and forwards to the next handler if not.

   ```cpp
   class LogHandler {
   public:
       virtual ~LogHandler() = default;

       void setNext(std::shared_ptr<LogHandler> next) { next_ = std::move(next); }

       void handle(LogLevel level, const std::string& message) {
           if (canHandle(level)) {
               process(message);
           } else if (next_) {
               next_->handle(level, message);
           }
       }

   protected:
       virtual bool canHandle(LogLevel level) const = 0;
       virtual void process(const std::string& message) const = 0;

   private:
       std::shared_ptr<LogHandler> next_;
   };
   ```

2. **Implement Concrete Handlers.** Each one owns a single, narrow responsibility:
   decide if it applies, and process if so.

   ```cpp
   class ConsoleHandler final : public LogHandler {
   protected:
       bool canHandle(LogLevel level) const override { return level == LogLevel::Info; }
       void process(const std::string& message) const override {
           std::cout << "console: " << message << "\n";
       }
   };
   ```

3. **Build the chain.** Wire handlers together in the desired order.

   ```cpp
   auto console = std::make_shared<ConsoleHandler>();
   auto email = std::make_shared<EmailHandler>();
   auto pager = std::make_shared<PagerHandler>();

   console->setNext(email);
   email->setNext(pager);
   ```

4. **Submit requests to the head of the chain.** The caller only ever talks to the
   first handler; it doesn't know or care which one ends up processing the request.

   ```cpp
   console->handle(LogLevel::Info, "server started");
   console->handle(LogLevel::Warning, "disk usage above 80%");
   console->handle(LogLevel::Error, "database connection lost");
   ```

## Implementation Tips

- Give each handler a single, narrow responsibility rather than a broad conditional
  covering many cases — that keeps the chain composable and each link easy to
  reason about on its own.
- Always provide a way to terminate the chain safely — either a default/fallback
  handler at the end that handles anything nothing else claimed, or an explicit
  check after submission so requests don't silently vanish.
- Keep chain *construction* (wiring handlers together with `setNext`) separate from
  handler *logic* (`canHandle`/`process`) — this makes it easy to reconfigure the
  chain's order or membership without touching handler implementations.
- If a handler needs to both process a request *and* still forward it onward (rather
  than stopping the chain), make that explicit in the handler's logic rather than
  assuming the base class's default "handle or forward" behavior always applies.
- Consider whether handlers should be shared (`std::shared_ptr`) or owned in a single
  chain-owning container; shared ownership is convenient when the same handler
  instance might participate in more than one chain.