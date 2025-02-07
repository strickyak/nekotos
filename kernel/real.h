// Real time 

// This clock should steadily increase with each 60hz interrupt,
// except it rolls back to zero every 18.204444 hours.

// If you want to do something once per second, poll the seconds
// here, and see when it changes.  (You'll have to remember the
// previous value.)

struct real {
    byte ticks;  // Changes at 60Hz:  0 to 5
    byte decis;  // Tenths of a second: 0 to 9
    uint seconds;  // 0 to 65535
};  // Instance is named Real.

void Real_IncrementTicks(void);
