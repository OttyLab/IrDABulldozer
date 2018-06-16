#include <stdio.h>
#include "irda.h"
IRDA::IRDA(
  unsigned long time,
  void (*on_commit)(unsigned long),
  void (*on_repeat)(unsigned long),
  void (*on_error)(char*)) :
  state(INIT),
  start_time(time),
  prev_time(time),
  cnt(0),
  on_commit(on_commit),
  on_repeat(on_repeat),
  on_error(on_error) {
}

IRDA::~IRDA() {
}

void IRDA::detect(unsigned long now) {
  long diff = now - prev_time;
  if (state == INIT
    && compare(diff, READER_TIME)) {
    start_time = now - READER_TIME;
    data = 0;
    cnt = 0;
    state = CODE;
  } else if(state == CODE) {
    if (compare(diff, ZERO_TIME)) {
      data <<= 1;
    } else if (compare(diff, ONE_TIME)) {
      data <<= 1;
      data += 1;
    } else {
      state = INIT;
    }

    if (cnt++ == sizeof(data) * 8) {
      on_commit(data);
      state = REPEAT;
    }
  } else if (state == REPEAT
    && compare(diff, REPEAT_TIME)) {
    state = STOP;
  } else if (state == STOP
    && compare(diff, STOP_TIME)) {
    start_time = now - STOP_TIME;
    on_repeat(data);
    state = REPEAT;
  } else {
    state = INIT;
  }

  prev_time = now;
}

bool IRDA::compare(
  unsigned long diff, unsigned long target) {
  return diff > target * 0.85 && diff < target * 1.15;
}
