# 3djoy

Control your CNC or 3D-Printer using game controller.


### Install

```bash
make
sudo make install
```

### Quickstart

```bash
3djoy /dev/ttyACM0 # Default baudrate: 115200
```

#### Help

```bash
3djoy --help
Usage: 3djoy [OPTION...] OUTPUT
Serial Interface multiplexer

  -b, --baudrate=BAUDRATE    Baudrate, default: 115200
  -i, --input=INPUT          Input device, default: /dev/input/js0
  -p, --port=TCPPORT         Remote TCP port, default: 5600
  -v, --verbose              Verbose Mode
  -?, --help                 Give this help list
      --usage                Give a short usage message
  -V, --version              Print program version

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.

Report bugs to http://github.com/pylover/3djoy.
```


### Systemd

First you have to install [serialhub](https://github.com/pylover/serialhub).


Create a file named `/etc/systemd/system/3djoy.service`.

```systemd
[Unit]
Description=3djoy
After=network.target
Wants=serialhub.serivce

[Service]
ExecStart=/usr/local/bin/3djoy --baudrate 115200 unix:///run/serialhub.socket
Restart=on-failure
KillSignal=SIGINT

[Install]
WantedBy=multi-user.target
```

Then:

```bash
systemctl daemon-reload
```


### UDEV

```bash
KERNEL=="js?", ACTION=="add", ENV{ID_MODEL}=="usb_gamepad", RUN+="/bin/systemctl start 3djoy.service"
KERNEL=="js?", ACTION=="remove", ENV{ID_MODEL}=="usb_gamepad", RUN+="/bin/systemctl stop 3djoy.service"
```

