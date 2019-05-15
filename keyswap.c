#include <stdio.h>
#include <stdlib.h>

#include <linux/input.h>
#include <unistd.h>

// clang-format off
const struct input_event
esc_up          = {.type = EV_KEY, .code = KEY_ESC,      .value = 0},
ctrl_up         = {.type = EV_KEY, .code = KEY_LEFTCTRL, .value = 0},
capslock_up     = {.type = EV_KEY, .code = KEY_CAPSLOCK, .value = 0},
esc_down        = {.type = EV_KEY, .code = KEY_ESC,      .value = 1},
ctrl_down       = {.type = EV_KEY, .code = KEY_LEFTCTRL, .value = 1},
capslock_down   = {.type = EV_KEY, .code = KEY_CAPSLOCK, .value = 1},
esc_repeat      = {.type = EV_KEY, .code = KEY_ESC,      .value = 2},
ctrl_repeat     = {.type = EV_KEY, .code = KEY_LEFTCTRL, .value = 2},
capslock_repeat = {.type = EV_KEY, .code = KEY_CAPSLOCK, .value = 2},
syn             = {.type = EV_SYN, .code = SYN_REPORT,   .value = 0};
// clang-format on

/// Compare two input_events for equality (.type, .code, & .value)
int equal(const struct input_event *first, const struct input_event *second) {
  return first->type == second->type && first->code == second->code &&
         first->value == second->value;
}

/// Read one input_event from stdin into *event, returning true on success
int read_event(struct input_event *event) {
  return fread(event, sizeof(struct input_event), 1, stdin) == 1;
}

/// Write one input_event to stdout. Terminates program on failure
void write_event(const struct input_event *event) {
  if (fwrite(event, sizeof(struct input_event), 1, stdout) != 1)
    exit(EXIT_FAILURE);
}

int main(void) {
  int capslock_is_down = 0, esc_give_up = 0;
  struct input_event input;

  // Set stdin and stdout to be unbuffered (read infinitely)
  setbuf(stdin, NULL), setbuf(stdout, NULL);

  while (read_event(&input)) {
    // MSC_SCAN returns device-specific scancodes - not useful
    if (input.type == EV_MSC && input.code == MSC_SCAN)
      continue;

    // Pass non-keypress events through unaltered
    if (input.type != EV_KEY) {
      write_event(&input);
      continue;
    }

    if (capslock_is_down) {
      // If input is (the continuation of) the capslock press event, we already
      // know
      if (equal(&input, &capslock_down) || equal(&input, &capslock_repeat))
        continue;

      // Capslock was released
      if (equal(&input, &capslock_up)) {
        capslock_is_down = 0;

        if (esc_give_up) {
          // Some other key received
          esc_give_up = 0;
          write_event(&ctrl_up);
          continue;
        }

        // No other key received
        write_event(&esc_down);
        write_event(&syn);
        usleep(20000);
        write_event(&esc_up);
        continue;
      }

      // When capslock is down and another key with a valid value is pressed for
      // the first time
      if (!esc_give_up && input.value) {
        esc_give_up = 1;
        write_event(&ctrl_down); // New fake ctrl keypress
        write_event(&syn);       // Marker for synchronous keypresses
        usleep(20000);
        // Wait a bit, then write out input at end of loop
      }
    } else if (equal(&input, &capslock_down)) {
      // Capslock is now pressed
      capslock_is_down = 1;
      continue;
    }

    // Original behavior, undesired
    /*
    if (input.code == KEY_ESC)
      input.code = KEY_CAPSLOCK;
    */

    // clang-format off
    switch (input.code) {
      case KEY_RIGHTALT:    input.code = KEY_CAPSLOCK;    break;
      case KEY_RIGHTSHIFT:  input.code = KEY_UP;          break;
      case KEY_UP:          input.code = KEY_RIGHTSHIFT;  break;
    }
    // clang-format on

    write_event(&input);
  }
}
