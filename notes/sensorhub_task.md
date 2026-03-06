# 🔥 SensorHub - CODING SESSION (2–2.5h)

We implement **SensorHub v0 — Process + Pipe Foundation**

You will build:

```
sensor_writer   →   pipe   →   router_reader
```

Two processes.

One pipe.

Proper closing.

Correct EOF handling.

Framing included.

---

# Step 1 (15 min) — Minimal fork + pipe skeleton

Program structure:

```c
int fd[2];
pipe(fd);

pid_t pid = fork();

if (pid == 0) {
    // child
} else {
    // parent
}
```

Child:

- Close write end
- Read loop
- Print received messages

Parent:

- Close read end
- Write 5 framed messages
- Close write end
- wait()

Success condition:

Child exits cleanly after parent closes write-end.

---

# Step 2 (30–40 min) — Add framing

Define message format:

```
[uint32_t length][payload bytes]
```

Parent:

- send 5 messages like: "msg0", "msg1" ...

Child:

- read length
- read payload
- print safely

You must implement:

```c
int read_exact(int fd, void *buf, size_t n);
int write_exact(int fd, const void *buf, size_t n);
size_t recv_exact(int fd, void *buf, size_t len);
size_t send_all(int fd, const void *buf, size_t len);
```

Handle partial reads/writes.

---

# Step 3 (20 min) — Break It

Add option:

- Parent sleeps before closing write-end
- Test what happens if you forget to close write-end

Observe blocking behavior.

---

# Step 4 (20 min) — Invariant Logging

Add logs:

- When writer closes
- When reader detects EOF
- When length invalid (> 1024)

---

# Architecture Discipline Checklist (important)

Before writing code, decide:

1. Who owns shutdown?
2. Who closes write-end?
3. What ends are closed immediately after fork?
4. What invariant protects framing?

Write those 4 answers before coding.

---

# After coding

Paste:

- Your fork+pipe skeleton
- Your read_exact implementation
- One bug you hit
- Whether EOF behaved as expected

Then I’ll:

- Review correctness
- Ask 3 pressure questions
- And move you to fork+exec pattern (next critical step)

No perfection.

Working > elegant.