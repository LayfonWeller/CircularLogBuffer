#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <algorithm>
#include <stdio.h>
#include <array>

#include <assert.h>

#ifndef DEBUG
#define DEBUG
#endif

#define DEBUG_LOGITERATOR 1
#include <CircularLogsIterator.hpp>

static void print_memoryOrder();

#define BED_EXIT_LOG_CAPACITY 10

bool bBedExitLogFull   = false;
int  byBedExitLogIndex = 0;

typedef struct BedExitLogData_s {
  uint32_t date;
  uint32_t log;
} BedExitLogData;

typedef struct BedExitLog_s {
  /**
   * @brief The number of elements in the Bed Exit Log
   *
   */
  uint16_t byDataCount;
  /**
   * @brief The index of the current log in the Bed Exit Log
   *
   */
  uint16_t       byDataNumber;
  BedExitLogData sLogData;
} BedExitLog;

static BedExitLogData sBedExitLogData[BED_EXIT_LOG_CAPACITY];

using BedExitLogIterator = LogIterator<BedExitLogData, BED_EXIT_LOG_CAPACITY>;

void Quicksort(BedExitLogData arr[BED_EXIT_LOG_CAPACITY], const size_t low, const size_t high, const bool logFull)
{
  auto beg = BedExitLogIterator(arr, low, false);
  auto end = BedExitLogIterator(arr, high, logFull);
  std::stable_sort(beg, end, [](const BedExitLogData &a, const BedExitLogData &b) { return a.date < b.date; });
}

static void SortLogs()
{
  // //Quicksort(sBedExitLogData, 0, byBedExitLogIndex, true);
  // if (bBedExitLogFull)
  //   Quicksort(sBedExitLogData, byBedExitLogIndex, BED_EXIT_LOG_CAPACITY);
  // else
  //   Quicksort(sBedExitLogData, 0, byBedExitLogIndex);

  if (bBedExitLogFull)
    Quicksort(sBedExitLogData, byBedExitLogIndex, byBedExitLogIndex, bBedExitLogFull);
  else
    Quicksort(sBedExitLogData, 0, byBedExitLogIndex, bBedExitLogFull);
}

static void SortLogs2()
{
  // Quicksort(sBedExitLogData, 0, byBedExitLogIndex);
  // if (bBedExitLogFull)
  //   Quicksort(sBedExitLogData, byBedExitLogIndex, BED_EXIT_LOG_CAPACITY);
  SortLogs();
}

void bexm_AddBedExitLog(const uint32_t time, const uint32_t eLogMode)
{
  sBedExitLogData[byBedExitLogIndex].date = time;
  sBedExitLogData[byBedExitLogIndex].log  = eLogMode;
  // printf("Set [%d:%d] in %d\n", sBedExitLogData[byBedExitLogIndex].date,
  // sBedExitLogData[byBedExitLogIndex].log, byBedExitLogIndex);

  ++byBedExitLogIndex;
  if (byBedExitLogIndex >= BED_EXIT_LOG_CAPACITY) {
    // Log is full, save the flag into EEPROM
    bBedExitLogFull = true;

    byBedExitLogIndex = 0;
  }
  // puts("PreSort");
  // print_memoryOrder();
  SortLogs();
  // puts("PostSort");
  // print_memoryOrder();
}

void bexm_AddBedExitLog_NoSort(const uint32_t time, const uint32_t eLogMode)
{
  sBedExitLogData[byBedExitLogIndex].date = time;
  sBedExitLogData[byBedExitLogIndex].log  = eLogMode;

  ++byBedExitLogIndex;
  if (byBedExitLogIndex >= BED_EXIT_LOG_CAPACITY) {
    // Log is full, save the flag into EEPROM
    bBedExitLogFull = true;

    byBedExitLogIndex = 0;
  }
}

uint8_t bexm_GetBedExitLogCount(void)
{
  if (bBedExitLogFull) {
    return BED_EXIT_LOG_CAPACITY;
  } else {
    return byBedExitLogIndex;
  }
}

uint8_t bexm_GetBedExitLogIndex(void) { return byBedExitLogIndex; }

BedExitLogData *bexm_GetLog(void) { return sBedExitLogData; }

void bexm_LoadBedExitLog(unsigned int count)
{
  // Get LogIndex
  // NVDM_Read(NVDMData_BedExitHistoryLogStart,(uint8_t*)&byBedExitLogIndex,1);
  byBedExitLogIndex = count % BED_EXIT_LOG_CAPACITY;
  bBedExitLogFull   = count >= BED_EXIT_LOG_CAPACITY;

  // Get Bed Exit Log
  int index = 0;
  for (size_t i = 0; i < count; ++i) {
    sBedExitLogData[index].date = i / 3 % 3;
    sBedExitLogData[index].log  = i + 1;

    ++index;
    if (index >= BED_EXIT_LOG_CAPACITY) {
      // Log is full, save the flag into EEPROM

      index = 0;
    }
  }

  // If the LogIndex is not initialized, initialize it
  if (byBedExitLogIndex == 0xFF) {
    byBedExitLogIndex = 0;
  }

  SortLogs();
}

static uint8_t byLogNumber              = 1;
static size_t  byNextBedExitIndexToSend = BED_EXIT_LOG_CAPACITY;

enum STATE {
  STATE_NOTHING,
  STATE_STARTING,
  STATE_RUNNING,
  STATE_FINISHED,
};
static STATE curState = STATE_NOTHING;
void         sclm_SetCommandStatus(const STATE newState) { curState = newState; }

void lin_SendBedExitLog()
{
  sclm_SetCommandStatus(STATE_STARTING);
  uint8_t byLogCount = bexm_GetBedExitLogCount();
  // If the log has any data
  if (byLogCount > 0) {
    byLogNumber = 1;

    // If the log is not full, start sending from last index
    if (byLogCount < BED_EXIT_LOG_CAPACITY) {
      byNextBedExitIndexToSend = byLogCount - 1;
      puts("a");
    }
    // If the log is full, start sending from BEFORE the log index
    else {
      if (bexm_GetBedExitLogIndex() > 0) {
        byNextBedExitIndexToSend = bexm_GetBedExitLogIndex() - 1;
        puts("b");
      } else {
        byNextBedExitIndexToSend = BED_EXIT_LOG_CAPACITY - 1;
        puts("c");
      }
    }
  }
}

void lin_StopSendingBedExitLog()
{
  // tmr_SetTimer(TMR_BEDEXIT_LOG, Timer_OnShot, (uint64_t)0);
  // lin_RemoveGivenRequest(SEND_BED_EXIT_LOG);

  byLogNumber              = 1;
  byNextBedExitIndexToSend = BED_EXIT_LOG_CAPACITY;
}

void lin_actualSendBedExitLog()
{
  sclm_SetCommandStatus(STATE_RUNNING);

  uint8_t    byLogCount = bexm_GetBedExitLogCount();
  BedExitLog sLinBedExitLog;

  if (byLogCount > 0) {
    // If the Bed Exit Index is valid
    if (byNextBedExitIndexToSend < byLogCount) {
      sLinBedExitLog.sLogData     = bexm_GetLog()[byNextBedExitIndexToSend];
      sLinBedExitLog.byDataNumber = byLogNumber;
      sLinBedExitLog.byDataCount  = byLogCount;

      // Prepare to send the next log if there's any
      if (sLinBedExitLog.byDataCount > byLogNumber) {
        byLogNumber++;
        if (byNextBedExitIndexToSend > 0) {
          byNextBedExitIndexToSend--;
        } else {
          byNextBedExitIndexToSend = byLogCount - 1u;
        }
        // tmr_SetTimer(TMR_BEDEXIT_LOG, Timer_OnShot, (uint64_t)300);
      }
      // Stop sending the bed exit log if done
      else {
        lin_StopSendingBedExitLog();
        sclm_SetCommandStatus(STATE_FINISHED);
      }
    } else {
      sLinBedExitLog.byDataNumber = 0;
      sLinBedExitLog.byDataCount  = 0;
      lin_StopSendingBedExitLog();
      sclm_SetCommandStatus(STATE_FINISHED);
    }
  }
  // Send empty log
  else {
    sLinBedExitLog.byDataNumber = 0;
    sLinBedExitLog.byDataCount  = 0;
    lin_StopSendingBedExitLog();
    sclm_SetCommandStatus(STATE_FINISHED);
  }

  printf("[%d/%d] %d : %d\n", sLinBedExitLog.byDataNumber, sLinBedExitLog.byDataCount, sLinBedExitLog.sLogData.date,
         sLinBedExitLog.sLogData.log);
}
#ifndef NUMBER_OF_LOGS
#define NUMBER_OF_LOGS 20
#endif
#if !defined(RUNTIME) && !defined(LOADTIME) || RUNTIME == 0
#define RUNTIME
#endif

static void print_memoryOrder()
{
  puts("****Memory Order****");
  printf("Current Count %d\n", bexm_GetBedExitLogCount());
  for (int i = 0; i < BED_EXIT_LOG_CAPACITY; ++i) {
    printf("%d : %d", bexm_GetLog()[i].date, bexm_GetLog()[i].log);
    if (i == bexm_GetBedExitLogIndex())
      puts(" <--");
    else
      puts("");
  }
  puts("");
}

int main()
{
  // #ifdef RUNTIME
  //     puts("RUNTIME");
  //     for (int i = 0; i < NUMBER_OF_LOGS; ++i) {
  //         bexm_AddBedExitLog(i / 3 % 3, i + 1);
  //     }
  // #else
  //     puts("LOAD");
  //     bexm_LoadBedExitLog(NUMBER_OF_LOGS);
  // #endif
  //     puts("*********************************************************************"
  //          "********");
  //     puts("Result ");
  //     puts("");
  //     print_memoryOrder();
  //     puts("****SEND ORDER****");
  //     lin_SendBedExitLog();
  //     while (curState != STATE_FINISHED) {
  //         lin_actualSendBedExitLog();
  //     }
  // return 0;

  BedExitLogIterator a(sBedExitLogData, 1, true);
  BedExitLogIterator b(sBedExitLogData, 2, true);
  BedExitLogIterator c(sBedExitLogData, 1, false);
  BedExitLogIterator d(sBedExitLogData, 2, false);

  puts("");
  puts("");
  puts("Substration w/iterators");
  puts("");

  assert(a - a == 0);
  assert(a - b == -1);
  assert(a - c == 10);
  assert(a - d == 9);

  assert(b - a == 1);
  assert(b - b == 0);
  assert(b - c == 11); // FIXME
  assert(b - d == 10);

  assert(c - a == -10);
  assert(c - b == -11); // FIXME
  assert(c - c == 0);
  assert(c - d == -1);

  assert(d - a == -9);
  assert(d - b == -10);
  assert(d - c == 1);
  assert(d - d == 0);

  BedExitLogIterator e = d;
  assert(e == d);
  --e;
  BedExitLogIterator gg = d - 1;
  assert(e == c);
  assert(e == gg);
  BedExitLogIterator g(sBedExitLogData, 0, false);
  BedExitLogIterator gg2 = d - 2;
  --e;
  assert(e == g);
  assert(e == gg2);

  BedExitLogIterator f = b;
  assert(f == b);
  --f;
  BedExitLogIterator gb = b - 1;
  assert(f == a);
  assert(f == gb);

  --f;
  BedExitLogIterator g2(sBedExitLogData, 0, true);
  BedExitLogIterator gb2 = b - 2;
  assert(f == g2);
  assert(f == gb2);

  --f;
  BedExitLogIterator gb3 = b - 3;
  BedExitLogIterator g3(sBedExitLogData, 9, false);
  assert(f == g3);
  assert(f == gb3);

  // puts("");
  // puts("");
  // puts("Substration w/Number");
  // puts("");

  // assert((a - 1) == BedExitLogIterator(sBedExitLogData, 0, true));
  // // assert((a - 2) == BedExitLogIterator(sBedExitLogData, 1, true));
  // // assert((a - -1) == BedExitLogIterator(sBedExitLogData, 1, true));
  // // assert((a - -2) == BedExitLogIterator(sBedExitLogData, 1, true));
  // // assert((a - 10) == BedExitLogIterator(sBedExitLogData, 1, true));
  // // assert((a - -10) == BedExitLogIterator(sBedExitLogData, 1, true));
  // // puts("");
  // assert((b - 1) == BedExitLogIterator(sBedExitLogData, 1, true));
  // assert((b - 2) == BedExitLogIterator(sBedExitLogData, 2, true));
  // // printf("Distance %ld\n", b - 1);
  // // printf("Distance %ld\n", b - 2);
  // // printf("Distance %ld\n", b - -1);
  // // printf("Distance %ld\n", b - -2);
  // // printf("Distance %ld\n", b - 10);
  // // printf("Distance %ld\n", b - -10);
  // // puts("");
  // // printf("Distance %ld\n", c - 1);
  // // printf("Distance %ld\n", c - 2);
  // // printf("Distance %ld\n", c - -1);
  // // printf("Distance %ld\n", c - -2);
  // // printf("Distance %ld\n", c - 10);
  // // printf("Distance %ld\n", c - -10);
  // // puts("");
  // // printf("Distance %ld\n", d - 1);
  // // printf("Distance %ld\n", d - 2);
  // // printf("Distance %ld\n", d - -1);
  // // printf("Distance %ld\n", d - -2);
  // // printf("Distance %ld\n", d - 10);
  // // printf("Distance %ld\n", d - -10);

  // // puts("");
  // // puts("");
  // // puts("Addition w/Number");
  // // puts("");

  // // printf("Distance %ld\n", a + 1);
  // // printf("Distance %ld\n", a + 2);
  // // printf("Distance %ld\n", a + -1);
  // // printf("Distance %ld\n", a + -2);
  // // printf("Distance %ld\n", a + 10);
  // // printf("Distance %ld\n", a + -10);
  // // puts("");
  // // printf("Distance %ld\n", b + 1);
  // // printf("Distance %ld\n", b + 2);
  // // printf("Distance %ld\n", b + -1);
  // // printf("Distance %ld\n", b + -2);
  // // printf("Distance %ld\n", b + 10);
  // // printf("Distance %ld\n", b + -10);
  // // puts("");
  // // printf("Distance %ld\n", c + 1);
  // // printf("Distance %ld\n", c + 2);
  // // printf("Distance %ld\n", c + -1);
  // // printf("Distance %ld\n", c + -2);
  // // printf("Distance %ld\n", c + 10);
  // // printf("Distance %ld\n", c + -10);
  // // puts("");
  // // printf("Distance %ld\n", d + 1);
  // // printf("Distance %ld\n", d + 2);
  // // printf("Distance %ld\n", d + -1);
  // // printf("Distance %ld\n", d + -2);
  // // printf("Distance %ld\n", d + 10);
  // // printf("Distance %ld\n", d + -10);
  // return 0;
}