====================================================================
               IoT SENSOR GATEWAY DAEMON - ROADMAP
====================================================================

[ PHASE 1: The OS Handshake ] ──┐
  ├─ Fork & Orphan (setsid)     │ Detach from the terminal
  ├─ Redirect FDs (/dev/null)   │ Run in the background safely
  └─ Write PID File             │ Prevent duplicate processes
                                │
                                ▼
[ PHASE 2: Sight and Sound ] ───┐
  ├─ Setup Syslog               │ Essential for invisible daemons
  └─ Register Signal Handlers   │ Catch SIGTERM (kill) / SIGHUP
                                │
                                ▼
[ PHASE 3: Infrastructure ] ────┐
  ├─ Init Global State & Mutex  │ Protect shared sensor data
  ├─ Init Ring Buffer & Sems    │ Producer/Consumer pipeline
  └─ Init pthread_barrier       │ Synchronize thread startup
                                │
                                ▼
[ PHASE 4: Business Logic ] ────┐
  ├─ Thread 1: UDP Listener     │ (Producer) Push raw data to buffer
  ├─ Thread 2: Worker           │ (Consumer) Parse & update state
  ├─ Thread 3: TCP Admin        │ Serve processed data to clients
  └─ Thread 4: Alarm Monitor    │ Wait on Condvar for emergencies
                                │
                                ▼
[ PHASE 5: Graceful Exit ] ─────┘
  ├─ Catch SIGTERM flag
  ├─ Wake sleeping threads
  ├─ Join threads safely
  └─ Free Memory & Rm PID File