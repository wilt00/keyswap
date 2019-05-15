# Keyswap

It turns out that the easiest way to rebind keys on Linux is to write your own C program to modify the stream of keypress events as they happen. God I love this platform.

Relies on Francisco Lopez's [Interception Tools](https://gitlab.com/interception/linux/tools) (and code is based very heavily on his [caps2esc](https://gitlab.com/interception/linux/plugins/caps2esc) plugin).

To install, first ensure those are installed, then put the following text in the file `/etc/udevmon.yaml`:
```yaml
- JOB: "intercept -g $DEVNODE | keyswap | uinput -d $DEVNODE"
  DEVICE:
    EVENTS:
      EV_KEY: [KEY_CAPSLOCK, KEY_ESC, KEY_UP, KEY_RIGHTALT, KEY_RIGHTSHIFT]
```

Note that commands on that pipe can be composed - it's pretty cool. Also don't forget to change EV_KEY if the keys the program operates on change.

Then run:

```bash
make
sudo make install
sudo systemctl restart udevmon
```

Currently remapping:
* Up Arrow to Right Shift
* Right Shift to Up Arrow
* Right Alt to Caps Lock
* Caps Lock to Esc when tapped, to Right Ctrl when held with another key